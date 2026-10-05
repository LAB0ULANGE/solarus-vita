# Main Lua Script

`main.lua` is the entry point of your quest. Everything starts from here at runtime. Of course, you can make other script files, possibly organized in subdirectories, and call them from `main.lua` For instance, you will probably make a title screen and then start a game.

Here is an example of main script that does almost nothing:

```lua
-- This function is called when Solarus starts.
function sol.main.on_started()
  print("Welcome to my quest.")
end

-- This function is called when Solarus stops or is reset.
function sol.main.on_finished()
  print("See you!")
end
```

See the [Lua API](../lua-api/index.md) for more information about Lua scripting in Solarus.
