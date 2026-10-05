local map = ...
local game = map:get_game()

function map:on_opening_transition_finished()

  -- Set invincible with no limit.
  hero:set_invincible(true)
  assert(hero:is_invincible())

  sol.timer.start(10, function()
    assert(hero:is_invincible())

    -- Stop invincibility.
    hero:set_invincible(false)
    assert(not hero:is_invincible())

    -- Set invincible again, this time with a limit.
    hero:set_invincible(true, 10)
    assert(hero:is_invincible())

    sol.timer.start(sol.main, 20, function()
      assert(not hero:is_invincible())
      hero:set_invincible(true,100)
      game:set_suspended(true)
      sol.timer.start(sol.main, 200, function()
        assert(hero:is_invincible())
        game:set_suspended(false)
      end)
      sol.timer.start(sol.main, 250, function()
        assert(hero:is_invincible())
      end)
      sol.timer.start(sol.main, 310, function()
        assert(not hero:is_invincible())
      	sol.main.exit()
      end) 
    end)
  end)
end
