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
#ifndef SOLARUS_MUSIC_H
#define SOLARUS_MUSIC_H

#include "solarus/core/Common.h"
#include "solarus/audio/MusicPtr.h"
#include "solarus/audio/Sound.h"
#include "solarus/lua/ScopedLuaRef.h"
#include <memory>
#include <string>
#include <vector>

namespace Solarus {

class ItDecoder;
class OggDecoder;
class SpcDecoder;

/**
 * \brief Represents a music that can be played.
 *
 * A music should be in format .spc (Snes), .it (Impulse Tracker module) or .ogg.
 * The .mp3 format will probably be supported in a future version.
 * Before using this class, the audio system should have been
 * initialized, by calling Sound::initialize().
 */
class SOLARUS_API Music: public ExportableToLua {

  public:
    /**
     * The music file formats recognized.
     */
    enum Format {
      FORMAT_NONE,      /**< No music. */
      FORMAT_SPC,       /**< Original Snes music. */
      FORMAT_IT,        /**< Impulse Tracker module. */
      FORMAT_OGG        /**< Ogg Vorbis. */
    };

    static const std::string none;               /**< Special id indicating that there is no music. */
    static const std::string unchanged;          /**< Special id indicating that the music is the same as before. */
    static const std::vector<std::string>
        format_names;                            /**< Name of each format. */

    Music();
    Music(const std::string& music_id, bool loop, const ScopedLuaRef& callback_ref);

    const std::string& get_id() const;
    Format get_format();
    int get_volume() const;
    void set_volume(int volume);
    int get_num_channels();
    int get_channel_volume(int channel);
    void set_channel_volume(int channel, int volume);
    int get_channel_pan(int channel);
    void set_channel_pan(int channel, int pan);
    int get_tempo();
    void set_tempo(int tempo);

    bool start();
    void stop();
    bool update_playing();

    bool is_playing() const;
    bool is_paused() const;
    void set_paused(bool pause);
    const ScopedLuaRef& get_callback() const;
    void set_callback(const ScopedLuaRef& callback_ref);

    void notify_device_disconnected();
    void notify_device_reconnected();
    void notify_global_volume_changed();

    const std::string& get_lua_type_name() const override;    

    static MusicPtr create(const std::string& music_id);

  private:
    void load(const std::string& music_id);
    void decode_spc(ALuint destination_buffer, ALsizei nb_samples);
    void decode_it(ALuint destination_buffer, ALsizei nb_samples);
    void decode_ogg(ALuint destination_buffer, ALsizei nb_samples);

    bool load_successful;                        /**< If the music loading phase has been successful. */
    bool playing;                                /**< If the music is currently playing. */
    std::string id;                              /**< id of this music */
    std::string file_name;                       /**< name of the file to play */
    Format format;                               /**< format of the music, detected from the file name */
    bool loop;                                   /**< Whether the music should loop. */
    ScopedLuaRef callback_ref;                   /**< Lua ref to a function to call when the music finishes. */

    static constexpr int nb_buffers = 8;
    static constexpr int buffer_size = 4096;
    ALuint buffers[nb_buffers];                  /**< multiple buffers used to stream the music */
    ALuint source;                               /**< the OpenAL source streaming the buffers */

    std::unique_ptr<SpcDecoder> spc_decoder;     /**< The SPC decoder. */
    std::unique_ptr<ItDecoder> it_decoder;       /**< The IT decoder. */
    std::unique_ptr<OggDecoder> ogg_decoder;     /**< The OGG decoder. */
    
    float volume;                                /**< Volume of this music (0.0 to 1.0) */
};

}

#endif

