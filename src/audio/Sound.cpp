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
#include "solarus/audio/MusicSystem.h"
#include "solarus/audio/Sound.h"
#include "solarus/core/Arguments.h"
#include "solarus/core/Debug.h"
#include "solarus/core/PerfCounter.h"
#include "solarus/core/QuestFiles.h"
#include "solarus/core/ResourceProvider.h"
#include "solarus/lua/LuaContext.h"
#include <algorithm>
#ifdef SOLARUS_OPENAL_EXTENSIONS_RECONNECT
#  include <AL/alext.h>
#endif
#include <cstdlib>
#include <cstring>

namespace Solarus {

bool Sound::audio_enabled = false;
ALCdevice* Sound::device = nullptr;
ALCcontext* Sound::context = nullptr;
float Sound::global_volume = 1.0;
bool Sound::pc_play = false;
std::list<SoundPtr> Sound::current_sounds;
uint32_t Sound::next_device_detection_date = 0;
bool Sound::paused_by_system = false;
ResourceProvider* Sound::resource_provider = nullptr;

/**
 * \brief Creates a new Ogg Vorbis sound.
 * \param data The loaded sound data ready to be played.
 */
Sound::Sound(const SoundBuffer& data):
  data(data),
  source(AL_NONE),
  paused_by_script(false),
  volume(1.0) {
}

/**
 * \brief Destroys the sound.
 */
Sound::~Sound() {
  stop();
}

/**
 * \brief Creates a new Ogg Vorbis sound.
 * \param data The loaded sound data ready to be played.
 */
SoundPtr Sound::create(const SoundBuffer& data) {
  Sound* sound = new Sound(data);
  return SoundPtr(sound);
}

/**
 * \brief Initializes the audio (music and sound) system.
 *
 * This method should be called when the application starts.
 * If the argument -no-audio is provided, this function has no effect and
 * there will be no sound.
 * If the argument -perf-sound-play is provided and is "yes", sound
 * playing will be accounted using a performance counter.
 *
 * \param args Command-line arguments.
 * \param resource_provider The resource provider from the main loop if any, or nullptr.
 */
void Sound::initialize(const Arguments& args, ResourceProvider* resource_provider) {

  // Check the -no-audio option.
  audio_enabled = !args.has_argument("-no-audio");
  if (!audio_enabled) {
    return;
  }

  // Check the -perf-sound-play option.
  pc_play = args.get_argument_value("-perf-sound-play") == "yes";

  Sound::resource_provider = resource_provider;
  // Initialize OpenAL.
  update_device_connection();
  if (device == nullptr) {
    return;
  }

  alGenBuffers(0, nullptr);  // Necessary on some systems to avoid errors with the first sound loaded.
  alListenerf(AL_GAIN, 0.7);  // Reduce master volume, too loud by default.

  // initialize the music system
  MusicSystem::initialize();
}

/**
 * \brief Closes the audio (music and sound) system.
 *
 * This method should be called when exiting the application.
 */
void Sound::quit() {

  if (!is_initialized()) {
    return;
  }

  check_openal_clean_state("Sound::quit");

  // stop all currently playing sounds
  stop_all();

  // uninitialize the music subsystem
  MusicSystem::quit();

  // uninitialize OpenAL
  alcMakeContextCurrent(nullptr);
  alcDestroyContext(context);
  context = nullptr;
  alcCloseDevice(device);
  device = nullptr;
  global_volume = 1.0;
  current_sounds.clear();
  next_device_detection_date = 0;
  audio_enabled = false;
  resource_provider = nullptr;
}

/**
 * \brief Checks if the audio device is connected.
 */
void Sound::update_device_connection() {

#if SOLARUS_OPENAL_EXTENSIONS_RECONNECT
#define SOLARUS_OPENAL_DEVICE_SPECIFIER ALC_ALL_DEVICES_SPECIFIER
#else
#define SOLARUS_OPENAL_DEVICE_SPECIFIER ALC_DEVICE_SPECIFIER
#endif

  if (device != nullptr) {
    // Check if the device is still connected.
    ALCint is_connected = 0;
#ifdef SOLARUS_OPENAL_EXTENSIONS_RECONNECT
    alcGetIntegerv(device, ALC_CONNECTED, 1, &is_connected);
#else
    is_connected = device != nullptr;
#endif
    if (!is_connected) {
      Logger::info("Lost connection to audio device");
    } else {
      if (System::now_ms() >= next_device_detection_date) {
        // Check if this device is still the default one.
        next_device_detection_date = System::now_ms() + 1000;

        const ALchar* current_device_name = alcGetString(device, SOLARUS_OPENAL_DEVICE_SPECIFIER);
        const ALchar* default_device_name = alcGetString(nullptr, SOLARUS_OPENAL_DEVICE_SPECIFIER);
        if (current_device_name != nullptr &&
            default_device_name != nullptr &&
            std::strcmp(current_device_name, default_device_name)) {
          // This device is no longer the default one.
          Logger::info(std::string("Disconnecting from audio device '") + current_device_name +
                       "' because the default device is now '" + default_device_name + "'");
          is_connected = false;
        }
      }
    }
    if (!is_connected) {
      ALboolean success = alcMakeContextCurrent(nullptr);
      if (!success) {
        Debug::error("Failed to unset OpenAL context");
      }
      alcDestroyContext(context);
      context = nullptr;
      alcCloseDevice(device);
      device = nullptr;
      next_device_detection_date = System::now_ms();
      if (resource_provider != nullptr) {
        // Need to clear all cached sounds.
        for (SoundPtr sound: current_sounds) {
          sound->notify_device_disconnected();
        }
        resource_provider->notify_audio_device_disconnected();
      }
      MusicSystem::notify_device_disconnected_all();
    }
  }

  if (device == nullptr) {
    if (System::now_ms() >= next_device_detection_date) {
      // Try to connect or reconnect to an audio device.
      device = alcOpenDevice(nullptr);
      if (device == nullptr) {
        Debug::error("Cannot open audio device");
      } else {
        context = alcCreateContext(device, nullptr);
        if (context == nullptr) {
          Debug::error("Cannot create audio context");
          alcCloseDevice(device);
          device = nullptr;
        } else if (!alcMakeContextCurrent(context)) {
          Debug::error("Cannot activate audio context");
          alcDestroyContext(context);
          context = nullptr;
          alcCloseDevice(device);
          device = nullptr;
        } else {
          const ALchar* current_device_name = alcGetString(device, SOLARUS_OPENAL_DEVICE_SPECIFIER);
          Logger::info(std::string("Connected to audio device '") + (current_device_name ? current_device_name : "") + "'");
          MusicSystem::notify_device_reconnected_all();
        }
      }
      if (device == nullptr) {
        // The attempt failed: try again later.
        next_device_detection_date = System::now_ms() + 1000;
      }
    }
  }
}

/**
 * \brief Notifies this sound that the audio device was disconnected.
 */
void Sound::notify_device_disconnected() {

  // All sources and buffers are already destroyed by OpenAL at this point.
  source = AL_NONE;
}

/**
 * \brief Returns whether the audio (music and sound) system is initialized.
 * \return \c true if the audio (music and sound) system is initilialized.
 */
bool Sound::is_initialized() {
  return audio_enabled;
}

/**
 * \brief Returns the id of this sound.
 * \return The sound id.
 */
const std::string& Sound::get_id() const {
  return data.get_id();
}

/**
 * \brief Returns whether a sound exists.
 * \param sound_id id of the sound to test
 * \return true if the sound exists
 */
bool Sound::exists(const std::string& sound_id) {

  std::ostringstream oss;
  oss << "sounds/" << sound_id << ".ogg";
  return QuestFiles::data_file_exists(oss.str());
}

/**
 * \brief Starts playing the specified sound.
 * \param sound_id Id of the sound to play. An empty string does nothing.
 */
void Sound::play(const std::string& sound_id) {

  if (device == nullptr) {
    // Sound might be disabled.
    return;
  }

  if (sound_id.empty()) {
    return;
  }

  if (resource_provider == nullptr) {
    Debug::error("Cannot play sound '" + sound_id + "': missing resource provider");
  }
  if (pc_play) {
    PerfCounter::update("sound-play");
  }

  SoundBuffer& buffer = resource_provider->get_sound(sound_id);
  SoundPtr sound = Sound::create(buffer);
  sound->start();
}

/**
 * \brief Returns the default volume of sound effects.
 * \return The volume (0 to 100).
 */
int Sound::get_global_volume() {

  return static_cast<int>(global_volume * 100.0 + 0.5);
}

/**
 * \brief Sets the global volume of sound effects.
 * \param volume The new volume (0 to 100).
 */
void Sound::set_global_volume(int global_volume) {

  Sound::global_volume = std::min(100, std::max(0, global_volume)) / 100.0;
}

/**
 * \brief Returns the relative volume of this sound effect.
 *
 * This is relative to the global volume.
 *
 * \return The volume (0 to 100).
 */
int Sound::get_volume() const {

  return static_cast<int>(volume * 100.0 + 0.5);
}

/**
 * \brief Sets the relative volume of this sound effect.
 *
 * This is relative to the global volume.
 *
 * \return The volume (0 to 100).
 */
void Sound::set_volume(int volume) {

  this->volume = std::min(100, std::max(0, volume)) / 100.0;

  if (source != AL_NONE) {
    alSourcef(source, AL_GAIN, get_actual_volume());
  }
}

/**
 * \brief Returns the actual volume of this sound.
 *
 * This is <tt>get_volume() * get_global_volume()</tt>.
 *
 * \return The actual volume between 0.0 and 1.0.
 */
float Sound::get_actual_volume() const {

  return volume * global_volume;
}

/**
 * \brief Returns the current pan value of this sound.
 * 
 * 0.0 is the default value for center.
 * -1.0 is totally to the left.
 * 1.0 is totally to the right.
 * 
 * \return The pan value between -1.0 and 1.0.
 */
float Sound::get_pan() const {

  return pan;
}

/**
 * \brief Sets the pan value of this sound effect.
 *
 * This has no effect on stereo sounds.
 * 0.0 is the default value for center.
 * -1.0 is totally to the left.
 * 1.0 is totally to the right.
 *
 * \return The pan (-1.0 to 1.0).
 */
void Sound::set_pan(float pan) {
  this->pan = pan;

  if (source != AL_NONE) {
    alSourcef(source, AL_ROLLOFF_FACTOR, 0.0f);
    alSourcei(source, AL_SOURCE_RELATIVE, AL_TRUE);
    alSource3f(
      source,
      AL_POSITION,
      pan,
      0,
      -sqrtf(1.0f - pan * pan)
    );

    ALenum pan_error = alGetError();
    if (pan_error != AL_NO_ERROR) {
      std::ostringstream oss;
      oss << "Cannot set pan to sound '" << get_id() << "': error " << std::hex << pan_error;
      Debug::error(oss.str());
    }
  }
}

/**
 * \brief Returns the current pitch value of this sound.
 * 
 * 1.0 is the default value for normal speed.
 * 
 * \return The pitch value between 0.5 and 2.0.
 */
float Sound::get_pitch() const {

  return pitch;
}

/**
 * \brief Sets the pitch value of this sound effect.
 * 
 * The value acts as a sample rate multiplier.
 *
 * \return The pitch (0.5 to 2.0).
 */
void Sound::set_pitch(float pitch) {
  this->pitch = pitch;

  if (source != AL_NONE) {
    alSourcef(source, AL_PITCH, pitch);

    ALenum pitch_error = alGetError();
    if (pitch_error != AL_NO_ERROR) {
      std::ostringstream oss;
      oss << "Cannot set pitch to sound '" << get_id() << "': error " << std::hex << pitch_error;
      Debug::error(oss.str());
    }
  }
}

/**
 * \brief Updates the audio (music and sound) system.
 *
 * This function is called repeatedly by the game.
 *
 * \param resource_provider The resource provider.
 */
void Sound::update() {

  if (!is_initialized()) {
    return;
  }

  update_device_connection();

  if (device != nullptr) {

    // update the playing sounds
    std::list<SoundPtr> sounds_to_remove;
    for (const SoundPtr& sound: current_sounds) {
      if (!sound->update_playing()) {
        sounds_to_remove.push_back(sound);
      }
    }

    for (const SoundPtr& sound: sounds_to_remove) {
      current_sounds.remove(sound);
    }
  }

  check_openal_clean_state("Sound::update");

  // also update the music
  MusicSystem::update();
}

/**
 * \brief Updates this sound when it is playing.
 * \return true if the sound is still playing, false if it is finished.
 */
bool Sound::update_playing() {

  check_openal_clean_state("Sound::update_playing");

  // See if this sound is still playing.
  if (source == AL_NONE) {
    return false;
  }

  ALint status;
  alGetSourcei(source, AL_SOURCE_STATE, &status);

  if (status == AL_STOPPED) {  // The sound has just finished.
    stop();
  }

  return source != AL_NONE;
}

/**
 * \brief Plays the sound.
 * \return \c true if the sound was started successfully, \c false otherwise.
 */
bool Sound::start() {

  if (device == nullptr) {
    // Sound might be disabled.
    return false;
  }

  if (!check_openal_clean_state("Sound::start")) {
    return false;
  }

  ALuint buffer = data.get_buffer();
  if (buffer == AL_NONE) {
    return false;
  }

  if (source == AL_NONE) {
    // create a source
    alGenSources(1, &source);
    alSourcei(source, AL_BUFFER, buffer);
    alSourcef(source, AL_GAIN, get_actual_volume());
    alSourcei(source, AL_LOOPING, looped ? AL_TRUE : AL_FALSE);

    // update initial parameters
    set_pan(pan);
    set_pitch(pitch);

    // play the sound
    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
      std::ostringstream oss;
      oss << "Cannot attach buffer " << buffer
          << " to source " << source << " to play sound '" << get_id() << "': error " << std::hex << error;
      Debug::error(oss.str());
      alDeleteSources(1, &source);
      source = AL_NONE;
      return false;
    }
  }

