local map = ...
local game = map:get_game()
local hero = map:get_hero()

function map:on_opening_transition_finished()
  game:simulate_command_pressed("action")

  assert(
    hero:get_state() == "lifting",
    "The hero should be able to lift the destructible while overlapping with a custom entity"
  )
  sol.main.exit()
end
