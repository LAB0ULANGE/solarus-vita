local map = ...
local game = map:get_game()

function map:on_started()

  local initial_x, initial_y = thrown_entity:get_position()
  thrown_entity:set_follow_streams(true)
  thrown_entity:set_can_traverse("stream", true)
  local movement = sol.movement.create("straight")
  movement:set_angle(3 * math.pi / 2)
  movement:set_speed(200)
  movement:set_smooth(false)
  movement:set_max_distance(160)
  movement:start(thrown_entity, function()
    local x, y = thrown_entity:get_position()
    assert(x < initial_x)  -- The stream should have had an influence.

    -- Test with an entity created dynamically.
    local stream_x, stream_y, stream_layer = stream_1:get_position()
    local e2 = map:create_custom_entity({
      x = stream_x,
      y = stream_y,
      layer = stream_layer,
      width = 16,
      height = 16,
      sprite = "16x16",
      direction = 0,
    })
    e2:set_follow_streams(true)
    initial_x, initial_y = e2:get_position()
    sol.timer.start(map, 1000, function()
      local x, y = e2:get_position()
      assert(x < initial_x)  -- The stream should have had an influence.
      sol.main.exit()
    end)
  end)
end
