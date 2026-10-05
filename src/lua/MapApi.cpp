/*
 * Copyright (C) 2006-2019 Christopho, Solarus - http://www.solarus-games.org
 *
 * Solarus is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Solarus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "solarus/audio/Music.h"
#include "solarus/audio/Sound.h"
#include "solarus/core/CurrentQuest.h"
#include "solarus/core/Debug.h"
#include "solarus/core/Equipment.h"
#include "solarus/core/EquipmentItem.h"
#include "solarus/core/Game.h"
#include "solarus/core/MainLoop.h"
#include "solarus/core/QuestDatabase.h"
#include "solarus/core/Savegame.h"
#include "solarus/core/Map.h"
#include "solarus/core/ResourceProvider.h"
#include "solarus/core/Timer.h"
#include "solarus/core/Treasure.h"
#include "solarus/entities/Block.h"
#include "solarus/entities/Bomb.h"
#include "solarus/entities/Chest.h"
#include "solarus/entities/Crystal.h"
#include "solarus/entities/CrystalBlock.h"
#include "solarus/entities/CustomEntity.h"
#include "solarus/entities/Destination.h"
#include "solarus/entities/Destructible.h"
#include "solarus/entities/Door.h"
#include "solarus/entities/DynamicTile.h"
#include "solarus/entities/Enemy.h"
#include "solarus/entities/Entities.h"
#include "solarus/entities/EntityTypeInfo.h"
#include "solarus/entities/Explosion.h"
#include "solarus/entities/Fire.h"
#include "solarus/entities/GroundInfo.h"
#include "solarus/entities/Hero.h"
#include "solarus/entities/Jumper.h"
#include "solarus/entities/Npc.h"
#include "solarus/entities/Pickable.h"
#include "solarus/entities/Sensor.h"
#include "solarus/entities/Separator.h"
#include "solarus/entities/ShopTreasure.h"
#include "solarus/entities/Stairs.h"
#include "solarus/entities/Stream.h"
#include "solarus/entities/Switch.h"
#include "solarus/entities/Teletransporter.h"
#include "solarus/entities/TileInfo.h"
#include "solarus/entities/TilePattern.h"
#include "solarus/entities/Tileset.h"
#include "solarus/entities/Wall.h"
#include "solarus/lua/LuaBind.h"
#include "solarus/lua/LuaContext.h"
#include "solarus/lua/LuaTools.h"
#include <lua.hpp>
#include <sstream>

namespace Solarus {

namespace {

/**
 * \brief Lua equivalent of the deprecated map:move_camera() function.
 */
const char* move_camera_code =
"local map, x, y, speed, callback, delay_before, delay_after = ...\n"
"local camera = map:get_camera()\n"
"local game = map:get_game()\n"
"local hero = map:get_hero()\n"
"delay_before = delay_before or 1000\n"
"delay_after = delay_after or 1000\n"
"local back_x, back_y = camera:get_position_to_track(hero)\n"
"game:set_suspended(true)\n"
"camera:start_manual()\n"
"local movement = sol.movement.create(\"target\")\n"
"movement:set_target(camera:get_position_to_track(x, y))\n"
"movement:set_ignore_obstacles(true)\n"
"movement:set_speed(speed)\n"
"movement:start(camera, function()\n"
"  local timer_1 = sol.timer.start(map, delay_before, function()\n"
"    callback()\n"
"    local timer_2 = sol.timer.start(map, delay_after, function()\n"
"      local movement = sol.movement.create(\"target\")\n"
"      movement:set_target(back_x, back_y)\n"
"      movement:set_ignore_obstacles(true)\n"
"      movement:set_speed(speed)\n"
"      movement:start(camera, function()\n"
"        game:set_suspended(false)\n"
"        camera:start_tracking(hero)\n"
"        if map.on_camera_back ~= nil then\n"
"          map:on_camera_back()\n"
"        end\n"
"      end)\n"
"    end)\n"
"    timer_2:set_suspended_with_map(false)\n"
"  end)\n"
"  timer_1:set_suspended_with_map(false)\n"
"end)\n";

}  // Anonymous namespace.

/**
 * Name of the Lua table representing the map module.
 */
const std::string LuaContext::map_module_name = "sol.map";

/**
 * \brief Implementation of map:get_game().
 * \param map The map to call the method on.
 * \return Reference to the savegame.
 */
static Savegame & get_game(Map& map) {
  return *map.get_savegame();
}

/**
 * \brief Implementation of map:get_world().
 * \param map The map to call the method on.
 * \return A string containing the world name, or nullopt if no world is set.
 */
static std::optional<std::string> get_world(Map& map) {
  const std::string& world = map.get_world();
  return (world.empty()) ? std::nullopt : std::make_optional(world);
}

/**
 * \brief Implementation of map:set_world(world).
 * \param map The map to call the method on.
 * \param world_arg A string to set the world, nullopt to unset the world.
 */
static void set_world(Map& map, std::optional<std::string> world_arg) {
  std::string world = world_arg.value_or(std::string());
  map.set_world(world);
}

/**
 * \brief Implementation of map:get_size().
 * \param map The map to call the method on.
 * \return Width and height of the map.
 */
static std::tuple<int, int> get_size(Map& map) {
  return std::make_tuple(map.get_width(), map.get_height());
}

/**
 * \brief Implementation of map:get_location().
 * \param map The map to call the method on.
 * \return The horizontal and vertical location of the map.
 */
static std::tuple<int, int> get_location(Map& map) {
  const Rectangle& location = map.get_location();
  return std::make_tuple(location.get_x(), location.get_y());
}

/**
 * \brief Implementation of map:get_floor().
 * \param map The map to call the method on.
 * \return The floor the map is on, or nullopt if the map has no floor.
 */
static std::optional<int> get_floor(Map & map) {
  if (map.has_floor()) {
    return std::make_optional(map.get_floor());
  }
  return std::nullopt;
}

/**
 * \brief Implementation of map:set_floor(floor).
 * \param map The map to call the method on.
 * \param floor_arg An int to set the floor, or nullopt to unset the floor.
 */
static void set_floor(Map & map, std::optional<int> floor_arg) {
  const int floor = floor_arg.value_or(MapData::NO_FLOOR);
  map.set_floor(floor);
}

/**
 * \brief Implementation of map:get_music().
 * \param map The map to call the method on.
 * \return Music id, may be a special id.
 */
static std::optional<std::string> get_music(Map & map) {
  const std::string& music_id = map.get_music_id();
  if (Music::none == music_id) {
    return std::nullopt;
  } else if (Music::unchanged == music_id) {
    return std::make_optional("same");
  } else {
    return std::make_optional(music_id);
  }
}

/**
 * \brief Implementation of map:get_camera().
 * \param map The map to call the method on.
 * \return Pointer to a camera if the map has one, otherwise nullptr.
 */
static Camera * get_camera(Map & map) {
  const CameraPtr& camera = map.get_camera();
  return camera.get();
}

/**
 * \brief Implementation of map:get_camera_position().
 * \param context The context this Lua call was made in.
 * \param map The map to call the method on.
 * \return Number of values to return to Lua.
 */
static LuaBind::OnStack get_camera_position(
    LuaContext & context, Map & map) {
  context.warning_deprecated(
    { 1, 5 },
    "map:get_camera_position()",
    "Use map:get_camera():get_bounding_box() instead.");

  const CameraPtr& camera = map.get_camera();

  lua_State * l = context.get_internal_state();
  if (nullptr == camera) {
    lua_pushnil(l);
    return {1};
  }

  const Rectangle& camera_position = camera->get_bounding_box();

  lua_pushinteger(l, camera_position.get_x());
  lua_pushinteger(l, camera_position.get_y());
  lua_pushinteger(l, camera_position.get_width());
  lua_pushinteger(l, camera_position.get_height());
  return {4};
}

/**
 * \brief Implementation of map:move_camera().
 * \param context The context this Lua call was made in.
 * \param map The map to call the method on.
 * \return Number of values to return to Lua.
 */
static void move_camera(LuaContext & context, Map &) {
  context.warning_deprecated(
    { 1, 5 },
    "map:move_camera()",
    "Make a target movement on map:get_camera() instead.");

  lua_State * l = context.get_internal_state();
  // Wrapper handles the map.
  LuaTools::check_int(l, 2);
  LuaTools::check_int(l, 3);
  LuaTools::check_int(l, 4);
  LuaTools::check_type(l, 5, LUA_TFUNCTION);
  if (lua_gettop(l) >= 6) {
    LuaTools::check_int(l, 6);
  }
  if (lua_gettop(l) >= 7) {
    LuaTools::check_int(l, 7);
  }
  lua_settop(l, 7); // Make sure that we always have 7 arguments.

  lua_getfield(l, LUA_REGISTRYINDEX, "map.move_camera");
  if (!lua_isnil(l, -1)) {
    SOLARUS_ASSERT(lua_isfunction(l, -1), "map:move_camera() is not a function");
    lua_insert(l, 1);
    context.call_function(7, 0, "move_camera");
  }
}

/**
 * \brief Implementation of map:get_ground(x, y, layer).
 * \param map The map to call the method on.
 * \param x The horizontal position to check.
 * \param y The vertical position to check.
 * \param layer The layer (z position) to check.
 * \return The string reperenting the ground.
 */
