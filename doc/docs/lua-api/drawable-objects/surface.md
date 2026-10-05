# Surfaces

## Overview

A surface is a 2D image. It is essentially a rectangle of pixels. Its main feature is that you can draw objects on it.

## Functions of `sol.surface`

### `sol.surface.create([width, height])`

Creates an empty surface.

`width` (number, requires: `height`, optional)
: Width of the surface to create in pixels. The default value is the width of the logical screen.

`height` (number, requires: `width`, optional)
: Height of the surface to create in pixels. The default value is the height of the logical screen.

Return value (surface)
: The surface created.

### `[Deprecated] sol.surface.create(file_name, [language_specific])`

Creates a surface from an image file inside a predefined directory.

`file_name` (string)
: Name of the image file to load, relative to either the `sprites` directory or the `images` subdirectory of the current language directory.

`language_specific` (boolean, optional, default: `false`)
: `true` to load the image from the `images` subdirectory of the current language directory. The default is `false` and loads the image from the `sprites` directory.

Return value (surface or `nil`)
: The surface created, or `nil` if the image file could not be loaded.

!!! warning "Deprecated"

    This method is deprecated since Solarus 2.0.

Use [`sol.surface.load()`](./surface.md#solsurfaceloadfile_name) instead, which is safer and more general.

### `sol.surface.load(file_name)`

Creates a surface from an image file. Generates a Lua error if the image could not be loaded.

`file_name` (string)
: Name of the image file to load, relative to the quest data directory.

Return value (surface)
: The surface created.

## Methods Inherited from `drawable`

Surfaces are particular [drawable](./index.md) objects. Therefore, they inherit all methods from the type drawable.

See [`drawable`](./index.md) to know these methods.

## Methods of the type `surface`

The following methods are specific to surfaces.

### `surface:save(file_name)`

Saves the content of the surface into a PNG file.

If the file already exists, it will be overwritten. In case of failure, a Lua error is raised.

`file_name` (string)
: Name of the file to save, including its `.png` extension, relative to the [quest write directory](../general-features.md#solmainget_quest_write_dir).

### `surface:get_size()`

Returns the size of this surface.

Return value 1 (number)
: Width of the surface in pixels.

Return value 2 (number)
: Height of the surface in pixels.

### `surface:clear()`

Erases all pixels of this surface.

All pixels become transparent. The opacity property and the size of the surface are preserved.

### `surface:fill_color(color, [x, y, width, height])`

Fills a region of this surface with a color.

If no region is specified, the entire surface is filled. If the color has an alpha component different from 255 (opaque), then the color is blended onto the existing pixels.

`color` (table)
: The color as an array of 3 RGB values or 4 RGBA values (`0` to `255`)

`x` (number, requires: `y` and `width` and `height`, optional)
: X coordinate of the region to fill on this surface.

`y` (number, requires: `x` and `width` and `height`, optional)
: Y coordinate of the region to fill on this surface.

`width` (number, requires: `x` and `y` and `height`, optional)
: Width of the region to fill on this surface.

`height` (number, requires: `x` and `y` and `width`, optional)
: Height of the region to fill on this surface.

### `surface:get_pixels()`

Returns all pixel values of this surface.

Return value (string)
: The pixels as a pack of bytes in 32-bit RGBA format. Every pixel is stored on 4 bytes (red, green, blue, alpha).

### `surface:set_pixels(pixels)`

Sets all pixel values of this surface.

The number of pixels should match the size of the surface (the size does not change).

`pixels` (string)
: The pixels as a pack of bytes in 32-bit RGBA format. Every pixel is stored on 4 bytes (red, green, blue, alpha).

### `surface:gl_bind_as_target()`

Binds this surface as a target for next OpenGL calls.

This method is only useful for advanced users who want to directly call OpenGL primitives, which is possible with Lua bindings, for example via LuaJIT FFI.

This is equivalent to `glBindFramebuffer(GL_DRAW_FRAMEBUFFER, …)`. It will make this surface the destination for any further OpenGL draw call.

When you call OpenGL directly, it is your responsability to restore the OpenGL state if you want the Solarus rendering to remain correct. In particular, pay attention to restore :

- The currently bound `glProgram`,
- The current `glBlendModes`,
- The currently bound `VertexBufferObjects`,
- Face culling and alpha tests.

!!! note "Note"

    Note that Solarus surfaces have no depth buffer attached. A new framebuffer with depth attachement must be used to benefit from `GL_DEPTH_TEST`.

### `surface:gl_bind_as_texture()`

Bind this surface as the current texture for next OpenGL calls.

This method is only useful for advanced users who want to directly call OpenGL primitives, which is possible with Lua bindings, for example via LuaJIT FFI.

This is equivalent to `glBindTexture(…)`.

See [`surface:gl_bind_as_target()`](#surfacegl_bind_as_target) for details about calling OpenGL directly.
