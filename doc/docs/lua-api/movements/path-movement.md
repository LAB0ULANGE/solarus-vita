# Path Movement

A path movement follows a specified path on an 8×8 pixels grid, in an 8-direction system. A path is a succession of steps of 8 pixels in one of the 8 main directions. You can define each step of the path and make it repeated if you want.

## Methods Inherited from `movement`

Path movements are particular [movement](./index.md) objects. Therefore, they inherit all methods from the type `movement`.

See [movement](./index.md#methods-of-all-movement-types) to know these methods.

## Methods of the type `path_movement`

The following methods are specific to path movements.

### `path_movement:get_path()`

Returns the path of this movement.

Return value (table)
: The path as an array of integers. Each value is a number between `0` and `7` that represents a step (move of 8 pixels) in the path. `0` is East, `1` is North-East, etc.

### `path_movement:set_path(path)`

Sets the path of this movement.

`path` (table)
: The path as an array of integers. Each value is a number between `0` and `7` that represents a step (move of 8 pixels) in the path. `0` is East, `1` is North-East, etc.

Example of use:

```lua
-- Example of code from a map script.
-- Assume that there is an NPC called "scared_cat" on this map.
function scared_cat:on_interaction()
  -- The hero is talking to me: run away!
  sol.audio.play_sound("meow")
  local movement = sol.movement.create("path")
  -- This path is arbitrary, it's just an example (North and then West).
  movement:set_path{2, 2, 2, 2, 2, 2, 4, 4, 4, 4}
  movement:set_speed(80)
  self:start_movement(movement)
end
```

### `path_movement:get_speed()`

Returns the speed of this movement.

Return value (number)
: The speed in pixels per second.

### `path_movement:set_speed(speed)`

Sets the speed of this movement.

`speed` (number)
: The new speed in pixels per second.

### `path_movement:get_loop()`

Returns whether this movement repeats itself once the end of the path is reached.

Return value (boolean)
: `true` if the path repeats itself.

### `path_movement:set_loop([loop])`

Sets whether this movement repeats itself once the end of the path is reached.

`loop` (boolean, optional, default: `true`)
: `true` to make the path repeat itself.

### `path_movement:get_snap_to_grid()`

Returns whether this movement automatically snaps to the [map](../map.md) grid the [map entity](../map-entities/index.md) that it controls.

The map grid is composed of squares of 8×8 pixels. All tiles are aligned to the grid. This property has no effect if there is no current map of if this movement is not attached to a map entity.

Return value (boolean)
: `true` if this movement automatically snaps its map entity to the map grid.

### `path_movement:set_snap_to_grid([snap])`

Sets whether this movement should automatically snap to the [map](../map.md) grid the [map entity](../map-entities/index.md) that it controls.

The map grid is composed of squares of 8×8 pixels. All tiles are aligned to the grid. This property has no effect if there is no current map of if this movement is not attached to a [map entity](../map-entities/index.md).

`snap` (boolean, optional, default: `true`)
: `true` to make this movement automatically snap its map entity to the map grid.

### `path_movement:get_angle()`

Returns the angle of the current trajectory in radians.

East is `0`, North is `math.pi / 2`, West is `math.pi`, South is `3 * math.pi / 2`.

Return value (number)
: The angle in radians.

!!! note "Note"

    If you prefer a value in a 4-direction system, see [`movement:get_direction4()`](./index.md#movementget_direction4).

## Events Inherited from `movement`

Path movements are particular [movement](./index.md) objects. Therefore, they inherit all events from the type movement.

See [movement](./index.md#events-of-all-movement-types) to know these events.