static std::string get_ground(Map & map, int x, int y, int layer) {
  Ground ground = map.get_ground(layer, x, y, nullptr);
  return enum_to_name(ground);
}

/**
 * \brief Implementation of map:draw_visual(drawable, x, y).
 *
 * This one is just used for overload resolution of draw_visual.
 * \param map The map to call the method on.
 * \param drawable The visual object to draw to the map.
 * \param x The horizontal position to draw the object.
 * \param y The vertical position to draw the object.
 */
static void draw_visual(Map & map, Drawable & drawable, int x, int y) {
  map.draw_visual(drawable, x, y);
}

/**
 * \brief Implementation of map:draw_sprite(sprite, x, y).
 * \param map The map to call the method on.
 * \param drawable The visual object to draw to the map.
 * \param x The horizontal position to draw the object.
 * \param y The vertical position to draw the object.
 */
static void draw_sprite(LuaContext & context,
    Map & map, Sprite & sprite, int x, int y) {
  context.warning_deprecated(
    { 1, 5 },
    "map:draw_sprite()",
    "Use map:draw_visual() instead.");
  map.draw_visual(sprite, x, y);
}

/**
 * \brief Implementation of map:get_crystal_state()
 * \param map The map to call the method on.
 * \return The crystal state.
 */
static bool get_crystal_state(Map & map) {
  return map.get_game().get_crystal_state();
}

/**
 * \brief Implementation of map:set_crystal_state(state)
 * \param map The map to call the method on.
 * \param state The crystal state.
 */
static void set_crystal_state(Map & map, bool state) {
  Game& game = map.get_game();
  if (game.get_crystal_state() != state) {
    game.change_crystal_state();
  }
}

/**
 * \brief Implementation of map:change_crystal_state()
 * \param map The map to call the method on.
 */
static void change_crystal_state(Map & map) {
  map.get_game().change_crystal_state();
}

/**
 * \brief Implementation of map:open_doors(prefix)
 * \param context The context this Lua call was made in.
 * \param map The map to call the method on.
 * \param prefix Prefix of the names of the doors to open.
 */
static void open_doors(Map& map, const std::string& prefix) {
  if (!map.is_loaded()) {
    return;
  }
  Entities& entities = map.get_entities();
  std::set<std::string> sound_ids;
  const std::vector<EntityPtr>& doors =
      entities.get_entities_with_prefix(EntityType::DOOR, prefix);
  for (const EntityPtr& entity: doors) {
    Door& door = *std::static_pointer_cast<Door>(entity);
    if (!door.is_open() && !door.is_opening()) {
      door.open();
      sound_ids.insert(door.get_opening_sound_id());
    }
  }

  // plays every unique sound from all doors matching given name prefix
  for (const std::string& sound_id : sound_ids) {
    Sound::play(sound_id);
  }
}

/**
 * \brief Implementation of map:close_doors(prefix)
 * \param context The context this Lua call was made in.
 * \param map The map to call the method on.
 * \param prefix Prefix of the names of the doors to close.
 */
static void close_doors(Map& map, const std::string& prefix) {
  if (!map.is_loaded()) {
    return;
  }
  Entities& entities = map.get_entities();
  std::set<std::string> sound_ids;
  const std::vector<EntityPtr>& doors = entities.get_entities_with_prefix(EntityType::DOOR, prefix);
  for (const EntityPtr& entity: doors) {
    Door& door = *std::static_pointer_cast<Door>(entity);
    if (!door.is_closed() && !door.is_closing()) {
      door.close();
      sound_ids.insert(door.get_closing_sound_id());
    }
  }

  // plays every unique sound from all doors matching given name prefix
  for (const std::string& sound_id : sound_ids) {
    Sound::play(sound_id);
  }
}

/**
 * \brief Implementation of map:set_doors_open(prefix, [open]).
 * \param map The map to call the method on.
 * \param prefix Prefix of the doors to set.
 * \param open_arg True to open the door, false to close them (default is true).
 */
static void set_doors_open(
    Map& map, const std::string& prefix, std::optional<bool> open_arg) {
  if (!map.is_loaded()) {
    return;
  }
  bool open = open_arg.value_or(true);
  Entities& entities = map.get_entities();
  const std::vector<EntityPtr>& doors = entities.get_entities_with_prefix(EntityType::DOOR, prefix);
  for (const EntityPtr& entity: doors) {
    Door& door = *std::static_pointer_cast<Door>(entity);
    door.set_open(open);
  }
}

/**
 * \brief Implementation of map:get_entity(name).
 * \param map The map to call the method on.
 * \param name The map entity name to look-up.
 * \return Pointer to the entity if it was found, nullptr otherwise.
 */
static Entity* get_entity(Map& map, const std::string& name) {

  if (!map.is_loaded()) {
    return nullptr;
  }
  const EntityPtr& entity = map.get_entities().find_entity(name);
  if (entity != nullptr && !entity->is_being_removed()) {
    return entity.get();
  }
  return nullptr;
}

/**
 * \brief Implementation of map:has_entity(name).
 * \param map The map to call the method on.
 * \param name The map entity name to look-up.
 * \return True if the entity was found, nullptr otherwise.
 */
static bool has_entity(Map& map, const std::string& name) {
  if (!map.is_loaded()) {
    return false;
  }
  const EntityPtr& entity = map.get_entities().find_entity(name);
  return (entity != nullptr);
}

/**
 * \brief Implementation of map:get_entities([prefix]).
 * \param map The map to call the method on.
 * \param prefix_arg Prefix of the names of the entities to get.
 *   Empty optional is the same as an empty string.
 * \return Number of values to return to Lua.
 */
static LuaBind::OnStack get_entities(lua_State * l,
    Map& map, std::optional<std::string> prefix_arg) {

  std::string prefix = prefix_arg.value_or("");
  EntityVector entities;
  if (map.is_loaded()) {
    entities = map.get_entities().get_entities_with_prefix_z_sorted(prefix);
  }
  LuaContext::push_userdata_iterator(l, entities);
  return {1};
}

/**
 * \brief Implementation of map:get_entities_count(prefix).
 * \param map The map to call the method on.
 * \param prefix Prefix of the names of the entities to count.
 * \return Number of matching entities found.
 */
static int get_entities_count(Map& map, const std::string& prefix) {
  if (!map.is_loaded()) {
    return 0;
  }
  const EntityVector& entities =
      map.get_entities().get_entities_with_prefix(prefix);
  return static_cast<int>(entities.size());
}

/**
 * \brief Implementation of map:get_entities_count(prefix).
 * \param map The map to call the method on.
 * \param prefix Prefix of the names of the entities to check for.
 * \return True if a matching entity was found, false otherwise.
 */
static bool has_entities(Map& map, const std::string& prefix) {
  if (!map.is_loaded()) {
    return false;
  }
  return map.get_entities().has_entity_with_prefix(prefix);
}

/**
 * \brief Implementation of map:get_entities_by_type(type).
 * \param l The Lua state that called this method.
 * \param map The map to call the method on.
 * \param type The type of entity to get.
 * \return Number of values to return to Lua.
 */
static LuaBind::OnStack get_entities_by_type(lua_State* l,
    Map& map, EntityType type) {

  EntityVector entities;
  if (map.is_loaded()) {
    entities = map.get_entities().get_entities_by_type_z_sorted(type);
  }

  LuaContext::push_userdata_iterator(l, entities);
  return {1};
}

/**
 * \brief Implementation of map:get_entities_in_rectangle(x, y, width, height).
 * \param l The Lua state that called this method.
 * \param map The map to call the method on.
 * \param x The horizontal position of the rectangle's upper-left corner.
 * \param y The vertical position of the rectangle's upper-left corner.
 * \param width Width of the rectangle.
 * \param height Height of the rectangle.
 * \return Number of values to return to Lua.
 */
static LuaBind::OnStack get_entities_in_rectangle(lua_State* l,
    Map& map, int x, int y, int width, int height) {
  EntityVector entities;
  if (map.is_loaded()) {
    map.get_entities().get_entities_in_rectangle_z_sorted(
      Rectangle(x, y, width, height), entities
    );
  }

  LuaContext::push_userdata_iterator(l, entities);
  return {1};
}

/**
 * \brief Implementation of map:get_entities_in_region(...).
 * \param l The Lua state that called this method.
 * \param map The map to call the method on.
 * \return Number of values to return to Lua.
 */
static LuaBind::OnStack get_entities_in_region(lua_State* l, Map& map) {
  Point xy;
  EntityPtr entity;
  if (lua_isnumber(l, 2)) {
    int x = LuaTools::check_int(l, 2);
    int y = LuaTools::check_int(l, 3);
    xy = Point(x, y);
  }
  else if (LuaContext::is_entity(l, 2)) {
    entity = LuaContext::check_entity(l, 2);
    xy = entity->get_xy();
  }
  else {
    LuaTools::type_error(l, 2, "entity or number");
  }

  EntityVector entities;
  if (map.is_loaded()) {
    map.get_entities().get_entities_in_region_z_sorted(
        xy, entities
    );
  }

  if (entity != nullptr) {
    // Entity variant: remove the entity itself.
    const auto& it = std::find(entities.begin(), entities.end(), entity);
    if (it != entities.end()) {
      entities.erase(it);
    }
  }

  LuaContext::push_userdata_iterator(l, entities);
  return {1};
}

