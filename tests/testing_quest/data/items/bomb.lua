local item = ...

-- global bomb explode counter
bombs_exploded = 0

function item:on_created()
  item:set_savegame_variable("bombs")
  item:set_assignable()
  item:set_obtainable()
end

function item:on_using()
  item:set_finished()
  bombs_exploded = bombs_exploded+1
end
