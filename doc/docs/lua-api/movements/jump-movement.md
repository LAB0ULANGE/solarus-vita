# Jump Movement

This type of movement makes a jump above a specified rectilinear trajectory. We call this rectilinear trajectory the baseline. To use a jump movement, you typically specify the baseline (direction and distance), and the movement will jump above this baseline. The speed is adjusted automatically depending on the distance, but you can change it if you want.

For now, the baseline can only have one of the 8 main directions. This restriction may be removed in the future.

## Methods Inherited from `movement`

Jump movements are particular [movement](./index.md) objects. Therefore, they inherit all methods from the type movement.

See [movement](./index.md#methods-of-all-movement-types) to know these methods.

## Methods of the type `jump_movement`

The following methods are specific to jump movements.

### `jump_movement:get_direction8()`

Returns the direction of the baseline of this jump. The baseline always has one of the 8 main directions. `0` is East, `1` is North-East, etc.

Return value (number)
: The direction (`0` to `7`).

### `jump_movement:set_direction8(direction8)`

Sets the direction of the baseline of this jump. The baseline always has one of the 8 main directions. `0` is East, `1` is North-East, etc.

`direction8` (number)
: The direction (`0` to `7`).

### `jump_movement:get_distance()`

Returns the distance of the baseline of this jump.

Return value (number)
: The distance of the jump in pixels.

### `jump_movement:set_distance(distance)`

Sets the distance of the baseline of this jump.

`distance` (number)
: The new distance of the jump in pixels.

### `jump_movement:get_speed()`

Returns the speed of this movement.

Return value (number)
: The speed in pixels per second.

### `jump_movement:set_speed(speed)`

Sets the speed of this movement.

`speed` (number)
: The new speed in pixels per second. `0` means to set automatically a speed based on the distance of the jump.

## Events Inherited from `movement`

Jump movements are particular [movement](./index.md) objects. Therefore, they inherit all events from the type movement.

See [movement](./index.md#events-of-all-movement-types) to know these events.
