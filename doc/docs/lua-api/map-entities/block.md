# Block

## Overview

Blocks are solid [map entities](./index.md) that may be pushed or pulled by the [hero](./hero.md).

This type of [map entity](./index.md) can be declared in the [map data file](../map.md#map-files). It can also be created dynamically with [`map:create_block()`](../map.md#mapcreate_blockproperties).

Blocks that can move may be pushable, pullable or both pushable and pullable. A block can optionally be moved a limited number of times. It can be moved either to any direction (the four main directions) or to a specific direction only.

The size of a block is by default 16×16 pixels (the default size of the hero), but as always, its sprite may be larger.

Blocks are normally always moved by steps of 16 pixels. Thus, they can stay aligned on the 8x8 grid of the [map](../map.md). However, they may get stopped in the middle of their movement if they collide with entities like [enemies](./enemy.md) or [non-playing characters](./npc.md). In this case, they lose their alignment on the grid, and this might be a problem when want the to move a block precisely through narrow places and place it at an exact position to solve a puzzle. To deal with this potential issue, the engine automatically realigns the block to the 8x8 grid the next time it is moved.

## Methods Inherited from `entity`

Blocks are particular [map entities](./index.md). Therefore, they inherit all methods from the type map entity.

See [entity](./index.md#methods-of-all-entity-types) to know these methods.

## Methods of the type block

The following methods are specific to blocks.

### `block:reset()`

Restores the block at its initial position and resets its counter of moves.

It means that the hero will be able to move again a block that could only be moved a limited number of times even if the limit was reached.

### `block:is_pushable()`

Returns whether this block can be pushed.

This property is independent of whether or not the block was already moved its maximum number of times.

Return value (boolean)
: `true` if this block can be pushed.

### `block:set_pushable([pushable])`

Sets whether this block can be pushed.

This property is independent of whether or not the block was already moved its maximum number of times.

`pushable` (boolean, optional, default: `true`)
: `true` to make this block pushable.

### `block:is_pullable()`

Returns whether this block can be pulled.

This property is independent of whether or not the block was already moved its maximum number of times.

Return value (boolean)
: `true` if this block can be pulled.

### `block:set_pullable([pullable])`

Sets whether this block can be pulled.

This property is independent of whether or not the block was already moved its maximum number of times.

`pullable` (boolean, optional, default: `true`)
: `true` to make this block pullable.

### `block:get_max_moves()`

Returns the maximum number of times the block can be moved.

This function returns the maximum moves value that was set at creation time or by [`block:set_max_moves()`](#blockset_max_movesmax_moves), no matter if the block was moved then.

Return value (number or `nil`)
: How many times the block can be moved. `nil` means unlimited.

### `block:set_max_moves(max_moves)`

Sets the maximum number of times the block can be moved.

This resets the remaining allowed moves.

`maximum_moves` (number or `nil`)
: How many times the block can be moved. `nil` means unlimited.

### `block:get_direction()`

Returns the direction the block can be moved to, if it is restricted to a specific direction.

Return value (number or `nil`)
: A direction between `0` (East) and `3` (South), or nil if this block is not restricted to a specific direction.

### `block:get_moving_sound()`

Returns the sound id defined for this block when the hero is moving (push or pull) the block.

Return value (string)
: The sound id.

### `block:set_moving_sound([sound_id])`

Changes the sound to play when the hero is moving (push or pull) the block.

By default, the sound used is `"hero_pushes"`.

You can use this function if you want another sound to be played.

`sound_id` (string or `nil`, optional)
: The sound id. An empty string or `nil` means no sound.

### `block:get_falling_sound()`

Returns the sound id defined for this block when it is falling into a hole.

Return value (string)
: The sound id.

### `block:set_falling_sound([sound_id])`

Changes the sound to play when the block is falling into a hole.

By default, the sound used is `"jump"`.

You can use this function if you want another sound to be played.

`sound_id` (string or `nil`, optional)
: The sound id. An empty string or `nil` means no sound.

### `block:get_sinking_sound()`

Returns the sound id defined for this block when it is sinking into deep water or lava.

Return value (string)
: The sound id.

### `block:set_sinking_sound([sound_id])`

Changes the sound to play when the block is sinking into deep water or lava.

By default, the sound used is `"splash"`.

You can use this function if you want another sound to be played.

`sound_id` (string or `nil`, optional)
: The sound id. An empty string or `nil` means no sound.

## Events Inherited from `entity`

Events are callback methods automatically called by the engine if you define them.

Blocks are particular [map entities](./index.md). Therefore, they inherit all events from the type map entity.

See [entity](./index.md#events-of-all-entity-types) to know these events.

## Events of the type block

The following events are specific to blocks.

### `block:on_moving()`

Called when the [hero](./hero.md) starts moving the block of a step.

### `block:on_moved()`

Called when the [hero](./hero.md) has just moved this block of a step.

## Deprecated methods of the type block

The following methods are deprecated and may be removed it future releases.

### `block:get_maximum_moves()`

Same as [`block:get_max_moves()`](#blockget_max_moves).

Return value (number or `nil`)
: How many times the block can be moved. `nil` means unlimited.

!!! warning "Deprecated"

    This method is deprecated since Solarus 1.6. The corresponding parameter `maximum_moves` of [`map:create_block()`](../map.md#mapcreate_blockproperties) is also deprecated. Use [`block:get_max_moves()`](#blockget_max_moves) instead.

### `block:set_maximum_moves(maximum_moves)`

Same as [`block:set_max_moves()`](#blockget_max_moves).

`maximum_moves` (number)
: How many times the block can be moved. `nil` means unlimited.

!!! warning "Deprecated"

    This method is deprecated since Solarus 1.6. The corresponding parameter `maximum_moves` of[`map:create_block()`](../map.md#mapcreate_blockproperties) is also deprecated. Use [`block:set_max_moves()`](#blockset_max_movesmax_moves) instead.
