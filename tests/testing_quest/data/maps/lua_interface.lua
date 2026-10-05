-- Lua script of map lua_interface.
-- General tests for the conversions between Lua and the engine.

local map = ...
local game = map:get_game()

function map:on_started()

  -- Does the "middle type" check work.
  local x, y = 128, 128

  local sprite = sol.sprite.create('blocks/block_brown')
  self:draw_visual(sprite, x, y)

  local hero = self:get_hero()
  assert_error{self.draw_visual, self, hero, x, y}

  -- Double checking type checks on some related types.
  local straight = sol.movement.create'straight'
  local random = sol.movement.create'random'
  straight:get_speed()
  random:get_speed()

  sol.main.exit()
end
