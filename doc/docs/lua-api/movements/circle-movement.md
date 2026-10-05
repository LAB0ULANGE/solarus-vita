# Circle Movement

A circle movement makes a circular trajectory around a center point.

The center can either be some fixed coordinates or a possibly moving [map entity](../map-entities/index.md).

## Methods Inherited from `movement`

Circle movements are particular [movement](./index.md) objects. Therefore, they inherit all methods from the type movement.

See [movement](./index.md#methods-of-all-movement-types) to know these methods.

## Methods of the type `circle_movement`

The following methods are specific to circle movements.

### `circle_movement:get_center()`

Returns the center point of this movement.

Return value 1 (number)
: X coordinate of the center.

Return value 2 (number)
: Y coordinate of the center.

### `circle_movement:set_center(x, y), circle_movement:set_center(entity, [dx, dy])`

Sets the center of this movement as a fixed point or as a [map entity](../map-entities/index.md) (only during a [game](../game.md)).

| With a fixed point | With a [map entity](../map-entities/index.md) |
|--------------------|-----------------------------------------------|
| <dl><dt>`x` (number)</dt><dd>X coordinate of the center.</dd><dt>`y` (number)</dt><dd>Y coordinate of the center.</dd></dl> | <dl><dt>`entity` ([entity](../map-entities/index.md))</dt><dd>The center entity.</dd><dt>`dx` (number, optional, requires: `dy`, default: `0`)</dt><dd>X offset to add to the center entity's coordinates.</dd><dt>`dy` (number, optional, requires: `dx`, default: `0`)</dt><dd>Y offset to add to the center entity's coordinates.</dd></dl>|

### `circle_movement:get_radius()`

Returns the radius of circles to make.

If [`circle_movement:get_radius_speed()`](#circle_movementget_radius_speed) is not `0`, radius changes are made gradually.

Return value (number)
: The wanted radius in pixels.

### `circle_movement:set_radius(radius)`

Sets the radius of circles to make.

If [`circle_movement:get_radius_speed()`](#circle_movementget_radius_speed) is not `0`, the radius will be updated gradually.

`radius` (number)
: The new wanted radius in pixels.

### `circle_movement:get_radius_speed()`

Returns the speed of radius changes.

Return value (number)
: The speed in pixels per second, or `0` if radius changes are immediate.

### `circle_movement:set_radius_speed(radius_speed)`

Sets the radius to be updated immediately of gradually (at the specified speed) towards its wanted value.

`radius_speed` (number)
: The speed of radius changes in pixels per second, or `0` to make radius changes immediate.

### `circle_movement:is_clockwise()`

Returns whether circles are made clockwise or counter-clockwise.

Return value (boolean)
: `true` if circles are clockwise.

### `circle_movement:set_clockwise([clockwise])`

Sets whether circles are made clockwise or counter-clockwise.

`clockwise` (boolean, optional, default: `true`)
: `true` to make circles clockwise.

### `circle_movement:get_angle_from_center()`

Returns the angle indicating the current position on the circle.

This is the angle of the vector between the center and the current position.

Return value (number)
: The angle in radians.

### `circle_movement:set_angle_from_center(angle_from_center)`

Sets the position on the circle as an angle from its center.

This is the angle of the vector between the center and the new position to set.

`initial_angle` (number)
: The angle in radians.

### `circle_movement:get_angular_speed()`

Returns the speed of the angle variation.

Return value (number)
: The angle speed in radians per second.

### `circle_movement:set_angular_speed(angular_speed)`

Sets the speed of the angle variation.

`angular_speed` (number)
: The new angle speed in radians per second.

### `circle_movement:get_max_rotations()`

Returns the maximum number of rotations of this movement.

When this number of rotations is reached, the movement stops.

Return value (number)
: The maximum number of rotations to make (`0` means ∞).

### `circle_movement:set_max_rotations(max_rotations)`

Sets the maximum number of rotations of this movement.

When this number of rotations is reached, the movement stops.

`max_rotations` (number)
: The maximum number of rotations to make (`0` means ∞).

!!! note "Note"

    The movement stops itself by setting its radius to 0.

    Therefore, if the radius is set to change gradually (see [`circle_movement:get_radius_speed()`](#circle_movementget_radius_speed)), the movement will continue for a while until the radius reaches 0.

!!! note "Note"

    When the movement has stopped, it restarts later if it was set to loop (see [`circle_movement:get_loop_delay()`](#circle_movementget_loop_delay)), and again, possibly gradually.

### `circle_movement:get_duration()`

Returns the maximum duration of this movement.

When this delay is reached, the movement stops.

Return value (number)
: The duration of the movement in milliseconds (`0` means infinite).

### `circle_movement:set_duration(duration)`

Sets the maximum duration of this movement.

When this delay is reached, the movement stops.

`duration` (number)
: The duration of the movement in milliseconds (`0` means infinite).

!!! note "Note"

    The movement is stopped by automatically setting its radius to `0`.

    Therefore, if the radius is set to change gradually (see [`circle_movement:get_radius_speed()`](#circle_movementget_radius_speed)), the movement will continue for a while until the radius reaches `0`.

!!! note "Note"

    When the movement has stopped, it will then restart if it was set to loop (see [`circle_movement:set_loop_delay()`](#circle_movementset_loop_delayloop_delay)), and again, possibly gradually.

### `circle_movement:get_loop_delay()`

Returns the delay after which this movement restarts.

Return value (number)
: The restart delay in milliseconds (`0` means no restart).

### `circle_movement:set_loop_delay(loop_delay)`

Sets the delay after which this movement restarts.

`loop_delay` (number)
: The restart delay in milliseconds (`0` means no restart).

!!! note "Note"

    This delay is applied if the movement get stopped by reaching the [maximum number of rotations](#circle_movementget_max_rotations) or the [maximum duration](#circle_movementget_duration).

!!! note "Note"

    When the movement restarts, the radius starts from 0 and gets back to its previous value, possibly gradually (see [`circle_movement:set_radius_speed()`](#circle_movementset_radius_speedradius_speed)).

## Events Inherited from `movement`

Circle movements are particular [movement](./index.md) objects. Therefore, they inherit all events from the type movement.

See [movement](./index.md#events-of-all-movement-types) to know these events.

## Deprecated methods of the type circle movement

The following methods are deprecated and may be removed it future releases.

### `circle_movement:get_initial_angle()`

Returns the angle from where the first circle starts.

Return value (number)
: The initial angle in degrees.

!!! warning "Deprecated"

    This method is deprecated since Solarus 1.6. Angle functions now all use radians for consistency.

    Use [`circle_movement:get_angle_from_center()`](#circle_movementget_angle_from_center) instead.

### `circle_movement:set_initial_angle(initial_angle)`

Sets the angle from where the first circle should start.

`initial_angle` (number)
: The initial angle in degrees.

!!! warning "Deprecated"

    This method is deprecated since Solarus 1.6. Angle functions now all use radians for consistency.

    Use [`circle_movement:set_angle_from_center()`](#circle_movementset_angle_from_centerangle_from_center) instead.

### `circle_movement:get_angle_speed()`

Returns the speed of the angle variation.

Return value (number)
: The angle speed in degrees per second.

!!! warning "Deprecated"

    This method is deprecated since Solarus 1.6. Angle functions now all use radians for consistency.

    Use [`circle_movement:get_angular_speed()`](#circle_movementget_angular_speed) instead.

### `circle_movement:set_angle_speed(angle_speed)`

Sets the speed of the angle variation.

`angle_speed` (number)
: The new angle speed in degrees per second.

!!! warning "Deprecated"

    This method is deprecated since Solarus 1.6. Angle functions now all use radians for consistency.

    Use [`circle_movement:set_angular_speed()`](#circle_movementset_angular_speedangular_speed) instead.