  SoundPtr shared_this = std::static_pointer_cast<Sound>(shared_from_this());
  current_sounds.remove(shared_this);  // To avoid duplicates.
  current_sounds.push_back(shared_this);
  alSourcePlay(source);
  ALenum error = alGetError();
  if (error != AL_NO_ERROR) {
    std::ostringstream oss;
    oss << "Cannot play sound '" << get_id() << "': error " << std::hex << error;
    Debug::error(oss.str());
    return false;
  }

  return true;
}

/**
 * \brief Stops playing this sound, cleans the source and releases the buffer.
 */
void Sound::stop() {

  if (device == nullptr) {
    return;
  }

  if (source == AL_NONE) {
    return;
  }

  ALint status = 0;
  alGetSourcei(source, AL_SOURCE_STATE, &status);
  if (status == AL_PLAYING || status == AL_PAUSED) {
    // Stops the source if still active.
    alSourceStop(source);
  }
  alSourcei(source, AL_BUFFER, 0);  // This allows the buffer to get deleted later.
  alDeleteSources(1, &source);

  ALenum error = alGetError();
  if (error != AL_NO_ERROR) {
    std::ostringstream oss;
    oss << "Failed to delete AL source " << source
        << " for sound '" << get_id() << "': error " << std::hex << error;
    Debug::error(oss.str());
  }

  source = AL_NONE;
}

