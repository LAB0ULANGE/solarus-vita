# Drawable Objects

Drawable objects are things that can be drawn on a destination surface. They include the following types: [surface](./surface.md), [text surface](./text-surface.md) and [sprite](./sprite.md). This page describes the methods common to those types.

## Methods of all Drawable Types

These methods exist in all drawable types.

### `drawable:draw(dst_surface, [x, y])`

Draws this object on a destination surface.

`dst_surface` ([surface](./surface.md))
: The destination surface.

`x` (number, optional, requires: `y`, default: `0`)
: X coordinate of where to draw this object.

`y` (number, optional, requires: `x`, default: `0`)
: Y coordinate of where to draw this object.

### `drawable:draw_region(region_x, region_y, region_width, region_height, dst_surface, [x, y])`

Draws a subrectangle of this object on a destination surface.

`region_x` (number)
: X coordinate of the subrectangle to draw.

`region_y` (number)
: Y coordinate of the subrectangle to draw.

`region_width` (number)
: Width of the subrectangle to draw.

`region_height` (number)
: Height of the subrectangle to draw.

`dst_surface` ([surface](./surface.md))
: The destination surface.

`x` (number, optional, requires: `y`, default: `0`)
: X coordinate of where to draw this rectangle on the destination surface.

`y` (number, optional, requires: `x`, default: `0`)
: Y coordinate of where to draw this rectangle. on the destination surface.

### `drawable:get_rotation()`