/**
 * \brief Implementation of map:get_hero().
 * \param l The Lua state that called this method.
 * \param map The map to call the method on.
 * \return Reference to the hero.
 */
static Hero& get_hero(lua_State* l, Map& map) {
  LuaContext::check_map_has_game(l, map);

  // Return the hero even if it is no longer on this map.
  return map.get_default_hero();
}

/**
 * \brief Implementation of map:set_entities_enabled(prefix, [enabled]).
 * \param map The map to call the method on.
 * \param prefix Prefix of the name of entities to enable/disable.
 * \param enabled_arg Should the entities be enabled or disabled.
 *   Default value is true.
 */
static void set_entities_enabled(
    Map& map, const std::string& prefix, std::optional<bool> enabled_arg) {
  bool enabled = enabled_arg.value_or(true);

  std::vector<EntityPtr> entities;
  if (map.is_loaded()) {
    entities = map.get_entities().get_entities_with_prefix(prefix);
  }
  for (const EntityPtr& entity: entities) {
    entity->set_enabled(enabled);
  }
}

/**
 * \brief Implementation of map:remove_entities(prefix).
 * \param map The map to call the method on.
 * \param prefix Prefix of the name of entities to remove.
 */
static void remove_entities(Map& map, const std::string& prefix) {
  if (map.is_loaded()) {
    map.get_entities().remove_entities_with_prefix(prefix);
  }
}

/**
 * \brief Implementation of all entiy creation functions map:create_*.
 * \param context The context this Lua call was made in.
 * \param map The map to call the method on.
 * \return Number of values to return to Lua.
 */
static LuaBind::OnStack create_entity(LuaContext& context, Map& map) {
  lua_State* l = context.get_internal_state();
  if (!map.is_loaded()) {
    LuaTools::arg_error(l, 1, "This map is not loaded");
  }
  EntityType type = LuaTools::check_enum<EntityType>(
      l, lua_upvalueindex(1)
  );
  const EntityData& data = EntityData::check_entity_data(l, 2, type);

  context.create_map_entity_from_data_to_lua(map, data);
  return {1};
}

/**
 * \brief Implementation of map:get_cameras().
 * \param l The Lua state that called this method.
 * \param map The map to call the method on.
 * \return Number of values to return to Lua.
 */
static LuaBind::OnStack get_cameras(lua_State* l, Map& map) {
  Cameras cameras;
  if (map.is_loaded()) {
    cameras = map.get_entities().get_cameras();
  }
  LuaContext::push_userdata_iterator(l, cameras);
  return {1};
}

/**
 * \brief Implementation of map:get_heroes().
 * \param l The Lua state that called this method.
 * \param map The map to call the method on.
 * \return Number of values to return to Lua.
 */
static LuaBind::OnStack get_heroes(lua_State* l, Map& map) {
  Heroes heroes = map.get_entities().get_heroes();
  if (map.is_loaded()) {
    LuaContext::push_userdata_iterator(l, heroes);
  }
  return {1};
}

/**
 * \brief Initializes the map features provided to Lua.
 */
void LuaContext::register_map_module() {

  std::vector<luaL_Reg> methods = {
      { "get_id", LUA_TO_C_BIND(&Map::get_id) },
      { "get_game", LUA_TO_C_BIND(get_game) },
      { "get_world", LUA_TO_C_BIND(get_world) },
      { "set_world", LUA_TO_C_BIND(set_world) },
      { "get_min_layer", LUA_TO_C_BIND(&Map::get_min_layer) },
      { "get_max_layer", LUA_TO_C_BIND(&Map::get_max_layer) },
      { "get_size", LUA_TO_C_BIND(get_size) },
      { "get_location", LUA_TO_C_BIND(get_location) },
      { "get_floor", LUA_TO_C_BIND(get_floor) },
      { "set_floor", LUA_TO_C_BIND(set_floor) },
      { "get_tileset", LUA_TO_C_BIND(&Map::get_tileset_id) },
      { "set_tileset", LUA_TO_C_BIND(&Map::set_tileset) },
      { "get_music", LUA_TO_C_BIND(get_music) },
      { "get_camera", LUA_TO_C_BIND(get_camera) },
      { "get_camera_position", LUA_TO_C_BIND(get_camera_position) },
      { "move_camera", LUA_TO_C_BIND(move_camera) },
      { "get_ground", LUA_TO_C_BIND(get_ground) },
      { "draw_visual", LUA_TO_C_BIND(draw_visual) },
      { "draw_sprite", LUA_TO_C_BIND(draw_sprite) },
      { "get_crystal_state", LUA_TO_C_BIND(get_crystal_state) },
      { "set_crystal_state", LUA_TO_C_BIND(set_crystal_state) },
      { "change_crystal_state", LUA_TO_C_BIND(change_crystal_state) },
      { "open_doors", LUA_TO_C_BIND(open_doors) },
      { "close_doors", LUA_TO_C_BIND(close_doors) },
      { "set_doors_open", LUA_TO_C_BIND(set_doors_open) },
      { "get_entity", LUA_TO_C_BIND(get_entity) },
      { "has_entity", LUA_TO_C_BIND(has_entity) },
      { "get_entities", LUA_TO_C_BIND(get_entities) },
      { "get_entities_count", LUA_TO_C_BIND(get_entities_count) },
      { "has_entities", LUA_TO_C_BIND(has_entities) },
      { "get_entities_by_type", LUA_TO_C_BIND(get_entities_by_type) },
      { "get_entities_in_rectangle", LUA_TO_C_BIND(get_entities_in_rectangle) },
      { "get_entities_in_region", LUA_TO_C_BIND(get_entities_in_region) },
      { "get_hero", LUA_TO_C_BIND(get_hero) },
      { "set_entities_enabled", LUA_TO_C_BIND(set_entities_enabled) },
      { "remove_entities", LUA_TO_C_BIND(remove_entities) },
  };

  if (CurrentQuest::is_format_at_least({ 2, 0 })) {
    methods.insert(methods.end(), {
      // 2.0 features
      { "get_cameras", LUA_TO_C_BIND(get_cameras) },
      { "get_heroes", LUA_TO_C_BIND(get_heroes) },
    });
  }

  const std::vector<luaL_Reg> metamethods = {
      { "__gc", userdata_meta_gc },
      { "__newindex", userdata_meta_newindex_as_table },
      { "__index", userdata_meta_index_as_table },
  };

  register_type(map_module_name, {}, methods, metamethods);

  // Add map:create_* functions as closures because we pass the entity type as upvalue.
  luaL_getmetatable(current_l, map_module_name.c_str());
  for (const auto& kvp : EnumInfoTraits<EntityType>::names) {
    EntityType type = kvp.first;
    const std::string& type_name = kvp.second;
    if (!EntityTypeInfo::can_be_created_from_lua_api(type)) {
      continue;
    }
    std::string function_name = "create_" + type_name;
    push_string(current_l, type_name);
    lua_pushcclosure(current_l, LUA_TO_C_BIND(create_entity), 1);
    lua_setfield(current_l, -2, function_name.c_str());
  }

  // Add a Lua implementation of the deprecated map:move_camera() function.
  int result = luaL_loadstring(current_l, move_camera_code);
  if (result != 0) {
    Debug::error(std::string("Failed to initialize map:move_camera(): ") + lua_tostring(current_l, -1));
    lua_pop(current_l, 1);
  }
  else {
    SOLARUS_ASSERT(lua_isfunction(current_l, -1), "map:move_camera() is not a function");
    lua_setfield(current_l, LUA_REGISTRYINDEX, "map.move_camera");
  }
}

/**
 * \brief Returns whether a value is a userdata of type map.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return true if the value at this index is a map.
 */
bool LuaContext::is_map(lua_State* l, int index) {
  return is_userdata(l, index, map_module_name);
}

/**
 * \brief Checks that the userdata at the specified index of the stack is a
 * map that is currently loaded and returns it.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return The map.
 */
std::shared_ptr<Map> LuaContext::check_map(lua_State* l, int index) {

  return std::static_pointer_cast<Map>(check_userdata(
      l, index, map_module_name
  ));
}

/**
 * \brief Pushes a map userdata onto the stack.
 * \param l A Lua context.
 * \param game A game.
 */
void LuaContext::push_map(lua_State* l, Map& map) {

  push_userdata(l, map);
}

