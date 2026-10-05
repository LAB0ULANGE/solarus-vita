local map = ...
local game = map:get_game()

function map:on_opening_transition_finished()

  local movement = sol.movement.create("straight")
  movement:set_angle(3 * math.pi / 2)
  movement:set_speed(88)
  movement:start(npc_1)

  local sprite = npc_1:get_sprite()
  assert_equal(sprite:get_animation(), "walking")
  local initial_x, initial_y = npc_1:get_position()

  sol.timer.start(map, 1000, function()
    movement:stop()
    assert_equal(sprite:get_animation(), "stopped")
    local x, y = npc_1:get_position()
    assert_equal(x, initial_x)
    assert(y ~= initial_y)
    sol.main.exit()
  end)
end
