local map = ...
local game = map:get_game()

function map:on_opening_transition_finished()
  hero:set_direction(0)
  hero:set_direction(3)
  local success, error_message = pcall(function() hero:set_direction(5) end)
  assert_equal(success, false)
  print(error_message)
  assert(error_message:find("bad argument #1 to set_direction %(Invalid hero direction 5: should be between 0 and 3%)"))
  sol.main.exit()
end
