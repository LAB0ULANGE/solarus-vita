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
#include "solarus/audio/Music.h"
#include "solarus/audio/MusicSystem.h"
#include "solarus/core/CurrentQuest.h"
#include "solarus/core/MainLoop.h"
#include "solarus/core/ResourceProvider.h"
#include "solarus/lua/LuaBind.h"
#include "solarus/lua/LuaContext.h"
#include "solarus/lua/LuaTools.h"
#include <lua.hpp>

namespace Solarus {

/**
 * Name of the Lua table representing the audio module.
 */
const std::string LuaContext::audio_module_name = "sol.audio";

/**
 * \brief Implementation of sol.audio.play_sound().
 * \param context The LuaContext that is calling this function.
 * \param sound_id The id of the sound to play.
 */
static void play_sound(LuaContext & context, const std::string & sound_id) {
  if (!Sound::exists(sound_id)) {
    lua_State* l = context.get_internal_state();
    LuaTools::error(l, std::string("No such sound: '") + sound_id + "'");
  }
  SoundBuffer& sound_buffer = context.get_main_loop().get_resource_provider().get_sound(sound_id);
  SoundPtr sound = Sound::create(sound_buffer);
  sound->start();
}

/**
 * \brief Implementation of sol.audio.preload_sounds().
 * \param context The LuaContext that is calling this function.
 */
static void preload_sounds(LuaContext & context) {
  context.warning_deprecated(
      { 2, 0 },
      "sol.audio.preload_sounds()",
      "Sounds are always preloaded now."
  );
}

/**
 * \brief Implementation of sol.audio.play_music().
 * \param l The Lua context that is calling this function.
 * \param music_id_arg If provided, id of the music to play.
 */
static void play_music(lua_State* l, std::optional<std::string> music_id_arg) {
  const std::string& music_id = music_id_arg.value_or("");
  bool loop;
  ScopedLuaRef callback_ref;
  if (lua_gettop(l) < 2) {
    // If no additional parameter: then we loop.
    loop = true;
  } else if (lua_isboolean(l, 2)) {
    // There is a loop parameter: use it.
    loop = lua_toboolean(l, 2);
  } else {
    // There is a callback parameter: never loop and save the callback.
    loop = false;
    callback_ref = LuaTools::check_function(l, 2);
  }

  if (music_id.empty()) {
    // nil music: stop playing any music.
    MusicSystem::stop_playing();
  } else {
    if (!MusicSystem::exists(music_id)) {
      // Could not find the specified music.
      LuaTools::error(l, std::string("No such music: '") + music_id + "'");
    }

    // Valid music file name.
    MusicSystem::play(music_id, loop, callback_ref);
  }
}

/**
 * \brief Implementation of sol.audio.get_music().
 * \return The music id if music is playing, otherwise an empty optional.
 */
static std::optional<std::string> get_music() {
  const std::string& music_id = MusicSystem::get_current_music_id();

  if (music_id == Music::none) {
    return std::nullopt;
  } else {
    return std::make_optional(music_id);
  }
}

/**
 * \brief Implementation of sol.audio.get_music_format().
 * \return The format name if music is playing, otherwise an empty optional.
 */
static std::optional<std::string> get_music_format() {
  const Music::Format format = MusicSystem::get_current_music_format();

  if (format == Music::FORMAT_NONE) {
    // No music is playing.
    return std::nullopt;
  } else {
    return std::make_optional(Music::format_names[format]);
  }
}

/**
 * \brief Implementation of sol.audio.get_music_num_channels().
 * \return If the currently playing music, return its number of channels,
 *   otherwise an empty optional.
 */
static std::optional<int> get_music_num_channels() {
  if (MusicSystem::get_current_music_format() != Music::FORMAT_IT) {
    return std::nullopt;
  } else {
    return std::make_optional(MusicSystem::get_current_music_num_channels());
  }
}

/**
 * \brief Implementation of sol.audio.get_music_channel_volume().
 * \param l The Lua context that is calling this function.
 * \param channel Index of a channel.
 * \return If the playing music has channels, volume of the channel,
 *   otherwise an empty optional.
 */
static std::optional<int> get_music_channel_volume(lua_State * l, int channel) {
  if (MusicSystem::get_current_music_format() != Music::FORMAT_IT) {
    return std::nullopt;
  } else {
    if (CurrentQuest::is_format_at_most({ 1, 6 })) {
      // Offset channel number for 1.6 quests and lower
      channel++;
    }
    if (channel < 1 || channel > MusicSystem::get_current_music_num_channels()) {
      LuaTools::arg_error(l, 1,
        "Invalid channel number: " + std::to_string(channel));
      return std::nullopt;
    } else {
      return std::make_optional(MusicSystem::get_current_music_channel_volume(channel));
    }
  }
}

/**
 * \brief Implementation of sol.audio.set_music_channel_volume().
 * \param l The Lua context that is calling this function.
 * \param channel Index of a channel.
 * \param volume The new volume for the channel.
 * \return Whether or not the current music has channels.
 */
static bool set_music_channel_volume(lua_State * l, int channel, int volume) {
  if (MusicSystem::get_current_music_format() != Music::FORMAT_IT) {
    return false;
  } else {
    if (CurrentQuest::is_format_at_most({ 1, 6 })) {
      // Offset channel number for 1.6 quests and lower
      channel++;
    }
    if (channel < 1 || channel > MusicSystem::get_current_music_num_channels()) {
      LuaTools::arg_error(l, 1,
        "Invalid channel number: " + std::to_string(channel));
    } else {
      MusicSystem::set_current_music_channel_volume(channel, volume);
    }
    return true;
  }
}

/**
 * \brief Implementation of sol.audio.get_music_tempo().
 * \return The tempo value of the current music if it supports it,
 *   otherwise an empty optional.
 */
static std::optional<int> get_music_tempo() {
  if (MusicSystem::get_current_music_format() != Music::FORMAT_IT) {
    return std::nullopt;
  } else {
    return std::make_optional(MusicSystem::get_current_music_tempo());
  }
}

/**
 * \brief Implementation of sol.audio.set_music_tempo().
 * \param tempo The new tempo value for the current music.
 * \return Whether or not the current music supports tempo.
 */
static bool set_music_tempo(int tempo) {
  if (MusicSystem::get_current_music_format() != Music::FORMAT_IT) {
    return false;
  } else {
    MusicSystem::set_current_music_tempo(tempo);
    return true;
  }
}

/**
 * \brief Initializes the audio features provided to Lua.
 */
void LuaContext::register_audio_module() {

  // Functions of sol.audio.
  const std::vector<luaL_Reg> functions = {
      { "get_sound_volume", LUA_TO_C_BIND(Sound::get_global_volume) },
      { "set_sound_volume", LUA_TO_C_BIND(Sound::set_global_volume) },
      { "play_sound", LUA_TO_C_BIND(play_sound) },
      { "preload_sounds", LUA_TO_C_BIND(preload_sounds) },
      { "get_music_volume", LUA_TO_C_BIND(MusicSystem::get_global_volume) },
      { "set_music_volume", LUA_TO_C_BIND(MusicSystem::set_global_volume) },
      { "play_music", LUA_TO_C_BIND(play_music) },
      { "stop_music", LUA_TO_C_BIND(MusicSystem::stop_playing) },
      { "get_music", LUA_TO_C_BIND(get_music) },
      { "get_music_format", LUA_TO_C_BIND(get_music_format) },
      { "get_music_num_channels", LUA_TO_C_BIND(get_music_num_channels) },
      { "get_music_channel_volume", LUA_TO_C_BIND(get_music_channel_volume) },
      { "set_music_channel_volume", LUA_TO_C_BIND(set_music_channel_volume) },
      { "get_music_tempo", LUA_TO_C_BIND(get_music_tempo) },
      { "set_music_tempo", LUA_TO_C_BIND(set_music_tempo) }
  };
  register_functions(audio_module_name, functions);
}

}

