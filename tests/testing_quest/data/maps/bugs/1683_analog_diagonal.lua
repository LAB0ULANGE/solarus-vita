local map = ...


local hero = map:get_hero()
local controls = hero:get_controls()

sol.controls.set_analog_commands_enabled(true)

function map:on_opening_transition_finished()
  controls:simulate_axis_moved("X", 0.2)
  controls:simulate_axis_moved("Y", 0.8)
end

function trigger:on_activated()
  controls:simulate_axis_moved("X", 0.5)
  controls:simulate_axis_moved("Y", 0.0)
end

function trigger_2:on_activated()
  error("should not end up here")
end

function trigger_3:on_activated()
  sol.main.exit()
end