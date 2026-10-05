local map = ...

function map:on_opening_transition_finished()
  map:set_tileset("13")
  map:set_tileset(13)
  sol.main.exit()
end