namespace {

/**
 * \brief Checks and returns the layer of an entity to be created.
 *
 * The layer is assumed to be specified in a field "layer".
 * Throws a LuaException if the layer is invalid for the map.
 *
 * \param l A Lua state.
 * \param index Index of the argument in the Lua stack.
 * \param entity_data Description of the entity to create.
 * \param map The map where to create the entity.
 * \return The layer.
 */
int entity_creation_check_layer(
    lua_State* l,
    int index,
    const EntityData& entity_data,
    const Map& map) {

  const int layer = entity_data.get_layer();

  if (!map.is_valid_layer(layer)) {
    std::ostringstream oss;
    oss << "Invalid layer: " << layer;
    LuaTools::arg_error(l, index, oss.str());
  }

  return layer;
}

/**
 * \brief Checks and returns the size of an entity to be created.
 *
 * The size is assumed to be specified in fields "width" and "height".
 * Throws a LuaException if the size is invalid.
 *
 * \param l A Lua state.
 * \param index Index of the argument in the Lua stack.
 * \param entity_data Description of the entity to create.
 * \return The size.
 */
Size entity_creation_check_size(
    lua_State* l,
    int index,
    const EntityData& entity_data) {

  const Size size = {
      entity_data.get_integer("width"),
      entity_data.get_integer("height")
  };

  if (size.width < 0 || size.width % 8 != 0) {
    std::ostringstream oss;
    oss << "Invalid width " << size.width << ": should be a positive multiple of 8";
    LuaTools::arg_error(l, index, oss.str());
  }
  if (size.height < 0 || size.height % 8 != 0) {
    std::ostringstream oss;
    oss << "Invalid height " << size.height << ": should be a positive multiple of 8";
    LuaTools::arg_error(l, index, oss.str());
  }

  return size;
}

/**
 * \brief Checks and returns a mandatory savegame variable field for an
 * entity to be created.
 *
 * Throws a LuaException if the savegame variable name is invalid or empty.
 *
 * \param l A Lua state.
 * \param index Index of the argument in the Lua stack.
 * \param entity_data Description of the entity to create.
 * \param field_name Name of the savegame variable field to check.
 * \return The savegame variable name.
 */
std::string entity_creation_check_savegame_variable_mandatory(
    lua_State* l,
    int index,
    const EntityData& entity_data,
    const std::string& field_name
) {

  const std::string& savegame_variable = entity_data.get_string(field_name);
  
  if (!LuaTools::is_valid_lua_identifier(savegame_variable)) {
    LuaTools::field_error(l, index, field_name,
        "invalid savegame variable identifier: '" + savegame_variable + "'");
  }

  return savegame_variable;
}

/**
 * \brief Checks and returns an optional savegame variable field for an
 * entity to be created.
 *
 * Throws a LuaException if the savegame variable name is invalid.
 *
 * \param l A Lua state.
 * \param index Index of the argument in the Lua stack.
 * \param entity_data Description of the entity to create.
 * \param field_name Name of the savegame variable field to check.
 * \return The savegame variable name.
 * An empty string means no savegame variable.
 */
std::string entity_creation_check_savegame_variable_optional(
    lua_State* l,
    int index,
    const EntityData& entity_data,
    const std::string& field_name
) {
  const std::string& savegame_variable = entity_data.get_string(field_name);

  if (savegame_variable.empty()) {
    return savegame_variable;
  }

  return entity_creation_check_savegame_variable_mandatory(l, index, entity_data, field_name);
}

/**
 * \brief Checks an enum field for an entity to be created.
 *
 * Throws a LuaException if the value is not a string or if the string is not
 * a valid name for the enum.
 * This is a useful function for mapping strings to C or C++ enums.
 *
 * \param l A Lua state.
 * \param index Index of the argument in the Lua stack.
 * \param entity_data Description of the entity to
 * \param field_name Name of the field to check.
 * \param names A mapping of enum values to strings to search in.
 * \return The enumerated value corresponding to this string.
 */
template<typename E>
E entity_creation_check_enum(
    lua_State* l,
    int index,
    const EntityData& entity_data,
    const std::string& field_name,
    const std::map<E, std::string>& names
) {
  const std::string& name = entity_data.get_string(field_name);
  for (const auto& kvp: names) {
    if (kvp.second == name) {
      return kvp.first;
    }
  }

  LuaTools::field_error(l, index, field_name,
      LuaTools::check_enum_error_message(name, names));
}

template<typename E>
E entity_creation_check_enum(
    lua_State* l,
    int index,
    const EntityData& entity_data,
    const std::string& field_name
) {
  return entity_creation_check_enum<E>(l, index, entity_data, field_name, EnumInfoTraits<E>::names);
}

}

