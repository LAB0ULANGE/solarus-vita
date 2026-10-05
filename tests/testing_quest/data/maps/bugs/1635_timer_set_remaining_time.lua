local map = ...

function map:on_opening_transition_finished()
  local first_time = true
  local timer
  timer = sol.timer.start(map, 100, function()
    assert_equal(timer:get_remaining_time(), 0)
    assert_equal(timer:get_duration(), first_time and 100 or 200)
    timer:set_duration(200)
    if not first_time then
      sol.main.exit()
    end
    first_time = false
    return true
  end)

  assert_equal(timer:get_remaining_time(), 100)
  assert_equal(timer:get_duration(), 100)

  sol.timer.start(map, 10, function()
    assert_equal(timer:get_remaining_time(), 90)
    assert_equal(timer:get_duration(), 100)

    timer:set_remaining_time(40)
    assert_equal(timer:get_remaining_time(), 40)
    assert_equal(timer:get_duration(), 100)
  end)
end
