# Upgrade Your Quest from Solarus 1.6 to Solarus 2.0

The multiplayer update! Solarus now supports multiple maps, cameras, heroes and game controllers.
We also provide brand new APIs for [controllers](../../lua-api/controls/index.md) with much better joypad support, [music](../../lua-api/audio/music.md) and [sound](../../lua-api/audio/sound.md) with a lot of customization.

As always, you should make a backup before any upgrade.

In this page, we only focus on the incompatibilities. See the [changelog](https://gitlab.com/solarus-games/solarus/-/blob/master/changelog.md) to know more about the new features.

!!! note "Compatibility layer with 1.5/1.6"
    Good news for players!
    Quests in format 1.5 and 1.6 can still be played by Solarus 2.0 with no modification. Indeed, Solarus 2.0 will detect their old version in `quest.dat` and will keep the old and fully compatible API for them. It is only when you upgrade your quest to 2.0 that your quest gets the new API, and that you need to take care of instructions on this page.

## Upgrading Data Files

Data files other than scripts can be upgraded automatically with Solarus Editor. Open your quest with Solarus Editor 2.0 and a dialog will guide you to upgrade data files.

## Upgrading Lua Scripts

The Lua scripting API of Solarus 2.0 introduces a small number of incompabilities in order to enable better joypad support.
We give below the list of potential incompatilibies you may have to take care of when upgrading your quest to the Solarus 2.0 format.

### Joypad input event button names

Joypad input events now provide button names instead of numbers.

In more details,
`on_joypad_button_pressed(button_number)`,
`on_joypad_button_released(button_number)` and
`on_joypad_axis_moved(axis_number)`
are replaced by
[`on_joypad_button_pressed(button_name, joypad)`](../../lua-api/controls/inputs.md#handleron_joypad_button_pressedbutton-joypad),
[`on_joypad_button_released(button_name, joypad)`](../../lua-api/controls/inputs.md#handleron_joypad_button_releasedbutton-joypad),
[`on_joypad_axis_moved(axis_name, joypad)`](../../lua-api/controls/inputs.md#handleron_joypad_axis_movedaxis-state-joypad).

These events exist on various Solarus objects: [sol.main](../../lua-api/general-features.md), the current [game](../../lua-api/game.md), [maps](../../lua-api/map.md), hero [custom states](../../lua-api/custom-states.md) and [menus](../../lua-api/menus.md). Instead of being called with a number that does not help much, they will now be called with a meaningful name.

For compatibility, as explained in the "compatibility layer" note above, quests with format 1.5 or 1.6 in their `quest.dat` file will still receive a number as parameter in these events, even when you run them with Solarus 2.0.

### No more automatic saving of commands

Commands are no longer saved by default, because there can be now multiple joypads, multiple inputs bound to the same commands, custom commands and even possibly multiple heroes. So we think is better to let quests handle saving controls their own way, as there are now too many possible use cases.
However, you can use [`game:set_legacy_controls_storage(true)`](../../lua-api/game.md#gameset_legacy_controls_storagelegacy_controls_storage) to continue to automatically save commands the pre Solarus 2.0 way.

Quests with format 1.5 or 1.6, and quests that call [`game:set_legacy_controls_storage(true)`](../../lua-api/game.md#gameset_legacy_controls_storagelegacy_controls_storage) will still automatically load and save game commands for the main hero.
This function is provided for quests that were designed before Solarus 2.0, and for quests that accept the limitations of this automatic commands storage.

### Camera and hero teletransportation

Teleportation is now driven by cameras, as there might be multiple heroes and multiple cameras.

- [`hero:teleport()`](../../lua-api/map-entities/hero.md#heroteleportmap_id-destination_name-transition_style) now also teleports the linked camera instead of recreating one.
  If no camera is tracking the hero when you call `hero:teleport()` (which can happen during cutscenes), then:
  - If the destination map is already loaded (meaning you are using the new 2.0 multiplayer features), the camera stays on the previous map.
  - If the destination map is not loaded yet, you will get a warning but the camera will automatically be re-linked to the hero for compatibility.

### Drawing onto the camera surface

Due to new camera features possibly involving complex transformations like scaling and rotation, drawing entities onto the camera surface now expects map coordinates instead of screen coordinates. For instance, with [`entity:set_draw_override()`](../../lua-api/map-entities/index.md#entityset_draw_overridedraw_override) or [`entity:on_pre_draw()`](../../lua-api/map-entities/index.md#entityon_pre_drawcamera), quests with format 2.0 should pass map coordinates rather than screen coordinates.
Yes, it is a breaking change, but the good news is that it simplifies your code, since you no longer have to subtract camera coordinates: you will be able to do
```
camera:get_surface():draw(your_sprite, entity_x, entity_y)
```
instead of
```
camera:get_surface():draw(your_sprite, entity_x - camera_x, entity_y - camera_y)
```

During `map:on_draw()` however, transformations do not apply and the camera does still expect screen coordinates when you draw onto its surface.

That is why generally, it is recommended to use [`map:draw_visual`](../../lua-api/map.md#mapdraw_visualdrawable-x-y) which is less tricky and always takes map coordinates.

### Loading a surface from a file

`sol.surface.create(file_name, [language_specific])` is now deprecated. It was meant to load an image file from either the `sprites` directory or from the `images` directory of the current language.
It it now recommended to use the new function [`sol.surface.load(file_name)`](../../lua-api/drawable-objects/surface.md#solsurfaceloadfile_name), that simply loads a file name relative to the quest data directory.

### Spin attack ability

The `sword_knowledge` [ability](../../lua-api/game.md#gameget_abilityability_name), that allowed to enable or disable the super spin attack, is now deprecated. There is now a new ability `spin_attack`, that controls the level of spin attack, allowing to disable the spin attack completely (0), to enable the normal spin attack (1) or the super spin attack (2).
The now deprecated `sword_knowledge` ability is equivalant to a `spin_attack` ability of lvel 1.

### Audio API changes

There is a whole new audio API with proper types for [music](../../lua-api/audio/music.md) and [sound](../../lua-api/audio/sound.md). The only slightly breaking change is detailed below.

- `hero:get_sword_sound_id()` is now deprecated, please use `hero:get_sword_sound()` instead. This function was renamed for consistency with the rest of the API. Additionally, it now returns `nil` rather than an empty string if there is no sword sound.
- If you are using IT music files, channels numbers are now between 1 and the number of channels in order to stick to tracker software conventions.

### Optional booleans are more strict

Most functions that take a boolean parameter with a default value were wrongly allowing `nil` instead of a boolean value or no value.
As an example, `entity:set_visible()` (no value) is allowed, is correct, and is equivalent to `entity:set_visible(true)`.
However, `entity:set_visible(nil)` was very confusing and will now generate an error for quests with format 2.0.
The engine still allows `entity:set_visible(nil)` for quests with format < 2.0 for compatibility, with `nil` confusingly meaning "default value" and not `false`.