/**
 * \brief Creates a tile on the map.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_create_tile(lua_State* l) {

  return state_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    if (!map.is_loaded() && !map.is_loading()) {
      LuaTools::arg_error(l, 1, "Cannot create tile: this map is not initialized");
    }
    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));
    const int layer = entity_creation_check_layer(l, 1, data, map);
    const int x = data.get_xy().x;
    const int y = data.get_xy().y;
    const Size size =  entity_creation_check_size(l, 1, data);
    const std::string& tile_pattern_id = data.get_string("pattern");
    std::string tileset_id = data.get_string("tileset");

    bool use_map_tileset = tileset_id.empty();
    if (use_map_tileset) {
      tileset_id = map.get_tileset_id();
    }
    ResourceProvider& resource_provider = map.get_game().get_resource_provider();
    const Tileset& tileset = resource_provider.get_tileset(tileset_id);
    std::shared_ptr<TilePattern> pattern = tileset.get_tile_pattern(tile_pattern_id);
    if (pattern == nullptr) {
      LuaTools::error(l, "No such pattern in tileset '" + tileset_id + "': '" + tile_pattern_id + "'");
    }
    const Size& pattern_size = pattern->get_size();
    Entities& entities = map.get_entities();

    // If the tile is big, divide it in several smaller tiles so that
    // most of them can still be optimized away.
    // Otherwise, tiles expanded in big rectangles like a lake or a dungeon
    // floor would be entirely redrawn at each frame when just one small
    // animated tile overlaps them.

    TileInfo tile_info;
    tile_info.layer = layer;
    tile_info.box = { Point(), pattern_size };
    tile_info.pattern_id = tile_pattern_id;
    tile_info.pattern = pattern;

    if (!use_map_tileset) {
      tile_info.tileset = &tileset;
    }

    for (int current_y = y; current_y < y + size.height; current_y += pattern->get_height()) {
      for (int current_x = x; current_x < x + size.width; current_x += pattern->get_width()) {
        tile_info.box.set_xy(current_x, current_y);
        // The tile will actually be created only if it cannot be optimized away.
        entities.add_tile_info(
            tile_info
        );
      }
    }

    return 0;
  });
}

/**
 * \brief Creates a destination on the map.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_create_destination(lua_State* l) {

  return state_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    if (!map.is_loaded() && !map.is_loading()) {
      LuaTools::arg_error(l, 1, "Cannot create destination: this map is not loaded");
    }
    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));

    std::shared_ptr<Destination> entity = std::make_shared<Destination>(
        data.get_name(),
        entity_creation_check_layer(l, 1, data, map),
        data.get_xy(),
        data.get_integer("direction"),
        data.get_string("sprite"),
        data.get_boolean("default")
    );
    StartingLocationMode starting_location_mode =
        entity_creation_check_enum<StartingLocationMode>(l, 1, data, "starting_location_mode");
    entity->set_starting_location_mode(starting_location_mode);
    entity->set_user_properties(data.get_user_properties());
    entity->set_enabled(data.is_enabled_at_start());
    map.get_entities().add_entity(entity);

    if (map.is_started()) {
      push_entity(l, *entity);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Creates a teletransporter on the map.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_create_teletransporter(lua_State* l) {

  return state_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    if (!map.is_loaded() && !map.is_loading()) {
      LuaTools::arg_error(l, 1, "Cannot create teletransporter: this map is not loaded");
    }
    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));

    EntityPtr entity = std::make_shared<Teletransporter>(
        data.get_name(),
        entity_creation_check_layer(l, 1, data, map),
        data.get_xy(),
        entity_creation_check_size(l, 1, data),
        data.get_string("sprite"),
        data.get_string("sound"),
        entity_creation_check_enum<Transition::Style>(l, 1, data, "transition"),
        data.get_string("destination_map"),
        data.get_string("destination")
    );
    entity->set_user_properties(data.get_user_properties());
    entity->set_enabled(data.is_enabled_at_start());
    map.get_entities().add_entity(entity);

    if (map.is_started()) {
      push_entity(l, *entity);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Creates a pickable treasure on the map.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_create_pickable(lua_State* l) {

  return state_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    if (!map.is_loaded() && !map.is_loading()) {
      LuaTools::arg_error(l, 1, "Cannot create pickable: this map is not loaded");
    }
    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));

    Game& game = map.get_game();
    bool force_persistent = false;
    FallingHeight falling_height = FALLING_MEDIUM;
    if (!map.is_loaded()) {
      // Different behavior when the pickable is already placed on the map.
      falling_height = FALLING_NONE;
      force_persistent = true;
    }

    const std::shared_ptr<Pickable>& entity = Pickable::create(
        game,
        data.get_name(),
        entity_creation_check_layer(l, 1, data, map),
        data.get_xy(),
        Treasure(
            data.get_string("treasure_name"),
            data.get_integer("treasure_variant"),
            entity_creation_check_savegame_variable_optional(l, 1, data, "treasure_savegame_variable")
        ),
        falling_height,
        force_persistent
    );

    if (entity == nullptr) {
      lua_pushnil(l);
      return 1;
    }

    entity->set_user_properties(data.get_user_properties());
    entity->set_enabled(data.is_enabled_at_start());
    map.get_entities().add_entity(entity);

    if (map.is_started()) {
      push_entity(l, *entity);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Creates a destructible object on the map.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_create_destructible(lua_State* l) {

  return state_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    if (!map.is_loaded() && !map.is_loading()) {
      LuaTools::arg_error(l, 1, "Cannot create destructible: this map is not loaded");
    }
    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));

    std::shared_ptr<Destructible> destructible = std::make_shared<Destructible>(
        data.get_name(),
        entity_creation_check_layer(l, 1, data, map),
        data.get_xy(),
        data.get_string("sprite"),
        Treasure(
            data.get_string("treasure_name"),
            data.get_integer("treasure_variant"),
            entity_creation_check_savegame_variable_optional(l, 1, data, "treasure_savegame_variable")
        ),
        entity_creation_check_enum<Ground>(l, 1, data, "ground")
    );
    destructible->set_destruction_sound(data.get_string("destruction_sound"));
    destructible->set_weight(data.get_integer("weight"));
    destructible->set_can_be_cut(data.get_boolean("can_be_cut"));
    destructible->set_can_explode(data.get_boolean("can_explode"));
    destructible->set_can_regenerate(data.get_boolean("can_regenerate"));
    destructible->set_damage_on_enemies(data.get_integer("damage_on_enemies"));
    destructible->set_user_properties(data.get_user_properties());
    destructible->set_enabled(data.is_enabled_at_start());
    map.get_entities().add_entity(destructible);
    if (map.is_started()) {
      push_entity(l, *destructible);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Creates a chest on the map.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_create_chest(lua_State* l) {

  return state_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    if (!map.is_loaded() && !map.is_loading()) {
      LuaTools::arg_error(l, 1, "Cannot create chest: this map is not loaded");
    }
    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));

    Chest::OpeningMethod opening_method = entity_creation_check_enum<Chest::OpeningMethod>(
        l,
        1,
        data,
        "opening_method",
        Chest::opening_method_names
    );

    // Check the value of opening_condition depending on the opening method.
    Game& game = map.get_game();
    const std::string& opening_condition = data.get_string("opening_condition");
    if (opening_method == Chest::OpeningMethod::BY_INTERACTION_IF_SAVEGAME_VARIABLE) {
      entity_creation_check_savegame_variable_mandatory(l, 1, data, "opening_condition");
    }
    else if (opening_method == Chest::OpeningMethod::BY_INTERACTION_IF_ITEM) {
      if (!game.get_equipment().item_exists(opening_condition)) {
        LuaTools::field_error(l, 1, "opening_condition",
            "no such equipment item: '" + opening_condition + "'");
      }
      EquipmentItem& item = game.get_equipment().get_item(opening_condition);
      if (!item.is_saved()) {
        LuaTools::field_error(l, 1, "opening_condition",
            "equipment item '" + opening_condition + "' is not saved");
      }
    }

    std::shared_ptr<Chest> chest = std::make_shared<Chest>(
        map.get_game(),
        data.get_name(),
        entity_creation_check_layer(l, 1, data, map),
        data.get_xy(),
        data.get_string("sprite"),
        Treasure(
            data.get_string("treasure_name"),
            data.get_integer("treasure_variant"),
            entity_creation_check_savegame_variable_optional(l, 1, data, "treasure_savegame_variable")
        )
    );
    chest->set_opening_method(opening_method);
    chest->set_opening_condition(opening_condition);
    chest->set_opening_condition_consumed(data.get_boolean("opening_condition_consumed"));
    chest->set_cannot_open_dialog_id(data.get_string("cannot_open_dialog"));
    chest->set_user_properties(data.get_user_properties());
    chest->set_enabled(data.is_enabled_at_start());
    map.get_entities().add_entity(chest);
    if (map.is_started()) {
      push_entity(l, *chest);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Creates a jumper on the map.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_create_jumper(lua_State* l) {

  return state_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    if (!map.is_loaded() && !map.is_loading()) {
      LuaTools::arg_error(l, 1, "Cannot create jumper: this map is not loaded");
    }
    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));

    EntityPtr entity = std::make_shared<Jumper>(
        data.get_name(),
        entity_creation_check_layer(l, 1, data, map),
        data.get_xy(),
        entity_creation_check_size(l, 1, data),
        data.get_integer("direction"),
        data.get_integer("jump_length")
    );
    entity->set_user_properties(data.get_user_properties());
    entity->set_enabled(data.is_enabled_at_start());
    map.get_entities().add_entity(entity);
    if (map.is_started()) {
      push_entity(l, *entity);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Creates an enemy on the map.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_create_enemy(lua_State* l) {

  return state_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    if (!map.is_loaded() && !map.is_loading()) {
      LuaTools::arg_error(l, 1, "Cannot create enemy: this map is not loaded");
    }
    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));

    const std::string& breed = data.get_string("breed");
    const QuestDatabase& database = CurrentQuest::get_database();
    if (!database.resource_exists(ResourceType::ENEMY, breed)) {
      LuaTools::arg_error(l, 2, "No such enemy breed: '" + breed + "'");
    }

    Game& game = map.get_game();
    EntityPtr entity = Enemy::create(
        game,
        breed,
        entity_creation_check_savegame_variable_optional(l, 1, data, "savegame_variable"),
        data.get_name(),
        entity_creation_check_layer(l, 1, data, map),
        data.get_xy(),
        data.get_integer("direction"),
        Treasure(
            data.get_string("treasure_name"),
            data.get_integer("treasure_variant"),
            entity_creation_check_savegame_variable_optional(l, 1, data, "treasure_savegame_variable")
        )
    );
    if (entity == nullptr) {
      lua_pushnil(l);
      return 1;
    }
    entity->set_user_properties(data.get_user_properties());
    entity->set_enabled(data.is_enabled_at_start());
    map.get_entities().add_entity(entity);
    if (map.is_started()) {
      push_entity(l, *entity);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Creates an NPC on the map.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_create_npc(lua_State* l) {

  return state_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    if (!map.is_loaded() && !map.is_loading()) {
      LuaTools::arg_error(l, 1, "Cannot create NPC: this map is not loaded");
    }
    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));

    Game& game = map.get_game();
    EntityPtr entity = std::make_shared<Npc>(
        game,
        data.get_name(),
        entity_creation_check_layer(l, 1, data, map),
        data.get_xy(),
        Npc::Subtype(data.get_integer("subtype")),
        data.get_string("sprite"),
        data.get_integer("direction"),
        data.get_string("behavior")
    );
    entity->set_user_properties(data.get_user_properties());
    entity->set_enabled(data.is_enabled_at_start());
    map.get_entities().add_entity(entity);
    if (map.is_started()) {
      push_entity(l, *entity);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Creates a block on the map.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_create_block(lua_State* l) {

  return state_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    if (!map.is_loaded() && !map.is_loading()) {
      LuaTools::arg_error(l, 1, "Cannot create block: this map is not loaded");
    }
    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));

    int max_moves = data.get_integer("max_moves");
    if (max_moves < -1) {  // -1 means unset here.
      std::ostringstream oss;
      oss << "Invalid max_moves (should be 0, positive or nil): " << max_moves;
      LuaTools::arg_error(l, 1, oss.str());
    }

    // Deprecated version.
    int maximum_moves = data.get_integer("maximum_moves");
    if (maximum_moves != -1) {
      if (maximum_moves < 0 || maximum_moves > 2) {
        std::ostringstream oss;
        oss << "Invalid maximum_moves (should be 0, 1 or 2): " << maximum_moves;
        LuaTools::arg_error(l, 1, oss.str());
      }
      if (max_moves != -1) {
        LuaTools::arg_error(l, 1, "Only one of max_moves and maximum_moves can be set");
      }
      if (maximum_moves == 2) {
        // Replace the old special value 2 by the new proper value to mean unlimited.
        maximum_moves = -1;
      }
      max_moves = maximum_moves;
    }

    std::shared_ptr<Block> entity = std::make_shared<Block>(
        data.get_name(),
        entity_creation_check_layer(l, 1, data, map),
        data.get_xy(),
        data.get_integer("direction"),
        data.get_string("sprite"),
        data.get_boolean("pushable"),
        data.get_boolean("pullable"),
        max_moves
    );
    entity->set_user_properties(data.get_user_properties());
    entity->set_enabled(data.is_enabled_at_start());
    map.get_entities().add_entity(entity);
    if (map.is_started()) {
      push_entity(l, *entity);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Creates a dynamic tile on the map.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_create_dynamic_tile(lua_State* l) {

  return state_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    if (!map.is_loaded() && !map.is_loading()) {
      LuaTools::arg_error(l, 1, "Cannot create dynamic tile: this map is not loaded");
    }
    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));

    const std::string& pattern_id = data.get_string("pattern");
    const std::string& tileset_id = data.get_string("tileset");
    const Tileset* tileset = nullptr;
    std::shared_ptr<TilePattern> pattern = nullptr;
    if (!tileset_id.empty()) {
      ResourceProvider& resource_provider = map.get_game().get_resource_provider();
      tileset = &resource_provider.get_tileset(tileset_id);
      pattern = tileset->get_tile_pattern(pattern_id);
      if (pattern == nullptr) {
        LuaTools::error(l, "No such pattern in tileset '" + tileset_id + "': '" + pattern_id + "'");
      }
    } else {
      // Use the tileset of the map.
      pattern = map.get_tileset().get_tile_pattern(pattern_id);
      if (pattern == nullptr) {
        LuaTools::error(l, "No such pattern in tileset '" + map.get_tileset_id() + "': '" + pattern_id + "'");
      }
    }

    EntityPtr entity = std::make_shared<DynamicTile>(
        data.get_name(),
        entity_creation_check_layer(l, 1, data, map),
        data.get_xy(),
        entity_creation_check_size(l, 1, data),
        pattern_id,
        pattern,
        tileset
    );
    entity->set_user_properties(data.get_user_properties());
    entity->set_enabled(data.is_enabled_at_start());
    map.get_entities().add_entity(entity);
    if (map.is_started()) {
      push_entity(l, *entity);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Creates a switch on the map.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_create_switch(lua_State* l) {

  return state_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    if (!map.is_loaded() && !map.is_loading()) {
      LuaTools::arg_error(l, 1, "Cannot create switch: this map is not loaded");
    }
    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));

    EntityPtr entity = std::make_shared<Switch>(
        data.get_name(),
        entity_creation_check_layer(l, 1, data, map),
        data.get_xy(),
        entity_creation_check_enum<Switch::Subtype>(l, 1, data, "subtype", Switch::subtype_names),
        data.get_string("sprite"),
        data.get_string("sound"),
        data.get_boolean("needs_block"),
        data.get_boolean("inactivate_when_leaving")
    );
    entity->set_user_properties(data.get_user_properties());
    entity->set_enabled(data.is_enabled_at_start());
    map.get_entities().add_entity(entity);
    if (map.is_started()) {
      push_entity(l, *entity);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Creates a wall on the map.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_create_wall(lua_State* l) {

  return state_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    if (!map.is_loaded() && !map.is_loading()) {
      LuaTools::arg_error(l, 1, "Cannot create wall: this map is not loaded");
    }
    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));

    EntityPtr entity = std::make_shared<Wall>(
        data.get_name(),
        entity_creation_check_layer(l, 1, data, map),
        data.get_xy(),
        entity_creation_check_size(l, 1, data),
        data.get_boolean("stops_hero"),
        data.get_boolean("stops_enemies"),
        data.get_boolean("stops_npcs"),
        data.get_boolean("stops_blocks"),
        data.get_boolean("stops_projectiles")
    );
    entity->set_user_properties(data.get_user_properties());
    entity->set_enabled(data.is_enabled_at_start());
    map.get_entities().add_entity(entity);
    if (map.is_started()) {
      push_entity(l, *entity);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Creates a sensor on the map.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_create_sensor(lua_State* l) {

  return state_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    if (!map.is_loaded() && !map.is_loading()) {
      LuaTools::arg_error(l, 1, "Cannot create sensor: this map is not loaded");
    }
    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));

    EntityPtr entity = std::make_shared<Sensor>(
        data.get_name(),
        entity_creation_check_layer(l, 1, data, map),
        data.get_xy(),
        entity_creation_check_size(l, 1, data)
    );
    entity->set_user_properties(data.get_user_properties());
    entity->set_enabled(data.is_enabled_at_start());
    map.get_entities().add_entity(entity);
    if (map.is_started()) {
      push_entity(l, *entity);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Creates a crystal on the map.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_create_crystal(lua_State* l) {

  return state_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    if (!map.is_loaded() && !map.is_loading()) {
      LuaTools::arg_error(l, 1, "Cannot create crystal: this map is not loaded");
    }
    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));

    EntityPtr entity = std::make_shared<Crystal>(
        data.get_name(),
        entity_creation_check_layer(l, 1, data, map),
        data.get_xy()
    );
    entity->set_user_properties(data.get_user_properties());
    entity->set_enabled(data.is_enabled_at_start());
    map.get_entities().add_entity(entity);
    if (map.is_started()) {
      push_entity(l, *entity);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Creates a crystal block on the map.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_create_crystal_block(lua_State* l) {

  return state_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    if (!map.is_loaded() && !map.is_loading()) {
      LuaTools::arg_error(l, 1, "Cannot create crystal block: this map is not loaded");
    }
    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));

    Game& game = map.get_game();
    EntityPtr entity = std::make_shared<CrystalBlock>(
        game,
        data.get_name(),
        entity_creation_check_layer(l, 1, data, map),
        data.get_xy(),
        entity_creation_check_size(l, 1, data),
        CrystalBlock::Subtype(data.get_integer("subtype"))
    );
    entity->set_user_properties(data.get_user_properties());
    entity->set_enabled(data.is_enabled_at_start());
    map.get_entities().add_entity(entity);
    if (map.is_started()) {
      push_entity(l, *entity);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Creates a shop treasure on the map.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_create_shop_treasure(lua_State* l) {

  return state_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    if (!map.is_loaded() && !map.is_loading()) {
      LuaTools::arg_error(l, 1, "Cannot create shop treasure: this map is not loaded");
    }
    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));

    Game& game = map.get_game();
    EntityPtr entity = ShopTreasure::create(
        game,
        data.get_name(),
        entity_creation_check_layer(l, 1, data, map),
        data.get_xy(),
        Treasure(
            data.get_string("treasure_name"),
            data.get_integer("treasure_variant"),
            entity_creation_check_savegame_variable_optional(l, 1, data, "treasure_savegame_variable")
        ),
        data.get_integer("price"),
        data.get_string("font"),
        data.get_string("dialog")
    );
    if (entity == nullptr) {
      lua_pushnil(l);
      return 1;
    }

    entity->set_user_properties(data.get_user_properties());
    entity->set_enabled(data.is_enabled_at_start());
    map.get_entities().add_entity(entity);
    if (map.is_started()) {
      push_entity(l, *entity);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Creates a stream on the map.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_create_stream(lua_State* l) {

  return state_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    if (!map.is_loaded() && !map.is_loading()) {
      LuaTools::arg_error(l, 1, "Cannot create stream: this map is not loaded");
    }
    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));

    std::shared_ptr<Stream> stream = std::make_shared<Stream>(
        data.get_name(),
        entity_creation_check_layer(l, 1, data, map),
        data.get_xy(),
        data.get_integer("direction"),
        data.get_string("sprite")
    );
    stream->set_speed(data.get_integer("speed"));
    stream->set_allow_movement(data.get_boolean("allow_movement"));
    stream->set_allow_attack(data.get_boolean("allow_attack"));
    stream->set_allow_item(data.get_boolean("allow_item"));
    stream->set_user_properties(data.get_user_properties());
    stream->set_enabled(data.is_enabled_at_start());
    map.get_entities().add_entity(stream);
    if (map.is_started()) {
      push_stream(l, *stream);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Creates a door on the map.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_create_door(lua_State* l) {

  return state_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    if (!map.is_loaded() && !map.is_loading()) {
      LuaTools::arg_error(l, 1, "Cannot create door: this map is not loaded");
    }
    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));

    Door::OpeningMethod opening_method = entity_creation_check_enum<Door::OpeningMethod>(
        l,
        1,
        data,
        "opening_method",
        Door::opening_method_names
    );

    // Check the value of opening_condition depending on the opening method.
    Game& game = map.get_game();
    const std::string& opening_condition = data.get_string("opening_condition");
    if (opening_method == Door::OpeningMethod::BY_INTERACTION_IF_SAVEGAME_VARIABLE) {
      entity_creation_check_savegame_variable_mandatory(l, 1, data, "opening_condition");
    }
    else if (opening_method == Door::OpeningMethod::BY_INTERACTION_IF_ITEM) {
      if (!game.get_equipment().item_exists(opening_condition)) {
        LuaTools::field_error(l, 1, "opening_condition",
            "no such equipment item: '" + opening_condition + "'");
      }
      EquipmentItem& item = game.get_equipment().get_item(opening_condition);
      if (!item.is_saved()) {
        LuaTools::field_error(l, 1, "opening_condition",
            "equipment item '" + opening_condition + "' is not saved");
      }
    }
    std::shared_ptr<Door> door = std::make_shared<Door>(
        game,
        data.get_name(),
        entity_creation_check_layer(l, 1, data, map),
        data.get_xy(),
        data.get_integer("direction"),
        data.get_string("sprite"),
        entity_creation_check_savegame_variable_optional(l, 1, data, "savegame_variable")
    );
    door->set_opening_method(opening_method);
    door->set_opening_condition(opening_condition);
    door->set_opening_condition_consumed(data.get_boolean("opening_condition_consumed"));
    door->set_cannot_open_dialog_id(data.get_string("cannot_open_dialog"));
    door->set_user_properties(data.get_user_properties());
    door->set_enabled(data.is_enabled_at_start());
    map.get_entities().add_entity(door);
    if (map.is_started()) {
      push_entity(l, *door);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Creates stairs on the map.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_create_stairs(lua_State* l) {

  return state_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    if (!map.is_loaded() && !map.is_loading()) {
      LuaTools::arg_error(l, 1, "Cannot create stairs: this map is not loaded");
    }
    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));

    EntityPtr entity = std::make_shared<Stairs>(
        data.get_name(),
        entity_creation_check_layer(l, 1, data, map),
        data.get_xy(),
        data.get_integer("direction"),
        Stairs::Subtype(data.get_integer("subtype"))
    );
    entity->set_user_properties(data.get_user_properties());
    entity->set_enabled(data.is_enabled_at_start());
    map.get_entities().add_entity(entity);
    if (map.is_started()) {
      push_entity(l, *entity);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Creates a separator on the map.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_create_separator(lua_State* l) {

  return state_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    if (!map.is_loaded() && !map.is_loading()) {
      LuaTools::arg_error(l, 1, "Cannot create separator: this map is not loaded");
    }
    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));

    EntityPtr entity = std::make_shared<Separator>(
        data.get_name(),
        entity_creation_check_layer(l, 1, data, map),
        data.get_xy(),
        entity_creation_check_size(l, 1, data)
    );
    entity->set_user_properties(data.get_user_properties());
    entity->set_enabled(data.is_enabled_at_start());
    map.get_entities().add_entity(entity);
    if (map.is_started()) {
      push_entity(l, *entity);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Creates a custom entity on the map.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_create_custom_entity(lua_State* l) {

  return state_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    if (!map.is_loaded() && !map.is_loading()) {
      LuaTools::arg_error(l, 1, "Cannot create custom entity: this map is not loaded");
    }
    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));

    const std::string& model = data.get_string("model");
    const QuestDatabase& database = CurrentQuest::get_database();
    if (!model.empty() && !database.resource_exists(ResourceType::ENTITY, model)) {
      LuaTools::arg_error(l, 2, "No such custom entity model: '" + model + "'");
    }

    Game& game = map.get_game();
    EntityPtr entity = std::make_shared<CustomEntity>(
        game,
        data.get_name(),
        data.get_integer("direction"),
        entity_creation_check_layer(l, 1, data, map),
        data.get_xy(),
        entity_creation_check_size(l, 1, data),
        Point(data.get_integer("origin_x"), data.get_integer("origin_y")),
        data.get_string("sprite"),
        model
    );
    entity->set_tiled(data.get_boolean("tiled"));
    entity->set_user_properties(data.get_user_properties());
    entity->set_enabled(data.is_enabled_at_start());
    map.get_entities().add_entity(entity);
    if (map.is_started()) {
      push_entity(l, *entity);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Creates a bomb on the map.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_create_bomb(lua_State* l) {

  return state_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    if (!map.is_loaded() && !map.is_loading()) {
      LuaTools::arg_error(l, 1, "Cannot create bomb: this map is not loaded");
    }
    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));

    EntityPtr entity = std::make_shared<Bomb>(
        data.get_name(),
        entity_creation_check_layer(l, 1, data, map),
        data.get_xy()
    );
    entity->set_user_properties(data.get_user_properties());
    entity->set_enabled(data.is_enabled_at_start());
    map.get_entities().add_entity(entity);
    if (map.is_started()) {
      push_entity(l, *entity);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Creates an explosion on the map.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_create_explosion(lua_State* l) {

  return state_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    if (!map.is_loaded() && !map.is_loading()) {
      LuaTools::arg_error(l, 1, "Cannot create explosion: this map is not loaded");
    }
    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));

    const bool with_damage = true;
    EntityPtr entity = std::make_shared<Explosion>(
        data.get_name(),
        entity_creation_check_layer(l, 1, data, map),
        data.get_xy(),
        with_damage
    );
    entity->set_user_properties(data.get_user_properties());
    entity->set_enabled(data.is_enabled_at_start());
    map.get_entities().add_entity(entity);
    if (map.is_started()) {
      push_entity(l, *entity);
      return 1;
    }
    return 0;
  });
}

/**
 * @brief Creates a camera on the map
 * @param  l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::l_create_camera(lua_State* l) {
  return state_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));

    CameraPtr entity = std::make_shared<Camera>(
        data.get_name()
    );
    entity->set_xy(data.get_xy());
    entity->set_layer(data.get_layer());
    entity->set_user_properties(data.get_user_properties());
    entity->set_enabled(data.is_enabled_at_start());

    entity->place_on_map(map);
    if (map.is_started()) {
      push_entity(l, *entity);
      return 1;
    }
    return 0;
  });
}

/**
 * @brief Creates a hero on the map
 * @param  l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::l_create_hero(lua_State* l) {
  return state_boundary_handle(l, [&]{
    Map& map = *check_map(l, 1);
    Game& game = map.get_game();

    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));

    //Create new volatile savegame (no file name) to hold hero equipement
    SavegamePtr save = std::make_shared<Savegame>(game.get_savegame().get_main_loop(), "");
    // Save information is different but shares the same game
    save->set_game(&game);

    //Initialize the save and equipment in-place
    save->initialize();

    HeroPtr entity = std::make_shared<Hero>(
     save->get_equipment(), data.get_name()
    );

    ControlsPtr cmds = ControlsDispatcher::get().create_commands_from_keyboard();
    entity->set_controls(cmds);
    entity->start_free();
    entity->place_on_map(map);
    entity->set_xy(data.get_xy());

    entity->set_layer(data.get_layer());
    entity->set_user_properties(data.get_user_properties());
    entity->set_enabled(data.is_enabled_at_start());

    if (map.is_started()) {
      push_entity(l, *entity);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Creates a fire entity on the map.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_create_fire(lua_State* l) {

  return state_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));

    EntityPtr entity = std::make_shared<Fire>(
        data.get_name(),
        entity_creation_check_layer(l, 1, data, map),
        data.get_xy(),
        map.get_default_hero().shared_from_this_cast<Hero>() //In doubt, default hero //TODO allow to specify throwing hero
    );
    entity->set_user_properties(data.get_user_properties());
    entity->set_enabled(data.is_enabled_at_start());
    map.get_entities().add_entity(entity);
    if (map.is_started()) {
      push_entity(l, *entity);
      return 1;
    }
    return 0;
  });
}

const std::map<EntityType, lua_CFunction> LuaContext::entity_creation_functions = {
    { EntityType::TILE, LuaContext::l_create_tile },
    { EntityType::DESTINATION, LuaContext::l_create_destination },
    { EntityType::TELETRANSPORTER, LuaContext::l_create_teletransporter },
    { EntityType::PICKABLE, LuaContext::l_create_pickable },
    { EntityType::DESTRUCTIBLE, LuaContext::l_create_destructible },
    { EntityType::CHEST, LuaContext::l_create_chest },
    { EntityType::JUMPER, LuaContext::l_create_jumper },
    { EntityType::ENEMY, LuaContext::l_create_enemy },
    { EntityType::NPC, LuaContext::l_create_npc },
    { EntityType::BLOCK, LuaContext::l_create_block },
    { EntityType::DYNAMIC_TILE, LuaContext::l_create_dynamic_tile },
    { EntityType::SWITCH, LuaContext::l_create_switch },
    { EntityType::WALL, LuaContext::l_create_wall },
    { EntityType::SENSOR, LuaContext::l_create_sensor },
    { EntityType::CRYSTAL, LuaContext::l_create_crystal },
    { EntityType::CRYSTAL_BLOCK, LuaContext::l_create_crystal_block },
    { EntityType::SHOP_TREASURE, LuaContext::l_create_shop_treasure },
    { EntityType::STREAM, LuaContext::l_create_stream },
    { EntityType::DOOR, LuaContext::l_create_door },
    { EntityType::STAIRS, LuaContext::l_create_stairs },
    { EntityType::SEPARATOR, LuaContext::l_create_separator },
    { EntityType::CUSTOM, LuaContext::l_create_custom_entity },
    { EntityType::EXPLOSION, LuaContext::l_create_explosion },
    { EntityType::HERO, LuaContext::l_create_hero},
    { EntityType::BOMB, LuaContext::l_create_bomb },
    { EntityType::FIRE, LuaContext::l_create_fire },
};

/**
 * \brief Creates on the current map an entity from the specified data.
 *
 * This function is meant to be called from C++.
 * It pushes nothing to Lua and does not raise Lua errors.
 * In case of failure, it prints an error message.
 *
 * \param map The map where to create an entity.
 * \param entity_data Description of the entity to create.
 * \return \c true if the entity was successfully created.
 */
