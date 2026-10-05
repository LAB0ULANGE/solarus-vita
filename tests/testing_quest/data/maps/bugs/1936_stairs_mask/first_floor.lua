-- Lua script of map bugs/1936_stairs_mask/first_floor.
local map = ...
local game = map:get_game()


function map:on_opening_transition_finished()
  game:simulate_command_pressed('up')
end
