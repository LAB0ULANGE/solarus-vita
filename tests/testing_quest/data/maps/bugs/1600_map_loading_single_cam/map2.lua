
-- Lua script of map second_map
local map = ...
local game = map:get_game()

function map:on_opening_transition_finished()
	print("Map 2 Started")
	hero:teleport("bugs/1600_map_loading_single_cam/map1")
end