bool LuaContext::create_map_entity_from_data(Map& map, const EntityData& entity_data) {

  lua_pushcfunction(current_l, entity_creation_functions.find(entity_data.get_type())->second);
  push_map(current_l, map);
  lua_pushlightuserdata(current_l, const_cast<EntityData*>(&entity_data));
  return call_function(2, 0, ("create_" + enum_to_name(entity_data.get_type())).c_str());
}

/**
 * \brief Creates on the current map an entity from the specified data.
 *
 * This function is meant to be called from a Lua API call.
 * Pushes onto the Lua stack the created entity.
 * In case of failure, raises a Lua error.
 *
 * \param map The map where to create an entity.
 * \param entity_data Description of the entity to create.
 */
void LuaContext::create_map_entity_from_data_to_lua(Map &map, const EntityData &entity_data) {

  lua_pushcfunction(current_l, entity_creation_functions.find(entity_data.get_type())->second);
  push_map(current_l, map);
  lua_pushlightuserdata(current_l, const_cast<EntityData*>(&entity_data));
  lua_call(current_l, 2, 1);
}

/**
 * \brief __index function of the environment of the map's code.
 *
 * This special __index function allows the map's Lua code to get a map
 * entity like a global value.
 * If an entity exists with the specified name, this entity is returned.
 * Otherwise, we fall back to the usual behavior of global values:
 * a global value with this name (or \c nil) is returned.
 *
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_get_map_entity_or_global(lua_State* l) {

  return state_boundary_handle(l, [&] {
    lua_pushvalue(l, lua_upvalueindex(1));  // Because check_map does not like pseudo-indexes.
    Map& map = *check_map(l, -1);
    const std::string& name = LuaTools::check_string(l, 2);

    EntityPtr entity = nullptr;
    if (map.is_started()) {
      entity = map.get_entities().find_entity(name);
    }

    if (entity != nullptr && !entity->is_being_removed()) {
      push_entity(l, *entity);
    }
    else {
      lua_getglobal(l, name.c_str());
    }
    return 1;
  });
}

/**
 * \brief __index function of the easy environment for the console.
 *
 * See do_string_with_easy_env() for more details.
 *
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_easy_index(lua_State* l) {

  return state_boundary_handle(l, [&] {

    const std::string& name = LuaTools::check_string(l, 2);

    LuaContext& lua_context = get();
    Game* game = lua_context.get_main_loop().get_game();

    if (game != nullptr) {
      // A game is running.
      if (name == "game") {
        push_game(l, game->get_savegame());
        return 1;
      }

      if (game->has_current_map()) {
        Map& map = game->get_default_map(); //TODO : see if we can search many maps
        if (name == "map") {
          push_map(l, map);
          return 1;
        }

        if (name == "tp") {
          push_hero(l, *game->get_hero());
          lua_pushcclosure(l, l_hero_teleport, 1);
          return 1;
        }

        EntityPtr entity = map.get_entities().find_entity(name);
        if (entity != nullptr && !entity->is_being_removed()) {
          push_entity(l, *entity);
          return 1;
        }
      }
    }

    lua_getglobal(l, name.c_str());
    return 1;
  });
}

/**
 * \brief Like hero:teleport(), but the hero is passed as an upvalue.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_hero_teleport(lua_State* l) {

  return state_boundary_handle(l, [&] {
    lua_pushvalue(l, lua_upvalueindex(1));
    HeroPtr hero_ptr = check_hero(l, -1);
    Hero& hero = *hero_ptr;
    lua_pop(l, 1);
    Game& game = hero.get_game();
    const std::string& map_id = LuaTools::check_string(l, 1);
    const std::string& destination_name = LuaTools::opt_string(l, 2, "");
    Transition::Style transition_style = LuaTools::opt_enum<Transition::Style>(
        l, 3, game.get_default_transition_style());

    if (!CurrentQuest::resource_exists(ResourceType::MAP, map_id)) {
      LuaTools::arg_error(l, 2, std::string("No such map: '") + map_id + "'");
    }

    hero.get_game().teleport_hero(hero_ptr, map_id, destination_name, transition_style);

    return 0;
  });
}

/**
 * \brief Closure of an iterator over a list of entities.
 *
 * This closure expects 3 upvalues in this order:
 * - The array of entities.
 * - The size of the array (for performance).
 * - The current index in the array.
 *
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_entity_iterator_next(lua_State* l) {

  return state_boundary_handle(l, [&] {

    // Get upvalues.
    const int table_index = lua_upvalueindex(1);
    const int size =  static_cast<int>(lua_tointeger(l, lua_upvalueindex(2)));
    int index =  static_cast<int>(lua_tointeger(l, lua_upvalueindex(3)));

    if (index > size) {
      // Finished.
      return 0;
    }

    // Get the next value.
    lua_rawgeti(l, table_index, index);

    // Increment index.
    ++index;
    lua_pushinteger(l, index);
    lua_replace(l, lua_upvalueindex(3));

    return 1;
  });
}

/**
 * \brief Generates a Lua error if a map is not in an existing game.
 * \param l A Lua context.
 * \param map The map to check.
 */
