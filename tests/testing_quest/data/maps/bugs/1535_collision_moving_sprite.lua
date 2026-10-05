local map = ...

entity_1:add_collision_test("sprite", function(_, other_entity, entity_1_sprite, entity_2_sprite)
  if other_entity == entity_2 then
    assert_equal(entity_1_sprite, entity_1:get_sprite())
    assert_equal(entity_2_sprite, entity_2:get_sprite())
    sol.main.exit()
  end
end)

function map:on_opening_transition_finished()
  -- Move the sprite of entity_2 to the position of entity 1.
  local x1, y1 = entity_1:get_position()
  local x2, y2 = entity_2:get_position()
  local offset_x, offset_y = x1 - x2, y1 - y2
  entity_2:get_sprite():set_xy(offset_x, offset_y)
end
