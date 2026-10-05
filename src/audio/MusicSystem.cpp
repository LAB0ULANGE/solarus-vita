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
#include "solarus/audio/ItDecoder.h"
#include "solarus/audio/Music.h"
#include "solarus/audio/MusicSystem.h"
#include "solarus/audio/OggDecoder.h"
#include "solarus/audio/SpcDecoder.h"
#include "solarus/core/QuestFiles.h"
#include <lua.hpp>
#include <algorithm>

namespace Solarus {

std::shared_ptr<Music> MusicSystem::current_music = nullptr;
std::list<MusicPtr> MusicSystem::current_musics;
bool MusicSystem::initialized = false;
float MusicSystem::global_volume = 1.0f;

/**
 * \brief Initializes the music system.
 */
void MusicSystem::initialize() {
  global_volume = 1.0;
  initialized = true;
}

/**
 * \brief Exits the music system.
 */
void MusicSystem::quit() {

  if (is_initialized()) {
    if (current_music != nullptr) {
      current_music->stop();
    }
    current_music = nullptr;

    for (const MusicPtr& music: current_musics) {
      if (music->is_playing()) {
        music->stop();
      }
    }
    current_musics.clear();

    global_volume = 1.0;
  }
}

/**
 * \brief Returns whether the music system is initialized.
 * \return \c true if the music system is initialized.
 */
bool MusicSystem::is_initialized() {
  return initialized;
}

/**
 * \brief Returns the current volume of musics.
 * \return the volume (0 to 100)
 */
int MusicSystem::get_global_volume() {
  return static_cast<int>(global_volume * 100.0 + 0.5);
}

/**
 * \brief Sets the volume of musics.
 * \param volume the new volume (0 to 100)
 */
void MusicSystem::set_global_volume(int volume) {
  global_volume = std::min(100, std::max(0, volume)) / 100.0;
  if (current_music != nullptr) {
    current_music->notify_global_volume_changed();
  }
  for (const MusicPtr& music: current_musics) {
    music->notify_global_volume_changed();
  }
}

/**
 * \brief Returns the format of the current music played by the sol.audio legacy API.
 * \return the music format.
 */
Music::Format MusicSystem::get_current_music_format() {
  if (!is_initialized() || current_music == nullptr) {
    return Music::FORMAT_NONE;
  }

  return current_music->get_format();
}

/**
 * \brief Returns the number of channels for the current music played by the sol.audio legacy API.
 * \return the number of channels.
 */
int MusicSystem::get_current_music_num_channels() {
  if (!is_initialized() || current_music == nullptr) {
    return 0;
  }

  return current_music->get_num_channels();
}

/**
 * \brief Returns the volume for the specified channel of the current music played by the sol.audio legacy API.
 * \param channel the channel for which to get the volume.
 * \return the volume.
 */
float MusicSystem::get_current_music_channel_volume(int channel) {
  if (!is_initialized() || current_music == nullptr) {
    return 0;
  }

  return current_music->get_channel_volume(channel);
}

/**
 * \brief Sets the volume for the specified channel of the current music played by the sol.audio legacy API.
 * \param channel the channel for which to set the volume.
 * \param volume the volume to set.
 */
void MusicSystem::set_current_music_channel_volume(int channel, int volume) {
  if (!is_initialized() || current_music == nullptr) {
    return;
  }

  current_music->set_channel_volume(channel, volume);
}

/**
 * \brief Returns the tempo for the current music played by the sol.audio legacy API.
 * \return the tempo.
 */
int MusicSystem::get_current_music_tempo() {
  if (!is_initialized() || current_music == nullptr) {
    return 0;
  }

  return current_music->get_tempo();
}

/**
 * \brief Sets the tempo for the current music played by the sol.audio legacy API.
 * \param tempo the tempo.
 */
void MusicSystem::set_current_music_tempo(int tempo) {
  if (!is_initialized() || current_music == nullptr) {
    return;
  }

  current_music->set_tempo(tempo);
}

/**
 * \brief Tries to find a music file from a music id.
 * \param music_id Id of the music to find (file name without
 * directory or extension). Cannot be Music::none or Music::unchanged.
 * \param file_name Resulting file name with its extension
 * (empty string if not found).
 * \param format Resulting music format.
 */
void MusicSystem::find_music_file(const std::string& music_id, std::string& file_name, Music::Format& format) {

  file_name = "";
  format = Music::FORMAT_OGG;

  std::string file_name_start = std::string("musics/" + music_id);
  if (QuestFiles::data_file_exists(file_name_start + ".ogg")) {
    format = Music::FORMAT_OGG;
    file_name = file_name_start + ".ogg";
  }
  else if (QuestFiles::data_file_exists(file_name_start + ".it")) {
    format = Music::FORMAT_IT;
    file_name = file_name_start + ".it";
  }
  else if (QuestFiles::data_file_exists(file_name_start + ".spc")) {
    format = Music::FORMAT_SPC;
    file_name = file_name_start + ".spc";
  }
}

/**
 * \brief Returns whether a music exists.
 * \param music_id Id of the music to test. Music::none and Music::unchanged
 * are also considered valid.
 * \return true If this music exists.
 */
bool MusicSystem::exists(const std::string& music_id) {

  if (music_id == Music::none || music_id == Music::unchanged) {
    return true;
  }

  std::string file_name;
  Music::Format format;
  find_music_file(music_id, file_name, format);

  return !file_name.empty();
}

/**
 * \brief Returns the id of the music currently playing.
 * \return the id of the current music, or "none" if no music is being played
 */
const std::string& MusicSystem::get_current_music_id() {
  return current_music != nullptr ? current_music->get_id() : Music::none;
}

/**
 * \brief Add a music to the list of music handled by sol.music API.
 * \param music the music pointer to add.
 */
void MusicSystem::add_music(MusicPtr music) {
  current_musics.push_back(music);
}

/**
 * \brief Remove a music to the list of music handled by sol.music API.
 * \param music the music pointer to remove.
 */
void MusicSystem::remove_music(MusicPtr music) {
  current_musics.remove(music);
}

/**
 * \brief Plays a music from the legacy sol.audio API.
 *
 * If the music is different from the current one, the current one is stopped.
 * The music specified can also be Music::none_id (then the current music is
 * just stopped).
 * or even Music::unchanged_id (the current music continues in this case).
 *
 * \param music_id Id of the music to play (file name without extension).
 * \param loop Whether the music should loop when reaching its end
 * (if there is an end).
 */
void MusicSystem::play(const std::string& music_id, bool loop) {

  play(music_id, loop, ScopedLuaRef());
}

/**
 * \brief Plays a music with an optional Lua callback to call when it finishes.
 * \param music_id Id of the music to play (file name without extension).
 * \param loop Whether the music should loop when reaching its end
 * (if there is an end).
 * \param callback_ref Lua function to call when the music finishes or
 * an empty ref. There cannot be both a loop and a callback at the same time.
 */
void MusicSystem::play(
    const std::string& music_id,
    bool loop,
    const ScopedLuaRef& callback_ref
) {
  if (music_id != Music::unchanged && music_id != get_current_music_id()) {
    // The music is changed.
    if (current_music != nullptr) {
      // Stop the music that was played.
      current_music->stop();
      current_music = nullptr;
    }

    if (music_id != Music::none) {
      // Play another music.
      current_music = std::shared_ptr<Music>(
        new Music(music_id, loop, callback_ref)
      );
      if (!current_music->start()) {
        // Could not play the music.
        current_music = nullptr;
      }
    }
  }
}

/**
 * \brief Updates the music system.
 *
 * When a music is playing, this function continues the streaming.
 */
void MusicSystem::update() {

  if (!is_initialized()) {
    return;
  }

  // Update current music from sol.audio API
  if (current_music != nullptr) {
    bool playing = current_music->update_playing();
    if (!playing) {
      // Music is finished.
      ScopedLuaRef callback_ref = current_music->get_callback();
      current_music->stop();
      current_music = nullptr;
      callback_ref.call("music callback");
    }
  }

  // Update current musics from sol.music API
  std::list<MusicPtr> musics_to_remove;
  for (const MusicPtr& music: current_musics) {
    if (music->is_playing()) {
      if (!music->update_playing()) {
        musics_to_remove.push_back(music);
      }
    }
  }

  for (const MusicPtr& music: musics_to_remove) {
    current_musics.remove(music);
  }
}

/**
 * \brief Stops playing any music.
 *
 * The callback if any is not called.
 */
void MusicSystem::stop_playing() {

  play(Music::none, false);
}

/**
 * \brief Pauses any music currently playing.
 *
 * The callback if any is not called.
 */
void MusicSystem::pause_playing() {

  if (current_music != nullptr) {
    current_music->set_paused(true);
  }
}

/**
 * \brief Resumes playing any music previously paused.
 *
 * The callback if any is not called.
 */
void MusicSystem::resume_playing() {

  if (current_music != nullptr) {
    current_music->set_paused(false);
  }
}

/**
 * \brief Notifies the music system that the audio device was disconnected.
 */
void MusicSystem::notify_device_disconnected_all() {

  if (current_music != nullptr) {
    current_music->notify_device_disconnected();
  }

  for (MusicPtr music: current_musics) {
    music->notify_device_disconnected();
  }
}

/**
 * \brief Notifies the music system that an audio device was reconnected.
 */
void MusicSystem::notify_device_reconnected_all() {

  if (current_music != nullptr) {
    current_music->notify_device_reconnected();
  }

  for (MusicPtr music: current_musics) {
    music->notify_device_reconnected();
  }
}

}
