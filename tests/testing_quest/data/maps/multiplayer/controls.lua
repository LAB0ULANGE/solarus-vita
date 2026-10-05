-- Lua script of map multiplayer/controls.
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

local function check_bindings(ref, other)
  for k,refv in pairs(ref) do
    assert(other[k])
    v = other[k]
    for i, refcmd in ipairs(refv) do
      assert_equal(v[i], refcmd)
    end
  end
end

-- Event called after the opening transition effect of the map,
-- that is, when the player takes control of the hero.
function map:on_opening_transition_finished()
  local controls = sol.controls.create_from_keyboard()
  local jpbindings = {
    a = {"1", "2", "3", "4"},
    b = {"5", "6", "7", "8"},
    ["left_x +"] = {"right"}
  }

  local kbbindings = {
    c = {"1", "2", "3", "4"},
    d = {"5", "6", "7", "8"}
  }
  controls:set_keyboard_bindings(kbbindings)
  controls:set_joypad_bindings(jpbindings)

  assert_equal(controls:get_joypad_binding("4"), "a")  

  local jpbindings2 = controls:get_joypad_bindings()
  local kbbindings2 = controls:get_keyboard_bindings()

  check_bindings(jpbindings, jpbindings2)
  check_bindings(kbbindings, kbbindings2)

  local axis_bindings = {
    left_x = {"X +", "x2 +", "smth -"}
  };
  
  controls:set_joypad_axis_bindings(axis_bindings)

  -- check that the singular methods get the first binding
  assert_equal(controls:get_joypad_axis_binding("x2"), "left_x +")

  -- should modify just first binding
  controls:set_joypad_axis_binding("X", "left_x -")

  -- reflect the change on our local copy
  axis_bindings.left_x[1] = "X -"  

  -- check we are on the same page
  local jabindings = controls:get_joypad_axis_bindings()
  check_bindings(axis_bindings, jabindings)

  -- Check that setting and getting nil joypad works
  controls:set_joypad(nil)
  assert_equal(controls:get_joypad(), nil)

  sol.main.exit()
end
