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
#include "solarus/core/CurrentQuest.h"
#include "solarus/core/Game.h"
#include "solarus/core/Geometry.h"
#include "solarus/core/MainLoop.h"
#include "solarus/core/Map.h"
#include "solarus/entities/Entities.h"
#include "solarus/entities/Hero.h"
#include "solarus/graphics/Drawable.h"
#include "solarus/lua/ExportableToLua.h"
#include "solarus/lua/ExportableToLuaPtr.h"
#include "solarus/lua/LuaBind.h"
#include "solarus/lua/LuaContext.h"
#include "solarus/lua/LuaTools.h"
#include "solarus/movements/CircleMovement.h"
#include "solarus/movements/JumpMovement.h"
#include "solarus/movements/PathFindingMovement.h"
#include "solarus/movements/PathMovement.h"
#include "solarus/movements/PixelMovement.h"
#include "solarus/movements/RandomMovement.h"
#include "solarus/movements/RandomPathMovement.h"
#include "solarus/movements/TargetMovement.h"

namespace Solarus {

/**
 * Name of the Lua table representing the movement module.
 */
const std::string LuaContext::movement_module_name = "sol.movement";

/**
 * Name of the Lua table representing the straight movement module.
 */
const std::string LuaContext::movement_straight_module_name = "sol.straight_movement";

/**
 * Name of the Lua table representing the target movement module.
 */
const std::string LuaContext::movement_target_module_name = "sol.target_movement";

/**
 * Name of the Lua table representing the random movement module.
 */
const std::string LuaContext::movement_random_module_name = "sol.random_movement";

/**
 * Name of the Lua table representing the path movement module.
 */
const std::string LuaContext::movement_path_module_name = "sol.path_movement";

/**
 * Name of the Lua table representing the random path movement module.
 */
const std::string LuaContext::movement_random_path_module_name = "sol.random_path_movement";

/**
 * Name of the Lua table representing the path finding movement module.
 */
const std::string LuaContext::movement_path_finding_module_name = "sol.path_finding_movement";

/**
 * Name of the Lua table representing the circle movement module.
 */
const std::string LuaContext::movement_circle_module_name = "sol.circle_movement";

/**
 * Name of the Lua table representing the jump movement module.
 */
const std::string LuaContext::movement_jump_module_name = "sol.jump_movement";

/**
 * Name of the Lua table representing the pixel movement module.
 */
const std::string LuaContext::movement_pixel_module_name = "sol.pixel_movement";

/**
 * \brief Implementation of sol.movement.create().
 * \param lua_context The current LuaContext.
 * \param type The name of the type of movement to create.
 * \return The number of arguments to return in Lua.
 */
static LuaBind::OnStack create(LuaContext& lua_context,
    const std::string& type) {
  lua_State* L = lua_context.get_internal_state();
  std::shared_ptr<Movement> movement;
  if (type == "straight") {
    std::shared_ptr<StraightMovement> straight_movement =
        std::make_shared<StraightMovement>(false, true);
    straight_movement->set_speed(32);
    movement = straight_movement;
  } else if (type == "random") {
    movement = std::make_shared<RandomMovement>(32);
  } else if (type == "target") {
    if (Game* game = lua_context.get_main_loop().get_game()) {
      // If we are on a map, the default target is the hero.
      movement = std::make_shared<TargetMovement>(
          game->get_hero(), 0, 0, 32, false
      );
    } else {
      movement = std::make_shared<TargetMovement>(
          nullptr, 0, 0, 32, false
      );
    }
  } else if (type == "path") {
    movement = std::make_shared<PathMovement>(
         "", 32, false, false, false
    );
  } else if (type == "random_path") {
    movement = std::make_shared<RandomPathMovement>(32);
  } else if (type == "path_finding") {
    std::shared_ptr<PathFindingMovement> path_finding_movement =
        std::make_shared<PathFindingMovement>(32);
    if (Game* game = lua_context.get_main_loop().get_game()) {
      // If we are on a map, the default target is the hero.
      path_finding_movement->set_target(game->get_hero());
    }
    movement = path_finding_movement;
  } else if (type == "circle") {
    movement = std::make_shared<CircleMovement>();
  } else if (type == "jump") {
    movement = std::make_shared<JumpMovement>(0, 0, 0, false);
  } else if (type == "pixel") {
    movement = std::make_shared<PixelMovement>("", 30, false, false);
  } else {
    LuaTools::arg_error(L, 1, "should be one of: "
        "'straight', "
        "'random', "
        "'target', "
        "'path', "
        "'random_path', "
        "'path_finding', "
        "'circle', "
        "'jump' or "
        "'pixel'");
  }

  // Exported manually due to lifetime issues.
  LuaContext::push_userdata(L, *movement);
  return {1};
}

/**
 * \brief Implementation of movement:get_xy().
 * \param movement The movement to query.
 * \return The x and y location of the object controled by this movement.
 */
static std::tuple<int, int> get_xy(Movement& movement) {
  const Point& xy = movement.get_xy();
  return std::make_tuple(xy.x, xy.y);
}

/**
 * \brief Implementation of movement:stop().
 * \param context The current LuaContext.
 * \param movement The movement to update.
 */
static void stop(LuaContext& context, Movement& movement) {
  if (Entity* entity = movement.get_entity()) {
    // The object controlled is a map entity.
    entity->clear_movement();
    entity->notify_movement_finished();
  } else if (Drawable* drawable = movement.get_drawable()) {
    // The object controlled is a drawable.
    drawable->stop_movement();
  } else {
    // The object controlled is a point.
    std::shared_ptr<Movement> ptr = movement.shared_from_this_cast<Movement>();
    context.stop_movement_on_point(ptr);
  }
}

/**
 * \brief Implementation of movement:start().
 * \param context The current LuaContext.
 * \param movement The movement to update.
 */
static void start(LuaContext& context, Movement& movement) {
  // First, stop any previous movement. (This is why they are out of order.)
  stop(context, movement);

  std::shared_ptr<Movement> ptr = movement.shared_from_this_cast<Movement>();
  lua_State* l = context.get_internal_state();
  if (lua_type(l, 2) == LUA_TTABLE) {
    context.start_movement_on_point(ptr, 2);
  } else if (LuaContext::is_entity(l, 2)) {
    Entity& entity = *LuaContext::check_entity(l, 2);
    if (!entity.is_on_map() ||
        !entity.get_map().is_started()
    ) {
      LuaTools::arg_error(l, 2, "This entity is not on the current map");
    }
    entity.clear_movement();
    entity.set_movement(ptr);
  } else if (LuaContext::is_drawable(l, 2)) {
    Drawable& drawable = *LuaContext::check_drawable(l, 2);
    drawable.start_movement(ptr);
  } else {
    LuaTools::type_error(l, 2, "table, entity or drawable");
  }

  ScopedLuaRef callback_ref = LuaTools::opt_function(l, 3);
  movement.set_finished_callback(callback_ref);
}

/**
 * \brief Implementation of movement:set_ignore_suspend().
 * \param movement The movement to update.
 * \param ignore_suspend_arg The new value, true if absent.
 */
static void set_ignore_suspend(Movement& movement,
    std::optional<bool> ignore_suspend_arg) {
  bool ignore_suspend = ignore_suspend_arg.value_or(true);
  movement.set_ignore_suspend(ignore_suspend);
}

/**
 * \brief Implementation of movement:set_ignore_obstacles().
 * \param movement The movement to update.
 * \param ignore_obstacles_arg The new value, true if absent.
 */
static void set_ignore_obstacles(Movement& movement,
    std::optional<bool> ignore_obstacles_arg) {
  bool ignore_obstacles = ignore_obstacles_arg.value_or(true);
  movement.set_ignore_obstacles(ignore_obstacles);
}

/**
 * \brief Implementation of movement:get_type().
 * \param movement The movement to update.
 * \return The type name.
 */
static std::string get_type(Movement& movement) {
  // The type name actually stores the module name, with "sol." prefix.
  const std::string& module_name = movement.get_lua_type_name();
  return LuaTools::get_type_name(module_name);
}

/**
 * \brief Implementation of straight_movement:set_smooth().
 * \param movement The movement to update.
 * \param smooth_arg The new value, true if absent.
 */
static void straight_set_smooth(
    StraightMovement& movement, std::optional<bool> smooth_arg) {
  bool smooth = smooth_arg.value_or(true);
  movement.set_smooth(smooth);
}

/**
 * \brief Implementation of random_movement:get_speed().
 *
 * We need a wrapper to refine the argument type,
 * since get_speed is defined on StraightMovement.
 * \param movement The movement to query.
 * \return The speed in pixels per second.
 */
static int random_get_speed(RandomMovement& movement) {
  return movement.get_speed();
}

/**
 * \brief Implementation of random_movement:is_smooth().
 *
 * We need a wrapper to refine the argument type,
 * since get_speed is defined on StraightMovement.
 * \param movement The movement to query.
 * \return True if the movement is smooth, otherwise false.
 */
static bool random_is_smooth(RandomMovement& movement) {
  return movement.is_smooth();
}

/**
 * \brief Implementation of random_movement:set_smooth().
 * \param movement The movement to update.
 * \param smooth_arg New smooth value, considered true if empty.
 */
static void random_set_smooth(
    RandomMovement& movement, std::optional<bool> smooth_arg) {
  bool smooth = smooth_arg.value_or(true);
  movement.set_smooth(smooth);
}

/**
 * \brief Implementation of target_movement:set_target().
 * \param l The Lua state the call was made from.
 * \param movement The movement to update.
 */
static void target_set_target(lua_State* l, TargetMovement& movement) {
  if (lua_isnumber(l, 2)) {
    // The target is a fixed point.
    int x = LuaTools::check_int(l, 2);
    int y = LuaTools::check_int(l, 3);
    movement.set_target(nullptr, Point(x, y));
  } else {
    // The target is an entity, possibly with an offset.
    EntityPtr target = LuaContext::check_entity(l, 2);
    int dx = 0;
    int dy = 0;
    if (lua_isnumber(l, 3)) {
      dx = LuaTools::check_int(l, 3);
      dy = LuaTools::check_int(l, 4);
    }
    movement.set_target(target, Point(dx, dy));
  }
}

/**
 * \brief Implementation of target_movement:get_speed().
 *
 * This function is used for type refinement.
 * \param movement The movement to query.
 * \return The speed in pixels per second.
 */
static int target_get_speed(TargetMovement& movement) {
  return movement.get_speed();
}

/**
 * \brief Implementation of target_movement:get_angle().
 *
 * This function is used for type refinement.
 * \param movement The movement to query.
 * \return The angle in radians, counter-clockwise from right axis.
 */
static double target_get_angle(const TargetMovement& movement) {
  return movement.get_angle();
}

/**
 * \brief Implementation of target_movement:is_smooth().
 *
 * This function is used for type refinement.
 * \param movement The movement to query.
 * \return True if movement is smooth, false otherwise.
 */
static bool target_is_smooth(TargetMovement& movement) {
  return movement.is_smooth();
}

/**
 * \brief Implementation of target_movement:set_smooth().
 * \param movement The movement to update.
 * \param smooth_arg New smooth value, considered true if empty.
 */
static void target_set_smooth(
    TargetMovement& movement, std::optional<bool> smooth_arg) {
  bool smooth = smooth_arg.value_or(true);
  movement.set_smooth(smooth);
}

/**
 * \brief Implementation of path_movement:get_path().
 * \param l The Lua state the call was made from.
 * \param movement The movement to query.
 * \return Number of values to return to Lua.
 */
static LuaBind::OnStack path_get_path(lua_State* l, PathMovement& movement) {
  const std::string& path = movement.get_path();

  // Build a Lua array containing the path.
  lua_createtable(l, static_cast<int>(path.size()), 0);
  for (size_t i = 0 ; i < path.size() ; ++i) {
    int direction8 = (path[i] - '0');
    lua_pushinteger(l, direction8);
    lua_rawseti(l, -2, static_cast<int>(i + 1));
  }

  return {1};
}

/**
 * \brief Implementation of path_movement:set_path().
 * \param l The Lua state the call was made from.
 * \param movement The movement to update.
 */
static void path_set_path(lua_State* l, PathMovement& movement) {
  LuaTools::check_type(l, 2, LUA_TTABLE);

  // Build the path as a string from the Lua table.
  std::string path = "";
  lua_pushnil(l);
  while (lua_next(l, 2) != 0) {
    int direction8 = LuaTools::check_int(l, -1);
    path += ('0' + direction8);
    lua_pop(l, 1); // pop the value, let the key for the iteration
  }
  movement.set_path(path);
}

/**
 * \brief Implementation of path_movement:set_loop().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
static void path_set_loop(
    PathMovement& movement, std::optional<bool> loop_arg) {
  bool loop = loop_arg.value_or(true);
  movement.set_loop(loop);
}

/**
 * \brief Implementation of path_movement:set_snap_to_grid().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
static void path_set_snap_to_grid(
    PathMovement& movement, std::optional<bool> snap_to_grid_arg) {
  bool snap_to_grid = snap_to_grid_arg.value_or(true);
  movement.set_snap_to_grid(snap_to_grid);
}

/**
 * \brief Implementation of random_path_movement:get_speed().
 *
 * This function is used for type refinement.
 * \param movement The movement to query.
 * \return The speed in pixels per second.
 */
static int random_path_get_speed(RandomPathMovement& movement) {
  return movement.get_speed();
}

/**
 * \brief Implementation of random_path_movement:get_angle().
 *
 * This function is used for type refinement.
 * \param movement The movement to query.
 * \return The angle in radians, counter-clockwise from right axis.
 */
static double random_path_get_angle(RandomPathMovement& movement) {
  return movement.get_angle();
}

/**
 * \brief Implementation of random_path_movement:set_speed().
 *
 * This function is used for type refinement.
 * \param movement The movement to update.
 * \param speed The new speed in pixels per second.
 */
static void random_path_set_speed(RandomPathMovement& movement, int speed) {
  movement.set_speed(speed);
}

/**
 * \brief Implementation of path_finding_movement:set_target().
 * \param movement The movement to update.
 * \param target The new target to follow.
 */
static void path_finding_set_target(
    PathFindingMovement& movement, Entity& target) {
  EntityPtr target_ptr = target.shared_from_this_cast<Entity>();
  movement.set_target(target_ptr);
}

/**
 * \brief Implementation of path_finding_movement:get_speed().
 *
 * This function is used for type refinement.
 * \param movement The movement to query.
 * \return The speed in pixels per second.
 */
static int path_finding_get_speed(PathFindingMovement& movement) {
  return movement.get_speed();
}

/**
 * \brief Implementation of path_finding_movement:set_speed().
 * \param movement The movement to update.
 * \param speed The new speed in pixels per second.
 */
static void path_finding_set_speed(PathFindingMovement& movement, int speed) {
  movement.set_speed(speed);
}

/**
 * \brief Implementation of path_finding_movement:get_angle().
 * \param movement The movement to query.
 * \return The angle in radians, counter-clockwise from right axis.
 */
static double path_finding_get_angle(PathFindingMovement& movement) {
  return movement.get_angle();
}

/**
 * \brief Implementation of circle_movement:get_center().
 * \param movement The movement to query.
 * \return The x and y coordinates of the center point.
 */
static std::tuple<int, int> circle_get_center(CircleMovement& movement) {
  const Point& xy = movement.get_center();
  return std::make_tuple(xy.x, xy.y);
}

/**
 * \brief Implementation of circle_movement:set_center().
 * \param l The Lua state the call was made from.
 * \param movement The movement to update.
 */
static void circle_set_center(lua_State* l, CircleMovement& movement) {
  if (lua_isnumber(l, 2)) {
    // The center is a fixed point.
    int x = LuaTools::check_int(l, 2);
    int y = LuaTools::check_int(l, 3);
    movement.set_center(Point(x, y));
  } else {
    // The center is an entity, optionally with an offset.
    EntityPtr center = LuaContext::check_entity(l, 2);
    int dx = 0;
    int dy = 0;
    if (lua_isnumber(l, 3)) {
      dx = LuaTools::check_int(l, 3);
      dy = LuaTools::check_int(l, 4);
    }
    movement.set_center(center, dx, dy);
  }
}

/**
 * \brief Implementation of circle_movement:set_clockwise().
 * \param movement The movement to update.
 * \param clockwise_arg The new value of clockwise, true if absent.
 */
static void circle_set_clockwise(
    CircleMovement& movement, const std::optional<bool>& clockwise_arg) {
  bool clockwise = clockwise_arg.value_or(true);
  movement.set_clockwise(clockwise);
}

/**
 * \brief Implementation of circle_movement:get_initial_angle().
 * \param context The current LuaContext.
 * \param movement The movement to query.
 * \return The initial angle in degrees, counter-clockwise from right axis.
 */
static int circle_get_initial_angle(LuaContext& context,
    CircleMovement& movement) {
  context.warning_deprecated(
      { 1, 6 },
      "circle_movement:get_initial_angle()",
      "Use circle_movement:get_angle_from_center() in radians instead."
  );
  return Geometry::radians_to_degrees(movement.get_initial_angle());
}

/**
 * \brief Implementation of circle_movement:set_initial_angle().
 * \param context The current LuaContext.
 * \param movement The movement to query.
 * \param initial_angle_degrees The new initial angle in degrees, counter-clockwise from right axis.
 */
static void circle_set_initial_angle(LuaContext& context,
    CircleMovement& movement, int initial_angle_degrees) {
  context.warning_deprecated(
      { 1, 6 },
      "circle_movement:set_initial_angle()",
      "Use circle_movement:set_angle_from_center() in radians instead."
  );
  movement.set_angle_from_center(Geometry::degrees_to_radians(initial_angle_degrees));
}

/**
 * \brief Implementation of circle_movement:get_angle_speed().
 * \param context The current LuaContext.
 * \param movement The movement to query.
 * \return The angular speed in degrees per second.
 */
static int circle_get_angle_speed(LuaContext& context,
    CircleMovement& movement) {
  context.warning_deprecated(
      { 1, 6 },
      "circle_movement:get_angle_speed()",
      "Use circle_movement:get_angular_speed() in radians instead."
  );
  return Geometry::radians_to_degrees(movement.get_angular_speed());
}

/**
 * \brief Implementation of circle_movement:set_angle_speed().
 * \param context The current LuaContext.
 * \param movement The movement to update.
 * \param angle_speed_degrees The new angular speed in degrees per second.
 */
static void circle_set_angle_speed(LuaContext& context,
    CircleMovement& movement, int angle_speed_degrees) {
  context.warning_deprecated(
      { 1, 6 },
      "circle_movement:set_angle_speed()",
      "Use circle_movement:set_angular_speed() in radians instead."
  );
  movement.set_angular_speed(Geometry::degrees_to_radians(angle_speed_degrees));
}

/**
 * \brief Implementation of circle_movement:get_duration().
 *
 * This function is used for type refinement.
 * \param movement The movement to query.
 * \return The duration in milliseconds (0 means infinite).
 */
static int circle_get_duration(CircleMovement& movement) {
  return movement.get_duration();
}

/**
 * \brief Implementation of circle_movement:set_duration().
 *
 * This function is used for type refinement.
 * \param movement The movement to update.
 * \param duration The new duration in milliseconds (0 means infinite).
 */
static void circle_set_duration(CircleMovement& movement, int duration) {
  movement.set_duration(duration);
}

/**
 * \brief Implementation of circle_movement:get_loop_delay().
 *
 * This function is used for type refinement.
 * \param movement The movement to query.
 * \return The restart time in milliseconds (0 is no restart).
 */
static int circle_get_loop_delay(CircleMovement& movement) {
  return movement.get_loop();
}

/**
 * \brief Implementation of circle_movement:set_loop_delay().
 *
 * This function is used for type refinement.
 * \param movement The movement to update.
 * \param loop_delay The new restart time in milliseconds (0 is no restart).
 */
static void circle_set_loop_delay(CircleMovement& movement, int loop_delay) {
  movement.set_loop(loop_delay);
}

/**
 * \brief Implementation of pixel_movement:get_trajectory().
 * \param l The Lua state the call was made from.
 * \return Number of values to return to Lua.
 */
static LuaBind::OnStack pixel_get_trajectory(
    lua_State* l, PixelMovement& movement) {

  const std::list<Point>& trajectory = movement.get_trajectory();
  // Build a Lua array containing the trajectory.
  lua_settop(l, 1);
  lua_newtable(l);
  int i = 1;
  for (const Point& xy: trajectory) {
    lua_createtable(l, 2, 0);
    lua_pushinteger(l, xy.x);
    lua_rawseti(l, 3, 1);
    lua_pushinteger(l, xy.y);
    lua_rawseti(l, 3, 2);
    lua_rawseti(l, 2, i);
    ++i;
  }
  return {1};
}

/**
 * \brief Implementation of pixel_movement:set_trajectory().
 * \param l The Lua state the call was made from.
 * \return Number of values to return to Lua.
 */
static void pixel_set_trajectory(
    lua_State* l, PixelMovement& movement) {
  LuaTools::check_type(l, 2, LUA_TTABLE);

  // Build the trajectory as a string from the Lua table.
  std::list<Point> trajectory;
  lua_pushnil(l); // First key.
  while (lua_next(l, 2) != 0) {
    LuaTools::check_type(l, 4, LUA_TTABLE);
    lua_rawgeti(l, 4, 1);
    lua_rawgeti(l, 4, 2);
    int x = LuaTools::check_int(l, 5);
    int y = LuaTools::check_int(l, 6);
    trajectory.emplace_back(x, y);
    lua_settop(l, 3); // Just the key for the next iteration.
  }
  movement.set_trajectory(trajectory);
}

/**
 * \brief Implementation of pixel_movement:set_loop().
 * \param movement The movement to update.
 * \param loop_arg The new value, true if absent.
 */
static void pixel_set_loop(
    PixelMovement& movement, std::optional<bool> loop_arg) {
  bool loop = loop_arg.value_or(true);
  movement.set_loop(loop);
}

/**
 * \brief Implementation of pixel_movement:get_delay().
 *
 * This function is used for type refinement.
 * \param movement The movement to query.
 * \return Number of values to return to Lua.
 */
static int pixel_get_delay(PixelMovement& movement) {
  return int(movement.get_delay());
}

/**
 * \brief Implementation of pixel_movement:set_delay().
 *
 * This function is used for type refinement.
 * \param movement The movement to update.
 * \param delay
 */
static void pixel_set_delay(PixelMovement& movement, int delay) {
  movement.set_delay(uint32_t(delay));
}

/**
 * \brief Initializes the movement features provided to Lua.
 */
void LuaContext::register_movement_module() {

  // Functions of sol.movement.
  const std::vector<luaL_Reg> movement_functions = {
      { "create", LUA_TO_C_BIND(create) },
  };

  // Methods common to all movement types.
  std::vector<luaL_Reg> movement_common_methods = {
      { "get_xy", LUA_TO_C_BIND(get_xy) },
      { "set_xy", LUA_TO_C_BIND_AS(void(Movement::*)(int, int), &Movement::set_xy) },
      { "start", LUA_TO_C_BIND(start) },
      { "stop", LUA_TO_C_BIND(stop) },
      { "is_suspended", LUA_TO_C_BIND(&Movement::is_suspended) },
      { "get_ignore_suspend", LUA_TO_C_BIND(&Movement::get_ignore_suspend) },
      { "set_ignore_suspend", LUA_TO_C_BIND(set_ignore_suspend) },
      { "get_ignore_obstacles", LUA_TO_C_BIND(&Movement::are_obstacles_ignored) },
      { "set_ignore_obstacles", LUA_TO_C_BIND(set_ignore_obstacles) },
      { "get_direction4", LUA_TO_C_BIND(&Movement::get_displayed_direction4) },
  };
  if (CurrentQuest::is_format_at_least({ 2, 0 })) {
    movement_common_methods.insert(movement_common_methods.end(), {
      { "get_type", LUA_TO_C_BIND(get_type) },
    });
  }

  // Metamethods of all movement types.
  const std::vector<luaL_Reg> metamethods = {
      { "__gc", userdata_meta_gc },
      { "__newindex", userdata_meta_newindex_as_table },
      { "__index", userdata_meta_index_as_table },
  };

  register_type(
      movement_module_name,
      movement_functions,
      movement_common_methods,
      metamethods
  );

  // Straight movement.
  std::vector<luaL_Reg> straight_movement_methods = {
      { "get_speed", LUA_TO_C_BIND(&StraightMovement::get_speed) },
      { "set_speed", LUA_TO_C_BIND(&StraightMovement::set_speed) },
      { "get_angle", LUA_TO_C_BIND(&StraightMovement::get_angle) },
      { "set_angle", LUA_TO_C_BIND(&StraightMovement::set_angle) },
      { "get_max_distance", LUA_TO_C_BIND(&StraightMovement::get_max_distance) },
      { "set_max_distance", LUA_TO_C_BIND(&StraightMovement::set_max_distance) },
      { "is_smooth", LUA_TO_C_BIND(&StraightMovement::is_smooth) },
      { "set_smooth", LUA_TO_C_BIND(straight_set_smooth) },
  };
  straight_movement_methods.insert(
        straight_movement_methods.end(),
        movement_common_methods.begin(),
        movement_common_methods.end()
  );
  register_type(
      movement_straight_module_name,
      {},
      straight_movement_methods,
      metamethods
  );

  // Random movement.
  std::vector<luaL_Reg> random_movement_methods = {
      { "get_speed", LUA_TO_C_BIND(random_get_speed) },
      { "set_speed", LUA_TO_C_BIND(&RandomMovement::set_normal_speed) },
      { "get_angle", LUA_TO_C_BIND(&RandomMovement::get_angle) },
      { "get_max_distance", LUA_TO_C_BIND(&RandomMovement::get_max_radius) },
      { "set_max_distance", LUA_TO_C_BIND(&RandomMovement::set_max_radius) },
      { "is_smooth", LUA_TO_C_BIND(random_is_smooth) },
      { "set_smooth", LUA_TO_C_BIND(random_set_smooth) },
  };
  random_movement_methods.insert(
        random_movement_methods.end(),
        movement_common_methods.begin(),
        movement_common_methods.end()
  );
  register_type(
      movement_random_module_name,
      {},
      random_movement_methods,
      metamethods
  );

  // Target movement.
  std::vector<luaL_Reg> target_movement_methods = {
      { "set_target", LUA_TO_C_BIND(target_set_target) },
      { "get_speed", LUA_TO_C_BIND(target_get_speed) },
      { "set_speed", LUA_TO_C_BIND(&TargetMovement::set_moving_speed) },
      { "get_angle", LUA_TO_C_BIND(target_get_angle) },
      { "is_smooth", LUA_TO_C_BIND(target_is_smooth) },
      { "set_smooth", LUA_TO_C_BIND(target_set_smooth) },
  };
  target_movement_methods.insert(
        target_movement_methods.end(),
        movement_common_methods.begin(),
        movement_common_methods.end()
  );
  register_type(
      movement_target_module_name,
      {},
      target_movement_methods,
      metamethods
  );

  // Path movement.
  std::vector<luaL_Reg> path_movement_methods = {
      { "get_path", LUA_TO_C_BIND(path_get_path) },
      { "set_path", LUA_TO_C_BIND(path_set_path) },
      { "get_speed", LUA_TO_C_BIND(&PathMovement::get_speed) },
      { "set_speed", LUA_TO_C_BIND(&PathMovement::set_speed) },
      { "get_loop", LUA_TO_C_BIND(&PathMovement::get_loop) },
      { "set_loop", LUA_TO_C_BIND(path_set_loop) },
      { "get_snap_to_grid", LUA_TO_C_BIND(&PathMovement::get_snap_to_grid) },
      { "set_snap_to_grid", LUA_TO_C_BIND(path_set_snap_to_grid) },
  };
  if (CurrentQuest::is_format_at_least({ 1, 6 })) {
    path_movement_methods.insert(path_movement_methods.end(), {
        { "get_angle", LUA_TO_C_BIND(&PathMovement::get_angle) },
    });
  }

  path_movement_methods.insert(
        path_movement_methods.end(),
        movement_common_methods.begin(),
        movement_common_methods.end()
  );
  register_type(
      movement_path_module_name,
      {},
      path_movement_methods,
      metamethods
  );

  // Random path movement.
  std::vector<luaL_Reg> random_path_movement_methods = {
      { "get_speed", LUA_TO_C_BIND(random_path_get_speed) },
      { "set_speed", LUA_TO_C_BIND(random_path_set_speed) },
  };
  if (CurrentQuest::is_format_at_least({ 1, 6 })) {
    random_path_movement_methods.insert(random_path_movement_methods.end(), {
        { "get_angle", LUA_TO_C_BIND(random_path_get_angle) },
    });
  }
  random_path_movement_methods.insert(
        random_path_movement_methods.end(),
        movement_common_methods.begin(),
        movement_common_methods.end()
  );
  register_type(
      movement_random_path_module_name,
      {},
      random_path_movement_methods,
      metamethods
  );

  // Path finding movement.
  std::vector<luaL_Reg> path_finding_movement_methods = {
      { "set_target", LUA_TO_C_BIND(path_finding_set_target) },
      { "get_speed", LUA_TO_C_BIND(path_finding_get_speed) },
      { "set_speed", LUA_TO_C_BIND(path_finding_set_speed) },
  };
  if (CurrentQuest::is_format_at_least({ 1, 6 })) {
    path_finding_movement_methods.insert(path_finding_movement_methods.end(), {
        { "get_angle", LUA_TO_C_BIND(path_finding_get_angle) },
    });
  }
  path_finding_movement_methods.insert(
        path_finding_movement_methods.end(),
        movement_common_methods.begin(),
        movement_common_methods.end()
  );
  register_type(
      movement_path_finding_module_name,
      {},
      path_finding_movement_methods,
      metamethods
  );

  // Circle movement.
  std::vector<luaL_Reg> circle_movement_methods = {
      { "set_center", LUA_TO_C_BIND(circle_set_center) },
      { "get_radius", LUA_TO_C_BIND(&CircleMovement::get_radius) },
      { "set_radius", LUA_TO_C_BIND(&CircleMovement::set_radius) },
      { "get_radius_speed", LUA_TO_C_BIND(&CircleMovement::get_radius_speed) },
      { "set_radius_speed", LUA_TO_C_BIND(&CircleMovement::set_radius_speed) },
      { "is_clockwise", LUA_TO_C_BIND(&CircleMovement::is_clockwise) },
      { "set_clockwise", LUA_TO_C_BIND(circle_set_clockwise) },
      { "get_initial_angle", LUA_TO_C_BIND(circle_get_initial_angle) },
      { "set_initial_angle", LUA_TO_C_BIND(circle_set_initial_angle) },
      { "get_angle_speed", LUA_TO_C_BIND(circle_get_angle_speed) },
      { "set_angle_speed", LUA_TO_C_BIND(circle_set_angle_speed) },
      { "get_max_rotations", LUA_TO_C_BIND(&CircleMovement::get_max_rotations) },
      { "set_max_rotations", LUA_TO_C_BIND(&CircleMovement::set_max_rotations) },
      { "get_duration", LUA_TO_C_BIND(circle_get_duration) },
      { "set_duration", LUA_TO_C_BIND(circle_set_duration) },
      { "get_loop_delay", LUA_TO_C_BIND(circle_get_loop_delay) },
      { "set_loop_delay", LUA_TO_C_BIND(circle_set_loop_delay) }
  };
  if (CurrentQuest::is_format_at_least({ 1, 6 })) {
    circle_movement_methods.insert(circle_movement_methods.end(), {
        { "get_center", LUA_TO_C_BIND(circle_get_center) },
        { "get_angle_from_center", LUA_TO_C_BIND(&CircleMovement::get_angle_from_center) },
        { "set_angle_from_center", LUA_TO_C_BIND(&CircleMovement::set_angle_from_center) },
        { "get_angular_speed", LUA_TO_C_BIND(&CircleMovement::get_angular_speed) },
        { "set_angular_speed", LUA_TO_C_BIND(&CircleMovement::set_angular_speed) },
    });
  }
  circle_movement_methods.insert(
        circle_movement_methods.end(),
        movement_common_methods.begin(),
        movement_common_methods.end()
  );
  register_type(
      movement_circle_module_name,
      {},
      circle_movement_methods,
      metamethods
  );

  // Jump movement.
  std::vector<luaL_Reg> jump_movement_methods = {
      { "get_direction8", LUA_TO_C_BIND(&JumpMovement::get_direction8) },
      { "set_direction8", LUA_TO_C_BIND(&JumpMovement::set_direction8) },
      { "get_distance", LUA_TO_C_BIND(&JumpMovement::get_distance) },
      { "set_distance", LUA_TO_C_BIND(&JumpMovement::set_distance) },
      { "get_speed", LUA_TO_C_BIND(&JumpMovement::get_speed) },
      { "set_speed", LUA_TO_C_BIND(&JumpMovement::set_speed) },
  };
  jump_movement_methods.insert(
        jump_movement_methods.end(),
        movement_common_methods.begin(),
        movement_common_methods.end()
  );
  register_type(
      movement_jump_module_name,
      {},
      jump_movement_methods,
      metamethods
  );

  // Pixel movement.
  std::vector<luaL_Reg> pixel_movement_methods = {
      { "get_trajectory", LUA_TO_C_BIND(pixel_get_trajectory) },
      { "set_trajectory", LUA_TO_C_BIND(pixel_set_trajectory) },
      { "get_loop", LUA_TO_C_BIND(&PixelMovement::get_loop) },
      { "set_loop", LUA_TO_C_BIND(pixel_set_loop) },
      { "get_delay", LUA_TO_C_BIND(pixel_get_delay) },
      { "set_delay", LUA_TO_C_BIND(pixel_set_delay) },
  };
  pixel_movement_methods.insert(
        pixel_movement_methods.end(),
        movement_common_methods.begin(),
        movement_common_methods.end()
  );
  register_type(
      movement_pixel_module_name,
      {},
      pixel_movement_methods,
      metamethods
  );

  // Create the table that will store the movements applied to x,y points.
                                  // ...
  lua_newtable(current_l);
                                  // ... movements
  lua_newtable(current_l);
                                  // ... movements meta
  lua_pushstring(current_l, "v");
                                  // ... movements meta "v"
  lua_setfield(current_l, -2, "__mode");
                                  // ... movements meta
  lua_setmetatable(current_l, -2);
                                  // ... movements
  lua_setfield(current_l, LUA_REGISTRYINDEX, "sol.movements_on_points");
                                  // ...
}

/**
 * \brief Returns whether a value is a userdata of type movement.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return \c true if the value at this index is a movement.
 */
bool LuaContext::is_movement(lua_State* l, int index) {
  return is_straight_movement(l, index)
      || is_random_movement(l, index)
      || is_target_movement(l, index)
      || is_path_movement(l, index)
      || is_random_path_movement(l, index)
      || is_path_finding_movement(l, index)
      || is_circle_movement(l, index)
      || is_jump_movement(l, index)
      || is_pixel_movement(l, index);
}

/**
 * \brief Checks that the userdata at the specified index of the stack is a
 * movement (of any subtype) and returns it.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return The movement.
 */
std::shared_ptr<Movement> LuaContext::check_movement(lua_State* l, int index) {

  if (is_movement(l, index)) {
    const ExportableToLuaPtr& userdata = *(static_cast<ExportableToLuaPtr*>(
      lua_touserdata(l, index)
    ));
    return std::static_pointer_cast<Movement>(userdata);
  }
  else {
    LuaTools::type_error(l, index, "movement");
  }
}

/**
 * \brief Pushes a movement userdata onto the stack.
 * \param l A Lua context.
 * \param movement A movement.
 */
void LuaContext::push_movement(lua_State* l, Movement& movement) {

  push_userdata(l, movement);
}

/**
 * \brief Starts moving an x,y point.
 *
 * The point is a Lua table with two fields x and y.
 * Fields x and y may be initially missing.
 *
 * \param movement The movement to apply to the points.
 * \param point_index Index of the x,y table in the Lua stack.
 */
void LuaContext::start_movement_on_point(
    const std::shared_ptr<Movement>& movement, int point_index
) {
  int x = 0;
  int y = 0;
                                  // ...
  lua_getfield(current_l, LUA_REGISTRYINDEX, "sol.movements_on_points");
                                  // ... movements
  push_movement(current_l, *movement);
                                  // ... movements movement
  lua_pushvalue(current_l, point_index);
                                  // ... movements movement xy
  lua_getfield(current_l, -1, "x");
                                  // ... movements movement xy x/nil
  if (lua_isnil(current_l, -1)) {
                                  // ... movements movement xy nil
    lua_pop(current_l, 1);
                                  // ... movements movement xy
    lua_pushinteger(current_l, 0);
                                  // ... movements movement xy 0
    lua_setfield(current_l, -2, "x");
                                  // ... movements movement xy
  }
  else {
                                  // ... movements movement xy x
    x = LuaTools::check_int(current_l, -1);
    lua_pop(current_l, 1);
                                  // ... movements movement xy
  }
  lua_getfield(current_l, -1, "y");
                                  // ... movements movement xy y/nil
  if (lua_isnil(current_l, -1)) {
                                  // ... movements movement xy nil
    lua_pop(current_l, 1);
                                  // ... movements movement xy
    lua_pushinteger(current_l, 0);
                                  // ... movements movement xy 0
    lua_setfield(current_l, -2, "y");
                                  // ... movements movement xy
  }
  else {
                                  // ... movements movement xy y
    y = LuaTools::check_int(current_l, -1);
    lua_pop(current_l, 1);
                                  // ... movements movement xy
  }

  lua_settable(current_l, -3);
                                  // ... movements
  lua_pop(current_l, 1);
                                  // ...
  movement->set_xy(x, y);

  // Tell the movement it is now controlling this table.
  movement->notify_object_controlled();
}

/**
 * \brief Stops moving an x,y point.
 * \param movement The movement to stop.
 */
void LuaContext::stop_movement_on_point(const std::shared_ptr<Movement>& movement) {

                                  // ...
  lua_getfield(current_l, LUA_REGISTRYINDEX, "sol.movements_on_points");
                                  // ... movements
  push_movement(current_l, *movement);
                                  // ... movements movement
  lua_pushnil(current_l);
                                  // ... movements movement nil
  lua_settable(current_l, -3);
                                  // ... movements
  lua_pop(current_l, 1);
                                  // ...
}

/**
 * \brief Updates all movements applied to x,y points.
 *
 * Movements applied to map entities or drawables are already updated
 * by the entity or the drawable.
 * This may change in the future in order to unify the handling of movements.
 */
void LuaContext::update_movements() {

  lua_getfield(current_l, LUA_REGISTRYINDEX, "sol.movements_on_points");
  std::vector<std::shared_ptr<Movement>> movements;
  lua_pushnil(current_l);  // First key.
  while (lua_next(current_l, -2)) {
    const std::shared_ptr<Movement>& movement = check_movement(current_l, -2);
    movements.push_back(movement);
    lua_pop(current_l, 1);  // Pop the value, keep the key for next iteration.
  }
  lua_pop(current_l, 1);  // Pop the movements table.

  // Work on a copy of the list because the list may be changed during the iteration.
  for (const std::shared_ptr<Movement>& movement : movements) {
    movement->update();
  }
}

/**
 * \brief Returns whether a value is a userdata of type straight movement.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return \c true if the value at this index is a straight movement.
 */
bool LuaContext::is_straight_movement(lua_State* l, int index) {
  return is_userdata(l, index, movement_straight_module_name);
}

/**
 * \brief Checks that the userdata at the specified index of the stack is a
 * straight movement and returns it.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return The movement.
 */
std::shared_ptr<StraightMovement> LuaContext::check_straight_movement(lua_State* l, int index) {

  return std::static_pointer_cast<StraightMovement>(check_userdata(
      l, index, movement_straight_module_name
  ));
}

/**
 * \brief Returns whether a value is a userdata of type random movement.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return \c true if the value at this index is a random movement.
 */
bool LuaContext::is_random_movement(lua_State* l, int index) {
  return is_userdata(l, index, movement_random_module_name);
}

/**
 * \brief Checks that the userdata at the specified index of the stack is a
 * random movement and returns it.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return The movement.
 */
std::shared_ptr<RandomMovement> LuaContext::check_random_movement(lua_State* l, int index) {
  return std::static_pointer_cast<RandomMovement>(check_userdata(
      l, index, movement_random_module_name
  ));
}

/**
 * \brief Returns whether a value is a userdata of type target movement.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return \c true if the value at this index is a target movement.
 */
bool LuaContext::is_target_movement(lua_State* l, int index) {
  return is_userdata(l, index, movement_target_module_name);
}

/**
 * \brief Checks that the userdata at the specified index of the stack is a
 * target movement and returns it.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return The movement.
 */
std::shared_ptr<TargetMovement> LuaContext::check_target_movement(lua_State* l, int index) {
  return std::static_pointer_cast<TargetMovement>(check_userdata(
      l, index, movement_target_module_name
  ));
}

/**
 * \brief Returns whether a value is a userdata of type path movement.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return \c true if the value at this index is a path movement.
 */
bool LuaContext::is_path_movement(lua_State* l, int index) {
  return is_userdata(l, index, movement_path_module_name);
}

/**
 * \brief Checks that the userdata at the specified index of the stack is a
 * path movement and returns it.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return The movement.
 */
std::shared_ptr<PathMovement> LuaContext::check_path_movement(lua_State* l, int index) {
  return std::static_pointer_cast<PathMovement>(check_userdata(
      l, index, movement_path_module_name
  ));
}

/**
 * \brief Checks that the userdata at the specified index of the stack is a
 * random path movement and returns it.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return The movement.
 */
std::shared_ptr<RandomPathMovement> LuaContext::check_random_path_movement(lua_State* l, int index) {
  return std::static_pointer_cast<RandomPathMovement>(check_userdata(
      l, index, movement_random_path_module_name
  ));
}

/**
 * \brief Returns whether a value is a userdata of type random path movement.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return \c true if the value at this index is a random path movement.
 */
bool LuaContext::is_random_path_movement(lua_State* l, int index) {
  return is_userdata(l, index, movement_random_path_module_name);
}

/**
 * \brief Returns whether a value is a userdata of type path finding movement.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return \c true if the value at this index is a path finding  movement.
 */
bool LuaContext::is_path_finding_movement(lua_State* l, int index) {
  return is_userdata(l, index, movement_path_finding_module_name);
}

/**
 * \brief Checks that the userdata at the specified index of the stack is a
 * path finding movement and returns it.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return The movement.
 */
std::shared_ptr<PathFindingMovement> LuaContext::check_path_finding_movement(lua_State* l, int index) {
  return std::static_pointer_cast<PathFindingMovement>(check_userdata(
      l, index, movement_path_finding_module_name
  ));
}

/**
 * \brief Returns whether a value is a userdata of type circle movement.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return \c true if the value at this index is a circle movement.
 */
bool LuaContext::is_circle_movement(lua_State* l, int index) {
  return is_userdata(l, index, movement_circle_module_name);
}

/**
 * \brief Checks that the userdata at the specified index of the stack is a
 * circle movement and returns it.
 * \param l a Lua context
 * \param index An index in the stack.
 * \return The movement.
 */
std::shared_ptr<CircleMovement> LuaContext::check_circle_movement(lua_State* l, int index) {
  return std::static_pointer_cast<CircleMovement>(check_userdata(
      l, index, movement_circle_module_name
  ));
}

/**
 * \brief Returns whether a value is a userdata of type jump movement.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return \c true if the value at this index is a jump movement.
 */
bool LuaContext::is_jump_movement(lua_State* l, int index) {
  return is_userdata(l, index, movement_jump_module_name);
}

/**
 * \brief Checks that the userdata at the specified index of the stack is a
 * jump movement and returns it.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return The movement.
 */
std::shared_ptr<JumpMovement> LuaContext::check_jump_movement(lua_State* l, int index) {
  return std::static_pointer_cast<JumpMovement>(check_userdata(
      l, index, movement_jump_module_name
  ));
}

/**
 * \brief Returns whether a value is a userdata of type pixel movement.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return \c true if the value at this index is a pixel movement.
 */
bool LuaContext::is_pixel_movement(lua_State* l, int index) {
  return is_userdata(l, index, movement_pixel_module_name);
}

/**
 * \brief Checks that the userdata at the specified index of the stack is a
 * pixel movement and returns it.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return The movement.
 */
std::shared_ptr<PixelMovement> LuaContext::check_pixel_movement(lua_State* l, int index) {
  return std::static_pointer_cast<PixelMovement>(check_userdata(
      l, index, movement_pixel_module_name
  ));
}

/**
 * \brief Calls the on_position_changed() method of a Lua movement.
 *
 * Does nothing if the method is not defined.
 *
 * \param movement A movement.
 * \param xy The new coordinates.
 */
void LuaContext::movement_on_position_changed(
    Movement& movement, const Point& xy) {

  run_on_main([this,&movement,&xy](lua_State* current_l){
                                    // ...
    push_movement(current_l, movement);
                                    // ... movement
    lua_getfield(current_l, LUA_REGISTRYINDEX, "sol.movements_on_points");
                                    // ... movement movements
    lua_pushvalue(current_l, -2);
                                    // ... movement movements movement
    lua_gettable(current_l, -2);
                                    // ... movement movements xy/nil
    if (!lua_isnil(current_l, -1)) {
                                    // ... movement movements xy
      lua_pushinteger(current_l, xy.x);
                                    // ... movement movements xy x
      lua_setfield(current_l, -2, "x");
                                    // ... movement movements xy
      lua_pushinteger(current_l, xy.y);
                                    // ... movement movements xy y
      lua_setfield(current_l, -2, "y");
                                    // ... movement movements xy
    }
    lua_pop(current_l, 2);
                                    // ... movement
    if (userdata_has_field(movement, "on_position_changed")) {
      on_position_changed(xy);
    }
    lua_pop(current_l, 1);
  });
}

/**
 * \brief Calls the on_obstacle_reached() method of a Lua movement.
 *
 * Does nothing if the method is not defined.
 *
 * \param movement A movement.
 */
void LuaContext::movement_on_obstacle_reached(Movement& movement) {

  if (!userdata_has_field(movement, "on_obstacle_reached")) {
    return;
  }

  run_on_main([this,&movement](lua_State* l){
    push_movement(l, movement);
    on_obstacle_reached();
    lua_pop(l, 1);
  });
}

/**
 * \brief Calls the on_changed() method of a Lua movement.
 *
 * Does nothing if the method is not defined.
 *
 * \param movement A movement.
 */
void LuaContext::movement_on_changed(Movement& movement) {

  if (!userdata_has_field(movement, "on_changed")) {
    return;
  }

  run_on_main([this,&movement](lua_State* l){
    push_movement(l, movement);
    on_changed();
    lua_pop(l, 1);
  });
}

/**
 * \brief Calls the on_finished() method of a Lua movement.
 *
 * Does nothing if the method is not defined.
 *
 * \param movement A movement.
 */
void LuaContext::movement_on_finished(Movement& movement) {

  if (!userdata_has_field(movement, "on_finished")) {
    return;
  }

  run_on_main([this,&movement](lua_State* l){
    push_movement(l, movement);
    on_finished();
    lua_pop(l, 1);
  });
}

}
