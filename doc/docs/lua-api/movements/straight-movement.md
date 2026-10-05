# Straight Movement

A straight movement follows a rectilinear trajectory. You can define the trajectory as an angle and a speed.

## Methods Inherited from `movement`

Straight movements are particular [movement](./index.md) objects. Therefore, they inherit all methods from the type movement.

See [movement](./index.md#methods-of-all-movement-types) to know these methods.

## Methods of the type `straight_movement`

The following methods are specific to straight movements.

### `straight_movement:get_speed()`

Returns the speed of this movement.

Return value (number)
: The speed in pixels per second.

### `straight_movement:set_speed(speed)`

Sets the speed of this movement.

`speed` (number)
: The new speed in pixels per second.

### `straight_movement:get_angle()`

Returns the angle of the trajectory in radians.

East is `0`, North is `math.pi / 2`, West is `math.pi`, South is `3 * math.pi / 2` and any intermediate value is possible.

Return value (number)
: The angle in radians.

!!! note "Note"

    If you prefer a value in a 4-direction system, see [`movement:get_direction4()`](./index.md#movementget_direction4).

### `straight_movement:set_angle(angle)`

Sets the angle of the trajectory in radians.

East is `0`, North is `math.pi / 2`, West is `math.pi`, South is `3 * math.pi / 2` and any intermediate value is possible. Negative values and values greater to `2 * math.pi` are also accepted.

`angle` (number)
: The new angle in radians.

### `straight_movement:get_max_distance()`

Returns the maximum distance of this movement.

The movement will stop when this distance is reached.

Return value (number)
: The maximum distance in pixels (`0` means no limit).

### `straight_movement:set_max_distance(max_distance)`

Sets the maximum distance of this movement.

The movement will stop when this distance is reached.

`max_distance` (number)
: The maximum distance in pixels (`0` means no limit).

### `straight_movement:is_smooth()`

Returns whether this movement adjusts its trajectory when an obstacle of the [map](../map.md) is reached. This property has no effect if the movement is not attached to a [map entity](../map-entities/index.md) or if the movement ignores obstacles.

Return value (boolean)
: `true` if this movement is smooth.

### `straight_movement:set_smooth([smooth])`

Sets whether this movement should adjust its trajectory when an obstacle of the [map](../map.md) is reached. This property has no effect if the movement is not attached to a [map entity](../map-entities/index.md) or if the movement ignores obstacles.

`smooth` (boolean, optional, default: `true`)
: `true` to make this movement smooth.

## Events Inherited from `movement`

Straight movements are particular [movement](./index.md) objects. Therefore, they inherit all events from the type movement.

See [movement](./index.md#events-of-all-movement-types) to know these events.
