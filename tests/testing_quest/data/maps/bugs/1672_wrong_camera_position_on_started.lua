local map = ...

local sx, sy

function map:on_started()
  sx, sy = self:get_camera():get_position()
end

function map:on_opening_transition_finished()
  local x, y = self:get_camera():get_position()
  assert(x == sx and y == sy, "Camera position after the opening transition should be the same as position on start")
  sol.main.exit()
end
