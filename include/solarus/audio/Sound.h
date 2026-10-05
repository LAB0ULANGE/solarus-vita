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
#ifndef SOLARUS_SOUND_H
#define SOLARUS_SOUND_H

#include "solarus/audio/SoundPtr.h"
#include "solarus/core/Common.h"
#include "solarus/lua/ExportableToLua.h"
#include <cstdint>
#include <string>
#include <list>
#include <AL/al.h>
#include <AL/alc.h>
#include <vorbis/vorbisfile.h>

namespace Solarus {

class Arguments;
class ResourceProvider;
class SoundBuffer;

/**
 * \brief Represents a sound effect that can be played in the program.
 *
 * Represents the state of plyaing the sound effect.
 * This class also handles the initialization of the whole audio system.
 */
class SOLARUS_API Sound: public ExportableToLua {

  public:

    static SoundPtr create(const SoundBuffer& data);
    ~Sound();

    const std::string& get_id() const;
    bool start();
    void stop();
    bool is_playing() const;
    bool is_paused() const;
    void set_paused(bool paused);
    bool is_paused_by_script() const;
    void set_paused_by_script(bool paused_by_script);
    void update_paused();
    bool is_looped() const;
    void set_looped(bool looped);
    int get_volume() const;
    void set_volume(int volume);
    float get_pan() const;
    void set_pan(float pan);
    float get_pitch() const;
    void set_pitch(float pitch);

    static bool exists(const std::string& sound_id);
    static void play(const std::string& sound_id);
    static void pause_all();
    static void resume_all();
    static void stop_all();

    static void initialize(const Arguments& args, ResourceProvider* resource_provider);
    static void quit();
    static bool is_initialized();
    static void update();
    static bool check_openal_clean_state(const std::string& function_name);

    static int get_global_volume();
    static void set_global_volume(int global_volume);

    const std::string& get_lua_type_name() const override;

  private:

    explicit Sound(const SoundBuffer& data);
    float get_actual_volume() const;
    bool update_playing();
    static void update_device_connection();
    void notify_device_disconnected();

    const SoundBuffer& data;                     /**< The loaded sound data. */
    ALuint source = 0;                           /**< The source currently playing this sound. */
    bool looped = false;                         /**< If the sound should be played in a loop. */
    bool paused_by_script = false;               /**< Whether the sound is paused by a Lua script. */
    float volume = 1.0;                          /**< Volume of this sound effect
                                                  * (0.0 to 1.0, relative to the global volume). */
    float pan = 0.0;                             /**< Pan of this sound effect (-1.0 to 1.0, default is 0.0). */
    float pitch = 1.0;                           /**< Pitch of this sound effect (0.5 to 2.0, default is 1.0). */
    static bool paused_by_system;                /**< Whether sounds are currently paused by the main loop,
                                                  * e.g. when losing focus */

    static bool audio_enabled;                   /**< \c true unless -no-audio was passed. */
    static ALCdevice* device;                    /**< OpenAL device, nullptr if disconnected. */
    static ALCcontext* context;                  /**< OpenAL context. */
    static std::list<SoundPtr>
        current_sounds;                          /**< The sounds currently playing. */

    static float global_volume;                  /**< Global volume of sound effects (0.0 to 1.0). */
    static uint32_t next_device_detection_date;  /**< Date of the next attempt to detect an audio device. */
    static bool pc_play;                         /**< Whether playing performance counter is used. */
    static ResourceProvider* resource_provider;  /**< The main loop resource cache. */
};

}

#endif
