local map = ...
local game = map:get_game()

function map:on_opening_transition_finished()

  local hero = map:get_hero()

  -- Test an item the player does not have.
  local bombs = game:get_item("bombs_assignable")
  assert_equal(bombs:is_assignable(), true)

  assert_equal(game:get_item_assigned(1), nil)
  assert_equal(hero:get_item_assigned(1), nil)

  assert_equal(bombs:get_variant(), 0)
  bombs:set_variant(1)
  assert_equal(bombs:get_variant(), 1)

  game:set_item_assigned(1, bombs)
  assert_equal(game:get_item_assigned(1), bombs)
  hero:set_item_assigned(1, bombs)
  assert_equal(hero:get_item_assigned(1), bombs)

  game:set_item_assigned(1, nil)
  assert_equal(game:get_item_assigned(1), nil)
  hero:set_item_assigned(1, nil)
  assert_equal(hero:get_item_assigned(1), nil)

  bombs:set_variant(0)
  assert_equal(bombs:get_variant(), 0)
  local success, message = pcall(function() game:set_item_assigned(1, bombs) end)
  assert_equal(success, false)
  assert(message:find("The player does not have the item 'bombs_assignable'"))
  success, message = pcall(function() hero:set_item_assigned(1, bombs) end)
  assert_equal(success, false)
  assert(message:find("The player does not have the item 'bombs_assignable'"))

  -- Test a non-assignable item.
  local sword = game:get_item("sword")
  assert_equal(sword:is_assignable(), false)
  success, message = pcall(function() game:set_item_assigned(2, sword) end)
  assert_equal(success, false)
  assert(message:find("Item 'sword' is not assignable"))
  success, message = pcall(function() hero:set_item_assigned(2, sword) end)
  assert_equal(success, false)
  assert(message:find("Item 'sword' is not assignable"))

  sol.main.exit()
end
