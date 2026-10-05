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
#include "solarus/core/Size.h"
#include "solarus/graphics/SoftwareVideoMode.h"
#include "solarus/graphics/Video.h"
#include "solarus/lua/LuaBind.h"
#include "solarus/lua/LuaContext.h"
#include "solarus/lua/LuaTools.h"
#include <lua.hpp>

namespace Solarus {

/**
 * Name of the Lua table representing the video module.
 */
const std::string LuaContext::video_module_name = "sol.video";

/**
 * \brief Implementation of sol.video.get_mode().
 * \param context The Lua context that is calling this function.
 * \return Name of the current mode.
 */
static const std::string& get_mode(LuaContext& context) {
  context.warning_deprecated(
      { 1, 6 },
      "sol.video.get_mode()",
      "Use sol.video.get_shader() instead."
  );

  const SoftwareVideoMode& mode = Video::get_video_mode();
  return mode.get_name();
}

/**
 * \brief Implementation of sol.video.set_mode().
 * \param context The Lua context that is calling this function.
 * \param mode_name The name of the new mode.
 */
static void set_mode(LuaContext& context, const std::string& mode_name) {
  context.warning_deprecated(
      { 1, 6 },
      "sol.video.set_mode()",
      "Use sol.video.set_shader() instead."
  );

  const SoftwareVideoMode* mode = Video::get_video_mode_by_name(mode_name);

  if (mode != nullptr && Video::get_video_mode().get_name() != mode_name) {
    Video::set_video_mode(*mode);
  }
}

/**
 * \brief Implementation of sol.video.switch_mode().
 * \param context The Lua context that is calling this function.
 */
static void switch_mode(LuaContext& context) {
  context.warning_deprecated(
      { 1, 6 },
      "sol.video.switch_mode()",
      "Use sol.video.set_shader() instead."
  );

  Video::switch_video_mode();
}

/**
 * \brief Implementation of sol.video.get_modes().
 * \param context The Lua context that is calling this function.
 * \return The number of values to return to Lua.
 */
static LuaBind::OnStack get_modes(LuaContext& context) {
  context.warning_deprecated(
      { 1, 6 },
      "sol.video.get_modes()",
      "Use sol.main.get_resource_ids(\"shader\") instead."
  );

  lua_State* L = context.get_internal_state();
  const std::vector<const SoftwareVideoMode*>& modes =
      Video::get_video_modes();

  lua_createtable(L, static_cast<int>(modes.size()), 0);
  int i = 1;
  for (const SoftwareVideoMode* mode: modes) {
    lua_pushstring(L, mode->get_name().c_str());
    lua_rawseti(L, -2, i);
    ++i;
  }

  return {1};
}

/**
 * \brief Implementation of sol.video.is_mode_supported().
 * \param context The Lua context that is calling this function.
 * \param mode_name The name of the video mode to check.
 * \return Does the mode exist and is it supported.
 */
static bool is_mode_supported(LuaContext& context,
    const std::string& mode_name) {
  context.warning_deprecated(
      { 1, 6 },
      "sol.video.is_mode_supported()",
      "Use sol.shader.create() instead."
  );

  const SoftwareVideoMode* mode = Video::get_video_mode_by_name(mode_name);
  return (mode != nullptr && Video::is_mode_supported(*mode));
}

/**
 * \brief Implementation of sol.video.set_fullscreen().
 * \param fullscreen_arg The new value, true if absent.
 */
static void set_fullscreen(std::optional<bool> fullscreen_arg) {
  bool fullscreen = fullscreen_arg.value_or(true);
  Video::set_fullscreen(fullscreen);
}

/**
 * \brief Implementation of sol.video.set_cursor_visible().
 * \param visible_cursor_arg The new value, true if absent.
 */
static void set_cursor_visible(std::optional<bool> visible_cursor_arg) {
  bool visible_cursor = visible_cursor_arg.value_or(true);
  Video::set_cursor_visible(visible_cursor);
}

/**
 * \brief Implementation of sol.video.get_quest_size().
 * \return The quest size as expressed as a tuple.
 */
static std::tuple<int, int> get_quest_size() {
  const Size& quest_size = Video::get_quest_size();
  return std::tuple(quest_size.width, quest_size.height);
}

/**
 * \brief Implementation of sol.video.get_window_size().
 * \return The window size as expressed as a tuple.
 */
static std::tuple<int, int> get_window_size() {
  const Size& window_size = Video::get_window_size();
  return std::tuple(window_size.width, window_size.height);
}

/**
 * \brief Implementation of sol.video.set_window_size().
 * \param L The Lua context that is calling this function.
 * \param width The new width of the window, should be positive.
 * \param height The new height of the window, should be positive.
 */
static void set_window_size(lua_State* L, int width, int height) {
  if (width <= 0) {
    LuaTools::arg_error(L, 1, "Window width must be positive");
  }
  if (height <= 0) {
    LuaTools::arg_error(L, 2, "Window height must be positive");
  }

  Video::set_window_size(Size(width, height));
}

/**
 * \brief Implementation of sol.video.reset_window_size().
 * \param context The Lua context that is calling this function.
 */
static void reset_window_size(LuaContext& context) {
  context.warning_deprecated(
      { 1, 6 },
      "sol.video.reset_window_size()",
      "Use sol.video.set_window_size() instead."
  );

  Video::reset_window_size();
}

/**
 * \brief Implementation of sol.video.get_shader().
 * \param L The Lua context that is calling this function.
 * \return The number of values to return to Lua.
 */
static LuaBind::OnStack get_shader(lua_State* L) {
  const ShaderPtr& shader = Video::get_shader();
  if (shader == nullptr) {
    lua_pushnil(L);
  } else {
    LuaContext::push_shader(L, *shader);
  }
  return {1};
}

/**
 * \brief Implementation of sol.video.set_shader().
 * \param L The Lua context that is calling this function.
 */
static void set_shader(lua_State* L) {
  ShaderPtr shader = nullptr;
  if (!lua_isnil(L, 1)) {
    if (LuaContext::is_shader(L, 1)) {
      shader = LuaContext::check_shader(L, 1);
    } else {
      LuaTools::type_error(L, 2, "shader or nil");
    }
  }
  Video::set_shader(shader);
}

/**
 * \brief Initializes the video features provided to Lua.
 */
void LuaContext::register_video_module() {

  std::vector<luaL_Reg> functions = {
      { "get_window_title", LUA_TO_C_BIND(Video::get_window_title) },
      { "set_window_title", LUA_TO_C_BIND(Video::set_window_title) },
      { "get_mode", LUA_TO_C_BIND(get_mode) },
      { "set_mode", LUA_TO_C_BIND(set_mode) },
      { "switch_mode", LUA_TO_C_BIND(switch_mode) },
      { "is_mode_supported", LUA_TO_C_BIND(is_mode_supported) },
      { "get_modes", LUA_TO_C_BIND(get_modes) },
      { "is_fullscreen", LUA_TO_C_BIND(Video::is_fullscreen) },
      { "set_fullscreen", LUA_TO_C_BIND(set_fullscreen) },
      { "is_cursor_visible", LUA_TO_C_BIND(Video::is_cursor_visible) },
      { "set_cursor_visible", LUA_TO_C_BIND(set_cursor_visible) },
      { "get_quest_size", LUA_TO_C_BIND(get_quest_size) },
      { "get_window_size", LUA_TO_C_BIND(get_window_size) },
      { "set_window_size", LUA_TO_C_BIND(set_window_size) },
      { "reset_window_size", LUA_TO_C_BIND(reset_window_size) }
  };
  if (CurrentQuest::is_format_at_least({ 1, 6 })) {
    functions.insert(functions.end(), {
      { "get_shader", LUA_TO_C_BIND(get_shader) },
      { "set_shader", LUA_TO_C_BIND(set_shader) },
      { "set_geometry_mode", LUA_TO_C_BIND(Video::set_geometry_mode) }
    });
  }

  register_functions(video_module_name, functions);
  lua_getglobal(current_l, "sol");
                                  // ... sol
  lua_getfield(current_l, -1, "video");
                                  // ... sol video
  lua_setfield(current_l, LUA_REGISTRYINDEX, video_module_name.c_str());
                                  // ... sol
  lua_pop(current_l, 1);
}

/**
 * \brief Pushes the sol.video table onto the stack.
 * \param l A Lua state.
 */
void LuaContext::push_video(lua_State* l) {
  lua_getfield(l, LUA_REGISTRYINDEX, video_module_name.c_str());
}

/**
 * \brief Calls sol.video.on_draw() if it exists.
 * \param screen The destination surface representing the screen.
 */
void LuaContext::video_on_draw(const SurfacePtr &screen) {

  if (!CurrentQuest::is_format_at_least({ 1, 6 })) {
    return;
  }
  push_video(current_l);
  on_draw(screen);
  lua_pop(current_l, 1);
}

}

