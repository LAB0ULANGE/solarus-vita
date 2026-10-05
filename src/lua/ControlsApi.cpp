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

#include "solarus/core/ControlsDispatcher.h"

namespace Solarus {

const std::string LuaContext::controls_module_name = "sol.controls";

// Define marchaling for Commands
namespace LuaBind {
template<>
struct Marshalling<Command> {
    static inline Command check_arg(lua_State * L, int index, const CheckContext& context) {
    size_t length;
    if (const char * data = LuaTools::islstring(L, index, &length)) {
      return Controls::get_command_by_name(std::string(data, length));
    }
    LuaBind::type_error(context, L, index, "command");
  }

  static inline void push(lua_State * L, Command value) {
    const auto str = Controls::get_command_name(value);
    lua_pushlstring(L, str.c_str(), str.size());
  }
};

template<>
struct Marshalling<Axis> {
    static inline Axis check_arg(lua_State * L, int index, const CheckContext& context) {
      if (lua_isnil(L, index)){
        return Axis(AxisId::NONE);
      }
      size_t length;
      if (const char * data = LuaTools::islstring(L, index, &length)) {
        return Controls::get_axis_by_name(data);
      }
      type_error(context, L, index, "axis");
    }

    static inline void push(lua_State* L, const Axis& axis) {
      auto str = Controls::get_axis_name(axis);
      if(str.empty())
        lua_pushnil(L);
      else
        lua_pushstring(L, str.c_str());
    }
};

template<>
struct Marshalling<Solarus::Controls::JoypadBinding> {
    static inline Controls::JoypadBinding check_arg(lua_State * L, int index, const CheckContext& context) {
      if (lua_isnoneornil(L, index)){
        return Controls::JoypadBinding(JoyPadButton::INVALID);
      }
      size_t length;
      if (const char * data = LuaTools::islstring(L, index, &length)) {
        auto binding = Controls::JoypadBinding(data);
        if (!binding.is_invalid()){
          return binding;
        }
      }
      type_error(context, L, index, "joypadbinding");
    }

    static inline void push(lua_State* L, const Controls::JoypadBinding& binding) {
      if (binding.is_invalid())
        lua_pushnil(L);
      else
        lua_pushstring(L, binding.to_string().c_str());
    }
};

template<>
struct Marshalling<Controls::ControlAxisBinding> {
    static inline Controls::ControlAxisBinding check_arg(lua_State * L, int index, const CheckContext& context) {
      size_t length;
      if (const char * data = LuaTools::islstring(L, index, &length)) {
        auto cab = Controls::ControlAxisBinding::from_string(std::string(data, length));
        if (cab) {
          return cab.value();
        }
        error(context, L, index, std::string("invalid controlaxis binding : '") + data + "'");
      }
      type_error(context, L, index, "controlaxisbinding");
    }

    static inline void push(lua_State* L, const Controls::ControlAxisBinding& binding) {
      auto str = binding.to_string();
      lua_pushlstring(L, str.c_str(), str.size());
    }
};

template<>
struct Marshalling<Controls::JoypadAxisBinding>{
    static inline Controls::JoypadAxisBinding check_arg(lua_State * L, int index, const CheckContext& context) {
      size_t length;
      if (const char * data = LuaTools::islstring(L, index, &length)) {
        auto cab = Controls::JoypadAxisBinding::from_string(std::string(data, length));
        if (cab) {
          return cab.value();
        }
        error(context, L, index, std::string("invalid joypadaxisbinding : '") + data + "'");
      }
      type_error(context, L, index, "joypadaxisbinding");
    }

