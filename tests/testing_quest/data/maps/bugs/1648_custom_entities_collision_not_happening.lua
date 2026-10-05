local map = ...

function map:on_opening_transition_finished()
  local x, y, layer = map:get_hero():get_position()

  local custom_entity = map:create_custom_entity({
    x = x,
    y = y,
    layer = layer,
    width = 32,
    height = 32,
    direction = 0,
    name = "created_custom_entity"
  })
  custom_entity:add_collision_test("overlapping", function(_, other)
    if other:get_type() == "hero" then
      return
    end

    if other:get_type() == "custom_entity" then
      sol.main.exit()
    end
  end)
end
