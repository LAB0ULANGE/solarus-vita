/*
 * Copyright (C) 2018-2020 std::gregwar, Solarus - http://www.solarus-games.org
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
#include "solarus/core/Joypad.h"
#include "solarus/lua/LuaContext.h"
#include "solarus/core/InputEvent.h"

namespace Solarus {

const std::string EnumInfoTraits<JoyPadAxis>::pretty_name = "joypad axis";
const std::string EnumInfoTraits<JoyPadButton>::pretty_name = "joypad button";

const EnumInfo<JoyPadButton>::names_type EnumInfoTraits<JoyPadButton>::names = {
  {JoyPadButton::A, "a"},
  {JoyPadButton::B, "b"},
  {JoyPadButton::X, "x"},
  {JoyPadButton::Y, "y"},
  {JoyPadButton::BACK, "back"},
  {JoyPadButton::GUIDE, "guide"},
  {JoyPadButton::START, "start"},
  {JoyPadButton::LEFT_STICK, "left_stick"},
  {JoyPadButton::RIGHT_STICK, "right_stick"},
  {JoyPadButton::LEFT_SHOULDER, "left_shoulder"},
  {JoyPadButton::RIGHT_SHOULDER, "right_shoulder"},
  {JoyPadButton::DPAD_UP, "dpad_up"},
  {JoyPadButton::DPAD_DOWN, "dpad_down"},
  {JoyPadButton::DPAD_LEFT, "dpad_left"},
  {JoyPadButton::DPAD_RIGHT, "dpad_right"},
  {JoyPadButton::MISC_1, "misc_1"},
  {JoyPadButton::PADDLE_1, "paddle_1"},
  {JoyPadButton::PADDLE_2, "paddle_2"},
  {JoyPadButton::PADDLE_3, "paddle_3"},
  {JoyPadButton::PADDLE_4, "paddle_4"},
  {JoyPadButton::TOUCH_PAD, "touch_pad"},
};

const EnumInfo<JoyPadAxis>::names_type EnumInfoTraits<JoyPadAxis>::names = {
  {JoyPadAxis::LEFT_X, "left_x"},
  {JoyPadAxis::LEFT_Y, "left_y"},
  {JoyPadAxis::RIGHT_X, "right_x"},
  {JoyPadAxis::RIGHT_Y, "right_y"},
  {JoyPadAxis::TRIGGER_LEFT, "trigger_left"},
  {JoyPadAxis::TRIGGER_RIGHT, "trigger_right"},
};

std::map<std::string, std::string> Joypad::legacy_bindings_mapping = {
  {"button 0", "a"},
  {"button 1", "b"},
  {"button 2", "x"},
  {"button 3", "y"},
  {"button 4", "left_shoulder"},
  {"button 5", "right_shoulder"},
  {"button 6", "back"},
  {"button 7", "start"},
  {"button 8", "left_stick"},
  {"button 9", "right_stick"},
  {"hat 0 up", "dpad_up"},
  {"hat 0 down", "dpad_down"},
  {"hat 0 left", "dpad_left"},
  {"hat 0 right", "dpad_right"},

  {"axis 0 -", "left_x -"},
  {"axis 0 +", "left_x +"},
  {"axis 1 -", "left_y -"},
  {"axis 1 +", "left_y +"},
  {"axis 3 -", "right_x -"},
  {"axis 3 +", "right_x +"},
  {"axis 4 -", "right_y -"},
  {"axis 4 +", "right_y +"},
  {"axis 2 -", "trigger_left"},
  {"axis 5 -", "trigger_right"},
};

std::map<JoyPadButton, int> Joypad::legacy_button_number_mapping = {
  {JoyPadButton::A, 0},
  {JoyPadButton::B, 1},
  {JoyPadButton::X, 2},
  {JoyPadButton::Y, 3},
  {JoyPadButton::LEFT_SHOULDER, 4},
  {JoyPadButton::RIGHT_SHOULDER, 5},
  {JoyPadButton::BACK, 6},
  {JoyPadButton::START, 7},
  {JoyPadButton::LEFT_STICK, 8},
  {JoyPadButton::RIGHT_STICK, 9},
  {JoyPadButton::DPAD_UP, 10},
  {JoyPadButton::DPAD_DOWN, 11},
  {JoyPadButton::DPAD_LEFT, 12},
  {JoyPadButton::DPAD_RIGHT, 13},
};

Joypad::Joypad(SDL_GameController *sdl_gc, SDL_Joystick *sdl_js):
    controller(sdl_gc), joystick(sdl_js)
{
}

bool Joypad::is_button_pressed(JoyPadButton button) const {
  return SDL_GameControllerGetButton(controller.get(),(SDL_GameControllerButton)button);
}

double Joypad::get_axis(JoyPadAxis axis) const {
  return compute_axis_val(SDL_GameControllerGetAxis(controller.get(),
                                   (SDL_GameControllerAxis)axis));
}

std::string Joypad::get_name() const {
  const char* name = SDL_GameControllerName(controller.get());
  if (name == NULL) {
    return "";
  }
  return std::string(name);
}

void Joypad::rumble(float low_frequency_intensity, float high_frequency_intensity, uint32_t duration) {
  SDL_GameControllerRumble(
    controller.get(),
    65535 * low_frequency_intensity,
    65535 * high_frequency_intensity,
    duration
  );
}

bool Joypad::has_rumble() {
  return SDL_GameControllerHasRumble(controller.get());
}

bool Joypad::is_attached() {
  return static_cast<bool>(controller);
}

void Joypad::reset() {
  controller.reset();
  joystick.reset();
}

const std::string& Joypad::get_lua_type_name() const {
  return LuaContext::joypad_module_name;
}

double Joypad::compute_axis_val(int16_t axis) {
  auto deadzone = InputEvent::get_joypad_deadzone();
  if (std::abs(axis) < deadzone) {
    return 0.0;
  }
  else {
    return axis > 0 ? double(std::max(axis-deadzone,0)) / (32767-deadzone) : double(std::min(axis+deadzone,0)) / (32768-deadzone);
    //return axis > 0 ? double(axis) / (32767) : double(axis) / (32768); //Old straight deadzone computation
  }
}

}
