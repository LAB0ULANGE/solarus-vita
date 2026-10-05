local map = ...

function map:on_opening_transition_finished()
  custom_1:set_can_traverse("custom_entity", false)
  local mov = sol.movement.create("straight")
  mov:set_speed(100)
  mov:start(custom_1)

  sol.timer.start(map, 50, function()
    ---Checking for custom entity position
    local x = custom_1:get_position()
    assert(x < 225, "Entity has traversed but should have not")
    return true
  end)

  sol.timer.start(map, 3000, function()
    ---After 3 seconds, the entity should have NOT reached the limit
    sol.main.exit()
  end)
end
