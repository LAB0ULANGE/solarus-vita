-- Lua script of map bugs/1936_stairs_mask/second_floor.
local map = ...
local game = map:get_game()

-- Event called at initialization time, as soon as this map is loaded.
function map:on_started()
  game:simulate_command_released('up')
  hero:freeze()
  local x,y = target:get_position()
  hero:set_position(x,y)
end

function target:on_activated()
  sol.timer.start(500, function()
    hero:unfreeze()
    local bpp = 4
    local qw, qh = sol.video.get_quest_size()
    local surface = map:get_camera():get_surface()
    local bytes = surface:get_pixels()
    assert_equal(#bytes, qw*qh*bpp) -- make sure we have right px amount
    local x,y = target:get_position()
    local offset = (qw * y + x) * bpp
    -- checks if the hero overwrote the floor's carpet pixels that are (32,3,2,255)
    if string.byte(bytes, offset+1) == 32 then
      error("Hero is not visible")
    else
      sol.main.exit()
    end
  end)
end