# Game

## Overview

This module provides a datatype `game` that represents a savegame.

### Saved Data

On a `game` object, you can access and modify everything that is saved. The data saved and restored by the engine are the following:

- starting location of the player (map and destination on this map),
- life (maximum and current),
- money (maximum and current),
- magic (maximum and current),
- built-in ability levels (like attacking, swimming, running, etc.).
- possession state of each saved equipment item,
- equipment item assigned to each item command slot,
- keyboard and joypad associations to game commands,
- any key-value pair that you need to store for your quest (see [`game:get_value()`](#gameget_values) and [`game:set_value()`](#gameset_valuesavegame_variable-value)).

When a game is running, more features are available (like pausing the game, handling game commands, etc.). Only one game can be running at a time.

### Game Commands

Commands moved to [controls](./controls/index.md)

### Accessing the game like tables

Objects of the `game` type are `userdata`, but like most Solarus Lua types, they can also be accessed like tables. This is especially useful for the game type to add, next to the built-in game features, your own quest-specific functions and data (like your HUD and your pause menu).

## Functions of `sol.game`

### `sol.game.exists(file_name)`

Returns whether the specified savegame file exists.

A valid quest write directory must be set (in your [quest.dat file](../files-specs/properties-file.md) or by calling [`sol.main.set_quest_write_dir()`](./general-features.md#solmainset_quest_write_dirquest_write_dir)), otherwise savegames cannot be used and this function generates a Lua error.

`file_name` (string)
: Name of the file to test, relative to the [quest write directory](./general-features.md#solmainget_quest_write_dir).

Return value (boolean)
: `true` if there exists a file with this name in the quest write directory.

### `sol.game.delete(file_name)`

Deletes a savegame file.

A valid quest write directory must be set (in your [quest.dat](../files-specs/properties-file.md) file or by calling [`sol.main.set_quest_write_dir()`](./general-features.md#solmainset_quest_write_dirquest_write_dir)), otherwise savegames cannot be used and this function generates a Lua error.

`file_name` (string)
: Name of the file to delete, relative to the [quest write directory](./general-features.md#solmainget_quest_write_dir).

### `sol.game.load(file_name)`

Loads an existing savegame, or initializes a new one if it does not exist (but does not save it).

A valid quest write directory must be set (in your [quest.dat](../files-specs/properties-file.md) file or by calling [`sol.main.set_quest_write_dir()`](./general-features.md#solmainset_quest_write_dirquest_write_dir)), otherwise savegames cannot be used and this function generates a Lua error.

`file_name` (string)
: Name of a savegame file, relative to the to the [quest write directory](./general-features.md#solmainget_quest_write_dir).

Return value (game)
: The loaded (or created) game.

!!! note "Note"

    This function does not start the game, it just loads the savegame file and initializes all [equipment item scripts](./equipment-items.md). Then you can access the data saved in the savegame file and use the API of equipment items. To actually run the game, call [`game:start`](#gamestart)

## Methods of the type `game`

### `game:save()`

Saves this game into its savegame file.

A valid quest write directory must be set (in your [quest.dat file](../files-specs/properties-file.md) or by calling [`sol.main.set_quest_write_dir()`](./general-features.md#solmainset_quest_write_dirquest_write_dir)), otherwise savegames cannot be used and this function generates a Lua error.

### `game:start()`

Runs this game.

This function is typically called from your savegame menu, when the player chooses its savegame file.

If another game was running, it is stopped automatically because only one game can be running at a time.

You can also call this function to restart the current game itself, even if it was not saved recently (saved data will not be reset). This may be useful to restart the game after the [game-over sequence](#gameon_game_over_started).

### `game:is_started()`

Returns whether this game is currently running.

Only one game can be running at a time.

Return value (boolean)
: `true` if this game is running.

### `game:is_suspended()`

Returns whether this game is currently suspended.

The game is suspended when at least one of the following conditions is true:

- the game is [paused](#gameis_paused),
- or a [dialog](#gameis_dialog_enabled) is active,
- or the [game-over sequence](#gameis_game_over_enabled) is active,
- or a transition between two maps is playing,
- or you explicitly called [`game:set_suspended(true)`](#gameset_suspendedsuspended).

Return value (boolean)
: `true` if this game is currently suspended. Only possible when the game is running.

### `game:set_suspended([suspended])`

Suspends or unsuspends the game.

Note that the game is also automatically suspended by the engine in the following situations:

- when the game is [paused](#gameis_paused),
- or when a [dialog](#gameis_dialog_enabled) is active,
- or when the [game-over sequence](#gameis_game_over_enabled) is active,
- or when a transition between two maps is playing.

Therefore, if you call `game:set_suspended(false)` during one of these sequences, it will only take effect at the end of it.

`suspended` (boolean, optional, default: `true`)
: `true` to suspend the game, `false` to resume it.

!!! note "Note"

    When the hero goes to another map, the game is automatically unsuspended after the map opening transition.

### `game:is_paused()`

Returns whether this game is currently paused.

Return value (boolean)
: `true` if this game is paused. Only possible when the game is running.

### `game:set_paused([paused])`

Pauses or resumes the game explictly.

Note that by default, a built-in game command already exists to pause and unpause the game.

`paused` (boolean, optional, default: `true`)
: `true` to pause the game, `false` to unpause it. Only possible when the game is running.

### `game:is_pause_allowed()`

Returns whether the player can pause or unpause the [game](#game).

Return value (boolean)
: `true` if the player is allowed to pause the game.

### `game:set_pause_allowed([pause_allowed])`

Sets whether the player can pause or unpause the [game](#game).

`pause_allowed` (boolean, optional, default: `true`)
: `true` to allow the player to pause the game.

!!! note "Note"

    This function applies to the built-in [pause command](#game-commands). Your script can still pause the game explicitly by calling [`game:set_paused()`](#gameset_pausedpaused).

### `game:is_dialog_enabled()`

Returns whether this game is currently showing a dialog.

It does not matter whether the dialog is shown with the built-in, minimal dialog box or with your custom dialog box (see [`game:on_dialog_started()`](#gameon_dialog_starteddialog-info)).

Only possible when the game is running.

Return value (boolean)
: `true` if a dialog is being shown.

### `game:start_dialog(dialog_id, [info], [callback])`

Starts showing a dialog.

A dialog must not be already active. This function returns immediately, but you can provide a callback that will be executed when the dialog finishes. The game is suspended during the dialog, like when it is paused.

If the event [`game:on_dialog_started()`](#gameon_dialog_starteddialog-info) is not defined, then the engine will show a default, minimal dialog system without decoration. The user will be able to close the dialog by pressing the action command (you don't need to call [`game:stop_dialog()`](#gamestop_dialogstatus)).

On the contrary, if the event [`game:on_dialog_started()`](#gameon_dialog_starteddialog-info) is defined, the engine calls it and does nothing else. This is the recommended way, because you can make your custom dialog box implementation with any feature you need. The game will be suspended until you call [`game:stop_dialog()`](#gamestop_dialogstatus) explicitly.

`dialog_id` (string)
: Id of the dialog to show. The corresponding dialog must exist in the [dialogs.dat](../files-specs/languages/dialogs.md) file of the current [language](./language-functions.md).

`info` (any type except function, optional)
: Any information you want to pass to the [`game:on_dialog_started()`](#gameon_dialog_starteddialog-info) event. You can use this parameter to include in the dialog any information that is only known at runtime, for example the name of the player, the best score of a mini-game or the time spent so far in the game. See the examples below.

`callback` (function, optional)
: A function to be called when the dialog finishes. A status parameter (possibly `nil`) is passed to your function: its value is the argument of [`game:stop_dialog()`](#gamestop_dialogstatus) and it represents the result of the dialog. This feature may be used by your dialog box system to return any useful information to the map script, like the answer chosen by the player if the dialog was a question, or whether the dialog was skipped.

Example of a small map script with an NPC that shows a simple dialog:

```lua
local map = ...

function some_npc:on_interaction()
  -- Remember that you specify a dialog id, not directly the text to show.
  -- The text is defined in the dialogs.dat file of the current language.
  map:get_game():start_dialog("welcome_to_my_house")
end
```

Here is a more complex example, with an NPC that asks a question. This example assumes that your dialog box system can ask questions to the player, and returns the answer as a boolean value passed to [`game:stop_dialog()`](#gamestop_dialogstatus).

```lua
local map = ...
local game = map:get_game()
function another_npc:on_interaction()
  game:start_dialog("give_me_100_rupees_please", function(answer)
    if answer then
      if game:get_money() >= 100 then
        game:remove_money(100)
        game:start_dialog("thanks")
      else
        sol.audio.play_sound("wrong")
        game:start_dialog("not_enough_money")
      end
    else
      game:start_dialog("not_happy")
    end
  end)
end
```

Finally, to illustrate the use of the `info` parameter, let's modify the previous example to make the amount of money only determined at runtime. In other words, we want an NPC that can say "Please give me 100 rupees", but also "Please give me 25 rupees" or any number. Since the number is only known at runtime, it can no longer be hardcoded in the text of the [dialog](../files-specs/languages/dialogs.md). So let's assume that the text of the dialog contains instead a special sequence (like `"$v"`) to be substituted by the final value. (Note that [shop treasures](./map-entities/shop-treasure.md#dialogs-of-shop-treasures) use a very similar convention for their dialogs.)

```lua
local map = ...
local game = map:get_game()

function another_npc:on_interaction()
  local how_much = math.random(100)
  game:start_dialog("give_me_x_rupees_please", how_much, function(answer)
    if answer then
      if game:get_money() >= how_much then
        game:remove_money(how_much)
        -- ... The rest is unchanged.
```

To make this example work, you need a dialog box system that performs the substitution when `info` is set. See [`game:on_dialog_started()`](#gameon_dialog_starteddialog-info).

!!! note "Note"

    The `info` parameter of [`game:start_dialog()`](#gamestart_dialogdialog_id-info-callback) and the status parameter of the callback are a flexible way to make the map script communicate with the dialog box system in both directions.

    They can been seen as the parameter and the result (respectively) of the dialog being displayed. They can both be any value, like a table with many information.

### `game:stop_dialog([status])`

Stops the current dialog.

A dialog must be active when you call this function. The dialog stops being displayed and the game can resume. This function is typically called by your dialog box system when it wants to close the dialog.

The [`game:on_dialog_finished()`](#gameon_dialog_finisheddialog) event is first called (if it is defined). Then, the callback that was passed to [`game:start_dialog()`](#gamestart_dialogdialog_id-info-callback) is called (if it was defined) with the `status` argument.

`status` (any type, optional)
: Some information to return to the script that started the dialog. For example, you can pass the result of the dialog if it was a question, or whether it was skipped by the user before the end. See the [examples above](#gamestart_dialogdialog_id-info-callback).

### `game:is_game_over_enabled()`

Returns whether this game is currently showing a game-over sequence.

Only possible when the game is running.

The game-over sequence automatically starts when the player's life gets to zero, or when you call [`game:start_game_over()`](#gamestart_game_overhero) explicitly. Define the event [`game:on_game_over_started()`](#gameon_game_over_started) to show your game-over menu. If you don't define this event, by default, there is no game-over sequence and the engine immediately restarts the game (but does not save it).

Return value (boolean)
: `true` if a game-over sequence is running.

### `game:start_game_over([hero])`

Starts the game-over sequence manually.

Only possible when the game is running.

This function is seldom needed since the game-over sequence automatically starts when the hero's life reaches zero. But you can use it if you want to start a game-over sequence even when the hero's life is greater than zero.

`hero` ([hero](./map-entities/hero.md), optional)
: The hero that triggers the game-over sequence. No value means the main hero.

### `game:stop_game_over([hero])`

Finishes the game-over sequence of the hero.

Only possible during a game-over sequence.

The game is suspended during the whole game-over sequence. Call this function to resume it. If the [life](#gameget_life) is still zero at this point, then the engine automatically restores full life.

`hero` ([hero](./map-entities/hero.md), optional)
: The hero whose the game-over sequence should be stopped. No value means the main hero.

### `game:get_map()`

Returns the current map.

If more than one maps are loaded, returns the one that has the default hero if any.
Otherwise, returns the first map of the current maps.

Returns `nil` if the game is not running, or if the game has no map initialized yet, typically from `game:on_started()`.

Return value ([map](./map.md) or `nil`)
: The current map of this game, or `nil` if no map is currently running.

### `game:get_hero()`

Returns one of the current [heroes](./map-entities/hero.md).

Heroes are [map entities](./map-entities/index.md) that can follow cameras from map to map. For this reason, they can be seen as belonging to the game more than to the current maps. That's why this function exists.

Return value ([hero](./map-entities/hero.md) or `nil`)
: The hero, or `nil` if the game is not running.

!!! note "Note"

    Equivalent to `game:get_map():get_entity("hero")` if there is only one map / one hero.

### `game:get_value(savegame_variable)`

Returns a saved value.

`savegame_variable` (string)
: Name of the value to get from the savegame.

Return value (string or number or boolean or `nil`)
: The corresponding value (`nil` if no value is defined with this key).

### `game:set_value(savegame_variable, value)`

Sets a value in the savegame.

This function allows to store key-value pairs in the savegame. Values can be strings, integers or booleans.

`savegame_variable` (string)
: Name of the value to save (must contain alphanumeric characters or `'_'` only, and must start with a letter).

`value` (string or number or boolean or `nil`)
: The value to set, or `nil` to unset this value.

!!! note "Note"

    This method changes a value, but remember that the change will be saved in the savegame file only when you call [`game:save()`](#gamesave).

### `game:get_values()`

Returns all saved values.

This also include all built-in saved values, i.e. the ones starting with an underscore character `'_'`.

Return value (table)
: A table where every key-value pair is a savegame variable (a string) and its associated value (a string, an integer or a boolean).

### `game:get_starting_location()`

Returns the location where the hero is placed when this game is started or restarted.

Return value 1 (string or `nil`)
: Id of the starting map. `nil` means that it was not set: in this case, the first map declared in [project_db.dat](../files-specs/database-file.md) will be used.

Return value 2 (string or `nil`)
: Name of the destination where the hero will be placed on that map. `nil` means that it was not set: in this case, the default destination entity of that map will be used.

### `game:set_starting_location([map_id, [destination_name]])`

Sets the location where the hero should be placed when this game is started or restarted.

`map_id` (string, optional)
: Id of the starting map. By default, the first map declared in [project_db.dat](../files-specs/database-file.md) is used.

`destination_name` (string, optional, requires: `map_id`)
: Name of the destination where the hero should be placed on that map. By default, the default destination of the map is used.

!!! note "Note"

    When the hero moves from a map to another map that belongs to a different world (for example, from a dungeon to the outside world) using a destination entity, by default, the starting location is automatically set to this point.

    If this behavior is okay for your quest, you never need to call this function except the first time: when initializing a new savegame file.

    This behavior can be changed by setting the "Update starting location" property of destinations, from Solarus Editor or from a script (with [`destination:set_starting_location_mode()`](./map-entities/destination.md#destinationset_starting_location_modemode)).

### `game:get_transition_style()`

Returns the style of transition to use by default when changing maps.

This transition style is used when the game starts, and when calling [`hero:teleport()`](./map-entities/hero.md#heroteleportmap_id-destination_name-transition_style) without specifying the style of transition.

Return value (string)
: The default transition style. Can be one of:

    - `"immediate"`: No transition effect.
    - `"fade"`: Fade-out and fade-in effect (default).
    - `"scrolling"`: Scrolling between adjacent maps.

### `game:set_transition_style()`

Sets the style of transition to use by default when changing maps.

This transition style is used when the game starts, and when calling [`hero:teleport()`](./map-entities/hero.md#heroteleportmap_id-destination_name-transition_style) without specifying the style of transition.

Return value (string)
: The default transition style. See [`game:get_transition_style()`](#gameget_transition_style) for the possible values.

### `game:get_life()`

Returns the current level of life of the player.

Return value (number)
: The current life.

### `game:set_life(life)`

Sets the level of life of the player.

A negative value will be replaced by zero. A value greater than than the maximum level of life will be replaced by the maximum value.

`life` (number)
: Number of life points to set.

### `game:add_life(life)`

Adds some life to the player.

`life` (number)
: Number of life points to add. Must be a positive number or `0`.

!!! note "Note"

    Equivalent to `game:set_life(game:get_life() + life)`.

### `game:remove_life(life)`

Removes some life from the player.

`life` (number)
: Number of life points to remove. Must be a positive number or `0`.

!!! note "Note"

    Equivalent to `game:set_life(game:get_life() - life)`.

### `game:get_max_life()`

Returns the maximum level of life of the player.

Return value (number)
: The maximum number of life points.

### `game:set_max_life(life)`

Sets the maximum level of life of the player.

`life` (number)
: Maximum number of life points to set. Must be a positive number.

### `game:add_max_life(life)`

Increases the maximum level of life of the player.

`life` (number)
: Maximum number of life points to add to the maximum. Must be a positive number or `0`.

!!! note "Note"

    Equivalent to `game:set_max_life(game:get_max_life() + life)`.

### `game:get_money()`

Returns the amount of money of the player.

Return value (number)
: The current amount of money.

### `game:set_money(money)`

Sets the amount of money of the player.

A negative value will be replaced by zero. A value greater than than the maximum amount of money will be replaced by the maximum amount.

`money` (number)
: The amount of money to set.

### `game:add_money(money)`

Adds some money to the player.

`money` (number)
: Amount of money to add. Must be a positive number or `0`.

!!! note "Note"

    Equivalent to `game:set_money(game:get_money() + money)`.

### `game:remove_money(money)`

Removes some money from the player.

`money` (number)
: Amount of money to remove. Must be a positive number or `0`.

!!! note "Note"

    Equivalent to `game:set_money(game:get_money() - money)`.

### `game:get_max_money()`

Returns the maximum amount of money of the player.

Return value (number)
: The maximum money.

### `game:set_max_money(money)`

Sets the maximum amount of money of the player.

`money` (number)
: Maximum money to set. Must be a positive number or `0`.

### `game:get_magic()`

Returns the current number of magic points.

Return value (number)
: The current number of magic points.

### `game:set_magic(magic)`

Sets the amount of magic points of the player.

A negative value will be replaced by zero. A value greater than than the maximum number of magic points will be replaced by that maximum.

`magic` (number)
: The number of magic points to set.

### `game:add_magic(magic)`

Adds some magic points to the player.

`magic` (number)
: Number of magic points to add. Must be a positive number or `0`.

!!! note "Note"

    Equivalent to `game:set_magic(game:get_magic() + magic)`.

### `game:remove_magic(magic)`

Removes some magic points from the player.

`magic` (number)
: Number of magic points to remove. Must be a positive number or `0`.

!!! note "Note"

    Equivalent to `game:set_magic(game:get_magic() - magic)`.

### `game:get_max_magic()`

Returns the maximum number of magic points.

Return value (number)
: The maximum number of magic points.

### `game:set_max_magic(magic)`

Sets the maximum number of magic points.

`magic` (number)
: The maximum number of magic points to set. Must be a positive number or `0`.

### `game:has_ability(ability_name)`

Returns whether the player has a built-in ability.

`ability_name` (string)
: Name of the ability to get (see [`game:get_ability()`](#gameget_abilityability_name) for the list of valid ability names).

Return value (boolean)
: `true` if the player has this ability.

!!! note "Note"

    Equivalent to `game:get_ability(ability_name) > 0`.

### `game:get_ability(ability_name)`

Returns the level of a built-in ability.

Built-in ability levels indicate whether the hero can perform some built-in actions like attacking, swimming or running. The initial value is `0` unless specified otherwise.

`ability_name` (string)
: Name of the ability to get. Valid ability names are:

    - `"sword"`: Using the sword when pressing the attack command. Determines the default sword sprite.
    - `"sword_spin_attack"`: Ability to make a spin attack. 1 is a normal spin attack, 2 is a super spin attack where the hero makes circles.
    - `"sword_knowledge"`: Ability to make the super spin-attack. This is deprecated and equivalent to a `sword_spin_attack` ability value of 2.
    - `"tunic"`: Resistance level that reduces the damage received by the hero. Determines the default sprite used for the hero's body. The initial value is `1`
    - `"shield"`: Protection against enemies. Allows to avoid some attacks. Determines the default shield sprite.
    - `"lift"`: Ability to [lift other entities](./map-entities/index.md#entityget_weight).
    - `"swim"`: Ability to swim in deep water.
    - `"jump_over_water"`: Automatically jumping when arriving into water without the `"swim"` ability.
    - `"run"`: Running when pressing the action command.
    - `"push"`: Trying to push when walking towards an obstacle. The initial value is `1`
    - `"grab"`: Grabbing the faced obstacle when pressing the action command. The initial value is `1`
    - `"pull"`: Pulling the faced obstacle the hero is grabbing. The initial value is `1`
    - `"detect_weak_walls"`: Notifying the player with a sound when a weak wall is nearby.

Return value (number)
: Level of this ability (`0` means not having this ability yet).

### `game:set_ability(ability_name, level)`

Sets the level of an ability.

`ability_name` (string)
: Name of the ability to set (see [`game:get_ability()`](#gameget_abilityability_name) for the list of valid ability names).

`level` (number)
: Level of this ability to set (`0` removes the ability).

### `game:get_item(item_name)`

Returns an equipment item.

`item_name` (string)
: Name of the item to get.

Return value ([item](./equipment-items.md))
: The corresponding equipment item.

### `game:has_item(item_name)`

Returns whether the player has the specified [equipment item](./equipment-items.md) (only for a saved item).

`item_name` (string)
: Name of the item to check.

Return value (boolean)
: `true` if the player has at least the first variant of this item.

!!! note "Note"

    Equivalent to `game:get_item(item_name):get_variant() > 0`.

### `game:get_item_assigned(slot)`

Returns the equipment item assigned to a slot.

`slot` (number)
: The slot to get (`1` or `2`).

Return value ([item](./equipment-items.md) or `nil`)
: The equipment item associated to this slot (`nil` means none).

### `game:set_item_assigned(slot, item)`

Assigns an equipment item to a slot.

`slot` (number)
: The slot to set (`1` or `2`).

`item` ([item](./equipment-items.md) or `nil`)
: The equipment item to associate to this slot, or `nil` to make the slot empty.

### `game:get_command_effect(command)`

Returns the current built-in effect of a game command.

This function is useful if you want to show a HUD that indicates to the player the current effect of pressing a game command, especially for command `"action"` whose effect changes a lot depending on the context.

`command` (string)
: Name of a game command. Valid commands are:

    - `"action"`
    - `"attack"`
    - `"pause"`
    - `"item_1"`
    - `"item_2"`
    - `"right"`
    - `"up"`
    - `"left"`
    - `"down"`

Return value (string or `nil`)
: A string describing the current built-in effect of this game command. `nil` means that this command has currently no built-in effect (for example because the game is paused). Possible values are:

    - For command `"action"`: `"next"`, `"look"`, `"open"`, `"lift"`, `"throw"`, `"grab"`, `"speak"`, `"swim"`, `"run"` or `nil`
    - For command `"attack"`: `"sword"` or `nil`
    - For command `"pause"`: `"pause"`, `"return"` or `nil`
    - For command `"item_1"`: `"use_item_1"` or `nil`
    - For command `"item_2"`: `"use_item_2"` or `nil`
    - For command `"right"`: `"move_right"` or `nil`
    - For command `"left"`: `"move_left"` or `nil`
    - For command `"up"`: `"move_up"` or `nil`
    - For command `"down"`: `"move_down"` or `nil`

!!! note "Note"

    All these built-in game commands are initially mapped to some default keyboard and joypad inputs. You can use [`game:set_command_keyboard_binding()`](#gameset_command_keyboard_bindingcommand-key), [`game:set_command_joypad_binding()`](#gameset_command_joypad_bindingcommand-joypad_string) and [`game:capture_command_binding()`](#gamecapture_command_bindingcommand-callback) to change or even disable these mappings.

!!! note "Note"

    It is also possible to override the behavior of game commands by intercepting the events [`game:on_command_pressed()`](#gameon_command_pressedcommand) [`game:on_command_released()`](#gameon_command_releasedcommand).

!!! note "Note"

    Equivalent to `game:get_controls():get_effect(command)`.

### `game:get_command_keyboard_binding(command)`

Returns the keyboard key that triggers the specified game command.

`command` (string)
: Name of a game command. Valid commands are:

    - `"action"`
    - `"attack"`
    - `"pause"`
    - `"item_1"`
    - `"item_2"`
    - `"right"`
    - `"up"`
    - `"left"`
    - `"down"`

Return value (string or `nil`)
: Name of the keyboard key that triggers this game command, or `nil` if no keyboard key is mapped to this game command.

!!! note "Note"

    Equivalent to `game:get_controls():get_keyboard_binding(command)`.

### `game:set_command_keyboard_binding(command, key)`

Sets the keyboard key that triggers a game command.

`command` (string)
: Name of a game command. Valid commands are:

    - `"action"`
    - `"attack"`
    - `"pause"`
    - `"item_1"`
    - `"item_2"`
    - `"right"`
    - `"up"`
    - `"left"`
    - `"down"`

`key` (string or `nil`)
: Name of the keyboard key that should trigger this game command (`nil` means none).

!!! note "Note"

    If this keyboard key was already mapped to a command, keyboard keys of both commands are switched.

!!! note "Note"

    Equivalent to `game:get_controls():set_keyboard_binding(command, key)`.

### `game:get_command_joypad_binding(command)`

Returns the joypad input that triggers the specified game command.

`command` (string)
: Name of a game command. Valid commands are:

    - `"action"`
    - `"attack"`
    - `"pause"`
    - `"item_1"`
    - `"item_2"`
    - `"right"`
    - `"up"`
    - `"left"`
    - `"down"`

Return value (string)
: Joypad binding code (see [controls](./controls/index.md#controlsset_joypad_bindingcommand-button))

!!! note "Note"

    Equivalent to `game:get_controls():get_joypad_binding(command)`.

### `game:set_command_joypad_binding(command, joypad_string)`

Sets the joypad input that should trigger the specified game command.

`command` (string)
: Name of a game command. Valid commands are:

    - `"action"`
    - `"attack"`
    - `"pause"`
    - `"item_1"`
    - `"item_2"`
    - `"right"`
    - `"up"`
    - `"left"`
    - `"down"`

`joypad_string` (string)
: Joypad binding code (see [controls](./controls/index.md#controlsset_joypad_bindingcommand-button))

!!! note "Note"

    If this joypad input was already mapped to a command, joypad inputs of both commands are switched.

!!! note "Note"

    Equivalent to `game:get_controls():set_joypad_binding(command, joypad_string)`.

### `game:capture_command_binding(command, [callback])`

Makes the next keyboard or joypad input become the new binding for the specified game command.

This function returns immediately. After you call it, the next time the player presses a keyboard key or performs a joypad input, this input is treated differently: instead of being forwarded to your script or handled by the engine as usual, it automatically becomes the new keyboard or joypad binding for a game command.

`command` (string)
: Name of a game command. Valid commands are:

    - `"action"`
    - `"attack"`
    - `"pause"`
    - `"item_1"`
    - `"item_2"`
    - `"right"`
    - `"up"`
    - `"left"`
    - `"down"`

`callback` (function, optional)
: A function to call when the new input occurs.

!!! note "Note"

    If the keyboard (or joypad) input was already mapped to a command, keyboard (or joypad) inputs of both commands are switched.

!!! note "Note"

    Equivalent to `game:get_controls():capture_bindings(command, callback)`.

### `game:is_command_pressed(command)`

Returns whether a built-in game command is currently pressed.

`command` (string)
: Name of a game command. Valid commands are:

    - `"action"`
    - `"attack"`
    - `"pause"`
    - `"item_1"`
    - `"item_2"`
    - `"right"`
    - `"up"`
    - `"left"`
    - `"down"`

Return value (boolean)
: `true` if this game command is currently pressed by the player.

!!! note "Note"

    Equivalent to `game:get_controls():is_pressed(command)`.

### `game:get_commands_direction()`

Returns the direction (in an 8-direction system) formed by the combination of directional game commands currently pressed by the player.

Return value (number or `nil`)
: The direction wanted by the player (`0` to `7`), or `nil` for no direction. No direction means that no directional command is pressed, or that contradictory directional commands are pressed, like left and right at the same time (impossible with most joypads, but easy with a keyboard).

!!! note "Note"

    This function is provided for convenience. Its result can also be computed by calling [`game:is_command_pressed()`](#gameis_command_pressedcommand) four times (with the four directional game commands).

!!! note "Note"

    Equivalent to `game:get_controls():get_direction()`.

### `game:simulate_command_pressed(command)`

Creates a command pressed input event.

Everything acts like if the player had just pressed an input mapped to this game command.

`command` (string)
: Name of a game command. Valid commands are:

    - `"action"`
    - `"attack"`
    - `"pause"`
    - `"item_1"`
    - `"item_2"`
    - `"right"`
    - `"up"`
    - `"left"`
    - `"down"`

!!! note "Note"

    Equivalent to `game:get_controls():simulate_pressed(command)`.

### `game:simulate_command_released(command)`

Creates a command released input event.

Everything acts like if the player had just released an input mapped to this game command.

`command` (string)
: Name of a game command. Valid commands are:

    - `"action"`
    - `"attack"`
    - `"pause"`
    - `"item_1"`
    - `"item_2"`
    - `"right"`
    - `"up"`
    - `"left"`
    - `"down"`

!!! note "Note"

    Equivalent to `game:get_controls():simulate_released(command)`.

### `game:get_controls()`

Gets the [controls](./controls/index.md) object bound to the game (and the hero in the default case).

Return value ([controls](./controls/index.md))
: Main controls of the game.

### `game:set_controls(controls)`

Sets the [controls](./controls/index.md) object bound to the game.

`controls` ([controls](./controls/index.md))
: Controls to set as main game controls.

### `game:get_legacy_controls_storage()`

Returns whether main controls should be stored the legacy way (pre Solarus 2.0). See [`game:set_legacy_controls_storage()`](#gameset_legacy_controls_storagelegacy_controls_storage) for more details.

Return value (boolean)
: `true` if legacy controls storage is enabled.

### `game:set_legacy_controls_storage([legacy_controls_storage])`

Sets whether main controls should be stored the legacy way (pre Solarus 2.0).

When legacy storage is enabled, controls are loaded and saved automtically with this savegame, but with the following historical limitations:

- Does not support multiple heroes: only saves controls of the main hero.
- Limited support of multiple inputs bound to the same command: at most, only one keyboard input plus one joypad input.
- Does not support custom commands: scripts have to load and save them on their own.
- Does not apply to menus outside a game (like title screens) because this stores bindings in a savegame.

The default value is `false` to allow maximum customization. You can still enable this option if you are okay with the limitations or if your quest was designed before Solarus 2.0.

`legacy_controls_storage` (boolean, optional, default: `true`)
: `true` to enable legacy controls storage.

### `game:create_camera(id, map_id, [destination_id, [transition_style]])`

Creates a new camera on the map with id `map_id`. If the map is already loaded, camera is added to currently running map, else it loads the new map.

`id` (string)
: Name of this new camera.

`map_id` (string)
: Id of the map to create this camera onto

`destination_id` (string or `nil`, optional)
: Id of the destination that this camera should look at when created

`transition_style` (string, optional, requires: `destination_id`)
: Style of the transition the camera will use to fade-in.

### `game:remove_camera(camera, [transition_style])`

Remove a camera from this game. If the camera was the last on its map, the map gets unloaded and removed. This is different from `camera:remove()` that means removing the camera from the map.

`camera` ([camera](./map-entities/camera.md))
: Camera to remove.

`transition_style` (string, optional)
: Style of the transition the camera will use to fade-out.

### `game:get_cameras()`

Gets an iterator to all the cameras in the game, note that each of cameras could be on a separate map.

### `game:get_maps()`

Gets an iterator to all the maps currently loaded.

## Events of the type `game`

Events are callback methods automatically called by the engine if you define them. In the case of a game, they are only called on the game currently running, if any.

### `game:on_started()`

Called when this game starts running (including when you restart the same game).

### `game:on_finished()`

Called when this game stops running (including when you restart the same game).

### `game:on_update()`

Called at each cycle of the main loop while this game is running.

!!! note "Note"

    As this function is called at each cycle, it is recommended to use other solutions when possible, like [timers](./timers.md) and other events.

### `game:on_draw(dst_surface)`

Called when the game has just been redrawn by the engine.

The engine has already drawn the current map, since the map is always drawn before the game. If the game has [menus](./menus.md), these menu are not drawn yet at this point. Use this event if you want to draw some additional content before the menus.

`dst_surface` ([surface](./drawable-objects/surface.md))
: The surface where the game is drawn.

### `game:on_map_changed(map, camera)`

Called when the player has just entered a map.

The new map is already started at this point. For example, you may use this event if some parts of your HUD needs to be changed on particular maps.

`map` ([map](./map.md))
: The new active map.

`camera` ([camera](./map-entities/camera.md))
: The camera that transitioned to this map

!!! note "Note"

    When you [start or restart](#gamestart) the game, this event is always called, even if the new run happens to start on the same map as where the previous run finished.

### `game:on_world_changed(previous_world, new_world)`

Called when the player has just entered a map whose [world](./map.md#mapget_world) is different from the previous one.

Recall that maps without a world property are considered to be in their own world, different to all other maps. Therefore, if at least one of the previous and the new map has no world property, this event is necessarily called.

This event is called right after [`game:on_map_changed()`](#gameon_map_changedmap-camera).

`previous_world` (string or `nil`)
: The world of the previous map if any, or `nil` if the previous map had no world set or if there was no previous map.

`new_world` (string or `nil`)
: The world of the new map if any, or `nil` if the mew map had no world set.

!!! note "Note"

    When you [start or restart](#gamestart) the game, this event is always called, even if the new happens to start on a map with the same world (or even on the same map) as where the previous run finished.

!!! note "Note"

    Since the world is considered to be different from any other

### `game:on_paused()`

Called when the game has just been paused.

The game may have been paused by the player (by pressing the `"pause"`game command) or by you (by calling [`game:set_paused(true)`](#gameset_pausedpaused)).

This function is typically the place where you should start your pause menu.

### `game:on_unpaused()`

Called when the game is being resumed.

The game may have been unpaused by the player (by pressing the `"pause"`game command) or by you (by calling [`game:set_paused(false)`](#gameset_pausedpaused)).

This is probably a good place to stop your pause menu.

### `game:on_dialog_started(dialog, [info])`

Called when a dialog starts.

The dialog may be triggered by a Lua script (by calling [`game:start_dialog()`](#gamestart_dialogdialog_id-info-callback)) or by the engine in various situations (for example when finding a treasure).

If this event is not defined, the engine shows a minimal dialog box without decoration and you have nothing else to do.

If this event is defined, the engine does nothing and your script is responsible to show the dialog in any way you want, and to close it later by calling [`game:stop_dialog()`](#gamestop_dialogstatus). It is recommended to implement your dialog system as a [menu](./menus.md): if you do so, you will automatically get called by the engine when a command is pressed, when you need to draw the dialog box on the screen, etc.

`dialog` (table)
: All properties of the dialog to show. This table is identical to the one returned by [`sol.language.get_dialog()`](./language-functions.md#sollanguageget_dialogdialog_id). It is a table with at least the following two entries:

    `dialog_id` (string)
    : Id of the dialog.

    `text` (string)
    : Text of the dialog in the current language. It may have several lines. When it is not empty, it always ends with a newline character.

    The table also contains all custom entries defined in [text/dialogs.dat](../files-specs/languages/dialogs.md) for this dialog. These custom entries always have string keys and string values. Values that were defined as numbers in `"text/dialogs.dat"` are replaced in this table by their string representation, and values that were defined as booleans are replaced by the string `"1"` for `true` and `"0"` for `false`

`info` (any type except function, optional)
: Some additional information for this particular dialog. You can get here some data that is only known at runtime. See the examples of [`game:start_dialog()`](#gamestart_dialogdialog_id-info-callback).

### `game:on_dialog_finished(dialog)`

Called when the current dialog stops.

`dialog` (table)
: All properties of the dialog that was shown. See [`game:on_dialog_started()`](#gameon_dialog_starteddialog-info) for a description of this table.

### `game:on_game_over_started()`

Called when a game-over sequence starts.

This event is called when the player's life reaches zero, as soon as the [hero](./map-entities/hero.md) is in a state that allows game-over. It is also called if you started a game-over sequence manually with [`game:start_game_over()`](#gamestart_game_overhero).

If this event is not defined, there is no game-over sequence: the game restarts immediately, like if you called [`game:start()`](#gamestart), and the full life of the player is restored.

If this event is defined, the engine does nothing except suspending the game. Your script is then responsible to show a game-over sequence in any way you want, and to call [`game:stop_game_over()`](#gamestop_game_overhero) once you have finished.

For instance, you may create a [dialog](#gamestart_dialogdialog_id-info-callback) that lets the player [restart the game](#gamestart) or [save](#gamesave) and [quit](./general-features.md#solmainreset), or a [menu](./menus.md) with more options.

Actually, it is not even required to restart or quit the game after your game-over sequence (even if this is the most common case). Indeed, you can also just resume the game. In this case, the game continues normally like if nothing happened.

### `game:on_game_over_finished()`

Called when the current game-over sequence stops.

This event is also called if you did not define a game-over sequence.

### `game:on_command_pressed(command)`

Called when the player presses a game command (a keyboard key or a joypad action mapped to a built-in game behavior) while this game is running. You can use this event to override the normal built-in behavior of the game command.

`command` (string)
: Name of the built-in game command that was pressed. Possible commands are:

    - `"action"`
    - `"attack"`
    - `"pause"`
    - `"item_1"`
    - `"item_2"`
    - `"right"`
    - `"up"`
    - `"left"`
    - `"down"`

Return value (boolean)
: Indicates whether the event was handled. If you return `true`, the event won't be propagated to other objects (you are overriding the built-in behavior of pressing this game command).

!!! note "Note"

    This event is not triggered if you already handled its underlying low-level keyboard or joypad event.

### `game:on_command_released(command)`

Called when the player released a game command (a keyboard key or a joypad action mapped to a built-in game behavior). while this game is running. You can use this event to override the normal built-in behavior of the game command.

`command` (string)
: Name of the built-in game command that was released. Possible commands are:

    - `"action"`
    - `"attack"`
    - `"pause"`
    - `"item_1"`
    - `"item_2"`
    - `"right"`
    - `"up"`
    - `"left"`
    - `"down"`

Return value (boolean)
: Indicates whether the event was handled. If you return `true`, the event won't be propagated to other objects (you are overriding the built-in behavior of releasing this game command).

!!! note "Note"

    This event is not triggered if you already handled its underlying low-level keyboard or joypad event.

## Events as an Input Handler

In addition to its own events a game is also an input handler and receives all of those events.

The game is checked after `sol.main` but before the current map.

See [inputs](./controls/inputs.md) to see all of these events.
