/*
 * Copyright (C) 2018-2020 std::gregwar, Solarus - http://www.solarus-games.org
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
#include "solarus/lua/LuaBind.h"
#include "solarus/lua/LuaContext.h"
#include "solarus/lua/LuaTools.h"

namespace Solarus {

const std::string LuaContext::joypad_module_name = "sol.joypad";

/**
 * \brief Implementation of joypad:rumble(low_frequency_intensity, high_frequency_intensity, duration).
 * \param joypad The joypad to rumble.
 * \param low_frequency_intensity Strength of low frequency (left) rumble, in range 0-1.
 * \param high_frequency_intensity Strength of high frequency (right) rumble, in range 0-1.
 * \param duration Length of rumble in milliseconds.
 */
static void rumble(Joypad& joypad, double low_frequency_intensity, double high_frequency_intensity, int duration) {
  if (duration < 0) {
    Debug::error("negative rumble duration");
  }
  joypad.rumble(low_frequency_intensity, high_frequency_intensity, duration);
}

void LuaContext::register_joypad_module() {

  const std::vector<luaL_Reg> methods = {
    { "get_axis", LUA_TO_C_BIND(&Joypad::get_axis) },
    { "is_button_pressed", LUA_TO_C_BIND(&Joypad::is_button_pressed) },
    { "get_name", LUA_TO_C_BIND(&Joypad::get_name) },
    { "rumble", LUA_TO_C_BIND(rumble) },
    { "has_rumble", LUA_TO_C_BIND(&Joypad::has_rumble) },
    { "is_attached", LUA_TO_C_BIND(&Joypad::is_attached) }
  };

  const std::vector<luaL_Reg> metamethods = {
      { "__gc", userdata_meta_gc },
      { "__newindex", userdata_meta_newindex_as_table },
      { "__index", userdata_meta_index_as_table },
  };

  register_type(joypad_module_name, {}, methods, metamethods);
}

/**
 * @brief push a joypad userdata on the lua stack
 * @param current_l lua state
 * @param joypad
 */
void LuaContext::push_joypad(lua_State* current_l, Joypad& joypad) {
  push_userdata(current_l, joypad);
}

/**
 * @brief check wheter a value on the stack is a joypad
 * @param current_l lua state
 * @param index index on the stack
 * @return
 */
bool LuaContext::is_joypad(lua_State* current_l, int index) {
  return is_userdata(current_l,index,joypad_module_name);
}

/**
 * @brief return a Shared Joypad from the lua stack
 * @param current_l lua state
 * @param index index on the stack
 * @return a joypad if index point to a true joypad
 */
std::shared_ptr<Joypad> LuaContext::check_joypad(lua_State* current_l, int index) {
  return std::static_pointer_cast<Joypad>(check_userdata(
    current_l,index,joypad_module_name)
  );
}

// Events
bool LuaContext::on_joypad_axis_moved(Joypad& joypad, JoyPadAxis axis, double val) {
  if (!userdata_has_field(joypad, "on_axis_moved")) {
    return false;
  }

  push_joypad(current_l, joypad);
  bool handled = false;

  if(find_method("on_axis_moved")) {
    lua_pushstring(current_l,enum_to_name(axis).c_str());
    lua_pushnumber(current_l, val);
    bool success = call_function(3, 1, "on_axis_moved");

    if (!success) {
      // Something was wrong in the script: don't propagate the input to other objects.
      handled = true;
    }
    else {
      handled = lua_toboolean(current_l, -1);
      lua_pop(current_l, 1);
    }
  }
  lua_pop(current_l, 1);
  return handled;
}

bool LuaContext::on_joypad_button_pressed(Joypad& joypad, JoyPadButton button) {
  if (!userdata_has_field(joypad, "on_button_pressed")) {
    return false;
  }

  push_joypad(current_l, joypad);
  bool handled = false;

  if (find_method("on_button_pressed")) {
    lua_pushstring(current_l, enum_to_name(button).c_str());
    bool success = call_function(2, 1, "on_button_pressed");
    if (!success) {
      // Something was wrong in the script: don't propagate the input to other objects.
      handled = true;
    }
    else {
      handled = lua_toboolean(current_l, -1);
      lua_pop(current_l, 1);
    }
  }
  lua_pop(current_l, 1);
  return handled;
}

bool LuaContext::on_joypad_button_released(Joypad& joypad, JoyPadButton button) {
  if (!userdata_has_field(joypad, "on_button_released")) {
    return false;
  }

  push_joypad(current_l, joypad);
  bool handled = false;

  if (find_method("on_button_released")) {
    lua_pushstring(current_l, enum_to_name(button).c_str());

    bool success = call_function(2, 1, "on_button_released");

    if (!success) {
      // Something was wrong in the script: don't propagate the input to other objects.
      handled = true;
    }
    else {
      handled = lua_toboolean(current_l, -1);
      lua_pop(current_l, 1);
    }
  }
  lua_pop(current_l, 1);
  return handled;
}

bool LuaContext::on_joypad_removed(Joypad& joypad) {
  if (!userdata_has_field(joypad,"on_removed")) {
    return false;
  }
  push_joypad(current_l, joypad);

  bool handled = false;
  if (find_method("on_removed")) {
    bool success = call_function(1, 1, "on_removed");

    if (!success) {
      // Something was wrong in the script: don't propagate the input to other objects.
      handled = true;
    }
    else {
      handled = lua_toboolean(current_l, -1);
      lua_pop(current_l, 1);
    }
  }
  lua_pop(current_l, 1);
  return handled;
}

} // Solarus
