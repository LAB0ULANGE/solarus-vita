# Sprites

## Overview

A sprite is an animated image. It is managed by an animation set. The animation set defines which animations are available and describes, for each animation, a sequence of images in each direction.

A sprite has the following properties:

- A current animation from its animation set (like `"walking"` or `"hurt"`),
- A current direction that indicates where the sprite is facing,
- A current frame: the index of the current individual image in the sequence.

The animation set of a sprite is composed of one or several PNG images that store all the frames, and a data file that describes how frames are organized in the PNG images. The data file also indicates the delay to make between frames when animating them and other properties like whether the animation should loop. See the [sprites syntax](../../files-specs/sprite-data-file.md) for more information about the format of sprites.

We describe here the Lua API that you can use to show sprites during your game or your menus.

## Functions of `sol.sprite`

### `sol.sprite.create(animation_set_id)`

Creates a sprite from a sprite sheet.

Generates a Lua error if the sprite sheet could not be loaded.

`animation_set_id` (string)
: Name of the animation set to use. This name must correspond to a valid sprite sheet data file in the `sprites` directory (without its extension).

Return value (sprite)
: The sprite created.

## Methods Inherited from `drawable`

Sprites are particular [drawable](./index.md) objects. Therefore, they inherit all methods from the type drawable.

See [drawable](./index.md) to know these methods.

## Methods of the type `sprite`

The following methods are specific to sprites.

### `sprite:get_animation_set()`

Returns the name of the animation set used by this sprite.

Return value (string)
: Name of the animation set used by this sprite. This name corresponds to a sprite sheet data file in the `sprites` directory (without its extension).

### `sprite:has_animation(animation_name)`

Returns whether the specified animation exists on a sprite.

`animation_name` (string)
: Name of the animation to check.

Return value (boolean)
: `true` if the sprite animation set contains an animation with this name.

### `sprite:get_animation()`

Returns the name of the current animation of this sprite.

Return value (string)
: Name of the current animation.

### `sprite:set_animation(animation_name, [next_action])`

Sets the current animation of this sprite.

`animation_name` (string)
: Name of the animation to set. This animation must exist in the animation set.

`next_action` (function or string, optional)
: What to do when the animation finishes. This parameter only has an effect if the animation finishes, that is, if it does not loop.

    - If you pass a function, this function will be called when the animation finishes.
    - If you pass a string, this should be the name of an animation of the sprite, and this animation will automatically be played next.
    - No value means no action after the animation finishes: in this case, the sprite stops being displayed.

### `sprite:stop_animation()`

Stops playing the current animation of this sprite.

The sprite stops being displayed, like when the last frame finishes.

### `sprite:is_animation_started()`

Returns whether the current animation is being played.

