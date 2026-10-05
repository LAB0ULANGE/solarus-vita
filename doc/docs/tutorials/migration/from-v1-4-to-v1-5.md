# Upgrade Your Quest from Solarus 1.4 to Solarus 1.5

Solarus 1.5 brings important new features: a quest launcher GUI, an execution console, a customizable [camera as entity](http://www.solarus-games.org/doc/1.5/lua_api_camera.html), maps with more than 3 layers, and more. It also improves global performance, fixes a lot of issues. Dozens of new functions were added to the Lua API.

As always, you should make a backup before any upgrade.

In this page, we only focus on the incompatibilities. See the [changelog](https://gitlab.com/solarus-games/solarus/blob/v1.5/ChangeLog) to know more about the new features.

## Upgrading Data Files

Data files other than scripts can be upgraded automatically with the editor. Open your quest with Solarus Editor 1.5 and a dialog will let you to perform the upgrade.

## Solarus Launcher GUI

A new user interface allows users to select a quest, to run it and to change some settings like video and audio settings.

In this new quest launcher, each quest can now have a logo and some icons that are displayed.

The logo and icons are optional, but it is recommended to make some for your quest to be better identified in the quest list of the launcher GUI.

### Quest Logo

The logo of your quest should be a PNG image of size `200*140` pixels called `logos/logo.png`. It is optional.

### Quest Icons

An icon is also useful for the Solarus GUI to distinguish your quest. Like the logo, the icon is optional. Multiple icon sizes are allowed, and each size should be in a separate PNG file. The following icon file names are allowed, with the corresponding sizes from `16*16` pixels to `1024*1024` pixels:

- `logos/icon_16.png`
- `logos/icon_24.png`
- `logos/icon_32.png`
- `logos/icon_48.png`
- `logos/icon_64.png`
- `logos/icon_128.png`
- `logos/icon_256.png`
- `logos/icon_512.png`
- `logos/icon_1024.png`

It is possible (and recommended) to provide an icon with multiple sizes. The Solarus GUI will then automatically pick the most suitable one(s) to fit its needs.

## Upgrading Lua Scripts

The Lua scripting API of Solarus 1.5 introduces some incompabilities in order to get rid of legacy design choices. Luckily, most of them are very simple or will not impact you. Yet, we give below the exhaustive list of potential problems you may encounter.

### More Customizable Chests

`chest:on_empty()` no longer exists. It is replaced by the more general [`chest:on_opened(item, variant, savegame_variable)`](http://www.solarus-games.org/doc/1.5/lua_api_chest.html#lua_api_chest_on_opened), which is called no matter if the chest is empty or has a treasure.

This is more natural: if you don't define the event, then the built-in behavior of the engine is executed (giving the treasure if any), and if you define it you can customize what happens.

In almost all cases, you can simply replace `function chest:on_empty()` by `function chest:on_opened()`.

### No More Automatic Jump When Drowning

When walking into deep water without the `swim` ability, the hero no longer automatically jumps over the water: by default, he simply drowns instead. Most users don't want the jump.

If you need the jump like before, there is a new ability `"jump_over_water"`. Just call [`game:set_ability("jump_over_water", 1)`](http://www.solarus-games.org/doc/1.5/lua_api_game.html#lua_api_game_set_ability) when starting the savegame and you will get the old behavior back.

### No More Enemy Ranks

The `rank` property of enemies no longer exist. It is automatically removed from your map data files when upgrading. Its only effect was to set the initial hurt style to `"boss"`.

Instead, just call [`enemy:set_hurt_style("boss")`](http://www.solarus-games.org/doc/1.5/lua_api_enemy.html#lua_api_enemy_set_hurt_style) from enemy scripts that are bosses or minibosses.

### Animated Treasures

Because of a bug, when brandishing a treasure, for example from a chest or by calling [`hero:start_treasure()`](http://www.solarus-games.org/doc/1.5/lua_api_hero.html#lua_api_hero_start_treasure), the treasure sprite was not animated (it remained stuck at its first frame). The same was true for shop treasures.

This problem is now fixed, but you may be surprised if a fixed image was actually what you wanted!

If like me, you want hearts to be animated when they are appear as pickable treasures on the map (to make them fall nicely), but fixed when they are brandished from a treasure chest, then you need two different heart animations in the `entities/items` sprite. You should rename the `heart` animation to `heart_falling`, and make a fresh `heart` animation with only one frame. The `heart` item script should then explicitly set the `heart_falling` animation to pickable hearts:

```lua
function item:on_pickable_created(pickable)
  -- …
  pickable:get_sprite():set_animation("heart_falling")
end
```

### Better Position Detection

Collision checks were greatly improved. Collisions with non-moving entities were not always detected. Similarly, notifications of position or ground changes were not always sent, in particular at map initialization time.

It means that events like `entity:on_position_changed()`, `movement:on_position_changed()`, `custom_entity:on_ground_below_changed()` and custom entity collision callbacks are now called in situations where they were missed before due to bugs.

You might want to check that your enemies and custom entities still work as expected.

### Deprecated Functions

The following functions are now deprecated. You can still use them in Solarus 1.5, but they will show a warning. They might be removed in a future version.

- `map:draw_sprite()` is now deprecated, use [`map:draw_visual()`](http://www.solarus-games.org/doc/1.5/lua_api_map.html#lua_api_map_draw_visual) instead. This new function also works with surfaces and text surfaces.
- `map:get_camera_position()` is now deprecated, use [`map:get_camera():get_bounding_box()`](http://www.solarus-games.org/doc/1.5/lua_api_entity.html#lua_api_entity_get_bounding_box) instead. Because the [camera is now an entity](http://www.solarus-games.org/doc/1.5/lua_api_camera.html), you now have the full power of the entity API.
- `map:move_camera()`and`map:on_camera_back()`are now deprecated for the same reason. Use a movement on the camera entity instead, and you can fully customize this movement like you do for any other entity. If you need to reproduce the exact behavior of`map:move_camera()` for compatibility, here is [an implementation in pure Lua](http://www.solarus-games.org/doc/1.5/lua_api_map.html#lua_api_map_move_camera).

### Minor Incompatibilities

The remaining incompatibilities in the Solarus API are less disruptive. They don't have any consequence in most games, and if they do, they should be easy to address.

- In [`hero:get_state()`](http://www.solarus-games.org/doc/1.5/lua_api_hero.html#lua_api_hero_get_state), the possible result `"freezed"` is replaced by `"frozen"`.
- Items with amount now have a default [maximum amount](http://www.solarus-games.org/doc/1.5/lua_api_item.html#lua_api_item_set_max_amount) of `1000` instead of `0`, which was error-prone.
- Fix [`map:get_entities(prefix)`](http://www.solarus-games.org/doc/1.5/lua_api_map.html#lua_api_map_get_entities) never returning the hero.
- In [`map:create_custom_entity()`](http://www.solarus-games.org/doc/1.5/lua_api_map.html#lua_api_map_create_custom_entity), `width` and `height` are now mandatory. It was an error that they were not (their default value was `16`).
- Fix [`entity:set_enabled(true)`](http://www.solarus-games.org/doc/1.5/lua_api_entity.html#lua_api_entity_set_enabled) being delayed while it blocks the hero.
- Fix [`entity:set_enabled(true)`](http://www.solarus-games.org/doc/1.5/lua_api_entity.html#lua_api_entity_set_enabled) being delayed while it blocks the hero.
- [`circle_movement:get/set_initial_angle()`](http://www.solarus-games.org/doc/1.5/lua_api_circle_movement.html#lua_api_circle_movement_set_initial_angle) now use degrees. The previous implementation did not work anyway.
- The mouse cursor is now visible even in fullscreen. You can now show or hide the cursor with [`sol.video.set_cursor_visible()`](http://www.solarus-games.org/doc/1.5/lua_api_video.html#lua_api_video_set_cursor_visible).
