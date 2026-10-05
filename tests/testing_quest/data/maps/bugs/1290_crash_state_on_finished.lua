local map = ...
local game = map:get_game()
local hero = map:get_hero()

function map:on_opening_transition_finished()

  local state = sol.state.create()

  function state:on_finished()
    assert_error({function() hero:start_attack() end})  -- Illegal but should show a proper error rather than crash.
    sol.main.exit()
  end

  hero:start_state(state)
  sol.timer.start(map, 100, function()
    -- Go to "free" state.
    hero:unfreeze()
  end)
end
