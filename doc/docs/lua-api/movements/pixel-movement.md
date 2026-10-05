# Pixel Movement

A pixel movement makes a succession of basic translations, where each translation is a pixel-precise specified move (for example `(+2,-1)`). Each translation is immediate.

Unlike most other types of movements, there is no notion of speed in pixels per seconds. That's because a translation can transport the object instantly to another place — the movement is not necessarily continuous. Instead, you can set the delay between each translation.

!!! note "Note"

    Pixel movements are not often needed. Most of the time, you don't want to specify pixel-by-pixel trajectories. Higher-level types of movements like [path movement](./path-movement.md) or [target movement](./target-movement.md) usually fit your needs when you move [NPCs](../map-entities/npc.md) or [enemies](../map-entities/enemy.md).

## Methods Inherited from `movement`

Pixel movements are particular [movement](./index.md) objects. Therefore, they inherit all methods from the type movement.

See [movement](./index.md#methods-of-all-movement-types) to know these methods.

## Methods of the type `pixel_movement`

The following methods are specific to pixel movements.

### `pixel_movement:get_trajectory()`

Returns the trajectory of this movement.

Return value (table)
: An array of all successive translations. Each translation is itself an array of two integers (x and y).

### `pixel_movement:set_trajectory(trajectory)`

Sets the trajectory of this movement.

Any previous trajectory is removed and the movement starts at the beginning of the new trajectory.

`trajectory` (table)
: An array of all successive translations to make. Each translation should be an array of two integers (x and y).

### `pixel_movement:get_loop()`

Returns whether this movement restarts automatically when the trajectory is finished.

Return value (boolean)
: `true` if the movement loops.

### `pixel_movement:set_loop([loop])`

Sets whether this movement should restart automatically when the trajectory is finished.

`loop` (boolean, optional, default: `true`)
: `true` to make the movement loop.

### `pixel_movement:get_delay()`

Returns the delay between two steps of the trajectory.

Return value (number)
: The delay between two steps in milliseconds.

### `pixel_movement:set_delay(delay)`

Sets the delay between two steps of the trajectory.

`delay` (number)
: The delay between two steps in milliseconds.

## Events Inherited from `movement`

Pixel movements are particular [movement](./index.md) objects. Therefore, they inherit all events from the type movement.

See [movement](./index.md#events-of-all-movement-types) to know these events.
