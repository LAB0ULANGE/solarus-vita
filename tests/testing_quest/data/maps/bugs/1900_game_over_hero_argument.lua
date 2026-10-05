local map = ...
local game = map:get_game()

function map:on_opening_transition_finished()

  game:start_game_over()
  sol.main.exit()
end
