local item = ...
local game = item:get_game()

function item:on_started()
  item:set_savegame_variable("possession_bombs_assignable")
  item:set_assignable(true)
end

function item:on_using()
  item:set_finished()
end