void LuaContext::check_map_has_game(lua_State* l, const Map& map) {

  if (!map.is_game_running()) {
    // Return nil if the game is already destroyed.
    LuaTools::error(l, "The game of this map is no longer running");
  }
}

/**
 * \brief Calls the on_started() method of a Lua map.
 *
 * Does nothing if the method is not defined.
 *
 * \param map A map.
 * \param destination The destination point used (nullptr if it is a special one).
 */
void LuaContext::map_on_started(Map& map, const std::shared_ptr<Destination>& destination) {

  if (!userdata_has_field(map, "on_started")) {
    return;
  }

  run_on_main([this, &map, destination](lua_State* l){
    push_map(l, map);
    on_started(destination);
    lua_pop(l, 1);
  });
}

/**
 * \brief Calls the on_finished() method of a Lua map if it is defined.
 *
 * Also stops timers and menus associated to the map.
 *
 * \param map A map.
 */
void LuaContext::map_on_finished(Map& map) {

  push_map(current_l, map);
  if (userdata_has_field(map, "on_finished")) {
    on_finished();
  }
  remove_timers(-1);  // Stop timers and menus associated to this map.
  remove_menus(-1);
  lua_pop(current_l, 1);
}

/**
 * \brief Calls the on_update() method of a Lua map.
 *
 * Also calls the method on its menus.
 *
 * \param map A map.
 */
