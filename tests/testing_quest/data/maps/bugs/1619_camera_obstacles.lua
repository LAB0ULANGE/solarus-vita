local map = ...

-- The camera is on layer 2.
-- It should be able to traverse the walls and the traversable custom entity that are on layer 2.
-- It should be blocked by the blocker custom entity on layer 2, and by the separator on layer 0.

function map:on_started()

  local camera = map:get_camera()
  assert_equal(camera:get_layer(), 2)
  camera_blocker:set_traversable_by("camera", false)
  camera_traversable:set_traversable_by("camera", true)
end

function map:on_opening_transition_finished()

  local camera = map:get_camera()
  assert_equal(camera:get_layer(), 2)
  assert_equal(camera_blocker:get_layer(), 2)
  assert_equal(camera_traversable:get_layer(), 2)

  assert_equal(camera:test_obstacles(), true)
  assert_equal(camera:test_obstacles(1, 0), true)

  camera_blocker:set_enabled(false)
  assert_equal(camera:test_obstacles(), false)
  assert_equal(camera:test_obstacles(1, 0), true)

  camera_blocker:set_enabled(true)
  assert_equal(camera:get_layer(), 2)
  assert_equal(camera_blocker:get_layer(), 2)
  separator:set_enabled(false)
  assert_equal(camera:test_obstacles(), true)
  assert_equal(camera:test_obstacles(1, 0), true)

  camera_blocker:set_enabled(false)
  assert_equal(camera:test_obstacles(), false)
  assert_equal(camera:test_obstacles(1, 0), false)

  sol.main.exit()
end
