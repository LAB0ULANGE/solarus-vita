local map = ...
local game = map:get_game()

function map:on_opening_transition_finished()
  local success, error_message = pcall(function() game:set_value("function", true) end)
  assert_equal(success, false)
  sol.main.exit()
end
