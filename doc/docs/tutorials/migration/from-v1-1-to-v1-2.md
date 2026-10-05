# Upgrade Your Quest from Solarus 1.1 to Solarus 1.2

Solarus 1.2 brings a lot of new features ([custom entities](http://www.solarus-games.org/doc/1.2/lua_api_custom_entity.html), SDL2, [metatables](http://www.solarus-games.org/doc/1.2/lua_api_main.html) and more), a lot of new functions in the Lua API, and improves the format of some data files (most notably strings.dat). Some entities are more customizable, like destructibles and streams (formely known as conveyor belts).

As always, you should make a backup before any upgrade.

In this page, we only focus on the incompatibilities. See the [changelog](https://gitlab.com/solarus-games/solarus/blob/v1.2/ChangeLog) to know more about the new features.

## Upgrading Data Files

Data files other than scripts can be upgraded automatically with the editor. Open your quest with Solarus Editor 1.2 and a dialog will let you to perform the upgrade.

Note that the operation can also be done from the command line, by running the script `update_quest.lua` in the `tools` directory of the git repository. Internally, the editor actually calls this script.

The main changes in the data files are:

- The language-specific strings file `strings.dat` has a new syntax easier to read and parse.
- Sprite data files are more carefully checked. You will have an error if a frame is outside its source image. This was not detected before.
- There are minor changes in the syntax of map data files. Indeed, some type of entities were renamed (`conveyor_belt` becomes `stream`, `shop_item` becomes `shop_treasure`), added (`custom_entity`) or have new properties.

Due to an SDL2 bug, the font `fixed8.fon` that we used in _Zelda Mystery of Solarus DX_ no longer works. If you also use this font, you will have an error message `"Cannot load font from file 'text/fixed8.fon': Couldn't set font size"`. Also, the license of this font was unclear. For these two reasons, we changed it for another one (`minecraftia.ttf`, in size `8`).

## Engine Behavior Changes

Some improvements slightly change the specific behavior of some entities in some situations. If you have puzzles that rely on the old behavior, make sure to update them.

- Enemies no longer stay stuck on blocks or crystal blocks.
- The shield no longer protects the hero while using the sword or carrying.
- Thrown entities (pots, bombs…) now fall to the lower layer if the hero throw them from above.
- Running into a crystal or a solid switch now activates it.
- If the player has the ability to jump, then he can now jump between distant crystal blocks.
- The collision rules of streams (the new name of conveyor belts) have changed. If there is a space, say of 8 pixels, between a stream and the walls of the room, the hero can now walk on this space without activating the streams. This behavior is coherent with all special grounds like water, holes, prickles, etc.
- Diagonal jumper collisions have been fixed. Jumpers oriented to North-East or South-West were shifted of 8 pixels in the engine and also in the editor. Also, they did not block the hero enough, and some configurations of ### $1
  jumpers were traversable when they should not. You might want to check the position of diagonal jumpers in your existing maps.

## Upgrading Lua Scripts

The Lua scripting API of Solarus 1.2 introduces a lot of slight incompabilities. Luckily, most of them are very simple or will not impact you. Yet, we give below the exhaustive list of potential problems you may encounter.

### Video API

With the port to SDL2, the video API is more simple and more powerful. Switching to fullscreen is faster and no longer changes the resolution of your screen. You can now get and set independently the size of the window, the scaling algorithm and the fullscreen option. "Wide" video modes no longer exist because we don't need them anymore: SDL2 now correctly fits the quest image in the screen without deformation.

These improvements change the API of `sol.video.get_video_mode()` and `sol.video.set_video_mode()`. Possible video mode names are now: `normal`, `scale2x`, `hq2x`, `hq3x` and `hq4x`. Similarly, `sol.video.switch_mode()` no longer changes the fullscreen flag. Call `sol.video.set_fullscreen()` to set to fullscreen or windowed.

### Surfaces

`surface:set_transparency_color()` no longer exists. Surfaces are all in a 32-bit format with an alpha channel and there is no special pixel value. Use the new method `surface:clear()` to erase the content of a surface (this will make it fully transparent). Use the alpha channel of colors to make transparent or semi-transparent pixels.

This is the only incompatible change with surfaces. With SDL2, if video acceleration is available, drawing on surfaces is faster because some blits that were done in memory before are now performed by the GPU. Your FPS should be improved.

Video acceleration can be disabled at runtime with the command-line option `-video-acceleration=no`.

### Destructible Entities

Destructible objects (pots, bushes…) are now fully customizable. There are no longer built-in subtypes of destructibles. In [`map:create_destructible()`](http://www.solarus-games.org/doc/1.2/lua_api_map.html#lua_api_map_create_destructible), each property is now set separetely (the sprite, the sound, the weight, etc).

Also, the engine no longer shows a built-in dialog when trying to lift something that should be cut or something too heavy. There is a new event [`destructible:on_looked()`](http://www.solarus-games.org/doc/1.2/lua_api_destructible.html#lua_api_destructible_on_looked) that you can define to do whatever you want when this happens. To show dialogs like in Solarus 1.1, you can use the following code:

```lua
-- Initializes the behavior of destructible entities.
local function initialize_destructibles()

  local destructible_meta = sol.main.get_metatable("destructible")
  -- destructible_meta represents the shared behavior of all destructible objects.

  -- Show a dialog when the player cannot lift them.
  function destructible_meta:on_looked()

    local game = self:get_game()
    if self:get_can_be_cut()
        and not self:get_can_explode()
        and not game:has_ability("sword") then
      -- The destructible can be cut, but the player has no cut ability.
      game:start_dialog("_cannot_lift_should_cut");
    elseif not game:has_ability("lift") then
      -- No lift ability at all.
      game:start_dialog("_cannot_lift_too_heavy");
    else
      -- Not enough lift ability.
      game:start_dialog("_cannot_lift_still_too_heavy");
    end
  end
end
```

Call this function only once in your quest, even before starting a game. This will define a default `on_looked()` event for all future destructible objects, thanks to the new (and very powerful) mechanism of [metatables](http://www.solarus-games.org/doc/1.2/lua_api_main.html#lua_api_main_get_metatable).

### Enemies

There should not be special differences between bosses and other enemies. In 1.1 and before, bosses were automatically disabled when starting the map. This is no longer the case. Therefore, to keep the same behavior you have to call `enemy:set_enabled(false)` from the `enemy:on_created()` event of your boss if you were not doing it before. You can also do it in the `map:on_started()` event of the map, but remember that `enemy:on_created()` and `enemy:on_restarted()` are already called at this point.

It is recommended to check all bosses of your game anyway.

### Minor Incompatibilities

The remaining incompatibilities in the Solarus API are less disruptive. They should be easy to address or even have no consequence at all.

- In [`map:create_teletransporter()`](http://www.solarus-games.org/doc/1.2/lua_api_map.html#lua_api_map_create_teletransporter), the transition is now a string.
- `map:create_shop_item()` was renamed to [`map:create_shop_treasure()`](http://www.solarus-games.org/doc/1.2/lua_api_map.html#lua_api_map_create_teletransporter).
- `map:create_conveyor_belt()` was replaced by [`map:create_stream()`](http://www.solarus-games.org/doc/1.2/lua_api_map.html#lua_api_map_create_stream).
- In [`hero:get_state()`](http://www.solarus-games.org/doc/1.2/lua_api_hero.html#lua_api_hero_get_state), the state "conveyor belt" no longer exists.
- The default strength of swords of level 3 or more has changed. Define [`enemy:on_hurt_by_sword`](http://www.solarus-games.org/doc/1.2/lua_api_enemy.html#lua_api_enemy_on_hurt_by_sword) if you have more than 3 swords in your quest and the default strength is not okay for you.
- [`enemy:on_hurt()`](http://www.solarus-games.org/doc/1.2/lua_api_enemy.html#lua_api_enemy_on_hurt) is now called before [`enemy:on_dying()`](http://www.solarus-games.org/doc/1.2/lua_api_enemy.html#lua_api_enemy_on_dying).
- [`enemy:on_hurt()`](http://www.solarus-games.org/doc/1.2/lua_api_enemy.html#lua_api_enemy_on_hurt) no longer takes a `life_lost` parameter.
- The built-in defense of the tunic has changed. Define [`hero:on_taking_damage`](<http://www.solarus-games.org/doc/1.2/lua_api_hero.html#lua_api_hero_on_taking_damage()>) if the default defense is not okay for you.
- `enemy:get/set_magic_damage()` no longer exists. Use [`enemy:on_attacking_hero()`](http://www.solarus-games.org/doc/1.2/lua_api_enemy.html#lua_api_enemy_on_attacking_hero) to make customized damage.
- [`hero:start_hurt()`](http://www.solarus-games.org/doc/1.2/lua_api_hero.html#lua_api_hero_start_hurt) no longer takes a magic parameter. Simply call [`game:remove_magic()`](http://www.solarus-games.org/doc/1.2/lua_api_game.html#lua_api_game_remove_magic) if you want to do that.
- [`hero:start_hurt()`](http://www.solarus-games.org/doc/1.2/lua_api_hero.html#lua_api_hero_start_hurt) now hurts the hero even if he is temporarily invincible or when he is in a state where he cannot be hurt by enemies.
- Enemies have now a default size of `16*16` and origin of `x=8, y=13`.
- The size of enemies must now be a multiple of `8`.
- `item:on_pickable_movement_changed()` no longer exists. Use [`pickable:on_movement_changed()`](http://www.solarus-games.org/doc/1.2/lua_api_entity.html#lua_api_entity_on_movement_changed) instead.
- [`pickable:get_treasure()`](http://www.solarus-games.org/doc/1.2/lua_api_pickable.html#lua_api_pickable_get_treasure) now returns the item instead of the item's name.
- Timers: returning true in the callback now repeats the timer.
- [`sol.timer.start()`](http://www.solarus-games.org/doc/1.2/lua_api_timer.html#lua_api_timer_start) now always returns the timer, even if its duration is `0`.
- `sol.audio.play_music("none")` is replaced by `sol.audio.play_music(nil)` for sanity.
- [`on_key_pressed()`](http://www.solarus-games.org/doc/1.2/lua_api_main.html#lua_api_main_on_key_pressed) and [`on_character_pressed()`](http://www.solarus-games.org/doc/1.2/lua_api_main.html#lua_api_main_on_character_pressed) are now independent events. If you stop the propagation of `on_key_pressed()`, `on_character_pressed()` is still called. This was not the case before.
