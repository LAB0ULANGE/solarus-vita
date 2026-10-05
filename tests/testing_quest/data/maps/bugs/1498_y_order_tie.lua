local map = ...

local draw_order = 0

local function on_post_draw_track_order(entity)
  -- Track drawing order.
  entity.draw_order = draw_order
  draw_order = draw_order + 1
end

function map:on_started()

  for i = 1, 20 do
    local block = map:get_entity("block_" .. i)
    local npc = map:get_entity("npc_" .. i)

    if i > 4 then  -- 1 to 4 have different y positions, 5 to 20 have the exact same position.
      assert_equal_xy(block, npc)
    end

    block:set_drawn_in_y_order(true)
    block.on_post_draw = on_post_draw_track_order
    npc:set_drawn_in_y_order(true)
    npc.on_pre_draw = on_post_draw_track_order
  end

  sol.timer.start(map, 1000, sol.main.exit)
end

function map:on_draw()

  -- Check that entities were drawn in the correct, stable order.
  for i = 1, 20 do
    local block = map:get_entity("block_" .. i)
    local npc = map:get_entity("npc_" .. i)

    if i % 2 == 0 then
      -- Even numbers: the NPC should be displayed first.
      assert(npc.draw_order < block.draw_order)
    else
      -- Odd numbers: the block should be displayed first.
      assert(block.draw_order < npc.draw_order)
    end
  end

  -- Reset drawing order tracked.
  draw_order = 0
  for entity in map:get_entities("block_") do
    entity.draw_order = nil
  end
end
