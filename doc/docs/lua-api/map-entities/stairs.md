# Stairs

## Overview

Stairs make fancy animations, movements and sounds when the [hero](./hero.md) takes them.

This type of [map entity](./index.md) can be declared in the [map data file](../map.md#map-files). It can also be created dynamically with [`map:create_stairs()`](../map.md#mapcreate_stairsproperties).

!!! note "Note"

    Stairs entities provide very specific behavior for historical reasons and are not very customizable. If you need more flexibility, we recommend to use [custom entities](./custom-entity.md) instead and to script the behavior you want.

Stairs provide nice transitions between two [maps](../map.md) or two [layers](../map.md#coordinates-and-layer) on the same map.

They don't bring any fundamental feature: you can already use [teletransporters](./teletransporter.md) and [destinations](./destination.md) to make the [hero](./hero.md) go to another place. And you can use [sensors](./sensor.md) to make him go from a layer to another on the same map. But they add fanciness to these transitions: the sound of climbing or going down stairs, a special movement and appropriate sprite animations.

For stairs that teleport the hero to a different place, you still have to use a [teletransporter](./teletransporter.md) and a [destination](./destination.md) (stairs don't replace teletransporters and destinations). You just have put all three entities (the stairs, a teletransporter and a destination) at the same coordinates and the engine will activate the teletransporter when the stairs animation ends.

Stairs that move from a layer to another layer inside the same map are called inner stairs and automatically change the layer of the hero. They must be placed on the lowest one of those two layers. Their direction property corresponds to the direction when going upwards.

Stairs are invisible: it is up to you to place appropriate tiles where you make stairs.

## Methods Inherited from `entity`

Stairs are particular [map entities](./index.md). Therefore, they inherit all methods from the type map entity.

See [entity](./index.md#methods-of-all-entity-types) to know these methods.

## Methods of the type `stairs`

The following methods are specific to stairs.

### `stairs:get_direction()`

Returns the direction of these stairs.

Stairs can be taken in two opposite directions. The one returned here is the main one, which is: for stairs that teleport the hero, the direction when leaving the place using the stairs (and not the one when arriving from the stairs), and for inner stairs, the direction when going upwards.

Return value (number)
: The stairs direction between `0` (East) and `3` (South).

### `stairs:is_inner()`

Returns whether these stairs go from a layer to another layer on the same map or go to another place.

Return value (boolean)
: `true` if the stairs entity is considered inner.

## Events inherited from map entity

Events are callback methods automatically called by the engine if you define them.

Stairs are particular [map entities](./index.md). Therefore, they inherit all events from the type map entity.

See [entity](./index.md#events-of-all-entity-types) to know these events.

## Events of the type stairs

The following events are specific to stairs.

### `stairs:on_entered()`

Called when the [hero](./hero.md) has just entered these stairs.

### `stairs:on_exited()`

Called when the [hero](./hero.md) has just exited these stairs.
