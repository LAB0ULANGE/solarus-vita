local map = ...
local game = map:get_game()

function map:on_started()
  assert_equal(hero:get_layer(), 0)
end

function map:on_opening_transition_finished()
  assert_equal(hero:get_layer(), 0)
  sol.main.exit()
end
