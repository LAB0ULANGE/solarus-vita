# Chest

## Overview

A chest is a box that contains a treasure.

This type of [map entity](./index.md) can be declared in the [map data file](../map.md#map-files). It can also be created dynamically with [`map:create_chest()`](../map.md#mapcreate_chestproperties).

A chest may contain a treasure or be empty. When opening the chest, the following happens by default:

- If the chest contains a treasure, the engine automatically gives it to the player, unless the treasure is not [obtainable](../equipment-items.md#itemis_obtainable).
- If the chest is empty or contains a non-obtainable treasure, then nothing happens.

This is the default behavior of opening a chest, and it can be redefined by your script in the [`chest:on_opened()`](#cheston_openedtreasure_item-treasure_variant-treasure_savegame_variable-hero) event.

The state of a chest is either open or closed. When a chest is closed, its treasure (if any) is still inside and the [hero](./hero.md) can get it.

A chest appears initially open on the [map](../map.md) if its state is saved and the corresponding [boolean value](../game.md#gameget_valuesavegame_variable) is `true` If is possible to save the state of a chest (open or closed) even if it contains no treasure.

## Methods Inherited from `entity`

Chests are particular [map entities](./index.md). Therefore, they inherit all methods from the type map entity.

See [entity](./index.md#methods-of-all-entity-types) to know these methods.

## Methods of the type chest

The following methods are specific to chests.

### `chest:is_open()`

Returns the state of this chest (open or closed).

Return value (boolean)
: `true` if this chest is open, `false` if it is closed.

### `chest:set_open([open])`

Sets the state of this chest (open or closed). If you close the chest, its treasure (as returned by [`chest:get_treasure()`](#chestget_treasure)) is restored and can be obtained again later.

`open` (boolean, optional, default: `true`)
: `true` to make the chest open, `false` to make it closed.

### `chest:get_treasure()`

Returns the treasure the player will obtain when opening this chest.

If the chest is already open, this function still works: it returns the treasure that was inside the chest before it was open.

Return value 1 (string or `nil`)
: Name of an [equipment item](../equipment-items.md). `nil` means that the chest is empty.

Return value 2 (number or `nil`)
: Variant of this equipment item (`1` means the first variant). `nil` means that the chest is empty.

Return value 3 (string or `nil`)
: Name of the boolean value that stores in the [savegame](../game.md) whether the chest is open. `nil` means that the chest is not saved.

!!! note "Note"

    If the treasure is a [non-obtainable item](../equipment-items.md#itemis_obtainable), the hero will actually get no treasure when opening the chest.

### `chest:set_treasure([item_name, [variant, [savegame_variable]]])`

Sets the treasure the player will obtain when opening this chest.

If the chest is already open, this function still works, it sets the treasure that will be put back in case you [close](#chestset_openopen) the chest later.

`item_name` (string or `nil`, optional)
: Name of an [equipment item](../equipment-items.md). `nil` makes the chest empty.

`variant` (number or `nil`, optional, requires: `item_name`, default: `1`)
: Variant of this equipment item (`1` means the first variant). The default value is `1`. Must be `nil` when `item_name` is `nil`.

`savegame_variable` (string or `nil`, optional, requires: `variant`)
: Name of the boolean value that stores in the [savegame](../game.md) whether the chest is open. `nil` means that the chest is not saved.

!!! note "Note"

    If the treasure is a [non-obtainable item](../equipment-items.md#itemis_obtainable), the hero will actually get no treasure when opening the chest.

### `chest:get_opening_method()`

Returns a string describing the way this chest can be opened. Can be either `"interaction"` (when the hero interacts with it), `"interaction_if_item"` (when the hero interacts with it while possessing a certain item, returned by [`chest:get_opening_condition()`](#chestget_opening_condition)), `"interaction_if_savegame_variable"` (when the hero interacts with it while the value of a certain savegame variable, returned by [`chest:get_opening_condition()`](#chestget_opening_condition), is true),

Return value (string)
: The opening method.

### `chest:get_opening_condition()`

Returns the name of the item or savegame variable required to open this chest (if the [opening method](#chestget_opening_method) is `"interaction_if_item"` or `"interaction_if_savegame_variable"`, respectively).

Return value (string)
: item name or savegame variable.

### `chest:is_opening_condition_consumed()`

Returns whether the [opening condition](#chestget_opening_condition) of this chest is consumed when you open it (only if the [opening method](#chestget_opening_method) is `"interaction_if_item"` or `"interaction_if_savegame_variable"`) If the condition is an item (i.e. the [opening method](#chestget_opening_method) is `"interaction_if_item"`), the item is removed, which means setting its [possessed variant](../equipment-items.md#itemset_variantvariant) to `0`, unless it has an associated amount : in this case, the amount is decremented.

If the condition is a savegame variable (i.e. the [opening method](#chestget_opening_method) is `"interaction_if_savegame_variable"`),it is reset to `false`, `0` or `""` (depending on its type).

Return value (boolean)
: `true` if the condition is consumed, `false` if it is not.

### `chest:set_opening_method(method)`

Sets the opening method of this chest. See [`chest:get_opening_method()`](#chestget_opening_method) for a list of possible methods.

`method` (string)
: the string associated with the wanted method, as returned by [`chest:get_opening_method()`](#chestget_opening_method)

### `chest:set_opening_condition(condition)`

Sets the name of the item or savegame variable required to open this chest (if the [opening method](#chestget_opening_method) is `"interaction_if_item"` or `"interaction_if_savegame_variable"`, respectively).

`condition` (string)
: item name or savegame variable that will be required to open this chest.

### `chest:set_opening_condition_consumed([condition_consumed])`

Returns whether the [opening condition](#chestget_opening_condition) of this chest is consumed when you open it (only if the [opening method](#chestget_opening_method) is `"interaction_if_item"` or `"interaction_if_savegame_variable"`).

See [`chest:is_opening_condition_consumed()`](#chestis_opening_condition_consumed) for more information about what happens when the condition is consumed.

`condition_consumed` (boolean, optional, default: `true`)
: `true` if the condition is consumed, `false` if it is not.

### `chest:get_cannot_open_sound()`

Returns the sound id defined for this chest when the hero is unable to open it.

Return value (string)
: The sound id.

### `chest:set_cannot_open_sound([sound_id])`

Changes the sound to play when the hero is unable to open the chest.

By default, the sound used is `"wrong"`.

You can use this function if you want another sound to be played.

`sound_id` (string or `nil`, optional)
: The sound id. An empty string or `nil` means no sound.

### `chest:get_opening_sound()`

Returns the sound id for the chest opening phase.

Return value (string)
: The sound id.

### `chest:set_opening_sound([sound_id])`

Changes the sound to play when the chest is opening. By default, the sound used for chest opening phase is `"chest_open"`.

You can use this function if you want another sound to be played.

`sound_id` (string or `nil`, optional)
: The sound id. An empty string or `nil` means no sound.

## Events Inherited from `entity`

Events are callback methods automatically called by the engine if you define them.

Chests are particular [map entities](./index.md). Therefore, they inherit all events from the type map entity.

See [entity](./index.md#events-of-all-entity-types) to know these events.

## Events of the type chest

The following events are specific to chests.

### `chest:on_opened(treasure_item, treasure_variant, treasure_savegame_variable, hero)`

Called when a hero opens this chest.

At this point, if the chest is saved, then the engine has already set the corresponding savegame value to `true` (`treasure_savegame_variable`),no matter if this event is defined.

Then, if you don't define this event, by default, the engine gives the treasure to the player (if there is no treasure, then nothing else happens and the hero is automatically unfrozen).

Your script can define this event to customize what happens. By calling [`hero:start_treasure()`](./hero.md#herostart_treasuretreasure_name-treasure_variant-treasure_savegame_variable-callback), you can either give the chest's treasure or a treasure decided dynamically. Or you can do something else: show a dialog, play a sound, close the chest again, etc.

The hero is automatically frozen during the whole process of opening a chest. If you don't give him a treasure, then you have to unblock him explicitly by calling [`hero:unfreeze()`](./hero.md#herounfreeze) when you want to restore control to the player.

`treasure_item` ([item](../equipment-items.md) or `nil`)
: Equipment item in the chest, or `nil` if the chest is empty or contains a [non-obtainable item](../equipment-items.md#itemis_obtainable).

`treasure_variant` (number or `nil`)
: Variant of the treasure or `nil` if the chest is empty or contains a [non-obtainable item](../equipment-items.md#itemis_obtainable).

`treasure_savegame_variable` (string or `nil`)
: Name of the boolean value that stores in the [savegame](../game.md) whether this chest is open, or `nil` if this chest is not saved.

`hero` ([hero](hero.md))
: The hero who opened the chest.
