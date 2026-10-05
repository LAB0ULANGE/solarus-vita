-- Lua script of map straight_movement.
-- This script is executed every time the hero enters this map.

-- Feel free to modify the code below.
-- You can add more events and remove the ones you don't need.

-- See the Solarus Lua API documentation:
-- https://www.solarus-games.org/doc/latest

local map = ...
local game = map:get_game()

-- Event called at initialization time, as soon as this map is loaded.
function map:on_started()

  -- You can initialize the movement and sprites of various
  -- map entities here.
end

local hspeed = map:get_hero():get_walking_speed()
local on_return = false
-- Event called after the opening transition effect of the map,
-- that is, when the player takes control of the hero.
function map:on_opening_transition_finished()
  game:simulate_command_pressed("right")

  print(hspeed)
  local mov = sol.movement.create("straight")
  mov:set_speed(hspeed)
  mov:start(dummy)

  phase = 0.0

  local switch = true

  local function set_mov(speed, angle)
    if switch then
      mov:set_angle(angle)
      mov:set_speed(speed)
    else
      mov:set_speed(speed)
      mov:set_angle(angle)
    end
    switch = not switch
  end

  sol.timer.start(10, function()
    if not on_return then
      phase = phase + 0.1
      -- immitate gamepad jitter
      for i=1,100 do 
        angle = math.sin(phase)+math.random()*0.1
        speed = hspeed / math.cos(angle)
        set_mov(speed, angle)
      end
    end
        
    return not on_return
  end)
  
  local mov2 = sol.movement.create("straight")
  mov2:set_speed(1)
  mov2:start(dummy2)
  sol.timer.start(800, function() 
    local second_speed = 128
    local fx, fy = dummy2:get_position()
    mov2:set_speed(128)
    local x,y = dummy2:get_position()
    assert_equal(fx, x)
    sol.timer.start(1000, function()
      local expected_x = x + 1*second_speed
      local new_x = dummy2:get_position()
      assert_equal(new_x, expected_x)
    end)

  end)
end

function hero_arrived_sensor:on_activated()
  game:simulate_command_released("right")
  local m = dummy:get_movement()
  dummy:stop_movement()
  diff = map:get_hero():get_position() - dummy:get_position()
  
  print("diff", diff)
  assert(diff < 5)
  on_return = true
  
  sol.timer.start(dummy, 2000, function()
    m:set_angle(math.pi)
    m:set_speed(hspeed)
    m:start(dummy)
    game:simulate_command_pressed("left")
  end)
end

function hero_arrived_sensor_2:on_activated()
  diff = map:get_hero():get_position() - dummy:get_position()
  
  print("diff", diff)
  assert(diff < 5)
  sol.main.exit()
end
