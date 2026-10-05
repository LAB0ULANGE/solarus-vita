local map = ...
local game = map:get_game()

function map:on_opening_transition_finished()

  assert_equal(game:get_ability("sword_spin_attack"), 1)  -- Default value.
  assert_equal(game:get_ability("sword_knowledge"), 0)

  game:set_ability("sword_spin_attack", 2)
  assert_equal(game:get_ability("sword_spin_attack"), 2)
  assert_equal(game:get_ability("sword_knowledge"), 1)
  
  game:set_ability("sword_spin_attack", 0)
  assert_equal(game:get_ability("sword_spin_attack"), 0)
  assert_equal(game:get_ability("sword_knowledge"), 0)

  game:set_ability("sword_knowledge", 1)
  assert_equal(game:get_ability("sword_spin_attack"), 2)
  assert_equal(game:get_ability("sword_knowledge"), 1)

  game:set_ability("sword_knowledge", 0)
  assert_equal(game:get_ability("sword_spin_attack"), 1)  -- For compatibility we reset spin attack to default.
  assert_equal(game:get_ability("sword_knowledge"), 0)
  
  game:set_ability("sword_spin_attack", 0)
  assert_equal(game:get_ability("sword_spin_attack"), 0)
  assert_equal(game:get_ability("sword_knowledge"), 0)

  game:set_ability("sword_knowledge", 0)
  assert_equal(game:get_ability("sword_spin_attack"), 1)
  assert_equal(game:get_ability("sword_knowledge"), 0)

  sol.main.exit()
end
