local map = ...
local game = map:get_game()

function map:on_opening_transition_finished()
  assert_equal(hero:get_facing_entity(), facing)
  local movement = sol.movement.create("straight")
  movement:set_angle(3 * math.pi / 2)
  movement:set_speed(96)
  movement:set_max_distance(96)
  movement:start(facing, function()
    assert_equal(hero:get_facing_entity(), nil)
    sol.main.exit()
  end)
end
