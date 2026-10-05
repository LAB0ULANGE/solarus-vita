local map = ...

function map:on_opening_transition_finished()

  assert_equal(hero:get_movement():get_type(), "straight_movement")

  for _, movement_type in ipairs({
    "straight",
    "random",
    "target",
    "path",
    "random_path",
    "path_finding",
    "circle",
    "jump",
    "pixel",
  }) do
    local movement = sol.movement.create(movement_type)
    assert_equal(movement:get_type(), movement_type .. "_movement")
  end
  sol.main.exit()
end
