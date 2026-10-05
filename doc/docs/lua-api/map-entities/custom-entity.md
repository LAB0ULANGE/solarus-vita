# Custom Entity

## Overview

A custom entity is a map entity entirely defined by your Lua scripts.

This type of [map entity](./index.md) can be declared in the [map data file](../map.md#map-files). It can also be created dynamically with [`map:create_custom_entity()`](../map.md#mapcreate_custom_entityproperties).

Custom entities have no special properties or behavior. You can define them entirely in your scripts.

Optionally, a custom entity may be managed by a model. The model is the name of a Lua script that will be applied to all custom entities refering to it. This works exactly like the [breed](./enemy.md#enemyget_breed) of enemies, except that it is optional. The model is useful if you have a lot of identical (or very similar) custom entities in your game, like for example torches.

If you make an entity that is unique in your game, like for example, a big rock that blocks the entrance of a dungeon and that requires some special action from the player, you don't need a model. You can just program the behavior of your custom entity in the script of its map. Similarly, to define a customized weapon of the hero, like a hammer, you don't need a model. Just create a custom entity from the item script of the hammer and define its behavior there.

## Methods Inherited from `entity`

Custom entities are particular [map entities](./index.md). Therefore, they inherit all methods from the type map entity.

See [entity](./index.md#methods-of-all-entity-types) to know these methods.

## Methods of the type custom entity

The following methods are specific to custom entities.

### `custom_entity:get_model()`

Returns the model of this custom entity.

The model is the name of a Lua script in the `"entities"` directory that manages this custom entity. This works exactly like the breed of [enemies](./enemy.md), except that it is optional.

Return value (string or `nil`)
: The model of custom entity, or `nil` if the custom entity has no model script.

### `custom_entity:get_direction()`

Returns the direction of this custom entity.

This direction is set at creation time or when you can call [`custom_entity:set_direction()`](#custom_entityset_directiondirection).

Return value (number)
: The direction.

### `custom_entity:set_direction(direction)`

Sets the direction of this custom entity.

Sprites of your custom entity that have such a direction automatically take it.

Return value (number)
: The direction.

### `custom_entity:is_tiled()`

Returns whether the sprite is repeated with tiling to fit the size of the custom entity when the size is bigger than the sprite.

Return value (boolean)
: `true` if the sprite is tiled.

### `custom_entity:set_tiled([tiled])`

Sets whether the sprite should be repeated with tiling to fit the size of the custom entity when the size is bigger than the sprite.

`tiled` (boolean, optional, default: `true`)
: `true` to make the sprite tiled.

### `custom_entity:set_traversable_by([entity_type], traversable)`

Sets whether this custom entity can be traversed by other entities.

By default, a custom entity can be traversed.

`entity_type` (string, optional)
: A type of entity. See [`entity:get_type()`](./index.md#entityget_type) for the possible values. If not specified, the setting will be applied to all entity types that do not override it.

`traversable` (boolean or function or `nil`)
: Whether this entity type can traverse your custom entity. This can be:

    - A boolean: `true` to make your custom entity traversable by this entity type, `false` to make it obstacle.

    - A function: Custom test. This allows you to decide dynamically. The function takes your custom entity and then the other entity as parameters, and should return `true` if you allow the other entity to traverse your custom entity. This function will be called every time a [moving](../movements/index.md) entity of the specified type is about to overlap your custom entity.

    - `nil`: Clears any previous setting for this entity type and therefore restores the default value.

### `custom_entity:set_can_traverse([entity_type], traversable)`

Sets whether this custom entity can traverse other entities.

This is important only if your custom entity can [move](../movements/index.md).

By default, this depends on the other entities: for example, [sensors](./sensor.md) can be traversed by default while [doors](./door.md) cannot unless they are open.

`entity_type` (string, optional)
: A type of entity. See [`entity:get_type()`](./index.md#entityget_type) for the possible values. If not specified, the setting will be applied to all entity types for which you don't override this setting.

`traversable` (boolean or function or `nil`)
: Whether your custom entity can traverse the other entity type. This can be:

    - A boolean: `true` to allow your custom entity to traverse entities of the specified type, `false` otherwise.

    - A function: Custom test. This allows you to decide dynamically. The function takes your custom entity and then the other entity as parameters, and should return `true` if you allow your custom entity to traverse the other entity. When your custom entity has a [movement](../movements/index.md), this function will be called every time it is about to overlap an entity of the specified type.

    - `nil`: Clears any previous setting for this entity type and therefore restores the default value.

### `custom_entity:can_traverse_ground(ground)`

Returns whether this custom entity can traverse a kind of ground.

This is important only if your custom entity can [move](../movements/index.md).

The [ground](../map.md#mapget_groundx-y-layer) is the terrain property of the [map](../map.md). It is defined by [tiles](./tile.md) and by other entities that may change it dynamically.

`ground` (string)
: A kind of ground. See [`map:get_ground()`](../map.md#mapget_groundx-y-layer) for the possible values.

Return value (boolean)
: `true` if your custom entity can traverse this kind of ground.

### `custom_entity:set_can_traverse_ground(ground, traversable)`

Sets whether this custom entity can traverse a kind of ground.

This is important only if your custom entity can [move](../movements/index.md).

The [ground](../map.md#mapget_groundx-y-layer) is the terrain property of the [map](../map.md). It is defined by [tiles](./tile.md) and by other entities that may change it dynamically.

By default, this depends on the the ground: for example, the `"grass"` ground can be traversed by default while the `"low` wall" ground cannot.

`ground` (string)
: A kind of ground. See [`map:get_ground()`](../map.md#mapget_groundx-y-layer) for the possible values.

`traversable` (boolean)
: Whether your custom entity can traverse this kind of ground.

### `custom_entity:add_collision_test(collision_mode, callback)`

Registers a function to be called when your custom entity detects a collision when another entity.

`collision_mode` (string or function)
: Specifies what kind of collision you want to test. This may be one of:

    - `"overlapping"`: Collision if the [bounding box](./index.md#entityget_bounding_box) of both entities overlap. This is often used when the other entity can traverse your custom entity.

    - `"containing"`: Collision if the bounding box of the other entity is fully inside the bounding box of your custom entity.

    - `"origin"`: Collision if the [origin point](./index.md#entityget_origin) or the other entity is inside the bounding box of your custom entity.

    - `"center"`: Collision if the [center point](./index.md#entityget_center_position) of the other entity is inside the bounding box of your custom entity.

    - `"facing"`: Collision if the [facing position](./index.md#entityget_facing_position) of the other entity's bounding box is touching your custom entity's bounding box. Bounding boxes don't necessarily overlap, but they are in contact: there is no space between them. When you consider the bounding box of an entity, which is a rectangle with four sides, the facing point is the middle point of the side the entity is oriented to. This `"facing"` collision test is useful when the other entity cannot traverse your custom entity. For instance, if the other entity has direction "east", there is a collision if the middle of the east side of its bounding box touches (but does not necessarily overlap) your custom entity's bounding box. This is very often what you need, typically to let the hero interact with your entity when he is looking at it.

    - `"touching"`: Like `"facing"`, but accepts all four sides of the other entity's bounding box, no matter its direction.

    - `"sprite"`: Collision if a sprite of the other entity overlaps a sprite of your custom entity. The collision test is pixel precise.

    - A function: Custom collision test. The function takes your custom entity and then the other entity as parameters and should return `true` if there is a collision between them. This function will be called every time the engine needs to check collisions between your custom entity and any other entity.

`callback` (function)
: A function that will be called when the collision test detects a collision with another entity. This allows you to decide dynamically. This function takes your custom entity and then the other entity as parameters.

    If the collision test was `"sprite"`, both involved sprites are also passed as third and fourth parameters: the third parameter is the sprite of your custom entity, and the fourth parameter is the sprite of the other entity. This may be useful when your entities have several sprites, otherwise you can just ignore these additional sprite parameters.

!!! note "Note"

    See also [`entity:overlaps()`](./index.md#entityoverlapsx-y-width-height-entityoverlapsother_entity-collision_mode-entity_sprite-other_entity_sprite) to directly test a collision rather than registering a callback.

### `custom_entity:clear_collision_tests()`

Disables any collision test previously registered with [`custom_entity:add_collision_test()`](#custom_entityadd_collision_testcollision_mode-callback).

### `custom_entity:has_layer_independent_collisions()`

Returns whether this custom entity can detect collisions with entities even if they are not on the same layer.

By default, custom entities can only have collisions with entities on the same layer.

Return value (boolean)
: `true` if this entity can detect collisions even with entities on other layers.

### `custom_entity:set_layer_independent_collisions([independent])`

Sets whether this custom entity can detect collisions with entities even if they are not on the same layer.

By default, custom entities can only have collisions with entities on the same layer. If you set this property to `true`, the [collision tests](#custom_entityadd_collision_testcollision_mode-callback) will be performed even with entities that are on a different layer.

`independent` (boolean, optional, default: `true`)
: `true` to make this entity detect collisions even with entities on other layers.

### `custom_entity:get_modified_ground()`

Returns the kind of [ground](../map.md#mapget_groundx-y-layer) (terrain) defined by this custom entity on the map.

Return value (string or `nil`)
: The ground defined by this custom entity, or `nil` if this custom entity does not modify the ground. See [`map:get_ground()`](../map.md#mapget_groundx-y-layer) for the list of possible grounds.

### `custom_entity:set_modified_ground(modified_ground)`

Sets the kind of [ground](../map.md#mapget_groundx-y-layer) (terrain) defined by this custom entity on the map.

The ground of the map is normally defined by tiles, but other entities may modify it dynamically.

This property allows you to make a custom entity that modifies the ground of the map, for example a hole with a special sprite or ice with particular [collision callbacks](#custom_entityadd_collision_testcollision_mode-callback). The modified ground will be applied on the map in the rectangle of this custom entity's [bounding box](./index.md#entityget_bounding_box). Your custom entity can move: the ground will still be correctly applied.

`modified_ground` (string or `nil`)
: The ground defined by this custom entity, or `nil` (or `"empty"`) to make this custom entity stop modifying the ground. See [`map:get_ground()`](../map.md#mapget_groundx-y-layer) for the list of possible grounds.

!!! note "Note"

    If you only need to modify the ground of the map dynamically, for example to make a moving platform over holes, a [dynamic tile](./dynamic-tile.md) with a [movement](../movements/index.md) may be enough.

### `custom_entity:get_follow_streams()`

Returns whether this custom entity follows [streams](./stream.md).

By default, custom entities are not affected by streams and ignore them.

Return value (boolean)
: `true` if this custom entity follows streams, `false` if it ignores them.

### `custom_entity:set_follow_streams([follow_streams])`

Sets whether this custom entity should follow [streams](./stream.md).

By default, custom entities are not affected by streams and ignore them.

`follow_streams` (boolean, optional, default: `true`)
: `true` to make this custom entity follow streams, `false` to ignore them.

## Events Inherited from `entity`

Events are callback methods automatically called by the engine if you define them.

Custom entities are particular [map entities](./index.md). Therefore, they inherit all events from the type map entity.

See [entity](./index.md#events-of-all-entity-types) to know these events.

## Events of the type custom entity

The following events are specific to custom entities.

### `custom_entity:on_update()`

Called at each cycle while this custom entity lives on the map.

!!! note "Note"

    As this function is called at each cycle, it is recommended to use other solutions when possible, like [timers](../timers.md) and other events.

### `custom_entity:on_ground_below_changed(ground_below)`

Called when the kind of [ground](../map.md#mapget_groundx-y-layer) on the map below this custom entity has changed. It may change because this custom entity is moving, or when because another entity changes it.

`ground_below` (string or `nil`)
: The kind of ground at the [ground point](./index.md#entityget_ground_position) of this custom entity. `nil` means empty, that is, there is no ground at this point on the current layer.

### `custom_entity:on_interaction(hero)`

Called when the [hero](./hero.md) interacts with this custom entity, that is, when the player presses the [action command](../game.md#game-commands) while facing this custom entity.

`hero` ([hero](./hero.md))
: The hero who is triggering the interaction.

!!! note "Note"

    This event is also available with [NPCs](./npc.md#npcon_interaction).

### `custom_entity:on_interaction_item(item_used)`

Called when the [hero](./hero.md) uses any [equipment item](../equipment-items.md) (the player pressed an [item command](../game.md#game-commands)) while facing this custom entity.

`item_used` ([item](../equipment-items.md))
: The item currently used by the player.

Return value (boolean)
: `true` if an interaction happened. If you return `false` or nothing, then [`item_used:on_using()`](../equipment-items.md#itemon_usinghero) will be called (just like if there was no custom entity in front of the hero).

!!! note "Note"

    This event is also available with [NPCs](./npc.md#npcon_interaction_itemitem_used).
