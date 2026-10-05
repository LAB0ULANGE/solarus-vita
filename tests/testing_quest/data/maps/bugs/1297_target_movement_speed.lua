local map = ...

function map:on_opening_transition_finished()

  local movement = sol.movement.create("target")
  local speed = 96
  movement:set_target(target_sensor:get_position())
  movement:set_speed(speed)

  local initial_time = sol.main.get_elapsed_time()
  local expected_duration = block:get_distance(target_sensor) / speed * 1000
  movement:start(block, function()
    local duration = sol.main.get_elapsed_time() - initial_time
    assert_equal(duration, expected_duration)
    sol.main.exit()
  end)

end