void LuaContext::map_on_update(Map& map) {

  push_map(current_l, map);
  // This particular method is tried so often that we want to save optimize
  // the std::string construction.
  static const std::string method_name = "on_update";
  if (userdata_has_field(map, method_name)) {
    on_update();
  }
  menus_on_update(-1);
  lua_pop(current_l, 1);
}

/**
 * \brief Calls the on_draw() method of a Lua map if it is defined.
 *
 * Also calls the method on its menus.
 *
 * \param map A map.
 * \param dst_surface The destination surface.
 */
void LuaContext::map_on_draw(Map& map, const SurfacePtr& dst_surface) {

  push_map(current_l, map);
  if (userdata_has_field(map, "on_draw")) {
    on_draw(dst_surface);
  }
  menus_on_draw(-1, dst_surface);
  lua_pop(current_l, 1);
}

/**
 * \brief Notifies a Lua map that an input event has just occurred.
 *
 * The appropriate callback in the map is triggered if it exists.
 * Also notifies the menus of the game if the game itself does not handle the
 * event.
 *
 * \param event The input event to handle.
 * \param map A map.
 * \return \c true if the event was handled and should stop being propagated.
 */
bool LuaContext::map_on_input(Map& map, const InputEvent& event) {

  push_map(current_l, map);
  bool handled = on_input(event);
  if (!handled) {
    handled = menus_on_input(-1, event);
  }
  lua_pop(current_l, 1);
  return handled;
}

/**
 * \brief Calls the on_command_pressed() method of a Lua map.
 *
 * Also notifies the menus of the game if the game itself does not handle the
 * event.
 *
 * \param map A map.
 * \param command The command pressed.
 * \return \c true if the event was handled and should stop being propagated.
 */
bool LuaContext::map_on_control(Map& map, const ControlEvent& event) {

  bool handled = false;
  push_map(current_l, map);
  if (userdata_has_field(map, event.event_name())) {
    handled = on_command(event);
  }
  if (!handled) {
    handled = menus_on_command(-1, event);
  }
  lua_pop(current_l, 1);
  return handled;
}

/**
 * \brief Calls the on_suspended() method of a Lua map.
 *
 * Does nothing if the method is not defined.
 *
 * \param map A map.
 * \param suspended true if the map is suspended.
 */
void LuaContext::map_on_suspended(Map& map, bool suspended) {

  if (!userdata_has_field(map, "on_suspended")) {
    return;
  }

  push_map(current_l, map);
  on_suspended(suspended);
  lua_pop(current_l, 1);
}

/**
 * \brief Calls the on_opening_transition_finished() method of a Lua map.
 *
 * Does nothing if the method is not defined.
 *
 * \param map A map.
 * \param destination The destination point used (nullptr if it is a special one).
 */
void LuaContext::map_on_opening_transition_finished(Map& map,
    const std::shared_ptr<Destination>& destination) {

  if (!userdata_has_field(map, "on_opening_transition_finished")) {
    //return;
  }

  push_map(current_l, map);
  on_opening_transition_finished(destination);
  lua_pop(current_l, 1);
}

/**
 * \brief Calls the on_obtaining_treasure() method of a Lua map.
 *
 * Does nothing if the method is not defined.
 *
 * \param map A map.
 * \param treasure A treasure the hero is about to obtain on that map.
 */
void LuaContext::map_on_obtaining_treasure(Map& map, const Treasure& treasure, Hero& hero) {

  if (!userdata_has_field(map, "on_obtaining_treasure")) {
    return;
  }

  push_map(current_l, map);
  on_obtaining_treasure(treasure, hero);
  lua_pop(current_l, 1);
}

/**
 * \brief Calls the on_obtained_treasure() method of a Lua map.
 *
 * Does nothing if the method is not defined.
 *
 * \param map A map.
 * \param treasure The treasure just obtained.
 */
void LuaContext::map_on_obtained_treasure(Map& map, const Treasure& treasure, Hero& hero) {

  if (!userdata_has_field(map, "on_obtained_treasure")) {
    return;
  }

  push_map(current_l, map);
  on_obtained_treasure(treasure, hero);
  lua_pop(current_l, 1);
}

}

