-- Lua script of map bugs/1494_no_error_nonexistant_model.

local map = ...

-- Event called at initialization time, as soon as this map is loaded.
function map:on_started()
  assert_error{map.create_custom_entity, map, {
      name = 'test',
      direction = 1,
      layer = 0,
      x = 16, y = 16,
      height = 16, width = 16,
      model = 'this-should-not-exist',
    },
    label="Custom entity with nonexistant model.",
    msg="bad argument #2 to ? (No such custom entity model: 'this-should-not-exist')",
  }

  sol.main.exit()
end
