local map = ...
local game = map:get_game()

function map:on_opening_transition_finished()
  local state = sol.state.create()
  state:set_can_control_movement(false)
  state:set_can_control_direction(true)
  hero:start_state(state)

  local initial_x, initial_y = hero:get_position()
  assert_equal(hero:get_sprite():get_direction(), 1)
  game:simulate_command_pressed("right")

  sol.timer.start(map, 1000, function()
    assert_equal(hero:get_sprite():get_direction(), 0)
    local x, y = hero:get_position()
    assert_equal(x, initial_x)
    assert_equal(y, initial_y)
    sol.main.exit()
  end)
end

