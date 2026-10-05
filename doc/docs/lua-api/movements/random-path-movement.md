# Random Path Movement

The random path movement is a particular case of [path movement](./path-movement.md), where the path is chosen automatically. The resulting movement is composed of repeated random steps in the four main directions only and with a length that is a multiple of 8 pixels.

!!! note "Note"

    This movement is a typical random walk movement. You will probably use it for [NPCs](../map-entities/npc.md).

    Example of use:

    ```lua
    -- Example of code from a map script.
    -- Assume that there is an NPC called "bob" on this map.
    function sol.map:on_started()
      -- The map has just started: make bob walk.
      bob:start_movement(sol.movement.create("random_path"))
    end
    ```

## Methods Inherited from `movement`

Random path movements are particular [movement](./index.md) objects. Therefore, they inherit all methods from the type movement.

See [movement](./index.md#methods-of-all-movement-types) to know these methods.

## Methods of the type `random_path_movement`

The following methods are specific to random path movements.

### `random_path_movement:get_speed()`

Returns the speed of this movement.

Return value (number)
: The speed in pixels per second.

### `random_path_movement:set_speed(speed)`

Sets the speed of this movement.

`speed` (number)
: The new speed in pixels per second.

### `random_path_movement:get_angle()`

Returns the angle of the current trajectory in radians.

East is `0`, North is `math.pi / 2`, West is `math.pi`, South is `3 * math.pi / 2`.

Return value (number)
: The angle in radians.

!!! note "Note"

    If you prefer a value in a 4-direction system, see [`movement:get_direction4()`](./index.md#movementget_direction4).

## Events Inherited from `movement`

Random path movements are particular [movement](./index.md) objects. Therefore, they inherit all events from the type movement.

See [movement](./index.md#events-of-all-movement-types) to know these events.
