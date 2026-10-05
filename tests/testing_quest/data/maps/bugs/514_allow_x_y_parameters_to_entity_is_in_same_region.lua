local map = ...

function map:on_started()
  local w, h = self:get_size()
  local test_cases = {
    {0, 0, false},
    {w, 0, false},
    {w, h, false},
    {0, h, true},
  }
  local hero = self:get_hero()
  for _i, case in ipairs(test_cases) do
    local x, y, expected = case[1], case[2], case[3]
    local actual = hero:is_in_same_region(x, y)
    assert_equal(actual, expected, "Incorrect is_in_same_region result.")
  end

  sol.main.exit()
end
