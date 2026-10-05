local map = ...

function map:on_started()

  assert_equal(destination_0:get_direction(), 0)
  assert_equal(destination_1:get_direction(), 1)
  assert_equal(destination_2:get_direction(), 2)
  assert_equal(destination_3:get_direction(), 3)
  assert_equal(destination_nil:get_direction(), nil)
  sol.main.exit()
end
