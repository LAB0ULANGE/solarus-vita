# Path Finding Movement

A path finding movement is a particular [path movement](./path-movement.md) where the path is calculated to reach a target. The target is a [map entity](../map-entities/index.md) (by default the [hero](../map-entities/hero.md)).

The movement calculates repeatedly the shortest path towards the target entity, taking into account obstacles of the [map](../map.md). With this type of movement, an entity is capable of finding its way in a maze.

!!! note "Note"

    This type of movement computes a precise path on the map grid and avoids complex obstacles by using a sophisticated A.I. algorithm ([A\*](http://en.wikipedia.org/wiki/A*_search_algorithm)).

    If you just need to go straight towards a target, which may be more natural for basic enemies, see the [target movement](./target-movement.md) type.

## Methods Inherited from `movement`

Path finding movements are particular [movement](./index.md) objects. Therefore, they inherit all methods from the type movement.

See [movement](./index.md#methods-of-all-movement-types) to know these methods.

## Methods of the type `path_finding_movement`

The following methods are specific to path finding movements.

### `path_finding_movement:set_target(entity)`

Sets the target entity of this movement.

`entity` ([entity](../map-entities/index.md))
: The entity to target.

### `path_finding_movement:get_speed()`

Returns the speed of this movement.

Return value (number)
: The speed in pixels per second.

### `path_finding_movement:set_speed(speed)`

Sets the speed of this movement.

`speed` (number)
: The new speed in pixels per second.

### `path_finding_movement:get_angle()`

Returns the angle of the current trajectory in radians.

East is `0`, North is `math.pi / 2`, West is `math.pi`, South is `3 * math.pi / 2`.

Return value (number)
: The angle in radians.

!!! note "Note"

    If you prefer a value in a 4-direction system, see [`movement:get_direction4()`](./index.md#movementget_direction4).

## Events Inherited from `movement`

Path finding movements are particular [movement](./index.md) objects. Therefore, they inherit all events from the type movement.

See [movement](./index.md#events-of-all-movement-types) to know these events.
