# Target Movement

A target movement goes towards a target point. The target point can be a fixed point of an [entity](../map-entities/index.md) of the [map](../map.md). If the target is a moving map entity, the movement updates its angle to continue to go towards the entity. By default, the target is the [hero](../map-entities/hero.md) when a [game](../game.md) is running.

This type of movement can be seen as a particular case of the [straight movement](./straight-movement.md) type, where the angle is set automatically to go towards the target.

!!! note "Note"

    This type of movement goes straight towards the target.

    If you set the `smooth` property to `true`, it will try to avoid simple obstacles by moving to a side. This is usually enough for simple enemies that target the hero. If you want a more complex technique that calculates an intelligent path to the target, see the [path finding movement](./path-finding-movement.md) type.

## Methods Inherited from `movement`

Target movements are particular [movement](./index.md) objects. Therefore, they inherit all methods from the type movement.

See [movement](./index.md#methods-of-all-movement-types) to know these methods.

## Methods of the type `target_movement`

The following methods are specific to target movements.

### `target_movement:set_target(x, y), target_movement:set_target(entity, [x, y])`

Sets the target of this movement as a fixed point or a [map entity](../map-entities/index.md) (only during a [game](../game.md)).

| With coordinates | With a [map entity](../map-entities/index.md) |
|------------------|------------------------------|
| <dl><dt>`x` (number)</dt><dd>X coordinate of the target.</dd><dt>`y` (number)</dt><dd>Y coordinate of the target.</dd></dl> | <dl><dt>`entity` ([entity](../map-entities/index.md))</dt><dd>The entity to target.</dd><dt>`x` (number, optional, requires: `y`, default: `0`)</dt><dd>X offset to add to the target entity's coordinates.</dd><dt>`y` (number, optional, requires: `x`, default: `0`)</dt><dd>Y offset to add to the target entity's coordinates.</dd></dl>|

### `target_movement:get_speed()`

Returns the speed of this movement.

Return value (number)
: The speed in pixels per second.

### `target_movement:set_speed(speed)`

Sets the speed of this movement.

`speed` (number)
: The new speed in pixels per second.

### `target_movement:get_angle()`

Returns the angle of the trajectory in radians.

East is `0`, North is `math.pi / 2`, West is `math.pi`, South is `3 * math.pi / 2` and any intermediate value is possible.

Return value (number)
: The angle in radians.

!!! note "Note"

    If you prefer a value in a 4-direction system, see [`movement:get_direction4()`](./index.md#movementget_direction4).

### `target_movement:is_smooth()`

Returns whether this movement adjusts its trajectory when an obstacle of the [map](../map.md) is reached. This property has no effect if the movement is not attached to a [map entity](../map-entities/index.md) or if the movement ignores obstacles.

Return value (boolean)
: `true` if this movement is smooth.

### `target_movement:set_smooth([smooth])`

Sets whether this movement should adjust its trajectory when an obstacle of the [map](../map.md) is reached. This property has no effect if the movement is not attached to a [map entity](../map-entities/index.md) or if the movement ignores obstacles.

`smooth` (boolean, optional, default: `true`)
: `true` to make this movement smooth.

## Events Inherited from `movement`

Target movements are particular [movement](./index.md) objects. Therefore, they inherit all events from the type movement.

See [movement](./index.md#events-of-all-movement-types) to know these events.
