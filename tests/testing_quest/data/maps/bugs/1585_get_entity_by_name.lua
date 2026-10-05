-- Lua script of map bugs/1585_get_entity_by_name.
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
  local name = "test_enemy_guy"

  --Create an enemy and confirm it uses the name:
  local test_enemy = map:create_enemy{
    x = 200, y = 100, layer = 0, direction = 0, breed = "test_enemy",
    name = name,
  }
  assert(map:get_entity(name):get_name() == name, "We somehow did not create the enemy with the proper name, bad test logic")

  --Remove all enemies from the map, then create it back, and check that we properly created it with the same name:
  for enemy in map:get_entities_by_type("enemy") do
    enemy:remove()
  end
  local enemy = map:create_enemy{
    x = 200, y = 100, layer = 0, direction = 0, breed = "test_enemy",
    name = name,
  }
  local found_name
  for enemy in map:get_entities_by_type("enemy") do
    found_name = enemy:get_name()
  end
  assert(found_name == name, "We did not recreate an enemy with the same name we started with, bad test logic")
  assert(map:get_entity(found_name), "Cannot find the entity we created, bad test logic")

  --Wait a sec, then try to find the enemy that we just created:
  sol.timer.start(map, 200, function()
    assert(map:get_entity(name), "Cannot find the enemy we just created, this is the bug I'm talking about")

    sol.main.exit()
  end)

end
