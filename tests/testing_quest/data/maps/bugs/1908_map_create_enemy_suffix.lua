local map = ...

function map:on_opening_transition_finished()

  local enemy_1 = map:get_entity("enemy_1")
  assert_equal(enemy_1:get_name(), "enemy_1")

  local enemy_2 = map:create_enemy({
    layer = 0,
    x = 200,
    y = 100,
    direction = 0,
    breed = "test_enemy",
    name = "enemy_1",  -- Name already in use: should become "enemy_2".
  })
  assert(enemy_2 ~= nil)
  assert_equal(enemy_1:get_name(), "enemy_1")
  assert_equal(enemy_2:get_name(), "enemy_2")

  local enemy = map:create_enemy({
    layer = 0,
    x = 0,
    y = 100,
    direction = 0,
    breed = "test_enemy",
    name = "enemy",  -- Name not in use yet.
  })
  assert(enemy ~= nil)
  assert_equal(enemy_1:get_name(), "enemy_1")
  assert_equal(enemy_2:get_name(), "enemy_2")
  assert_equal(enemy:get_name(), "enemy")

  local enemy_3 = map:create_enemy({
    layer = 0,
    x = 300,
    y = 100,
    direction = 0,
    breed = "test_enemy",
    name = "enemy",  -- Name already in use: should become "enemy_3".
  })
  assert(enemy_3 ~= nil)
  assert_equal(enemy_1:get_name(), "enemy_1")
  assert_equal(enemy_2:get_name(), "enemy_2")
  assert_equal(enemy_3:get_name(), "enemy_3")

  enemy_1:set_name("enemy_1")
  assert_equal(enemy_1:get_name(), "enemy_1")
  assert_equal(enemy_2:get_name(), "enemy_2")
  assert_equal(enemy_3:get_name(), "enemy_3")

  enemy_1:set_name(nil)
  assert_equal(enemy_1:get_name(), nil)
  assert_equal(enemy_2:get_name(), "enemy_2")
  assert_equal(enemy_3:get_name(), "enemy_3")

  enemy_1:set_name("enemy_1")
  assert_equal(enemy_1:get_name(), "enemy_1")
  assert_equal(enemy_2:get_name(), "enemy_2")
  assert_equal(enemy_3:get_name(), "enemy_3")

  enemy_3:set_name("enemy_1")
  assert_equal(enemy_1:get_name(), "enemy_1")
  assert_equal(enemy_2:get_name(), "enemy_2")
  assert_equal(enemy_3:get_name(), "enemy_3")

  enemy_3:set_name(nil)
  assert_equal(enemy_1:get_name(), "enemy_1")
  assert_equal(enemy_2:get_name(), "enemy_2")
  assert_equal(enemy_3:get_name(), nil)

  enemy_3:set_name("enemy_2")
  assert_equal(enemy_1:get_name(), "enemy_1")
  assert_equal(enemy_2:get_name(), "enemy_2")
  assert_equal(enemy_3:get_name(), "enemy_3")

  sol.main.exit()
end