Returns the rotation angle of this object around its [transformation origin](#drawableset_transformation_originx-y). The rotation angle is in radians and goes in trigonometric direction.

Return value (number)
: The rotation angle in radians around the transformation origin. `0` means no rotation.

### `drawable:set_rotation(rotation)`

Sets the rotation angle of this object around its [transformation origin](#drawableset_transformation_originx-y). The rotation angle is in radians and goes in trigonometric direction.

`rotation` (number)
: The rotation angle in radians around the transformation origin. `0` means no rotation.

### `drawable:get_scale()`

Returns the scaling factors of this drawable object.

Return value 1 (number)
: Scale factor in the X dimension.

Return value 2 (number)
: Scale factor in the Y dimension.

### `drawable:set_scale(x, y)`

Set the scale factors for this drawable object. A scale of `1` corresponds to a normal size. A negative scale will flip the drawable in the corresponding axis. Scaling is performed around the [transformation origin](#drawableset_transformation_originx-y).

`x` (number)
: Scale factor in the X dimension.

`y` (number)
: Scale factor in the Y dimension.

### `drawable:get_transformation_origin()`

Returns the transformation origin of this drawable object. The tranformation origin point is the pivot point used for rotation and scaling.

The transformation point is relative to the origin point of the drawable object, which is always the upper-left corner for [surfaces](./surface.md) and [text surfaces](./text-surface.md), and the sprite origin point for [sprites](./sprite.md).

Return value 1 (number)
: X coordinate of the transform point.

Return value 2 (number)
: Y coordinate of the transform point.

### `drawable:set_transformation_origin(x, y)`

Set the transformation origin point of this drawable object. The tranformation origin point is the pivot point used for rotation and scaling.

The transformation point is relative to the origin point of the drawable object, which is always the upper-left corner for [surfaces](./surface.md) and [text surfaces](./text-surface.md), and the sprite origin point for [sprites](./sprite.md).

`x` (number)
: X coordinate of the transformation origin.

`y` (number)
: Y coordinate of the transformation origin.

### `drawable:get_blend_mode()`

Returns the blend mode of this drawable object.

The blend mode defines how this drawable object will be drawn on other surfaces when you call [`drawable:draw()`](#drawabledrawdst_surface-x-y) or [`drawable:draw_region()`](#drawabledraw_regionregion_x-region_y-region_width-region_height-dst_surface-x-y).

Return value (string)
: The blend mode. See [`drawable:set_blend_mode()`](#drawableset_blend_modeblend_mode) for the possible values.

### `drawable:set_blend_mode(blend_mode)`

Sets the blend mode of this drawable object.

The blend mode defines how this drawable object will be drawn on other surfaces when you call [`drawable:draw()`](#drawabledrawdst_surface-x-y) or [`drawable:draw_region()`](#drawabledraw_regionregion_x-region_y-region_width-region_height-dst_surface-x-y).

`blend_mode` (string)
: The blend mode. Possible values are:

    - `"blend"` (default): This drawable object is alpha-blended onto the destination surface. `dstRGB = (srcRGB * srcA) + (dstRGB * (1 - srcA))` `dstA = srcA + (dstA * (1 - srcA))`
    - `"none"`: No blending. The destination surface is replaced by the pixels of this drawable object. `dstRGBA = srcRGBA`
    - `"add"`: This drawable object is drawn onto the destination surface with additive blending. The clarity of the destination surface is kept. Useful to color and lighten the destination surface. `dstRGB = (srcRGB * srcA) + dstRGB`
    - `"multiply"`: Color modulation. Can be used to darken the destination surface without degrading its content. `dstRGB = srcRGB * dstRGB`

### `drawable:get_shader()`

Returns the shader applied to this object.

Return value ([shader](../shaders.md) or `nil`)
: The shader, or `nil` if no shader is set.

### `drawable:set_shader(shader)`

Sets the shader used to draw this object.

Default drawing is done with no shader.

`shader` ([shader](../shaders.md) or `nil`)
: The shader to set, or `nil` to reset drawing to normal.

### `drawable:get_opacity()`

Returns the opacity of this drawable.

Return value (integer)
: The opacity: `0` (transparent) to `255` (opaque).

### `drawable:set_opacity(opacity)`

Sets the opacity of this drawable.

All drawables are initially opaque.

`opacity` (integer)
: The opacity: `0` (transparent) to `255` (opaque).

### `drawable:get_color_modulation()`

Returns the color multiplier of this drawable.

Return value (table)
: The color as an array of RGBA values

### `drawable:set_color_modulation(color)`

Sets the color modulator of this drawable, default is plain white {255,255,255,255}. When the drawable is draw, all the pixels are multiplied by this color (white as no effect). Alpha channel and drawable opacity are multiplied together.

`color` (table)
: The color multiplier, as an array of RGBA values.

### `drawable:fade_in([delay], [callback])`

Starts a fade-in effect on this object.

You can specify a callback function to be executed when the fade-in effect finishes.

If the drawable object is a [sprite](./sprite.md) attached to a [map entity](../map-entities/index.md) during a game, the fade-in effect gets the lifetime of that entity. The behavior is probably what you expect: the fade-in effect gets suspended when the entity gets suspended, and it gets canceled (that is, the callback is never executed) when the map entity is destroyed.

`delay` (number, optional, default: `20`)
: Delay in milliseconds between two frames of the fade-in animation. The total number of frames of the animation is 32.

`callback` (function, optional)
: A function to call when the fade-in effect finishes.

!!! note "Note"

    When your drawable object does not belong to a [map entity](../map-entities/index.md) (typically in a title screen before a game is started, or in your pause menu), the fade-in effect continues until the drawable object is garbage-collected. In other words, the callback can be executed even if you have stopped using the drawable object in the meantime.

    Therefore, you should use the `callback` parameter with care. In these situations, using a [timer](../timers.md) for your callback is easier because timers have an explicit lifetime.

### `drawable:fade_out([delay], [callback])`

Starts a fade-out effect on this object.

You can specify a callback function to be executed when the fade-out effect finishes.

If the drawable object is a [sprite](./sprite.md) attached to a [map entity](../map-entities/index.md) during a game, the fade-out effect gets the lifetime of that entity. The behavior is probably what you expect: the fade-out effect gets suspended when the entity gets suspended, and it gets canceled (that is, the callback is never executed) when the map entity is destroyed.

`delay` (number, optional, default: `20`)
: Delay in milliseconds between two frames of the fade-out animation. The total number of frames of the animation is 32.

`callback` (function, optional)
: A function to call when the fade-out effect finishes.

!!! note "Note"

    When your drawable object does not belong to a [map entity](../map-entities/index.md) (typically in a title screen before a game is started, or in your pause menu), the fade-out effect continues until the drawable object is garbage-collected. In other words, the callback can be executed even if you have stopped using the drawable object in the meantime.

    Therefore, you should use the `callback` parameter with care. In these situations, using a [timer](../timers.md) for your callback is easier because timers have an explicit lifetime.

### `drawable:get_xy()`

Returns the offset added where this drawable object is drawn.

This value is initially `0,0`. It is added to whatever coordinates the object is drawn at.

They can be modified by a [movement](../movements/index.md) or by [`drawable:set_xy()`](#drawableset_xyx-y).

Return value 1 (number)
: X offset of the drawable object.

Return value 2 (number)
: Y offset of the drawable object.

### `drawable:set_xy(x, y)`

Sets the offset added where this drawable object is drawn.

This value is initially `0,0`. It is added to whatever coordinates the object is drawn at.

`x` (number)
: X offset to set.

`y` (number)
: Y offset to set.

### `drawable:get_movement()`

Returns the current movement of this drawable object.

Return value ([movement](../movements/index.md) or `nil`)
: The current movement, or `nil` if the drawable object is not moving.

### `drawable:stop_movement()`

Stops the current movement of this drawable object if any.
