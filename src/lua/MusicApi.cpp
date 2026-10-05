/*
 * Copyright (C) 2006-2025 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/audio/MusicSystem.h"
#include "solarus/core/CurrentQuest.h"
#include "solarus/core/MainLoop.h"
#include "solarus/core/ResourceProvider.h"
#include "solarus/lua/LuaContext.h"

namespace Solarus {

/**
 * Name of the Lua table representing the music module.
 */
const std::string LuaContext::music_module_name = "sol.music";

void LuaContext::register_music_module() {
  // Functions of sol.music.
  const std::vector<luaL_Reg> functions = {
    { "create", music_api_create },
  };

  // Methods of the music type.
  const std::vector<luaL_Reg> methods = {
    { "play", music_api_play },
    { "stop", music_api_stop },
    { "is_paused", music_api_is_paused },
    { "set_paused", music_api_set_paused },
    { "get_volume", music_api_get_volume },
    { "set_volume", music_api_set_volume },
    { "get_channel_volume", music_api_get_channel_volume },
    { "set_channel_volume", music_api_set_channel_volume },
    { "get_channel_pan", music_api_get_channel_pan },
    { "set_channel_pan", music_api_set_channel_pan },
  };

  const std::vector<luaL_Reg> metamethods = {
    { "__gc", userdata_meta_gc },
    { "__newindex", userdata_meta_newindex_as_table },
    { "__index", userdata_meta_index_as_table },
  };

  register_type(music_module_name, functions, methods, metamethods);
}

/**
 * \brief Returns whether a value is a userdata of type sound.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return true if the value at this index is a sound.
 */
bool LuaContext::is_music(lua_State* l, int index) {
  return is_userdata(l, index, music_module_name);
}

/**
 * \brief Checks that the userdata at the specified index of the stack is a
 * music and returns it.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return The music.
 */
MusicPtr LuaContext::check_music(lua_State* l, int index) {
  return std::static_pointer_cast<Music>(check_userdata(
    l, index, music_module_name
  ));
}

/**
 * \brief Pushes a music userdata onto the stack.
 * \param l A Lua context.
 * \param music A music.
 */
void LuaContext::push_music(lua_State* l, Music& music) {
  push_userdata(l, music);
}

/**
 * \brief Implementation of sol.music.create().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::music_api_create(lua_State* l) {
  return state_boundary_handle(l, [&] {
    const std::string& music_id = LuaTools::check_string(l, 1);
    MusicPtr music = Music::create(music_id);
    MusicSystem::add_music(music);
    push_music(l, *music);
    return 1;
  });
}

/**
 * \brief Implementation of music:play().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::music_api_play(lua_State* l) {

  return state_boundary_handle(l, [&] {
    Music& music = *check_music(l, 1);
    music.start();

    return 0;
  });
}

/**
 * \brief Implementation of music:stop().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::music_api_stop(lua_State* l) {

  return state_boundary_handle(l, [&] {
    Music& music = *check_music(l, 1);
    music.stop();

    return 0;
  });
}

/**
 * \brief Implementation of music:is_paused().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::music_api_is_paused(lua_State* l) {

  return state_boundary_handle(l, [&] {
    Music& music = *check_music(l, 1);
    lua_pushboolean(l, music.is_paused());

    return 1;
  });
}

/**
 * \brief Implementation of music:set_paused().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::music_api_set_paused(lua_State* l) {
  return state_boundary_handle(l, [&] {
    Music& music = *check_music(l, 1);
    bool paused = LuaTools::check_boolean(l, 2);
    music.set_paused(paused);

    return 0;
  });
}

/**
 * \brief Implementation of music:get_volume().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::music_api_get_volume(lua_State* l) {

  return state_boundary_handle(l, [&] {
    Music& music = *check_music(l, 1);

    lua_pushinteger(l, music.get_volume());

    return 1;
  });
}

/**
 * \brief Implementation of music:set_volume().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::music_api_set_volume(lua_State* l) {

  return state_boundary_handle(l, [&] {
    Music& music = *check_music(l, 1);
    int volume = LuaTools::check_int(l, 2);

    music.set_volume(volume);

    return 0;
  });
}

/**
 * \brief Implementation of music:get_channel_volume).
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::music_api_get_channel_volume(lua_State* l) {

  return state_boundary_handle(l, [&] {
    Music& music = *check_music(l, 1);
    int channel_index = LuaTools::check_int(l, 2);

    lua_pushinteger(l, music.get_channel_volume(channel_index));

    return 1;
  });
}

/**
 * \brief Implementation of music:set_channel_volume().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::music_api_set_channel_volume(lua_State* l) {

  return state_boundary_handle(l, [&] {
    Music& music = *check_music(l, 1);
    int channel_index = LuaTools::check_int(l, 2);
    int pan = LuaTools::check_int(l, 3);

    music.set_channel_volume(channel_index, pan);

    return 0;
  });
}

/**
 * \brief Implementation of music:get_channel_pan().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::music_api_get_channel_pan(lua_State* l) {

  return state_boundary_handle(l, [&] {
    Music& music = *check_music(l, 1);
    int channel_index = LuaTools::check_int(l, 2);

    lua_pushinteger(l, music.get_channel_pan(channel_index));

    return 1;
  });
}

/**
 * \brief Implementation of music:set_channel_pan().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::music_api_set_channel_pan(lua_State* l) {

  return state_boundary_handle(l, [&] {
    Music& music = *check_music(l, 1);
    int channel_index = LuaTools::check_int(l, 2);
    int pan = LuaTools::check_int(l, 3);

    music.set_channel_pan(channel_index, pan);

    return 0;
  });
}

}