    static inline void push(lua_State* L, const Controls::JoypadAxisBinding& binding) {
      if (binding.invalid()) {
        lua_pushnil(L);
      }

      auto str = binding.to_string();
      lua_pushlstring(L, str.c_str(), str.size());
    }
};

}

/**
 * \brief Implementation of sol.controls.create_from_keyboard().
 * \return A newly created control object with default keyboard mapping
 */
static ControlsPtr create_from_keyboard() {
  return ControlsDispatcher::get().create_commands_from_keyboard();
}

/**
 * \brief Implementation of sol.controls.create_from_joypad().
 * \param joypad a joypad to be tied to the control object
 * \return A newly created control object with default joypad mapping
 */
static ControlsPtr create_from_joypad(const JoypadPtr& joypad) {
    return ControlsDispatcher::get().create_commands_from_joypad(joypad);
}

/**
 * \brief Implementation of so.controls:get_direction
 * \param cmds the controls instance
 * \return a direction or nil if nothing is pressed
 */
static std::optional<int> get_direction(Controls& cmds) {
  int wanted_direction8 = cmds.get_wanted_direction8();
  if (wanted_direction8 == -1) {
    return {};
  }
  else {
    return wanted_direction8;
  }
}

/**
 * \brief Implementation of sol.controls:capture_bindings
 * \param cmds the control object
 * \param cmd the command to capture
 * \param callback an optional callback for when capture suceeds
 */
static void capture_bindings(Controls& cmds, Command cmd, std::optional<LuaBind::Callback> callback) {
  cmds.customize(cmd, callback.value_or(LuaBind::Callback()));
}


/**
 * \brief Implementation of sol.controls:set_keyboard_binding
 * \param cmds the control object
 * \param cmd the command to capture
 * \param a valid key code or nil to unbind
 */
static void set_keyboard_binding(Controls& cmds, Command cmd, std::optional<InputEvent::KeyboardKey> key) {
  cmds.set_keyboard_binding(cmd, key.value_or(InputEvent::KeyboardKey::NONE));
}

/**
 * \brief Implementation of sol.controls:simulate_pressed.
 * \param cmds the control object
 * \param command the command to simulate
 */
static void simulate_pressed(ControlsPtr cmds, Command command) {
  LuaContext::run_on_main([cmds, command](lua_State*){
    cmds->command_pressed(command);
  });
}

/**
 * \brief Implementation of sol.controls:simulate_released.
 * \param cmds the control object
 * \param command the command to simulate
 */
static void simulate_released(ControlsPtr cmds, Command command) {
  LuaContext::run_on_main([cmds, command](lua_State*){
    cmds->command_released(command);
  });
}

/**
 * \brief Implementation of sol.controls:simulate_axis_moved.
 * \param cmds the control object
 * \param axis the axis to move
 * \param state new state to put the axis in
 */
static void simulate_axis_moved(ControlsPtr cmds, Axis axis, double state) {
  LuaContext::run_on_main([cmds, axis, state](lua_State*){
    cmds->command_axis_moved(axis, state);
  });
}

void LuaContext::register_controls_module() {

  // Functions of sol.commands
  const std::vector<luaL_Reg> functions = {
    { "create_from_keyboard", LUA_TO_C_BIND(create_from_keyboard)},
    { "create_from_joypad", LUA_TO_C_BIND(create_from_joypad)},
    { "set_analog_commands_enabled", LUA_TO_C_BIND(Controls::set_analog_commands_enabled)},
    { "are_analog_commands_enabled", LUA_TO_C_BIND(Controls::are_analog_commands_enabled)}
  };

  // Methods of the commands type.
  const std::vector<luaL_Reg> methods = {
    { "is_pressed", LUA_TO_C_BIND(&Controls::is_command_pressed)},
    { "get_axis_state", LUA_TO_C_BIND(&Controls::get_axis_state)},
    { "get_direction", LUA_TO_C_BIND(get_direction)},
    { "set_keyboard_binding", LUA_TO_C_BIND(set_keyboard_binding)},
    { "get_keyboard_binding", LUA_TO_C_BIND(&Controls::get_keyboard_binding)},
    { "set_joypad_binding", LUA_TO_C_BIND(&Controls::set_joypad_binding)},
    { "get_joypad_binding", LUA_TO_C_BIND(&Controls::get_joypad_binding)},
    { "get_keyboard_axis_binding", LUA_TO_C_BIND(&Controls::get_keyboard_axis_binding)},
    { "set_keyboard_axis_binding", LUA_TO_C_BIND(&Controls::set_keyboard_axis_binding)},
    { "set_joypad_axis_binding", LUA_TO_C_BIND(&Controls::set_joypad_axis_binding)},
    { "get_joypad_axis_binding", LUA_TO_C_BIND(&Controls::get_joypad_axis_binding)},

    { "set_keyboard_bindings", LUA_TO_C_BIND(&Controls::set_keyboard_bindings)},
    { "get_keyboard_bindings", LUA_TO_C_BIND(&Controls::get_keyboard_bindings)},
    { "set_joypad_bindings", LUA_TO_C_BIND(&Controls::set_joypad_bindings)},
    { "get_joypad_bindings", LUA_TO_C_BIND(&Controls::get_joypad_bindings)},
    { "get_keyboard_axis_bindings", LUA_TO_C_BIND(&Controls::get_keyboard_axis_bindings)},
    { "set_keyboard_axis_bindings", LUA_TO_C_BIND(&Controls::set_keyboard_axis_bindings)},
    { "set_joypad_axis_bindings", LUA_TO_C_BIND(&Controls::set_joypad_axis_bindings)},
    { "get_joypad_axis_bindings", LUA_TO_C_BIND(&Controls::get_joypad_axis_bindings)},

    { "get_effect", LUA_TO_C_BIND(&Controls::get_effect_string)},

    { "capture_bindings", LUA_TO_C_BIND(capture_bindings)},
    { "simulate_pressed", LUA_TO_C_BIND(simulate_pressed)},
    { "simulate_released", LUA_TO_C_BIND(simulate_released)},
    { "simulate_axis_moved", LUA_TO_C_BIND(simulate_axis_moved)},
    { "set_joypad", LUA_TO_C_BIND(&Controls::set_joypad)},
    { "get_joypad", LUA_TO_C_BIND(&Controls::get_joypad)},
    { "remove", LUA_TO_C_BIND(&Controls::remove)}
  };

  // Metamethods of the commands type
  const std::vector<luaL_Reg> metamethods = {
    { "__gc", userdata_meta_gc },
    { "__newindex", userdata_meta_newindex_as_table },
    { "__index", userdata_meta_index_as_table },
  };

  register_type(controls_module_name, functions, methods, metamethods);
}

/**
 * @brief push a commands userdata on the lua stack
 * @param current_l lua state
 * @param joypad
 */
void LuaContext::push_controls(lua_State* current_l, Controls& commands) {
  push_userdata(current_l, commands);
}

/**
 * @brief check wheter a value on the stack is a joypad
 * @param current_l lua state
 * @param index index on the stack
 * @return
 */
bool LuaContext::is_controls(lua_State* current_l, int index) {
  return is_userdata(current_l,index, controls_module_name);
}

/**
 * @brief return a Shared Joypad from the lua stack
 * @param current_l lua state
 * @param index index on the stack
 * @return a joypad if index point to a true joypad
 */
std::shared_ptr<Controls> LuaContext::check_controls(lua_State* current_l, int index) {
  return std::static_pointer_cast<Controls>(check_userdata(
                                              current_l,index,controls_module_name)
                                            );
}

/**
 * @brief Check a command string on the stack
 * @param l the lua state
 * @param index index of the object on the stack
 * @return command sum type
 */
Command LuaContext::check_command(lua_State* l, int index) {
  std::string name = LuaTools::check_string(l, index);

  return Controls::get_command_by_name(name);
}

} //Solarus

