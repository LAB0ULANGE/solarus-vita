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

#include "solarus/audio/ALWrapper.h"
#include "solarus/core/Debug.h"
#include <sstream>

namespace Solarus {

void ALWrapper::clean_errors() {
  alGetError();
}

void ALWrapper::check_for_errors(const std::string& message) {
  ALenum error = alGetError();
  if (error != AL_NO_ERROR) {
    std::ostringstream oss;
    oss << message << ": " << std::hex << error;
    Debug::error(oss.str());
  }
}

void ALWrapper::generate_buffers(ALsizei n, ALuint *buffers) {
  clean_errors();
  alGenBuffers(n, buffers);
  check_for_errors("Generating n buffers");
}

void ALWrapper::delete_buffers(ALsizei n, const ALuint *buffers) {
  clean_errors();
  alDeleteBuffers(n, buffers);
  check_for_errors("Deleting n buffers");
}

void ALWrapper::set_buffer_data(ALuint buffer, ALenum format, const ALvoid *data, ALsizei size, ALsizei samplerate) {
  clean_errors();
  alBufferData(buffer, format, data, size, samplerate);
  check_for_errors("Setting data for buffer");
}

void ALWrapper::queue_source_buffers(ALuint source, ALsizei nb, const ALuint *buffers) {
  clean_errors();
  alSourceQueueBuffers(source, nb, buffers);
  check_for_errors("Queuing source buffers");
}

void ALWrapper::unqueue_source_buffers(ALuint source, ALsizei nb, ALuint *buffers) {
  clean_errors();
  alSourceUnqueueBuffers(source, nb, buffers);
  check_for_errors("Unqueuing source buffers");
}

void ALWrapper::generate_sources(ALsizei n, ALuint *sources) {
  clean_errors();
  alGenSources(n, sources);
  check_for_errors("Generating n sources");
}

void ALWrapper::delete_sources(ALsizei n, const ALuint *sources) {
  clean_errors();
  alDeleteSources(n, sources);
  check_for_errors("Deleting n sources");
}

void ALWrapper::get_source_int(ALuint source, ALenum param, ALint *value) {
  clean_errors();
  alGetSourcei(source, param, value);
  check_for_errors("Getting source param");
}

void ALWrapper::set_source_float(ALuint source, ALenum param, ALfloat value) {
  clean_errors();
  alSourcef(source, param, value);
  check_for_errors("Setting source param");
}

void ALWrapper::play_source(ALuint source) {
  clean_errors();
  alSourcePlay(source);
  check_for_errors("Playing source");
}

void ALWrapper::pause_source(ALuint source) {
  clean_errors();
  alSourcePause(source);
  check_for_errors("Pausing source");
}

void ALWrapper::stop_source(ALuint source) {
  clean_errors();
  alSourceStop(source);
  check_for_errors("Stopping source");
}

} // namepsace Solarus
