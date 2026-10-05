local map = ...

function map:on_started()

  assert(not sol.file.exists("dont_exist.dat"))
  sol.game.load("dont_exist.dat")  -- Not an error: just a new file.
  assert(sol.file.exists("sprites/blocks/block.png"))
  local success, message = pcall(function() sol.game.load("sprites/blocks/block.png") end)
  assert(not success)
  assert(message:match("Invalid savegame file: 'sprites/blocks/block.png'"))
  sol.main.exit()
end