/**
 * \brief Returns whether the sound is currently playing.
 * \return \c true if the sound is playing.
 */
bool Sound::is_playing() const {
  if (device == nullptr || source == AL_NONE) {
    return false;
  }

  ALint status;
  alGetSourcei(source, AL_SOURCE_STATE, &status);
  return status == AL_PLAYING;
}

/**
 * \brief Returns whether the sound is currently paused.
 * \return \c true if the sound is paused.
 */
bool Sound::is_paused() const {

  if (device == nullptr || source == AL_NONE) {
    return false;
  }

  ALint status;
  alGetSourcei(source, AL_SOURCE_STATE, &status);
  return status == AL_PAUSED;
}

/**
 * \brief Pauses or resumes this sound.
 * \param paused \c true to pause the source, \c false to resume it.
 */
void Sound::set_paused(bool paused) {

  if (device == nullptr || source == AL_NONE) {
    return;
  }

  ALint status;
  alGetSourcei(source, AL_SOURCE_STATE, &status);

  if (paused) {
    if (status == AL_PLAYING) {
      alSourcePause(source);
    }
  }
  else {
    if (status == AL_PAUSED) {
      alSourcePlay(source);
    }
  }
}

/**
 * \brief Returns whether the sound is currently paused by a script.
 * \return \c true if the sound is paused by a script.
 */
