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
#include "solarus/core/ControlsDispatcher.h"

#include "solarus/core/Controls.h"

namespace Solarus {

ControlsDispatcher* ControlsDispatcher::instance = nullptr;

ControlsDispatcher::ControlsDispatcher(MainLoop& main_loop) :
  main_loop(main_loop)
{
  instance = this;
}

ControlsDispatcher& ControlsDispatcher::get() {
  SOLARUS_REQUIRE(instance, "No current CommandsDispatcher");
  return *instance;
}

void ControlsDispatcher::notify_input(const InputEvent& event) {
  for(const auto& ptr : to_remove) {
    commands.erase(ptr);
  }

  to_remove.clear();

  const ControlSet commands_copy = commands;  // Iterate on a copy in case new controls are added during the event.
  for (const auto& wptr: commands_copy) {
    if (const auto ptr = wptr.lock(); ptr) {
      ptr->notify_input(event);
    }
  }
}

ControlsPtr ControlsDispatcher::create_commands_from_game(Game& game) {
  ControlsPtr commands =  std::make_shared<Controls>(main_loop, game);
  add_commands(commands);
  return commands;
}

ControlsPtr ControlsDispatcher::create_commands_from_keyboard() {
  ControlsPtr commands = std::make_shared<Controls>(main_loop);
  add_commands(commands);

  commands->load_default_keyboard_bindings();

  return commands;
}

ControlsPtr ControlsDispatcher::create_commands_from_joypad(const JoypadPtr& joypad) {
  ControlsPtr commands = std::make_shared<Controls>(main_loop);
  add_commands(commands);

  commands->load_default_joypad_bindings();
  commands->set_joypad(joypad);

  return commands;
}

void ControlsDispatcher::add_commands(const ControlsPtr& cmds) {
  commands.insert(std::weak_ptr<Controls>(cmds));
  to_remove.erase(cmds);
}

void ControlsDispatcher::remove_commands(const std::weak_ptr<Controls>& cmds) {
  to_remove.insert(cmds);
}

}
