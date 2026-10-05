-- Lua script of map bugs/1556_boolean_parameters_treat_nil_as_true.

local map = ...

function map:on_started()
  local entity = destination

  entity:set_enabled(false)
  assert_equal(entity:is_enabled(), false)

  assert_error{function() return entity:set_enabled(nil) end}
  assert_equal(entity:is_enabled(), false)

  entity:set_enabled()
  assert_equal(entity:is_enabled(), true)

  sol.main.exit()
end