Return value (boolean)
: `true` if the current animation is playing, `false` if it is finished or was [stopped](#spritestop_animation).

### `sprite:get_direction()`

Returns the current direction of this sprite.

Return value (number)
: The current direction (the first one is `0`)

### `sprite:set_direction(direction)`

Sets the current direction of this sprite.

`direction` (number)
: The direction to set (the first one is `0`) This direction must exist in the current animation.

### `sprite:get_num_directions([animation_name])`

Returns the number of direction of an animation of this sprite.

`animation_name` (string, optional)
: Name of an animation of the sprite. This animation must exist in the animation set. No value means the current animation.

Return value (number)
: The number of directions in this animation.

### `sprite:get_frame()`

Returns the index of the current frame of this sprite.

Return value (number)
: The current frame (the first one is `0`)

### `sprite:set_frame(frame)`

Sets the current frame of this sprite.

`frame` (number)
: The frame to set (the first one is `0`) This frame must exist in the current direction.

### `sprite:get_num_frames([animation_name, direction])`

Returns the number of frames of this sprites in an animation and direction.

`animation_name` (string, optional)
: Name of an animation of the sprite. This animation must exist in the animation set. No value means the current animation and the current direction.

`direction` (number, requires: `animation_name`, optional)
: A direction of this animation, between `0` and `sprite:get_num_directions(animation_name) - 1`.

Return value (number)
: The number of frames.

### `sprite:get_frame_delay([animation_name])`

Returns the delay between two frames of this sprite in an animation.

The delay of the current animation may be overriden by [`set_frame_delay()`](#spriteget_frame_delayanimation_name).

`animation_name` (string, optional)
: Name of an animation of the sprite. This animation must exist in the animation set. No value means the current animation.

Return value (number or `nil`)
: The delay in milliseconds between two frames in the current animation. `nil` means infinite and it is only allowed for single-frame animations.

### `sprite:set_frame_delay(delay)`

Changes the delay between two frames of this sprite in the current animation.

Use this function if you want to override the normal delay (the one defined in the sprite data file).

`delay` (number or `nil`)
: The new delay in milliseconds. `nil` means infinite and is only allowed for single-frame animations.

### `sprite:get_size([animation_name, direction])`

Returns the frame size of this sprite in an animation and direction.

`animation_name` (string, optional)
: Name of an animation of the sprite. This animation must exist in the animation set. No value means the current animation.

`direction` (number, optional)
: A direction of this animation, between `0` and `sprite:get_num_directions(animation_name) - 1`.

Return value 1 (number)
: The width of a frame.

Return value 2 (number)
: The height of a frame.

### `sprite:get_origin([animation_name, direction])`

Returns the coordinates of the origin point of this sprite in an animation and direction, relative to the upper left corner of a frame.

The origin is the point of synchronization for sprites that have several animations or directions of different sizes, and for entity sprites that are larger than the entity itself.

See [`entity:get_origin()`](../map-entities/index.md#entityget_origin) for more details.

In a given animation and direction of a sprite, the origin point is the same for all frames.

`animation_name` (string, optional)
: Name of an animation of the sprite. This animation must exist in the animation set. No value means the current animation.

`direction` (number, optional)
: A direction of this animation, between `0` and `sprite:get_num_directions(animation_name) - 1`.

Return value 1 (number)
: X coordinate of the origin point.

Return value 2 (number)
: Y coordinate of the origin point.

### `sprite:get_frame_src_xy([animation_name, direction, frame])`

Returns the coordinates of a frame in its source image.

`animation_name` (string, optional)
: Name of an animation of the sprite. This animation must exist in the animation set. No value means the current animation.

`direction` (number, optional)
: A direction of this animation, between `0` and `sprite:get_num_directions(animation_name) - 1`. No value means the current direction.

`frame` (number, optional)
: A frame number. No value means the current frame.

Return value 1 (number)
: X coordinate of the upper left corner of the frame in its source image.

Return value 2 (number)
: Y coordinate of the upper left corner of the frame in its source image.

### `sprite:is_paused()`

Returns whether this sprite is paused.

Return value (boolean)
: `true` if this sprite is paused.

### `sprite:set_paused([paused])`

Pauses or resumes the animation of this sprite.

`paused` (boolean, optional, default: `true`)
: `true` to pause the sprite, `false` to unpause it.

### `sprite:get_ignore_suspend()`

Returns whether the animation should continue even when the [game](../game.md) is suspended.

This setting only has an effect when a game is running. The default value is `false`

Return value (boolean)
: `true` to continue the animation even when the game is suspended.

### `sprite:set_ignore_suspend([ignore])`

Sets whether the animation should continue even when the [game](../game.md) is suspended.

`ignore` (boolean, optional, default: `true`)
: `true` to continue the animation even when the game is suspended.

### `sprite:synchronize([reference_sprite])`

Synchronizes the frames of this sprite with the frames of a reference sprite. The synchronization will be performed whenever both animation names match. The current sprite will no longer apply its normal frame delay: instead, it will now always set its current frame to the current frame of its reference sprite.

`reference_sprite` (sprite or `nil`, optional)
: The reference sprite. `nil` means stopping any previous synchronization.

## Events of the type `sprite`

Events are callback methods automatically called by the engine if you define them.

The following events are specific to sprites.

### `sprite:on_animation_finished(animation)`

Called when the current animation of this sprite is finished.

If the animation loops, this function is never called. Unless you start another animation, the sprite is no longer shown.

`animation` (string)
: Name of the animation that has just finished.

### `sprite:on_animation_changed(animation)`

Called whenever the animation of this sprite has changed.

`animation` (string)
: Name of the new animation.

### `sprite:on_direction_changed(animation, direction)`

Called whenever the direction of this sprite has changed.

`animation` (string)
: Name of the current animation.

`direction` (number)
: The new current direction (the first one is `0`).

### `sprite:on_frame_changed(animation, frame)`

Called whenever the frame of this sprite has changed.

`animation` (string)
: Name of the current animation.

`frame` (number)
: The new current frame (the first one is `0`).
