local map = ...
local game = map:get_game()

function map:on_opening_transition_finished()
  game:start_dialog("a")
  local timer = sol.timer.start(map, 1000, function()
    hero:start_treasure("item_dialog_sprite", 1)
    sol.main.exit()
  end)
  timer:set_suspended_with_map(false)
end
