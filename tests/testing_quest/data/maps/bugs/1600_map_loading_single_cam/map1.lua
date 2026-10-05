-- Lua script of map first_map
local map = ...
local game = map:get_game()

local first_time = true

function map:on_opening_transition_finished()
	print("Map 1 Started")
	if not game:get_value("is_jumped") then
		game:set_value("is_jumped", true)
		hero:teleport("bugs/1600_map_loading_single_cam/map2")
    first_time = false
	else
    -- we returned here, was the map correctly reloaded after being leaved ?
    assert(first_time, "Map was not correctly unloaded when leaved")
    sol.main.exit()
  end
end
