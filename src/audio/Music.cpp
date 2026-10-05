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

#include "solarus/audio/ALWrapper.h"
#include "solarus/audio/ItDecoder.h"
#include "solarus/audio/Music.h"
#include "solarus/audio/MusicSystem.h"
#include "solarus/audio/OggDecoder.h"
#include "solarus/audio/SpcDecoder.h"
#include "solarus/core/Debug.h"
#include "solarus/core/QuestFiles.h"
#include "solarus/lua/LuaContext.h"
#include <lua.hpp>
#include <algorithm>
#include <sstream>

namespace Solarus {

constexpr int Music::nb_buffers;
constexpr int Music::buffer_size;

const std::string Music::none = "none";
const std::string Music::unchanged = "same";

/**
 * \brief Lua name of each music format.
 */
const std::vector<std::string> Music::format_names = {
  "",
  "spc",
  "it",
  "ogg"
};

/**
 * \brief Creates an empty music.
 */
Music::Music():
  playing(false),
  id(none),
  format(FORMAT_NONE),
  loop(false),
  callback_ref(),
  source(AL_NONE),
  spc_decoder(std::unique_ptr<SpcDecoder>(new SpcDecoder())),
  it_decoder(std::unique_ptr<ItDecoder>(new ItDecoder())),
  ogg_decoder(std::unique_ptr<OggDecoder>(new OggDecoder())),
  volume(1.0) {

  for (int i = 0; i < nb_buffers; i++) {
    buffers[i] = AL_NONE;
  }
}

/**
 * \brief Creates a new music.
 * \param music_id Id of the music (file name without extension).
 * \param loop Whether the music should loop when reaching its end.
 * \param callback_ref Lua function to call when the music ends
 * or an empty ref.
 * There cannot be both a loop and a callback at the same time.
 */
Music::Music(
    const std::string& music_id,
    bool loop,
    const ScopedLuaRef& callback_ref):
  playing(false),
  id(music_id),
  format(FORMAT_OGG),
  loop(loop),
  callback_ref(callback_ref),
  source(AL_NONE),
  spc_decoder(std::unique_ptr<SpcDecoder>(new SpcDecoder())),
  it_decoder(std::unique_ptr<ItDecoder>(new ItDecoder())),
  ogg_decoder(std::unique_ptr<OggDecoder>(new OggDecoder())),
  volume(1.0) {

  SOLARUS_REQUIRE(!loop || callback_ref.is_empty(),
      "Attempt to set both a loop and a callback to music");

  for (int i = 0; i < nb_buffers; i++) {
    buffers[i] = AL_NONE;
  }
}

/**
 * \brief Creates a new music.
 */
MusicPtr Music::create(const std::string& music_id) {
  Music* music = new Music(music_id, true, ScopedLuaRef());
  return MusicPtr(music);
}

/**
 * \brief Performs the loading of music into memory
 */
void Music::load(const std::string& music_id) {
  Sound::check_openal_clean_state("Music::load");
  // Detect format from file_name
  std::string file_name;
  MusicSystem::find_music_file(music_id, file_name, format);

  // Load music into
  std::string sound_buffer;
  switch (format) {
    case FORMAT_SPC:
      sound_buffer = QuestFiles::data_file_read(file_name);
      spc_decoder->load((int16_t*) sound_buffer.data(), sound_buffer.size());
      load_successful = true;
      break;
    case FORMAT_IT:
      sound_buffer = QuestFiles::data_file_read(file_name);
      it_decoder->load(sound_buffer);
      load_successful = true;
      break;
    case FORMAT_OGG:
      sound_buffer = QuestFiles::data_file_read(file_name);
      load_successful = ogg_decoder->load(std::move(sound_buffer), loop);
      break;
    case FORMAT_NONE:
      Debug::die("start: Invalid music format");
      break;
  }
}

const std::string& Music::get_id() const {
  return id;
}

/**
 * \brief Returns the format of the current music.
 * \return The format of the current music.
 */
Music::Format Music::get_format() {
  return format;
}

/**
 * \brief Returns the current volume of music.
 * \return the volume (0 to 100)
 */
int Music::get_volume() const {
  return static_cast<int>(volume * 100.0 + 0.5);
}

/**
 * \brief Sets the volume of musics.
 * \param volume the new volume (0 to 100)
 */
void Music::set_volume(int volume) {
  this->volume = std::min(100, std::max(0, volume)) / 100.0;

  if (!this->playing) {
    return;
  }

  if (source != AL_NONE) {
    ALWrapper::set_source_float(source, AL_GAIN, this->volume * (MusicSystem::get_global_volume() / 100.0f));
  }
}

/**
 * \brief Returns the number of channels of the current music.
 *
 * This function is only supported for .it musics.
 *
 * \return The number of channels.
 */
int Music::get_num_channels() {

  SOLARUS_REQUIRE(get_format() == FORMAT_IT,
      "This function is only supported for .it musics");

  return it_decoder->get_num_channels();
}

/**
 * \brief Returns the volume of a channel of the current music.
 *
 * This function is only supported for .it musics.
 *
 * \param channel Index of a channel.
 * \return The volume of this channel.
 */
int Music::get_channel_volume(int channel) {

  SOLARUS_REQUIRE(get_format() == FORMAT_IT,
      "This function is only supported for .it musics");

  return it_decoder->get_channel_volume(channel);
}

/**
 * \brief Sets the volume of a channel of the current music.
 *
 * This function is only supported for .it musics.
 *
 * \param channel Index of a channel.
 * \param volume The volume to set.
 */
void Music::set_channel_volume(int channel, int volume) {

  SOLARUS_REQUIRE(get_format() == FORMAT_IT,
      "This function is only supported for .it musics");

  it_decoder->set_channel_volume(channel, volume);
}

/**
 * \brief Returns the pan of a channel of the current music.
 *
 * This function is only supported for .it musics.
 *
 * \param channel Index of a channel.
 * \return The pan of this channel.
 */
int Music::get_channel_pan(int channel) {
  SOLARUS_REQUIRE(get_format() == FORMAT_IT,
      "This function is only supported for .it musics");

  return it_decoder->get_channel_pan(channel);
}

/**
 * \brief Sets the pan of a channel of the current music.
 *
 * This function is only supported for .it musics.
 *
 * \param channel Index of a channel.
 * \param pan The pan to set.
 */
void Music::set_channel_pan(int channel, int pan) {

  SOLARUS_REQUIRE(get_format() == FORMAT_IT,
      "This function is only supported for .it musics");

  it_decoder->set_channel_pan(channel, pan);
}

/**
 * \brief Returns the tempo of the current music.
 *
 * This function is only supported for .it musics.
 *
 * \return The tempo of the current music.
 */
int Music::get_tempo() {

  SOLARUS_REQUIRE(get_format() == FORMAT_IT,
      "This function is only supported for .it musics");

  return it_decoder->get_tempo();
}

/**
 * \brief Sets the tempo of the current music.
 *
 * This function is only supported for .it musics.
 *
 * \param tempo The tempo to set.
 */
void Music::set_tempo(int tempo) {

  SOLARUS_REQUIRE(get_format() == FORMAT_IT,
      "This function is only supported for .it musics");

  it_decoder->set_tempo(tempo);
}

/**
 * \brief Updates this music when it is playing.
 *
 * This function handles the double buffering.
 *
 * \return \c true if the music keeps playing, \c false if the end is reached.
 */
bool Music::update_playing() {

  if (alcGetCurrentContext() == nullptr) {
    // Device disconnected.
    return true;
  }

  // Get the empty buffers.
  ALint nb_empty;
  ALWrapper::get_source_int(source, AL_BUFFERS_PROCESSED, &nb_empty);

  // Refill them.
  for (int i = 0; i < nb_empty; i++) {
    ALuint buffer;
    ALWrapper::unqueue_source_buffers(source, 1, &buffer); // Unqueue the buffer.

    // Fill it by decoding more data.
    switch (format) {
      case FORMAT_SPC:
        decode_spc(buffer, buffer_size);
        break;
      case FORMAT_IT:
        decode_it(buffer, buffer_size);
        break;
      case FORMAT_OGG:
        decode_ogg(buffer, buffer_size);
        break;
      case FORMAT_NONE:
        Debug::die("update_playing: Invalid music format");
        break;
    }

    ALWrapper::queue_source_buffers(source, 1, &buffer);  // Queue it again.
  }

  // Check whether there is still something playing.
  ALint status;
  ALWrapper::get_source_int(source, AL_SOURCE_STATE, &status);

  if (status != AL_PLAYING && status != AL_PAUSED) {
    // The end of the file is reached, or we need to decode more data.
    ALWrapper::play_source(source);
  }

  ALWrapper::get_source_int(source, AL_SOURCE_STATE, &status);

  return status == AL_PLAYING;
}

/**
 * \brief Notifies this music that the audio device was disconnected.
 */
void Music::notify_device_disconnected() {

  Sound::check_openal_clean_state("Music::notify_device_disconnected beginning");

  // All sources and buffers are already destroyed by OpenAL at this point.
  source = AL_NONE;
  for (int i = 0; i < nb_buffers; ++i) {
    buffers[i] = AL_NONE;
  }
  Sound::check_openal_clean_state("Music::notify_device_disconnected end");
}

/**
 * \brief Notifies this music that the audio device was reconnected.
 */
void Music::notify_device_reconnected() {

  Sound::check_openal_clean_state("Music::notify_device_reconnected beginning");

  if (buffers[0] == AL_NONE) {
    // Recreate a source and buffers.
    ALWrapper::generate_buffers(nb_buffers, buffers);
    ALWrapper::generate_sources(1, &source);
    ALWrapper::set_source_float(source, AL_GAIN, volume * (MusicSystem::get_global_volume() / 100.0f));

    // Continue playing music.
    // Buffer data that was already decoded to buffers before the
    // disconnection is lost, so we actually skipped a short period of time.
    for (int i = 0; i < nb_buffers; i++) {
      ALuint buffer = buffers[i];
      switch (format) {
        case FORMAT_SPC:
          decode_spc(buffer, buffer_size);
          break;
        case FORMAT_IT:
          decode_it(buffer, buffer_size);
          break;
        case FORMAT_OGG:
          decode_ogg(buffer, buffer_size);
          break;
        case FORMAT_NONE:
          Debug::die("notify_device_reconnected: Invalid music format");
          break;
      }
      ALWrapper::queue_source_buffers(source, 1, &buffer);
    }
    ALWrapper::play_source(source);
  }
  Sound::check_openal_clean_state("Music::notify_device_reconnected end");
}

/**
 * \brief Notifies this music that the MusicSystem global volume has changed.
 */
void Music::notify_global_volume_changed() {
  set_volume(get_volume());
}

/**
 * \brief Decodes a chunk of SPC data into PCM data for the current music.
 * \param destination_buffer the destination buffer to write
 * \param nb_samples number of samples to write
 */
void Music::decode_spc(ALuint destination_buffer, ALsizei nb_samples) {

  Sound::check_openal_clean_state("Music::decode_spc");

  // decode the SPC data
  std::vector<ALushort> raw_data(nb_samples);
  spc_decoder->decode((int16_t*) raw_data.data(), nb_samples);

  // put this decoded data into the buffer
  ALWrapper::set_buffer_data(destination_buffer, AL_FORMAT_STEREO16, raw_data.data(), nb_samples * 2, 32000);

  // TODO: Comment this and find a way to bring error context to wrapper

  // ALenum error = alGetError();
  // if (error != AL_NO_ERROR) {
  //   std::ostringstream oss;
  //   oss << "Failed to fill the audio buffer with decoded SPC data for music file '"
  //     << file_name << ": error " << error;
  //   Debug::error(oss.str());
  // }
}

/**
 * \brief Decodes a chunk of IT data into PCM data for the current music.
 * \param destination_buffer the destination buffer to write
 * \param nb_samples number of samples to write
 */
void Music::decode_it(ALuint destination_buffer, ALsizei nb_samples) {

  Sound::check_openal_clean_state("Music::decode_it");

  // Decode the IT data.
  std::vector<ALushort> raw_data(nb_samples);
  int bytes_read = it_decoder->decode(raw_data.data(), nb_samples);

  if (bytes_read == 0) {
    // End of file.
    ALWrapper::set_buffer_data(destination_buffer, AL_FORMAT_STEREO16, raw_data.data(), 0, 44100);
  }
  else {
    // Put this decoded data into the buffer.
    ALWrapper::set_buffer_data(destination_buffer, AL_FORMAT_STEREO16, raw_data.data(), nb_samples, 44100);
  }

  // TODO: Comment this and find a way to bring error context to wrapper

  // ALenum error = alGetError();
  // if (error != AL_NO_ERROR) {
  //   std::ostringstream oss;
  //   oss << "Failed to fill the audio buffer with decoded IT data for music file '"
  //       << file_name << ": error " << std::hex << error;
  //   Debug::error(oss.str());
  // }
}

/**
 * \brief Decodes a chunk of OGG data into PCM data for the current music.
 * \param destination_buffer The destination buffer to write.
 * \param nb_samples Number of samples to write.
 */
void Music::decode_ogg(ALuint destination_buffer, ALsizei nb_samples) {

  ogg_decoder->decode(destination_buffer, nb_samples);
}

/**
 * \brief Loads the file and starts playing this music.
 *
 * No other music should be playing.
 *
 * \return true if the music was loaded successfully
 */
bool Music::start() {
  if (!MusicSystem::is_initialized()) {
    return false;
  }

  Sound::check_openal_clean_state("Music::start");

  // First time: find the file.
  if (file_name.empty()) {
    MusicSystem::find_music_file(id, file_name, format);

    if (file_name.empty()) {
      Debug::error(std::string("Cannot find music file 'musics/")
          + id + "' (tried with extensions .ogg, .it and .spc)"
      );
      return false;
    }
  }

  // loading the music file
  load(id);

  // create the buffers and the source
  ALWrapper::generate_buffers(nb_buffers, buffers);
  ALWrapper::generate_sources(1, &source);
  ALWrapper::set_source_float(source, AL_GAIN, volume * (MusicSystem::get_global_volume() / 100.0f));

  // decode music from memory
  switch (format) {
    case FORMAT_SPC:
      for (int i = 0; i < nb_buffers; i++) {
        decode_spc(buffers[i], buffer_size);
      }
      break;
    case FORMAT_IT:
      for (int i = 0; i < nb_buffers; i++) {
        decode_it(buffers[i], buffer_size);
      }
      break;
    case FORMAT_OGG:
      if (load_successful) {
        for (int i = 0; i < nb_buffers; i++) {
          decode_ogg(buffers[i], buffer_size);
        }
      }
      break;
    case FORMAT_NONE:
      Debug::die("Music::start: Invalid music format");
      break;
  }

  if (!load_successful) {
    Debug::error("Cannot load music file '" + file_name + "'");
  }

  // start the streaming
  bool start_successful = true;
  ALWrapper::queue_source_buffers(source, nb_buffers, buffers);
  
  // TODO: Comment this and find a way to bring error context to wrapper

  // ALenum error = alGetError();
  // if (error != AL_NO_ERROR) {
  //   std::ostringstream oss;
  //   oss << "Cannot initialize buffers for music '"
  //       << file_name << "': error " << error;
  //   Debug::error(oss.str());
  //   start_successful = false;
  // }

  // The update() function will then take care of filling the buffers

  playing = true;

  return start_successful;
}

/**
 * \brief Stops playing the music.
 *
 * The callback if any is not called.
 */
void Music::stop() {

  if (!MusicSystem::is_initialized()) {
    return;
  }

  Sound::check_openal_clean_state("Music::stop beginning");

  // Release the callback if any.
  callback_ref.clear();

  // Empty the source.
  ALWrapper::stop_source(source);

  // ALenum error = alGetError();
  // if (error != AL_NO_ERROR) {
  //   std::ostringstream oss;
  //   oss << "Failed to stop music source" << source << ": " << std::hex << error;
  //   Debug::error(oss.str());
  // }

  // Unqueue buffers.
  ALint nb_processed = 0;
  ALuint processed_buffers[nb_buffers];
  ALWrapper::get_source_int(source, AL_BUFFERS_PROCESSED, &nb_processed);

  if (nb_processed > 0) {
    ALWrapper::unqueue_source_buffers(source, 1, processed_buffers);

    // error = alGetError();
    // if (error != AL_NO_ERROR) {
    //   std::ostringstream oss;
    //   oss << "Failed to unqueue " << nb_processed << " processed buffers: " << std::hex << error;
    //   Debug::error(oss.str());
    // }
  }

  ALint nb_queued = 0;
  ALuint queued_buffers[nb_buffers];
  ALWrapper::get_source_int(source, AL_BUFFERS_QUEUED, &nb_queued);

  if (nb_queued > 0) {
    alSourceUnqueueBuffers(source, 1, queued_buffers);
    alGetError();
    // OpenAL Soft refuses to unqueue buffers that were just queued but this is fine,
    // they will still deleted properly in alDeleteBuffers().
  }

  // Delete the source.
  ALWrapper::delete_sources(1, &source);

  // error = alGetError();
  // if (error != AL_NO_ERROR) {
  //   std::ostringstream oss;
  //   oss << "Failed to delete source " << source << ": " << std::hex << error;
  //   Debug::error(oss.str());
  // }

  // Delete the buffers.
  ALWrapper::delete_buffers(nb_buffers, buffers);

  // error = alGetError();
  // if (error != AL_NO_ERROR) {
  //   std::ostringstream oss;
  //   oss << "Failed to delete " << nb_buffers << " buffers: " << std::hex << error;
  //   Debug::error(oss.str());
  // }

  switch (format) {
    case FORMAT_SPC:
      break;
    case FORMAT_IT:
      it_decoder->unload();
      break;
    case FORMAT_OGG:
      ogg_decoder->unload();
      break;
    case FORMAT_NONE:
      Debug::die("stop: Invalid music format");
      break;
  }

  playing = false;
}

/**
 * \brief Returns the playing status of this music
 * 
 * \return \c true if the music is currently playing
 */
bool Music::is_playing() const {
  return playing;
}

/**
 * \brief Returns whether the music is paused.
 * \return true if the music is paused, false otherwise
 */
bool Music::is_paused() const {

  if (!MusicSystem::is_initialized()) {
    return false;
  }

  ALint status;
  ALWrapper::get_source_int(source, AL_SOURCE_STATE, &status);

  return status == AL_PAUSED;
}

/**
 * \brief Pauses or resumes the music.
 * \param pause true to pause the music, false to resume it
 */
void Music::set_paused(bool pause) {

  if (!MusicSystem::is_initialized()) {
    return;
  }

  if (pause) {
    ALWrapper::pause_source(source);
  }
  else {
    ALWrapper::play_source(source);
  }
}

const ScopedLuaRef& Music::get_callback() const {
  return callback_ref;
}

/**
 * \brief Sets a Lua function to call when the music ends.
 *
 * The previous callback if any is discarded.
 *
 * \param callback_ref Lua ref to a function to call when the music ends
 * or an empty ref.
 */
void Music::set_callback(const ScopedLuaRef& callback_ref) {
  this->callback_ref = callback_ref;
}

/**
 * \brief Returns the name identifying this type in Lua.
 * \return The name identifying this type in Lua.
 */
const std::string& Music::get_lua_type_name() const {
  return LuaContext::music_module_name;
}

}
