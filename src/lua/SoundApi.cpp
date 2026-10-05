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
#include "solarus/audio/Sound.h"
#include "solarus/core/CurrentQuest.h"
#include "solarus/core/MainLoop.h"
#include "solarus/core/ResourceProvider.h"
#include "solarus/lua/LuaContext.h"

namespace Solarus {

/**
 * Name of the Lua table representing the sound module.
 */
const std::string LuaContext::sound_module_name = "sol.sound";

/**
 * \brief Initializes the sound features provided to Lua.
 */
void LuaContext::register_sound_module() {

  if (!CurrentQuest::is_format_at_least({ 2, 0 })) {
    return;
  }

  // Functions of sol.sound.
  const std::vector<luaL_Reg> functions = {
      { "create", sound_api_create },
      { "stop_all", sound_api_stop_all },
  };

  // Methods of the sound type.
  const std::vector<luaL_Reg> methods = {
      { "play", sound_api_play },
      { "stop", sound_api_stop },
      { "is_playing", sound_api_is_playing },
      { "is_paused", sound_api_is_paused },
      { "set_paused", sound_api_set_paused },
      { "is_looped", sound_api_is_looped },
      { "set_looped", sound_api_set_looped },
      { "get_volume", sound_api_get_volume },
      { "set_volume", sound_api_set_volume },
      { "get_pan", sound_api_get_pan },
      { "set_pan", sound_api_set_pan },
      { "get_pitch", sound_api_get_pitch },
      { "set_pitch", sound_api_set_pitch },
  };

  const std::vector<luaL_Reg> metamethods = {
      { "__gc", userdata_meta_gc },
      { "__newindex", userdata_meta_newindex_as_table },
      { "__index", userdata_meta_index_as_table },
  };

  register_type(sound_module_name, functions, methods, metamethods);
}

/**
 * \brief Returns whether a value is a userdata of type sound.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return true if the value at this index is a sound.
 */
bool LuaContext::is_sound(lua_State* l, int index) {
  return is_userdata(l, index, sound_module_name);
}

/**
 * \brief Checks that the userdata at the specified index of the stack is a
 * sound and returns it.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return The sound.
 */
SoundPtr LuaContext::check_sound(lua_State* l, int index) {
  return std::static_pointer_cast<Sound>(check_userdata(
      l, index, sound_module_name
  ));
}

/**
 * \brief Pushes a sound userdata onto the stack.
 * \param l A Lua context.
 * \param sound A sound.
 */
void LuaContext::push_sound(lua_State* l, Sound& sound) {
  push_userdata(l, sound);
}

/**
 * \brief Implementation of sol.sound.create().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::sound_api_create(lua_State* l) {

  return state_boundary_handle(l, [&] {
    const std::string& sound_id = luaL_checkstring(l, 1);
    const bool language_specific = LuaTools::opt_boolean(l, 2, false);
    SoundBuffer& sound_buffer = get().get_main_loop().get_resource_provider().get_sound(sound_id, language_specific);
    SoundPtr sound = Sound::create(sound_buffer);
    push_sound(l, *sound);
    return 1;
  });
}

/**
 * \brief Implementation of sol.sound.stop_all().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::sound_api_stop_all(lua_State* l) {

  return state_boundary_handle(l, [&] {
    Sound::stop_all();
    return 0;
  });
}

/**
 * \brief Implementation of sound:play().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::sound_api_play(lua_State* l) {

  return state_boundary_handle(l, [&] {
    Sound& sound = *check_sound(l, 1);
    sound.start();

    return 0;
  });
}

/**
 * \brief Implementation of sound:stop().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::sound_api_stop(lua_State* l) {

  return state_boundary_handle(l, [&] {
    Sound& sound = *check_sound(l, 1);
    sound.stop();

    return 0;
  });
}

/**
 * \brief Implementation of sound:is_playing().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::sound_api_is_playing(lua_State* l) {

  return state_boundary_handle(l, [&] {
    const Sound& sound = *check_sound(l, 1);

    lua_pushboolean(l, sound.is_playing());
    return 1;
  });
}

/**
 * \brief Implementation of sound:is_paused().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::sound_api_is_paused(lua_State* l) {

  return state_boundary_handle(l, [&] {
    const Sound& sound = *check_sound(l, 1);

    lua_pushboolean(l, sound.is_paused_by_script());
    return 1;
  });
}

/**
 * \brief Implementation of sound:set_paused().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::sound_api_set_paused(lua_State* l) {

  return state_boundary_handle(l, [&] {
    Sound& sound = *check_sound(l, 1);
    bool paused = LuaTools::opt_boolean(l, 2, true);

    sound.set_paused_by_script(paused);

    return 0;
  });
}

/**
 * \brief Implementation of sound:is_looped().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::sound_api_is_looped(lua_State* l) {

  return state_boundary_handle(l, [&] {
    const Sound& sound = *check_sound(l, 1);

    lua_pushboolean(l, sound.is_looped());
    return 1;
  });
}

/**
 * \brief Implementation of sound:set_looped().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::sound_api_set_looped(lua_State* l) {

  return state_boundary_handle(l, [&] {
    Sound& sound = *check_sound(l, 1);
    bool looped = LuaTools::opt_boolean(l, 2, true);

    sound.set_looped(looped);

    return 0;
  });
}

/**
 * \brief Implementation of sound:get_volume().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::sound_api_get_volume(lua_State* l) {

  return state_boundary_handle(l, [&] {
    const Sound& sound = *check_sound(l, 1);

    lua_pushinteger(l, sound.get_volume());
    return 1;
  });
}

/**
 * \brief Implementation of sound:set_volume().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::sound_api_set_volume(lua_State* l) {

  return state_boundary_handle(l, [&] {
    Sound& sound = *check_sound(l, 1);
    const int volume = LuaTools::check_int(l, 2);
    sound.set_volume(volume);
    return 0;
  });
}

/**
 * \brief Implementation of sound:get_pan().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::sound_api_get_pan(lua_State* l) {

  return state_boundary_handle(l, [&] {
    const Sound& sound = *check_sound(l, 1);

    const float pan = sound.get_pan();
    lua_pushnumber(l, pan);

    return 1;
  });
}

/**
 * \brief Implementation of sound:set_pan().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::sound_api_set_pan(lua_State* l) {

  return state_boundary_handle(l, [&] {
    Sound& sound = *check_sound(l, 1);
    const float pan = LuaTools::check_number(l, 2);
    sound.set_pan(pan);
    return 0;
  });
}

/**
 * \brief Implementation of sound:get_pitch().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::sound_api_get_pitch(lua_State* l) {

  return state_boundary_handle(l, [&] {
    const Sound& sound = *check_sound(l, 1);

    const float pitch = sound.get_pitch();
    lua_pushnumber(l, pitch);

    return 1;
  });
}

/**
 * \brief Implementation of sound:set_pitch().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::sound_api_set_pitch(lua_State* l) {

  return state_boundary_handle(l, [&] {
    Sound& sound = *check_sound(l, 1);
    const float pitch = LuaTools::check_number(l, 2);
    sound.set_pitch(pitch);
    return 0;
  });
}

}