bool Sound::is_paused_by_script() const {
  return paused_by_script;
}

/**
 * \brief Pauses or resumes this sound from a script perspective.
 *
 * This is independent of the automatic pause/resume that can happen when
 * the window loses focus.
 *
 * \param paused \c true to pause the sound, \c false to resume it.
 */
void Sound::set_paused_by_script(bool paused_by_script) {

  this->paused_by_script = paused_by_script;
  update_paused();
}

/**
 * \brief Pauses all currently playing sounds.
 */
void Sound::pause_all() {

  paused_by_system = true;
  for (const SoundPtr& sound: current_sounds) {
    sound->update_paused();
  }
}

/**
 * \brief Resumes playing all sounds previously paused.
 */
void Sound::resume_all() {

  paused_by_system = false;
  for (const SoundPtr& sound: current_sounds) {
    sound->update_paused();
  }
}

/**
 * \brief Stops all currently playing sounds.
 */
void Sound::stop_all() {

  for (const SoundPtr& sound: current_sounds) {
    sound->stop();
  }
}

/**
 * \brief Returns whether the sound is looped.
 * \return true if the sound is looped.
 */
bool Sound::is_looped() const {
  return looped;
}

/**
 * \brief Sets if the sound should be played in a loop.
 * \param looped true if the sound is looped.
 */
void Sound::set_looped(bool looped) {
  this->looped = looped;
}

/**
 * \brief Pauses or resumes the sound depending on the current pause state.
 */
void Sound::update_paused() {

  set_paused(paused_by_script || paused_by_system);
}

/**
 * \brief Prints an error message if there is a current OpenAL error.
 * \param function_name Current function name for debugging purposes.
 * \return \c false if there is an error.
 */
bool Sound::check_openal_clean_state(const std::string& function_name) {

  ALenum error = alGetError();
  if (error != AL_NO_ERROR) {
    std::ostringstream oss;
    oss << "Previous audio error not cleaned in " << function_name << ": " << std::hex << error;
    Debug::error(oss.str());
    return false;
  }

  return true;
}


/**
 * \brief Returns the name identifying this type in Lua.
 * \return The name identifying this type in Lua.
 */
const std::string& Sound::get_lua_type_name() const {
  return LuaContext::sound_module_name;
}

}

