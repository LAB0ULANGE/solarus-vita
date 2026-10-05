# Random Movement

This type of movement is a rectilinear movement whose trajectory changes randomly over time. It can be seen as a particular case of the [straight movement](./straight-movement.md) type, where the angle is automatically changed after random delays.

## Methods Inherited from `movement`

Random movements are particular [movement](./index.md) objects. Therefore, they inherit all methods from the type movement.

See [movement](./index.md#methods-of-all-movement-types) to know these methods.

## Methods of the type `random_movement`

The following methods are specific to random movements.

### `random_movement:get_speed()`

Returns the speed applied to this movement when it is started.

Return value (number)
: The speed in pixels per second.

### `random_movement:set_speed(speed)`

Sets the speed applied to this movement when it is started.

`speed` (number)
: The new speed in pixels per second.

### `random_movement:get_angle()`

Returns the angle of the current trajectory in radians.

East is `0`, North is `math.pi / 2`, West is `math.pi`, South is `3 * math.pi / 2` and any intermediate value is possible.

Return value (number)
: The angle in radians.

!!! note "Note"

    If you prefer a value in a 4-direction system, see [`movement:get_direction4()`](./index.md#movementget_direction4).

### `random_movement:get_max_distance()`

Returns the maximum distance of this movement.

If the movement goes further than this distance, it automatically comes back towards the initial position.

Return value (number)
: The maximum distance in pixels (`0` means no limit).

### `random_movement:set_max_distance(max_distance)`

Sets the maximum distance of this movement.

If the movement goes further than this distance, it automatically comes back towards the initial position.

`max_distance` (number)
: The maximum distance in pixels (`0` means no limit).

### `random_movement:is_smooth()`

Returns whether this movement adjusts its trajectory when an obstacle of the [map](../map.md) is reached. This property has no effect if the movement is not attached to a [map entity](../map-entities/index.md) or if the movement ignores obstacles.

Return value (boolean)
: `true` if this movement is smooth.

### `random_movement:set_smooth([smooth])`

Sets whether this movement should adjust its trajectory when an obstacle of the [map](../map.md) is reached. This property has no effect if the movement is not attached to a [map entity](../map-entities/index.md) or if the movement ignores obstacles.

`smooth` (boolean, optional, default: `true`)
: `true` to make this movement smooth.

## Events Inherited from `movement`

Random movements are particular [movement](./index.md) objects. Therefore, they inherit all events from the type movement.

See [movement](./index.md#events-of-all-movement-types) to know these events.
