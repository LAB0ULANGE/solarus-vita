# Door

## Overview

A door is an obstacle that can be opened by Lua, and optionally by the [hero](./hero.md) under some conditions.

This type of [map entity](./index.md) can be declared in the [map data file](../map.md#map-files). It can also be created dynamically with [`map:create_door()`](../map.md#mapcreate_doorproperties).

A door may have four states: open, closed, opening or closing. States opening and closing are transitional states used to play a door opening or closing animation on the door's sprite. The sprite of a door must define at least an animation `"closed"` When the door is open, nothing is displayed. Animations `"opening"` and `"closing"` are optional: if they don't exist, the door won't use states opening and closing. If they exist, be aware that after you call a function like [`map:open_doors()`](../map.md#mapopen_doorsprefix), your door will first be in state opening (for the duration of its sprite `"opening"` animation), and only after that, will get in state open. In other words, [`door:is_open()`](#dooris_open) does not return true as soon as you open the door, unless the sprite has no animation `"opening"`

Doors often work by pairs. Indeed, when you have two adjacent rooms on the same [map](../map.md), you normally use two door entities. They can be linked by setting a common prefix to their name. Then, you can use [`map:open_doors()`](../map.md#mapopen_doorsprefix) and [`map:close_doors()`](../map.md#mapclose_doorsprefix) to handle both of them at the same time more easily. Additionaly, [dynamic tiles](./dynamic-tile.md) whose names also have this prefix and end by `_open` or `_closed` are automatically enabled or disabled, respectively. This helps a lot to make tiles follow harmoniously the state of doors. See [`door:open()`](#dooropen), and [`map:open_doors(prefix)`](../map.md#mapopen_doorsprefix) for more information about these mechanisms.

A door may be opened by one of the following methods:

- `"none"`: Cannot be opened by the player. You can only open it explicitly by calling [`map:open_doors()`](../map.md#mapopen_doorsprefix).
- `"interaction"`: Can be opened by pressing the [action command](../game.md#game-commands) in front of it.
- `"interaction_if_savegame_variable"`: Can be opened by pressing the [action command](../game.md#game-commands) in front of it, provided that a specific savegame variable is set.
- `"interaction_if_item"`: Can be opened by pressing the [action command](../game.md#game-commands) in front of it, provided that the player has a specific [equipment item](../equipment-items.md).
- `"explosion"`: Can be opened by an explosion.

For doors whose opening method is `"interaction_if_savegame_variable"` or `"interaction_if_item"`,you can specify which savegame variable or [equipment item](../equipment-items.md) is required. You can also choose whether opening the door should consume the savegame variable of equipment item that was required.

## Methods Inherited from `entity`

Doors are particular [map entities](./index.md). Therefore, they inherit all methods from the type map entity.

See [entity](./index.md#methods-of-all-entity-types) to know these methods.

## Methods of the type door

The following methods are specific to doors.

### `door:is_open()`

Returns whether this door is open.

Return value (boolean)
: `true` if this door is open, `false` if it is opening, closed or closing.

### `door:is_opening()`

Returns whether this door is being opened.

Return value (boolean)
: `true` if this door is being opened, `false` if it is open, closed or closing.

### `door:is_closed()`

Returns whether this door is closed.

Return value (boolean)
: `true` if this door is closed, `false` if it is closing, open or opening.

### `door:is_closing()`

Returns whether this door is being closed.

Return value (boolean)
: `true` if this door is being closed, `false` if it is closed, open or opening.

### `door:open()`

Opens this door, enables [dynamic tiles](./dynamic-tile.md) whose name starts with the door's name followed by `_open` and plays the `"door_open"` [sound](../audio/index.md#solaudioplay_soundsound_id).

!!! note "Note"

    The door will be really closed once the opening animation of its sprite is finished. However, it immediately becomes an obstacle.

### `door:close()`

Closes this door, disables [dynamic tiles](./dynamic-tile.md) whose name starts with the door's name followed by `_closed` and plays the `"door_closed"` [sound](../audio/index.md#solaudioplay_soundsound_id).

### `door:set_open([open])`

Makes the door open or closed like [`door:open()`](#dooropen) or [`door:close()`](#doorclose), but does not play any sound or any sprite animation.

This function is intended to be called when you don't want the player to notice the change, typically when your map starts (i.e. from the [`map:on_started()`](../map.md#mapon_starteddestination) event).

`open` (boolean, optional, default: `true`)
: `true` to open the door, `false` to close it.

### `door:get_savegame_variable()`

Returns the name of the savegame variable where the state of this door is stored.

Return value (string or nil)
: the name of the variable (or nil if the door is not saved).

## `door:get_opening_method()`

Returns a string describing the way this door can be opened. Can be either `"none"` (the door can't be opened) `"interaction"` (when the hero interacts with it), `"interaction_if_item"` (when the hero interacts with it while possessing a certain item, returned by [`door:get_opening_condition()`](#doorget_opening_condition)), `"interaction_if_savegame_variable"` (when the hero interacts with it while the value of a certain savegame variable, returned by [`door:get_opening_condition()`](#doorget_opening_condition), is true), `"explosion"` (this door is opened when struck by an [explosion](./explosion.md))

Return value (string)
: the opening method.

### `door:get_opening_condition()`

Returns the name of the item or savegame variable required to open this door (if the [opening method](#doorget_opening_method) is `"interaction_if_item"` or `"interaction_if_savegame_variable"`, respectively).

Return value (string)
: item name or savegame variable.

### `door:is_opening_condition_consumed()`

Returns whether the [opening condition](#doorget_opening_condition) of this door is consumed when you open it (only if the [opening method](#doorget_opening_method) is `"interaction_if_item"` or `"interaction_if_savegame_variable"`) If the condition is an item (i.e. the [opening method](#doorget_opening_method) is `"interaction_if_item"`), the item is removed, which means setting its [possessed variant](../equipment-items.md#itemset_variantvariant) to `0`, unless it has an associated amount : in this case, the amount is decremented.

If the condition is a savegame variable (i.e. the [opening method](#doorget_opening_method) is `"interaction_if_savegame_variable"`),it is reset to `false`, `0` or `""` (depending on its type).

Return value (boolean)
: `true` if the condition is consumed, `false` if it is not.

### `door:set_opening_method(method)`

Sets the opening method of this door. See [`door:get_opening_method()`](#doorget_opening_method) for a list of possible methods.

`method` (string)
: the string associated with the wanted method, as returned by [`door:get_opening_method()`](#doorget_opening_method)

### `door:set_opening_condition(condition)`

Sets the name of the item or savegame variable required to open this door (if the [opening method](#doorget_opening_method) is `"interaction_if_item"` or `"interaction_if_savegame_variable"`, respectively).

`condition` (string)
: item name or savegame variable that will be required to open this door.

### `door:set_opening_condition_consumed([condition_consumed])`

Returns whether the [opening condition](#doorget_opening_condition) of this door is consumed when you open it (only if the [opening method](#doorget_opening_condition) is `"interaction_if_item"` or `"interaction_if_savegame_variable"`) See lua_api_door_is_opening_condition_consumed "door:is_opening_condition_consumed()" for more information about what happens when the condition is consumed.

`condition_consumed` (boolean, optional, default: `true`)
: `true` if the condition is consumed, `false` if it is not.

### `door:get_cannot_open_sound()`

Returns the sound id defined for this door when the hero is unable to open it.

Return value (string)
: The sound id.

### `door:set_cannot_open_sound([sound_id])`

Changes the sound to play when the hero is unable to open the door.

By default, the sound used is `"wrong"`.

You can use this function if you want another sound to be played.

`sound_id` (string or `nil`, optional)
: The sound id. An empty string or `nil` means no sound.

### `door:get_opening_sound()`

Returns the sound id for the door opening phase.

Return value (string)
: The sound id.

### `door:set_opening_sound([sound_id])`

Changes the sound to play when the door is opening.

By default, the sound used for door opening phase is `"door_open"`.

You can use this function if you want another sound to be played.

`sound_id` (string or `nil`, optional)
: The sound id. An empty string or `nil` means no sound.

### `door:get_closing_sound()`

Returns the sound id for the door closing phase.

Return value (string)
: The sound id.

### `door:set_closing_sound([sound_id])`

Changes the sound to play when the door is closing.

By default, the sound used for door closing phase is `"door_open"`.

You can use this function if you want another sound to be played.

`sound_id` (string or `nil`, optional)
: The sound id. An empty string or `nil` means no sound.

### `door:get_unlocking_sound()`

Returns the sound id for the door unlocking phase.

Return value (string)
: The sound id.

### `door:set_unlocking_sound([sound_id])`

Changes the sound to play when the door is unlocking with an item (usually a key).

By default, the sound used for door unlocking phase is `"door_unlocked"`.

You can use this function if you want another sound to be played.

`sound_id` (string or `nil`, optional)
: The sound id. An empty string or `nil` means no sound.

## Events Inherited from `entity`

Events are callback methods automatically called by the engine if you define them.

Doors are particular [map entities](./index.md). Therefore, they inherit all events from the type map entity.

See [entity](./index.md#events-of-all-entity-types) to know these events.

## Events of the type door

The following events are specific to doors.

### `door:on_opened()`

Called when this door gets open, after the opening animation if any.

### `door:on_closed()`

Called when this door gets closed, after the closing animation if any.
