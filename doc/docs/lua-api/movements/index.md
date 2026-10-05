# Movements

If you need to move an [enemy](../map-entities/enemy.md) of the map, a [sprite](../drawable-objects/sprite.md) in a menu or simply an arbitrary point, you can create a movement object and set its properties.

There are several types of movements. They differ by the kind of trajectory they can make. When you create a movement, you obtain a value of the movement type you chose. Then, to get and set its properties (like the speed, the angle, etc.), a movement object has several methods available.

As detailed below, the methods available differ depending on the movement type because all movement types don't have the same properties.

The following movement types are available.

- [Straight Movement](./straight-movement.md): Rectilinear trajectory in any direction.
- [Random Movement](./random-movement.md): A straight movement whose direction changes randomly from time to time.
- [Target Movement](./target-movement.md): Straight trajectory towards a possibly moving target.
- [Path Movement](./path-movement.md): Predetermined path composed of steps in the 8 main directions.
- [Random Path Movement](./random-path-movement.md): Like a path movement, but with random steps.
- [Path Finding Movement](./path-finding-movement.md): Like a path movement, but calculated to reach a possibly moving target.
- [Circle Movement](./circle-movement.md): Circular trajectory around a possibly moving center.
- [Jump Movement](./jump-movement.md): An illusion of jump above a baseline.
- [Pixel Movement](./pixel-movement.md): A trajectory described pixel by pixel.

This page desribes the methods and callbacks common to all movement types.

