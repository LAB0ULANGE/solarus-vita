local map = ...

function map:on_opening_transition_finished()
  local mov = sol.movement.create("straight")
  mov:set_max_distance(64)
  mov.on_finished = function()
    sol.main.exit()
  end

  local x = some_entity:get_position()
  local should_check_position = false
  local should_check_next_frame = false

  sol.timer.start(map, 1000, function()
    mov:start(some_entity)
    should_check_next_frame = true
  end)

  sol.timer.start(map, 50, function()
    if should_check_position then
      local nx = some_entity:get_position()
      assert(nx - x < 24, "Movement should have increased coordinate by a small amount. Otherwise, this means the movement did not reset movement dates.")
      should_check_next_frame = false
      should_check_position = false
    end

    ---This prevent having no move in the same frame as the movement:start call
    if should_check_next_frame then
      should_check_position = true
    end

    return true
  end)
end
