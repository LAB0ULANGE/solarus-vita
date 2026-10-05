local map = ...

function map:on_started()

  local m = sol.movement.create("random")
  m:get_angle()
  sol.main.exit()
end
