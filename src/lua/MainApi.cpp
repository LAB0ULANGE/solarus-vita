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
#include "solarus/core/Savegame.h"
#include "solarus/core/Geometry.h"
#include "solarus/core/MainLoop.h"
#include "solarus/core/QuestFiles.h"
#include "solarus/core/QuestDatabase.h"
#include "solarus/core/QuestProperties.h"
#include "solarus/core/Settings.h"
#include "solarus/core/System.h"
#include "solarus/lua/LuaBind.h"
#include "solarus/lua/LuaContext.h"
#include "solarus/lua/LuaTools.h"
#include <lua.hpp>

namespace Solarus {

/**
 * Name of the Lua table representing the main module of Solarus.
 */
const std::string LuaContext::main_module_name = "sol.main";

template<typename T>
auto empty_to_opt(const T& val) -> std::optional<T> {
  if(val.empty()) {
    return std::nullopt;
  }
  return val;
}



/**
 * \brief Implementation of sol.main.get_solarus_version().
 * \return Solarus version string
 */
static const std::string& get_solarus_version() {
  static const std::string version(SOLARUS_VERSION);
  return version;
}

/**
 * \brief Implementation of sol.main.get_quest_format().
 * \return Quest format string
 */
static std::optional<std::string> get_quest_format() {
  return empty_to_opt(CurrentQuest::get_properties().get_solarus_version());
}

/**
 * @brief Implementation of sol.main.load_file()
 * @param ctx the lua context
 * @param filename name of the file to load
 * @return 1 OnStack chunk value
 */
static LuaBind::OnStack load_file(LuaContext& ctx, const std::string& filename) {
  if(!ctx.load_file(filename)) {
    lua_pushnil(ctx.get_internal_state());
  }
  return {1};
}

/**
 * @brief Implementation of sol.main.do_file()
 * @param ctx the lua context
 * @param filename name of the file to execute
 * @return 0 OnStack values
 */
static void do_file(LuaContext& ctx, const std::string& filename) {
  ctx.do_file(filename);
}

/**
 * @brief Implementation of sol.main.reset()
 * @param ctx the lua context
 */
static void reset(LuaContext& ctx) {
  ctx.get_main_loop().set_resetting();
}

/**
 * @brief Implementation of sol.main.exit()
 * @param ctx the lua context
 */
static void exit(LuaContext& ctx) {
  ctx.get_main_loop().set_exiting();
}

/**
 * @brief Implementation of sol.main.get_quest_write_dir()
 * @return [string]
 */
static std::optional<std::string> get_quest_write_dir() {
  return empty_to_opt(QuestFiles::get_quest_write_dir());
}

/**
 * @brief Implementation of sol.main.set_quest_write_dir(dir)
 * @param [string] dir
 */
static void set_quest_write_dir(const std::optional<std::string>& dir) {
  QuestFiles::set_quest_write_dir(dir.value_or(""));
}

/**
 * @brief Implementation of sol.main.load_settings(file_name)
 * @param ctx the lua context
 * @param ofile_name [string] settings file name name
 * @return bool : wether the loading was sucessful
 */
static bool load_settings(LuaContext& ctx, const std::optional<std::string>& ofile_name) {
  auto file_name = ofile_name ? *ofile_name : "settings.dat";
  if (QuestFiles::get_quest_write_dir().empty()) {
    LuaTools::error(ctx.get_internal_state(), "Cannot load settings: no write directory was specified in quest.dat");
  }

  bool success = false;
  if (QuestFiles::data_file_exists(file_name) &&
      !QuestFiles::data_file_is_dir(file_name)) {
    Settings settings;
    success = settings.load(file_name);
    if (success) {
      settings.apply_to_quest();
    }
  }

  return success;
}

/**
 * @brief Implementation of sol.main.save_settings(file_name)
 * @param ctx the lua context
 * @param ofile_name [string] : optional file name
 * @return bool : success
 */
static bool save_settings(LuaContext& ctx, const std::optional<std::string>& ofile_name) {
  auto file_name = ofile_name ? *ofile_name : "settings.dat";
  if (QuestFiles::get_quest_write_dir().empty()) {
    LuaTools::error(ctx.get_internal_state(), "Cannot save settings: no write directory was specified in quest.dat");
  }

  Settings settings;
  settings.set_from_quest();
  return settings.save(file_name);
}

/**
 * @brief Implementation of sol.main.get_distance(x1, y1, x2, y2)
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @return integer euclidian distance between pairs of coords
 */
static int get_distance(int x1, int y1, int x2, int y2) {
  return Geometry::get_distance(x1, y1, x2, y2);
}

/**
 * @brief Implementation of sol.main.get_angle(x1, y1, x2, y2)
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @return angle in radian between x axis and vector from p1 to p2
 */
static double get_angle(int x1, int y1, int x2, int y2) {
  return Geometry::get_angle(x1, y1, x2, y2);
}

/**
 * @brief Implementation of sol.main.get_type(obj)
 * @param ctx the lua context
 * @return string : typename
 */
static std::string get_type(lua_State* L) {
  luaL_checkany(L, 1);
  return LuaTools::get_type_name(L, 1);
}


/**
 * @brief Implementation of sol.main.get_metatable(type_name)
 * @param ctx the lua context
 * @param type_name a name
 * @return [table] : type's metatable if any
 */
static LuaBind::OnStack get_metatable(LuaContext& ctx, const std::string& type_name) {
  auto l = ctx.get_internal_state();
  luaL_getmetatable(l, (std::string("sol.") + type_name).c_str());
  return {1};
}

/**
 * @brief Implementation of sol.main.get_quest_version()
 * @return [string] : optional quest version string
 */
static std::optional<std::string> get_quest_version() {
  return empty_to_opt(CurrentQuest::get_properties().get_quest_version());
}

/**
 * @brief Implementation of sol.main.get_resource_ids(type)
 * @param type string : a resource type tag
 * @return array : list of resource ids
 */
static std::vector<std::string> get_resource_ids(ResourceType type) {
  const auto& elements = CurrentQuest::get_database().get_resource_elements(type);
  auto arr = std::vector<std::string>();
  arr.reserve(elements.size());
  for(const auto& [k,v] : elements) {
    arr.push_back(k);
  }
  return arr;
}

/**
 * @brief Implementation of sol.main.get_resource_description(type, id)
 * @param ctx the lua context
 * @param type resource type name
 * @param id
 * @return [string] : resource description
 */
static std::optional<std::string> get_resource_description(LuaContext& ctx, ResourceType type, const std::string& id) {
  const QuestDatabase& database = CurrentQuest::get_database();
  if (!database.resource_exists(type, id)) {
    LuaTools::arg_error(ctx.get_internal_state(), 2, "No such resource element: '" + id + "'");
  }

  return empty_to_opt(database.get_description(type, id));
}

/**
 * @brief Implementation of sol.main.add_resource(type, id, [description])
 * @param ctx the lua context
 * @param type resource type name
 * @param id resource id
 * @param description optional description
 */
static void add_resource(LuaContext& ctx, ResourceType type, const std::string& id, const std::optional<std::string>& description) {
  QuestDatabase& database = CurrentQuest::get_database();
  if (database.resource_exists(type, id)) {
    LuaTools::arg_error(ctx.get_internal_state(), 2, "Resource element already exists: '" + id + "'");
  }

  database.add(type, id, description ? *description : "");
}

/**
 * @brief Implementation of sol.main.remove_resource(type, id)
 * @param ctx the lua context
 * @param type resource type name
 * @param id resource id
 */
static void remove_resource(LuaContext& ctx, ResourceType type, const std::string& id) {
  QuestDatabase& database = CurrentQuest::get_database();
  if (!database.resource_exists(type, id)) {
    LuaTools::arg_error(ctx.get_internal_state(), 2, "No such resource element: '" + id + "'");
  }

  database.remove(type, id);
}

/**
 * @brief Implementation of sol.main.get_game()
 * @param ctx the lua context
 * @return [game] : the current game if any
 */
static Savegame* get_game(LuaContext& ctx) {
  Game* game = ctx.get_main_loop().get_game();
  if (game != nullptr && game->has_savegame()) {
    return &game->get_savegame();
  }
  return nullptr;
}

/**
 * @brief Implementation of sol.main.rawget(t, k)
 * @param l the lua state
 * @return
 */
static LuaBind::OnStack rawget(lua_State* l) {
  switch (lua_type(l, 1)) {
  case LUA_TUSERDATA:
    return {LuaContext::userdata_rawget_as_table(l)};
  case LUA_TTABLE:
    if (2 < LuaTools::check_mintop(l, 2)) {
      lua_settop(l, 2);
    }
    lua_rawget(l, 1);
    return {1};
  default:
    LuaTools::type_error(l, 1, "table or userdata");
  }
}

/**
 * @brief Implementation of sol.main.rawset(t, k, v)
 * @param l the lua state
 * @return
 */
static LuaBind::OnStack rawset(lua_State* l) {
  switch (lua_type(l, 1)) {
  case LUA_TUSERDATA:
    LuaContext::userdata_meta_newindex_as_table(l);
    // Take advantage of the fact newindex leaves the arguments in place.
    lua_settop(l, 1);
    return {1};
  case LUA_TTABLE:
    if (3 < LuaTools::check_mintop(l, 3)) {
      lua_settop(l, 3);
    }
    lua_rawset(l, 1);
    return {1};
  default:
    LuaTools::type_error(l, 1, "table or userdata");
  }
}

/**
 * \brief Initializes the main features provided to Lua.
 */
void LuaContext::register_main_module() {

  std::vector<luaL_Reg> functions = {
      { "get_solarus_version", LUA_TO_C_BIND(get_solarus_version) },
      { "get_quest_format", LUA_TO_C_BIND(get_quest_format) },
      { "load_file", LUA_TO_C_BIND(Solarus::load_file) },
      { "do_file", LUA_TO_C_BIND(Solarus::do_file) },
      { "reset", LUA_TO_C_BIND(reset) },
      { "exit", LUA_TO_C_BIND(Solarus::exit) },
      { "get_elapsed_time", LUA_TO_C_BIND(System::now_ms) },
      { "get_quest_write_dir", LUA_TO_C_BIND(get_quest_write_dir) },
      { "set_quest_write_dir", LUA_TO_C_BIND(set_quest_write_dir) },
      { "load_settings", LUA_TO_C_BIND(load_settings) },
      { "save_settings", LUA_TO_C_BIND(save_settings) },
      { "get_distance", LUA_TO_C_BIND(get_distance) },
      { "get_angle", LUA_TO_C_BIND(get_angle) },
      { "get_type", LUA_TO_C_BIND(get_type) },
      { "get_metatable", LUA_TO_C_BIND(get_metatable) },
      { "get_os", LUA_TO_C_BIND(System::get_os) }
  };
  if (CurrentQuest::is_format_at_least({ 1, 6 })) {
    functions.insert(functions.end(), {
        { "get_quest_version", LUA_TO_C_BIND(get_quest_version) },
        { "get_resource_ids", LUA_TO_C_BIND(get_resource_ids) },
        { "resource_exists", LUA_TO_C_BIND(CurrentQuest::resource_exists)},
        { "get_resource_description", LUA_TO_C_BIND(get_resource_description) },
        { "add_resource", LUA_TO_C_BIND(add_resource) },
        { "remove_resource", LUA_TO_C_BIND(remove_resource) },
        { "get_game", LUA_TO_C_BIND(get_game) },
    });
  }
  if (CurrentQuest::is_format_at_least({ 2, 0 })) {
    functions.insert(functions.end(), {
        { "rawget", LUA_TO_C_BIND(rawget) },
        { "rawset", LUA_TO_C_BIND(rawset) },
    });
  }

  register_functions(main_module_name, functions);

  // Store sol.main in the registry to access it safely
  // from C++ (and also slightly faster).
  // After that, the engine will never rely on the existence of a global
  // value called "sol". The user can therefore do whatever he wants, including
  // renaming the sol global table to something else in the unlikely case where
  // another Lua library called "sol" is required, or if he simply does not
  // like the name "sol".

                                  // ...
  lua_getglobal(current_l, "sol");
                                  // ... sol
  lua_getfield(current_l, -1, "main");
                                  // ... sol main
  lua_setfield(current_l, LUA_REGISTRYINDEX, main_module_name.c_str());
                                  // ... sol
  lua_pop(current_l, 1);
                                  // ...
}

/**
 * \brief Pushes the sol.main table onto the stack.
 * \param l A Lua state.
 */
void LuaContext::push_main(lua_State* l) {

  lua_getfield(l, LUA_REGISTRYINDEX, main_module_name.c_str());
}

/**
 * \brief Returns whether a value is the sol.main table.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return \c true if the value at this index is sol.main.
 */
bool LuaContext::is_main(lua_State* l, int index) {
  push_main(l);
  bool result = lua_equal(l, index, -1);
  lua_pop(l, 1);
  return result;
}

/**
 * \brief Calls sol.main.on_started() if it exists.
 *
 * This function is called when the engine requests Lua to show an
 * initial screen, i.e. at the beginning of the program
 * or when the program is reset.
 */
void LuaContext::main_on_started() {

  push_main(current_l);
  on_started();
  lua_pop(current_l, 1);
}

/**
 * \brief Calls sol.main.on_finished() if it exists.
 *
 * This function is called when the program is reset or stopped.
 */
void LuaContext::main_on_finished() {

  push_main(current_l);
  on_finished();
  remove_timers(-1);  // Stop timers associated to sol.main.
  remove_menus(-1);  // Stop menus associated to sol.main.
  lua_pop(current_l, 1);
}

/**
 * \brief Calls sol.main.on_update() if it exists.
 *
 * This function is called at each cycle by the main loop.
 */
void LuaContext::main_on_update() {
  current_l = main_l; //Always execute main on the main thread
  push_main(current_l);
  on_update();
  menus_on_update(-1);
  lua_pop(current_l, 1);
}

/**
 * \brief Calls sol.main.on_draw() if it exists.
 * \param dst_surface The destination surface.
 */
void LuaContext::main_on_draw(const SurfacePtr& dst_surface) {

  push_main(current_l);
  on_draw(dst_surface);
  menus_on_draw(-1, dst_surface);
  lua_pop(current_l, 1);
}

/**
 * \brief Notifies Lua that an input event has just occurred.
 *
 * The appropriate callback in sol.main is triggered if it exists.
 *
 * \param event The input event to handle.
 * \return \c true if the event was handled and should stop being propagated.
 */
bool LuaContext::main_on_input(const InputEvent& event) {

  push_main(current_l);
  bool handled = on_input(event);
  if (!handled) {
    handled = menus_on_input(-1, event);
  }
  lua_pop(current_l, 1);
  return handled;
}

/**
 * \brief Notifies Lua that an input event has just occurred.
 *
 * The appropriate callback in sol.main is triggered if it exists.
 *
 * \param event The input event to handle.
 * \return \c true if the event was handled and should stop being propagated.
 */
bool LuaContext::main_on_control(const ControlEvent& event) {

  push_main(current_l);
  bool handled = on_command(event);
  if (!handled) {
    handled = menus_on_command(-1, event);
  }
  lua_pop(current_l, 1);
  return handled;
}

}

