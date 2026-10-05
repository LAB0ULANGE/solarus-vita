-- Lua script of map bugs/1552_entire_savegame_access.

local map = ...

-- Only checks one level deep.
local function assert_equal_table(actual, expected)
  local same = {}
  local diff = {}
  local any_diff = false

  for key, act_value in pairs(actual) do
    local exp_value = expected[key]
    if act_value == exp_value then
      same[key] = true
    else
      diff[key] = true
      any_diff = true
    end
  end
  
  for key, exp_value in pairs(expected) do
    if not same[key] then
      diff[key] = true
      any_diff = true
    end
  end

  if any_diff then
    error("table mismatch")
  end
end

local function strip_reserved(save)
  local count = 0
  for key, _ in pairs(save) do
    if "_" == string.sub(key, 1, 1) then
      save[key] = nil
    end
  end
  return save, count
end

function map:on_started()
  local game = map:get_game()

  game:set_value("first", true)
  game:set_value("second", 2)
  game:set_value("third", "three")

  local values0 = game:get_values()
  assert_equal(values0['_version'], 2)  -- Check a built-in value.
  local save0, count0 = strip_reserved(values0)
  assert_equal_table(save0, {first = true, second = 2, third = "three"})

  game:set_value("second", nil)
  game:set_value("third", "trois")

  local values1 = game:get_values()
  assert_equal(values1['_version'], 2)
  local save1, count1 = strip_reserved(values1)
  assert_equal_table(save1, {first = true, third = "trois"})

  -- The magic values are likely to change, so just make sure they are stable:
  assert_equal(count0, count1, "Built-ins changed.")

  sol.main.exit()
end
