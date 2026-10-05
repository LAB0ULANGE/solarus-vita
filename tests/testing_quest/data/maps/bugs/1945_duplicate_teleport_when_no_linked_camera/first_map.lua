local map = ...
local game = map:get_game()
local hero = map:get_hero()
local camera = map:get_camera()

local function simulate_player_movement()
  game:simulate_command_pressed("up")
end

local function start_cutscene()
  hero:freeze()

  local camera_movement = sol.movement.create("target")
  camera_movement:set_ignore_obstacles(true)
  camera_movement:set_target(camera:get_position_to_track(teleporter))
  camera_movement:set_speed(128)
  camera_movement:start(camera, function()
    local hero_movement = sol.movement.create("straight")
    hero_movement:set_angle(math.pi / 2)
    hero_movement:set_speed(hero:get_walking_speed() / 2)
    hero_movement:set_max_distance(16)
    hero_movement:start(hero, function()
      hero:unfreeze()
      simulate_player_movement()
    end)
  end)
end

function map:on_opening_transition_finished()
  start_cutscene()
end

local activation_count = 0

function teleporter:on_activated()
  activation_count = activation_count + 1
  assert(activation_count == 1, "More than one activation count detected!")
end
