-- Lua script of map bugs/1551_error_teletransporter_set_destination_map.

local map = ...
local game = map:get_game()

local bad_map_id = "does_not_exist/xyz"
local tele = the_teletransporter

function map:on_started()
  tele:set_destination_map"bugs/1551_error_teletransporter_set_destination_map"

  assert_error{
    tele.set_destination_map, tele, "does_not_exist/xyz",
    label = "Bad set_destination_map value.",
    msg = "bad argument #2 to ? (No such map: 'does_not_exist/xyz')",
  }

  sol.main.exit()
end
