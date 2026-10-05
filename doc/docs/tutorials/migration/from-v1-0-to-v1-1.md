# Upgrade Your Quest from Solarus 1.0 to Solarus 1.1

Solarus 1.1 brings new features (separators, ice ground and more), a lot of new functions in the Lua API (changing the speed of the hero, new sprite callbacks…), and improves the format of some data files (sprites and project_db.dat). It also removes some built-in content: most importantly, the dialog box and the game-over menu. The dialog box and the game-over menu are now scripted in Lua.

In this page, we only focus on the incompatibilities. See the [changelog](https://gitlab.com/solarus-games/solarus/blob/v1.1/ChangeLog) to know more about the new features.

## Upgrading Data Files

Data files other than scripts can be upgraded automatically with the editor. Open your quest with Solarus Editor 1.1 and a dialog will let you to perform the upgrade.

Note that the operation can also be done from the command line, by running the script `update_quest.lua` in the `tools` directory of the git repository. Internally, the editor actually calls this script.

The main changes in the data files are:

- the [sprites](http://www.solarus-games.org/doc/1.1/quest_sprite_data_file.html|sprites)
- the [quest resource list file](http://www.solarus-games.org/doc/1.1/quest_resource_file.html) (`project_db.dat`) whose format has changed to a more readable syntax.
- the language list file (`languages.dat`) was removed because it was redundant. Some other files contain new optional values, but they do not require any change.

## Upgrading Lua Scripts

Upgrading your Lua code to Solarus 1.1 is not straightforward because some built-in content (the dialog box, the game-over menu, the dark rooms) must be reimplemented in Lua. So you will have some work to do (sorry). Luckily, this content has been reimplemented in Lua for _Zelda Mystery of Solarus DX_, so you can reuse the work.

### Dialogs

The dialog box system is now scripted in Lua, therefore, it is completely customizable. If you do nothing, a default and minimal built-in dialog box is shown.

See the Solarus Lua API reference fore more details about the [new dialog box system](http://www.solarus-games.org/doc/1.1/lua_api_game.html#lua_api_game_is_dialog_enabled).

You can reuse and adapt the [Lua dialog box of _Zelda Mystery of Solarus DX_](https://github.com/christopho/zsdx/blob/f55928a00ea77d2244a28ac81a247eb1688a19ca/data/menus/dialog_box.lua), which is a Lua version of the old built-in dialog box.

To do so, call this script with the game as parameter: `sol.main.load_file("path/to/dialog_box.lua")(game)`. Then, call `game:initialize_dialog_box()` when starting the game and `game:quit_dialog_box()` when the game stops.

**Warning:**
The dialog box script of _Zelda Mystery of Solarus DX_ contains several calls to `game:set_custom_command_effect()`, a function that only exists in the head-up display (HUD) of this game. This function is defined in `hud/hud.lua` and is used to make the action icon and the attack icon synchronized with the dialog. If your quest uses the same HUD as _Zelda Mystery of Solarus DX_, you will be okay. Otherwise, you can just comment out these calls to avoid an error.

The Lua dialog API was changed to be adapted to the customized dialog box.

- `map:is_dialog_enabled()` must be replaced by `game:is_dialog_enabled()`.
- `map:start_dialog()` must be replaced by `game:start_dialog()`.
- `map:draw_dialog_box()` no longer exists.
- `map:set_dialog_style()` and `map:set_dialog_position()` no longer exist. It is up to you to implement similar features if you need them. The `dialog_box.lua` script mentioned aboveimplements them to mimic the old built-in dialog box - see the comments inside.
- `map:set_dialog_variable()` was removed. Instead, use the second (optional) parameter of `game:start_dialog()` to pass any value to the dialog box.

### Game-Over Menu

The engine has no built-in game-over menu anymore. You can create one in Lua if you want. If you do nothing, the engine just restarts the game when the life of the player reaches zero.

See the Solarus Lua API reference fore more details about the [new game-over system](http://www.solarus-games.org/doc/1.1/lua_api_game.html#lua_api_game_is_game_over_enabled).

You can reuse and adapt the [Lua game-over menu of _Zelda Mystery of Solarus DX_](https://github.com/christopho/zsdx/tree/7ed4e26bc6ef6ee5df0c5577ea0eb1813ac11850/data/menus/game_over.lua), which is a Lua equivalent of the old built-in game-over menu.
To do so, call this script with the game as parameter:
`sol.main.load_file("path/to/game_over.lua")(game)`.

The only incompatibility is that the built-in ability `get_back_from_death` no longer exists when you call `game:get_ability()`, `game:has_ability()` and `game:set_ability()`. It was used to make a fairy save the hero during the game-over menu, but now you can do this directly in Lua.

The [bottle script of _Zelda Mystery of Solarus DX_](https://github.com/christopho/zsdx/blob/7ed4e26bc6ef6ee5df0c5577ea0eb1813ac11850/data/items/bottle.lua) was greatly simplified since this `get_back_from_death` ability was removed. If you use the same bottle script, you might be interested.

### Dark Rooms

The functions `map:get_light()` and `map:set_light()` allowed you to make a dark room with Solarus 1.0. This built-in feature was very specific and very easy to do in pure Lua, so we removed it.

You can reuse and adapt the [Lua dark room system of _Zelda Mystery of Solarus DX_](https://github.com/christopho/zsdx/tree/7ed4e26bc6ef6ee5df0c5577ea0eb1813ac11850/data/maps/lib/light_manager.lua), which is a Lua version of the old built-in dark rooms.
See the comments at the beginning of the script to know how to use it from your maps that want to be dark.

If you never used `map:get_light()` or `map:set_light()`, you don't need this script.

### Other Changes

- `map:get_entities(prefix)` now returns an iterator instead of an array.

This function allows to get all map entities whose name starts with the specified prefix. In all your scripts, whenever you made:

```lua
for i, entity in ipairs(map:get_entities("some_prefix")) do
  -- some code
end
```

You now do:

```lua
for entity in map:get_entities("some_prefix") do
  -- some code
end
```

Which is smarter. Unfortunately, the old syntax is not compatible with the new one, so you have to change it manually.

- `game:set_pause_enabled()` was replaced by `game:set_pause_allowed()` for consistency with other uses of the word "enabled" in the game API.

- `enemy:create_enemy()` now takes [a table as parameter](http://www.solarus-games.org/doc/1.1/lua_api_enemy.html#lua_api_enemy_create_enemy), like `map:create_enemy()`. You need to update your calls to this method.

- `sol.language.get_default_language()` was removed. It was useless and misleading.

- `sol.main.is_debug_enabled()` was removed. You decide yourself whether you want to do some debugging stuff. The `SOLARUS_DEBUG_KEYS` preprocessor constant in the engine was removed too.

- If you make an empty chest, the engine does nothing anymore by default. Before, it showed a dialog `_empty_chest` and played a sound `wrong`. Define the event `your_chest:on_empty()` to do this yourself if you want.