Movements can be applied in-game to [map entities](../map-entities/index.md), but also outside a game, typically in a [menu](../menus.md) to move a [sprite](../drawable-objects/sprite.md), an [image](../drawable-objects/surface.md) or just an `(x,y)` value. However, some properties of movements (like [`movement:set_ignore_obstacles()`](#movementset_ignore_obstaclesignore_obstacles)) only take effect in the case of a [map entity](../map-entities/index.md) because they refer to [map-specific](../map.md) notions like obstacles.

## Functions of `sol.movement`

### `sol.movement.create(movement_type)`

Creates a movement.

Depending on the movement type, several methods are then available to get and set its properties.

`movement_type` (string)
: Type of movement to create. Must be one of:

    - `"straight"`: Follows a rectilinear trajectory.
    - `"random"`: Like "straight" but with random, changing angles.
    - `"target"`: Like "straight" but goes into the direction of a fixed point or a moving entity.
    - `"path"`: Follows a specified succession of basic moves on an 8x8 pixels grid.
    - `"random_path"`: Like "path" but computes the path randomly.
    - `"path_finding"`: Like "path" but computes the shortest path to an entity, avoiding obstacles of the map (only possible in game).
    - `"circle"`: Follows a circular trajectory around a center.
    - `"jump"`: Makes a jump above a rectilinear trajectory.
    - `"pixel"`: Follows a trajectory specified pixel by pixel.

Return value (movement)
: The movement created. See the sections below to know the get and set methods available for your movement type.

## Methods of all movement types

These methods exist in all movement types.

### `movement:get_type()`

Returns the type of movement.

Return value (string)
: The type of this movement. Can be one of:

    - `"straight_movement"`
    - `"random_movement"`
    - `"target_movement"`
    - `"path_movement"`
    - `"random_path_movement"`
    - `"path_finding_movement"`
    - `"circle_movement"`
    - `"jump_movement"`
    - `"pixel_movement"`

### `movement:start(object_to_move, [callback])`

Starts this movement on an object.

The movement will be applied until it finishes (if it has an end) or until it is replaced by another one. It does not matter if the movement gets out of scope in your Lua script.

`object_to_move` ([map entity](../map-entities/index.md), [drawable object](../drawable-objects/index.md) or table)
: The object to move. It may be a map entity, a drawable object or a table with two fields `x` and `y` In the case of the table, if the fields `x` and `y` don't exist, they are created and initialized to `0`. An empty table will be initialized with `{x = 0, y = 0}`.

`callback` (function, optional)
: A function to call when the movement finishes.

!!! note "Note"

    The [hero](../map-entities/hero.md) is a [map entity](../map-entities/index.md) just like any other. So you can apply a custom movement to him using this function. The usual way to do this is to call [`hero:freeze()`](../map-entities/hero.md#herofreeze) first to properly remove control from the player, and then to start the movement. When you have finished, you can restore the control with [`hero:unfreeze()`](../map-entities/hero.md#herounfreeze).

    Indeed, changing the movement while the hero is in a state other than `"frozen"` might give surprising results. Your movement will be applied, replacing any built-in movement of the state, but whatever was happening in the state will still continue. Furthermore, your movement will disappear as soon as the state changes. So don't start a movement on the hero during an arbitrary state unless you know what you are doing.

### `movement:stop()`

Stops this movement and detaches it from the object that was moved.

### `movement:get_xy()`

Returns the coordinates of the object controlled by this movement.

The object controlled by this movement may be a [map entity](../map-entities/index.md), a [drawable object](../drawable-objects/index.md) or a point.

Return value 1 (number)
: X coordinate.

Return value 2 (number)
: Y coordinate.

### `movement:set_xy(x, y)`

Sets the coordinates of the object controlled by this movement.

The object controlled by this movement may be a [map entity](../map-entities/index.md), a [drawable object](../drawable-objects/index.md) or a point.

`x` (number)
: X coordinate to set.

`y` (number)
: Y coordinate to set.

### `movement:is_suspended()`

Returns whether this movement is currently suspended.

Return value (boolean)
: `true` if the movement is suspended.

### `movement:get_ignore_suspend()`

Returns whether the movement should continue even when the [game](../game.md) is suspended.

This setting only has an effect for movements attached to a [map entity](../map-entities/index.md) during a game. The default value is `false`

Return value (boolean)
: `true` to continue the movement even when the game is suspended.

### `movement:set_ignore_suspend([ignore])`

Sets whether the movement should continue even when the [game](../game.md) is suspended.

`ignore` (boolean, optional, default: `true`)
: `true` to continue the movement even when the game is suspended.

### `movement:get_ignore_obstacles()`

Returns whether this movement ignores obstacles of the map.

If the movement is not attached to a [map entity](../map-entities/index.md) yet, it is not an error to call this function: the result will have an effect when the movement gets attached to a map entity.

Return value (boolean)
: `true` if this movement ignores obstacles.

### `movement:set_ignore_obstacles([ignore_obstacles])`

Sets whether a map entity controlled by this movement should ignore obstacles of the map.

If the movement is not attached to a [map entity](../map-entities/index.md) yet, it is not an error to call this function: your choice will have an effect when the movement gets attached to a map entity.

`ignore_obstacles` (boolean, optional, default: `true`)
: `true` to make this movement ignore obstacles of the map.

### `movement:get_direction4()`

From the four main directions, returns the closest one to the current trajectory.

East is `0`, North is `1`, West is `2`, South is `3`. As the real trajectory does not necessarily follows one of the four main directions, it will be converted to the closest one.

If you use this movement to control a [sprite](../drawable-objects/sprite.md) (or a [map entity](../map-entities/index.md) that has a sprite), you can use this function to make the sprite face the direction of the movement.

Return value (number)
: The closest direction corresponding to the angle of this movement.

Example of use:

```lua
-- Example of code from an enemy script.

-- This function is called when the enemy should start or restart its movement.
function enemy:on_restarted()

  -- Create a movement that makes random straight trajectories.
  local movement = sol.movement.create("random")

  -- This function is called when the trajectory has changed.
  function movement:on_movement_changed()
    -- The angle of the movement has changed: update the sprite accordingly.
    local direction = movement:get_direction4()
    enemy:get_sprite():set_direction(direction)
  end

  movement:start(enemy)
end
```

## Events of all movement types

Events are callback methods automatically called by the engine if you define them.

The following events are common to all movement types.

!!! note "Note"

    All movement events are here (it turns out that no specific movement type define additional events).

### `movement:on_position_changed()`

Called when the coordinates controlled by this movement have just changed.

### `movement:on_obstacle_reached()`

During a [game](../game.md), called when the coordinates controlled by this movement have just failed to change because they would lead the [map entity](../map-entities/index.md) controlled into an obstacle of the [map](../map.md).

When an obstacle is reached, this event is called instead of [`movement:on_position_changed()`](#movementon_position_changed).

This event can only be called when all of these conditions are met:

- A [game](../game.md) is currently running.
- The movement is attached to a [map entity](../map-entities/index.md) (like an [enemy](../map-entities/enemy.md), an [NPC](../map-entities/npc.md), etc.).
- The movement does not ignore obstacles (i.e. [`movement:get_ignore_obstacles()`](#movementget_ignore_obstacles) returns `false`).

!!! note "Note"

    When the movement attempts to change the coordinates, one of [`movement:on_position_changed()`](#movementon_position_changed) or [`movement:on_obstacle_reached()`](#movementon_obstacle_reached) is guaranteed to be called.

### `movement:on_changed()`

Called when the characteristics of this movement (like speed or angle) have just changed.

### `movement:on_finished()`

Called when this movement has just finished (if there is an end).
