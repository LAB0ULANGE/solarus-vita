/*
 * Copyright (C) 2006-2025 Metallizer, Solarus - http://www.solarus-games.org
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
#ifndef SOLARUS_AL_WRAPPER_H
#define SOLARUS_AL_WRAPPER_H

#include <AL/al.h>
#include <AL/alc.h>
#include <string>

namespace Solarus {

class ALWrapper {
  public:
    // Buffers
    static void generate_buffers(ALsizei n, ALuint *buffers);
    static void delete_buffers(ALsizei n, const ALuint *buffers);
    static void set_buffer_data(ALuint buffer, ALenum format, const ALvoid *data, ALsizei size, ALsizei samplerate);
    static void queue_source_buffers(ALuint source, ALsizei nb, const ALuint *buffers);
    static void unqueue_source_buffers(ALuint source, ALsizei nb, ALuint *buffers);

    // Sources
    static void generate_sources(ALsizei n, ALuint *sources);
    static void delete_sources(ALsizei n, const ALuint *sources);
    static void get_source_int(ALuint source, ALenum param, ALint *value);
    static void set_source_float(ALuint source, ALenum param, ALfloat value);
    static void play_source(ALuint source);
    static void pause_source(ALuint source);
    static void stop_source(ALuint source);

  private:
    static void clean_errors();
    static void check_for_errors(const std::string& message);
};

} // namespace Solarus

#endif
