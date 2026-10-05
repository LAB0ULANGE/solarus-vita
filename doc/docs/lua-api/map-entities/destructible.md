# Destructible Object

## Overview

A destructible object is an entity that can be cut or [lifted](./index.md#entityget_weight) by the [hero](./hero.md) and that may hide a [pickable treasure](./pickable.md).

This type of [map entity](./index.md) can be declared in the [map data file](../map.md#map-files). It can also be created dynamically with [`map:create_destructible()`](../map.md#mapcreate_destructibleproperties).

Destructible objects can be customized in various ways. You can allow the hero to lift them or to cut them. The ones that can be lifted may require a minimum level of the `"lift"` [ability](../game.md#gameget_abilityability_name). Their size is always 16×16 pixels (like the [hero](./hero.md)).

## Methods Inherited from `entity`

Destructible objects are particular [map entities](./index.md). Therefore, they inherit all methods from the type map entity.

See [entity](./index.md#methods-of-all-entity-types) to know these methods.

## Methods of the type destructible

The following methods are specific to destructible objects.

### `destructible:get_treasure()`

Returns what [pickable treasure](./pickable.md) this object will drop when being lifted, when being cut or when exploding.

Return value 1 (string or `nil`)
: Name of an [equipment item](../equipment-items.md). `nil` means no item (in this case, other return values are `nil` too).

Return value 2 (number)
: Variant of this equipment item (`1` means the first variant).

Return value 3 (string or `nil`)
: Name of the boolean value that stores in the [savegame](../game.md) whether the treasure dropped is found. `nil` means that the treasure is not saved.

### `destructible:set_treasure([item_name, [variant, [savegame_variable]]])`

Sets the [pickable treasure](./pickable.md) that this object will drop when being lifted, when being cut or when exploding.

`item_name` (string or `nil`, optional)
: Name of an [equipment item](../equipment-items.md). `nil` or no value means no item.

`variant` (number, optional, requires: `item_name`, default: `1`)
: Variant of this equipment item (`1` means the first variant).

`savegame_variable` (string or `nil`, optional, requires: `variant`)
: Name of the boolean value that stores in the [savegame](../game.md) whether the treasure dropped is found. `nil` or no value means that the treasure is not saved.

### `destructible:get_destruction_sound()`

Returns the sound to be played when this object is cut or broken.

Return value (string or `nil`)
: Id of the destruction sound. `nil` means that no sound will be played.

### `destructible:set_destruction_sound(destruction_sound_id)`

Sets the sound to be played when this object is cut or broken.

`destruction_sound_id` (string or `nil`)
: Id of the destruction sound. `nil` means that no sound will be played.

### `destructible:get_exploding_sound()`

Returns the sound id when the destructible is exploding.

Return value (string)
: The sound id.

### `destructible:set_exploding_sound([sound_id])`

Changes the sound to play when the object is exploding. By default, the sound used for exploding is "explosion". You can use this function if you want another sound to be played.

`sound_id` (string or `nil`, optional)
: The sound id. An empty string or `nil` means no sound.

### `destructible:get_can_be_cut()`

Returns whether this object can be cut by the sword.

Return value (boolean)
: `true` if this object can be cut by the sword.

### `destructible:set_can_be_cut(can_be_cut)`

Sets whether this object can be cut by the sword.

`can_be_cut` (boolean, optional, default: `true`)
: `true` to allow the player to cut this object with the sword.

### `destructible:get_cut_method()`

Returns how the hero's sword can cut this object.

This only has an effect if [`destructible:get_can_be_cut()`](#destructibleget_can_be_cut) is `true`.

Return value (string)
: The cut method. Can be one of:

    - `"aligned"` (default): The hero must be sufficiently aligned with this object when the sword hits it. Only one object can be cut at a time with this setting (unless they overlap).
    - `"pixel"`: Any pixel-precise collision between the sword and this object will cut it.

### `destructible:set_cut_method(cut_method)`

Sets how the hero's sword can cut this object.

This only has an effect if [`destructible:get_can_be_cut()`](#destructibleget_can_be_cut) is `true`.

`cut_method` (string)
: The cut method. See [`destructible:get_cut_method()`](#destructibleget_cut_method) for the possible values.

### `destructible:get_can_explode()`

Returns whether this object explodes when it is hit or after a delay when it is lifted.

Return value (boolean)
: `true` if this object can explode.

### `destructible:set_can_explode([can_explode])`

Sets whether this object explodes when it is hit or after a delay when it is lifted.

`can_explode` (boolean, optional, default: `true`)
: `true` to make the object able to explode.

### `destructible:get_can_regenerate()`

Returns whether this object regenerates after a delay when it is destroyed.

Return value (boolean)
: `true` if this object can regenerate.

### `destructible:set_can_regenerate([can_regenerate])`

Sets whether this object regenerates after a delay when it is destroyed.

`can_regenerate` (boolean, optional, default: `true`)
: `true` to make the object able to regenerate.

### `destructible:get_damage_on_enemies()`

Returns the number of life points that an [enemy](./enemy.md) loses when the [hero](./hero.md) throws this object at it.

Return value (number)
: The number of life points to remove to an enemy hit by this object. `0` means that enemies will ignore this object.

### `destructible:set_damage_on_enemies(damage_on_enemies)`

Sets the number of life points that an [enemy](./enemy.md) loses when the [hero](./hero.md) throws this object at it.

`damage_on_enemies` (number)
: The number of life points to remove to an enemy hit by this object. `0` means that enemies will ignore this object.

### `destructible:get_modified_ground()`

Returns the ground defined by this destructible object on the map.

The presence of a destructible object can modify the ground of the map. The ground is usually `"wall"`, but it may sometimes be `"traversable"`,or for example `"grass"` to make the destructible object traversable too but with an additional grass sprite below the hero.

Return value (string)
: The ground defined by this destructible object. See [`map:get_ground()`](../map.md#mapget_groundx-y-layer) for the list of possible grounds.

## Events Inherited from `entity`

Events are callback methods automatically called by the engine if you define them.

Destructible objects are particular [map entities](./index.md). Therefore, they inherit all events from the type map entity.

See [entity](./index.md#events-of-all-entity-types) to know these events.

## Events of the type destructible

The following events are specific to destructible objects.

### `destructible:on_looked()`

Called when the [hero](./hero.md) looks this destructible object, that is, when the player presses the action key but is not allowed to lift the object.

By default, nothing happens in this case. You can for example show a dialog to give the player a hint like "This is too heavy".

!!! note "Note"

    If you want to do the same action for all destructible objects of your game, use the [metatable trick](../general-features.md#solmainget_metatabletype_name). Just define this event on the metatable of the destructible object type instead of each individual object, and it will be applied to all of them.

### `destructible:on_cut()`

Called when the hero has just cut this destructible object.

### `destructible:on_exploded()`

Called when this destructible object is exploding.

If [`destructible:get_can_explode()`](#destructibleget_can_explode) is `true`, the destructible object explodes when there is an [explosion](./explosion.md) nearby or when the hero lifts it, after a delay.

### `destructible:on_regenerating()`

Called when this destructible object regenerates.

If [`destructible:get_can_regenerate()`](#destructibleget_can_regenerate) is `true`,the destructible object regenerates after a delay when it was lifted or exploded.
