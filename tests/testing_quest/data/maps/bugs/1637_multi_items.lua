-- Lua script of map bugs/1637_multi_items.
-- This script is executed every time the hero enters this map.

-- Feel free to modify the code below.
-- You can add more events and remove the ones you don't need.

-- See the Solarus Lua API documentation:
-- https://www.solarus-games.org/doc/latest

local map = ...
local game = map:get_game()

-- Event called at initialization time, as soon as this map is loaded.
function map:on_started()

  -- You can initialize the movement and sprites of various
  -- map entities here.
end

-- Event called after the opening transition effect of the map,
-- that is, when the player takes control of the hero.
function map:on_opening_transition_finished()
  local x,y,l = alter_destination:get_position()
  alter_hero = map:create_hero{
    x = x,
    y = y,
    layer = l,
  }
  -- Giving an item to the second hero
  sol.timer.start(sol.main, 1000, function()
    game:get_controls():simulate_pressed("action")
  end)
  
  alter_hero:start_treasure("bomb", 1, "bombs", function()
    sol.timer.start(sol.main, 1000, function()
      item = alter_hero:get_item("bomb")
      alter_hero:start_item(item)
      -- Check that the bombs exploded once
      assert_equal(bombs_exploded, 1)
      sol.main.exit()
    end)
    
    
  end)
end
