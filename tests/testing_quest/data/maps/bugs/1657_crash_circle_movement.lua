local map = ...
local game = map:get_game()

function map:on_opening_transition_finished()
  local m = sol.movement.create("circle")
  m:set_radius(48)
  m:set_center(block:get_position())
  m:set_angular_speed(0)
  m:start(block)
end
