# Carried Object

## Overview

A carried object is a [map entity](./index.md) that the [hero](./hero.md) is lifting, carrying or throwing.

A carried object is created automatically by the engine when the [hero](./hero.md) [lifts a map entity](./index.md#entityget_weight), typically a [destructible object](./destructible.md). The carried object takes the sprite and the features of the lifted entity it is created from.

The hero can then walk with his carried object and throw it. He can even go to another map: the carried object is preserved.

### Carried Object Sprites

Two sprites for a carried object are automatically created by the engine. You can access them like for any other entity, specifying their name in [`entity:get_sprite(name)`](./index.md#entityget_spritename).

`"main"` (default)
: Main sprite representing the carried object. Its animation set is the one of the original entity he was created from (like a [destructible object](./destructible.md)).

    It should have animations `"stopped"`, `"walking"` and `"destroy"`, with the same direction as the hero. These animations will be played depending on the actions of the hero on the carried object.

    If the carried object was created from a [destructible object](./destructible.md) that can explode, then animations `"stopped_explosion_soon"` and `"walking_explosion_soon"` will be played instead.

`"shadow"`
: Shadow displayed under the carried object when thrown. Its animation set is `"entities/shadow"`, with the animation name `"big"`.

## Methods Inherited from `entity`

Carried objects are particular [map entities](./index.md). Therefore, they inherit all methods from the type map entity.

See [entity](./index.md#methods-of-all-entity-types) to know these methods.

## Methods of the type carried object

The following methods are specific to carried objects.

### `carried_object:get_carrier()`

Returns the [entity](./index.md) that carried this object.

Return value ([entity](./index.md))
: The carrier entity.

### `carried_object:get_damage_on_enemies()`

Returns the number of life points that an [enemy](./enemy.md) loses when the [hero](./hero.md) throws this object at it.

Return value (number)
: The number of life points to remove to an enemy hit by this object. `0` means that enemies will ignore this object.

### `carried_object:set_damage_on_enemies(damage_on_enemies)`

Sets the number of life points that an [enemy](./enemy.md) loses when the [hero](./hero.md) throws this object at it.

`damage_on_enemies` (number)
: The number of life points to remove to an enemy hit by this object. `0` means that enemies will ignore this object.

### `carried_object:get_object_height()`

Returns the height this object will be displayed at, relative to the [hero's carry height.](./hero.md#heroget_carry_height) This means that the actual height the object will be displayed at (above the hero's origin) is a sum of the hero's carry height and this value.

Return value (number)
: The value that is added to the carry height when displaying this carried object.

### `carried_object:set_object_height(height)`

Sets the height this object will me displayed at, relative to the [hero's carry height.](./hero.md#heroset_carry_heightheight) This means that the object will be actually displayed at the hero's carry height plus this value above the hero's origin.

`height` (number)
: The value that will be added to the carry height when displaying this carried object.

### `carried_object:get_destruction_sound()`

Returns the sound to be played when this object is cut or broken.

Return value (string or `nil`)
: Id of the destruction sound. `nil` means that no sound will be played.

### `carried_object:set_destruction_sound(destruction_sound_id)`

Sets the sound to be played when this object is cut or broken.

`destruction_sound_id` (string or `nil`)
: Id of the destruction sound. `nil` means that no sound will be played.

### `carried_object:get_throwing_sound()`

Returns the sound id when the carried_object is thrown.

Return value (string)
: The sound id.

### `carried_object:set_throwing_sound([sound_id])`

Changes the sound to play when the object is thrown. By default, the sound used for throwing is "throw". You can use this function if you want another sound to be played.

`sound_id` (string or `nil`, optional)
: The sound id. An empty string or `nil` means no sound.

### `carried_object:get_falling_sound()`

Returns the sound id when the carried_object is falling into a hole.

Return value (string)
: The sound id.

### `carried_object:set_falling_sound([sound_id])`

Changes the sound to play when the object is falling into a hole. By default, the sound used for falling is "jump". You can use this function if you want another sound to be played.

`sound_id` (string or `nil`, optional)
: The sound id. An empty string or `nil` means no sound.

### `carried_object:get_sinking_sound()`

Returns the sound id when the carried_object is sinking into deep water or lava.

Return value (string)
: The sound id.

### `carried_object:set_sinking_sound([sound_id])`

Changes the sound to play when the object is sinking into deep water or lava. By default, the sound used for sinking is "walk_on_water". You can use this function if you want another sound to be played.

`sound_id` (string or `nil`, optional)
: The sound id. An empty string or `nil` means no sound.

### `carried_object:get_exploding_sound()`

Returns the sound id when the carried_object is exploding.

Return value (string)
: The sound id.

### `carried_object:set_exploding_sound([sound_id])`

Changes the sound to play when the object is exploding. By default, the sound used for exploding is "explosion". You can use this function if you want another sound to be played.

`sound_id` (string or `nil`, optional)
: The sound id. An empty string or `nil` means no sound.

## Events Inherited from `entity`

Events are callback methods automatically called by the engine if you define them.

Carried objects are particular [map entities](./index.md). Therefore, they inherit all events from the type map entity.

See [entity](./index.md#events-of-all-entity-types) to know these events.

## Events of the type carried object

The following events are specific to carried objects.

### `carried_object:on_lifted()`

Called when this object has finished being lifted and is now actually carried.

### `carried_object:on_thrown()`

Called when this object starts being thrown.

### `carried_object:on_breaking()`

Called when this object falls on the ground and starts breaking.
