# Pickable Treasure

## Overview

A pickable treasure is a treasure on the ground and that the [hero](./hero.md) can pick up.

This type of [map entity](./index.md) can be declared in the [map data file](../map.md#map-files). It can also be created dynamically with [`map:create_pickable()`](../map.md#mapcreate_pickableproperties).

Pickable treasures may also be dropped by [enemies](./enemy.md) and by [destructible entities](./destructible.md).

The properties of a pickable treasure (like its [sprite](../drawable-objects/sprite.md) and its size) are essentially determined by the [equipment item](../equipment-items.md) given by the treasure. The script of that equipment item can call methods of this API to dynamically tune the behavior of the pickable treasure.

### Pickable Treasure Sprites

Two sprites for a pickable treasure are automatically created by the engine. You can access them like for any other entity, specifying their name in [`entity:get_sprite([name])`](./index.md#entityget_spritename).

- `"treasure"`: Main sprite representing the treasure. Its animation set is `"entities/items"`, with the animation of the treasure item's name and the direction of the treasure's variant. This is the default one in [`entity:get_sprite([name])`](./index.md#entityget_spritename).
- `"shadow"`: Optional shadow displayed under the treasure. Its animation set is `"entities/shadow"`, with the animation specified in [`item:set_shadow()`](../equipment-items.md#itemset_shadowshadow_animation) (`nil` means no shadow sprite).

## Methods Inherited from `entity`

Pickable treasures are particular [map entities](./index.md). Therefore, they inherit all methods from the type map entity.

See [entity](./index.md#methods-of-all-entity-types) to know these methods.

## Methods of the type `pickable`

### `pickable:has_layer_independent_collisions()`

Returns whether this pickable treasure can detect collisions with entities even if they are not on the same layer.

By default, pickable treasures can only have collisions with entities on the same layer.

Return value (boolean)
: `true` if this pickable treasure can detect collisions even with entities on other layers.

### `pickable:set_layer_independent_collisions([independent])`

Sets whether this pickable treasure can detect collisions with entities even if they are not on the same layer.

By default, pickable treasures can only have collisions with entities on the same layer. For example, you can call this method if your pickable treasure is a flying object that should be able to be picked by the [hero](./hero.md) no matter his current layer.

`independent` (boolean, optional, default: `true`)
: `true` to make this pickable treasure detect collisions even with entities on other layers.

### `pickable:get_followed_entity()`

Returns the [entity](./index.md) (if any) followed by this pickable treasure.

Pickable treasures get automatically attached to entities like the boomerang or the hookshot when such entities collide with them. You can use this function to know if it happens.

Return value ([map entity](./index.md) or `nil`)
: The entity this pickable treasure is attached to, or `nil` if the pickable treasure is free.

### `pickable:get_falling_height()`

Indicates how high this pickable treasure falls from.

This depends on how the pickable treasure was created. If is was placed on the map initially, it does not fall at all (`0` is returned). If it appears when the [hero](./hero.md) lifts a [destructible object](./destructible.md), it falls from a low height. If it is dropped by an [enemy](./enemy.md), it falls from higher.

By default, the engine sets a [movement](../movements/index.md) that makes the pickable treasure bounce of a few pixels over the ground during a fraction of second. The number of pixels, the duration and the number of bounces of the movement depends on this height. If you want to override that movement, (by calling [`movement:start(pickable)`](../movements/index.md#movementstartobject_to_move-callback)), you may also want to make it dependent of the falling height.

Return value (number)
: An integer indicating how high the pickable treasure falls from at creation time, between `0` (not falling at all) and `3` (falling from some high place).

### `pickable:get_treasure()`

Returns the kind of treasure represented by this pickable treasure.

Return value 1 ([item](../equipment-items.md))
: The equipment item of this treasure.

Return value 2 (number)
: Variant of this equipment item (`1` means the first variant).

Return value 3 (string or `nil`)
: Name of the boolean value that stores in the [savegame](../game.md) whether this pickable treasure is found. `nil` means that the treasure is not saved.

### `pickable:get_falling_sound()`

Returns the sound id when the pickable is falling into a hole.

Return value (string)
: The sound id.

### `pickable:set_falling_sound([sound_id])`

Changes the sound to play when the object is falling into a hole. By default, the sound used for falling is `"jump"`. You can use this function if you want another sound to be played.

`sound_id` (string or `nil`, optional)
: The sound id. An empty string or `nil` means no sound.

### `pickable:get_sinking_sound()`

Returns the sound id when the pickable is sinking into deep water or lava.

Return value (string)
: The sound id.

### `pickable:set_sinking_sound([sound_id])`

Changes the sound to play when the object is sinking into deep water or lava. By default, the sound used for sinking is `"splash"`. You can use this function if you want another sound to be played.

`sound_id` (string or `nil`, optional)
: The sound id. An empty string or `nil` means no sound.

## Events Inherited from `entity`

Events are callback methods automatically called by the engine if you define them.

Pickable treasures are particular [map entities](./index.md). Therefore, they inherit all events from the type map entity.

See [entity](./index.md#events-of-all-entity-types) to know these events.

## Events of the type `pickable`

None.
