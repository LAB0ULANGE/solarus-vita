local map = ...
local game = map:get_game()

function map:on_opening_transition_finished()
  game:simulate_command_pressed("right")
end

function reset_switch:on_activated()
  game:simulate_command_released("right")
  sol.main.reset()
  sol.main.exit()
end
