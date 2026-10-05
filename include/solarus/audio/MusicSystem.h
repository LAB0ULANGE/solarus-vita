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
#ifndef SOLARUS_MUSIC_SYSTEM_H
#define SOLARUS_MUSIC_SYSTEM_H

#include "solarus/core/Common.h"
#include "solarus/audio/Music.h"
#include "solarus/audio/MusicPtr.h"
#include "solarus/audio/Sound.h"
#include "solarus/lua/ScopedLuaRef.h"
#include <string>

namespace Solarus {

class SOLARUS_API MusicSystem {
  
  public:
    static void initialize();
    static void quit();
    static bool is_initialized();
    static void update();
    static void find_music_file(const std::string& music_id, std::string& file_name, Music::Format& format);
    static bool exists(const std::string& music_id);
    static void add_music(MusicPtr music);
    static void remove_music(MusicPtr music);

    static int get_global_volume();
    static void set_global_volume(int volume);
    static Music::Format get_current_music_format();
    static int get_current_music_num_channels();
    static float get_current_music_channel_volume(int channel);
    static void set_current_music_channel_volume(int channel, int volume);
    static int get_current_music_tempo();
    static void set_current_music_tempo(int tempo);

    static void play(const std::string& music_id, bool loop);
    static void play(const std::string& music_id, bool loop, const ScopedLuaRef& callback_ref);
    static void stop_playing();
    static const std::string& get_current_music_id();
    static void pause_playing();
    static void resume_playing();

    static void notify_device_disconnected_all();
    static void notify_device_reconnected_all();

  private:
    static bool initialized;                     /**< If the music system is initialized to prevent errors while controlling playback. */
    static float global_volume;                  /**< Global volume for all musics, can be modified in settings by the player. */

    static std::shared_ptr<Music> current_music; /**< The music currently played (if any) using sol.audio historical API. */
    static std::list<MusicPtr> current_musics;   /**< The musics currently playing using the sol.music API. */
};

}

#endif
