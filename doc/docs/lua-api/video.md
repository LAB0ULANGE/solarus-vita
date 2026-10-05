# Video

`sol.video` allows you to manage the window and the display.

The area where the game takes place has a fixed size called the quest size. The quest size is in a range specified in the [quest properties file](../files-specs/properties-file.md). This quest size is the logical size: it determines how much content the player can see on the map.

The quest size is typically 320x240 pixels, but some systems may prefer other sizes, like 400x240 on Android. You can set a range of supported quest sizes in the quest properties files for portability. However, it requires more work on your part: in particular, you have to implement menus and a HUD that can adapt to any size in this
range. And be aware that some players will be able to see more game content than others.

The quest can be played in windowed mode or in fullscreen. In windowed mode, the window can be resized by the user or by your scripts. The quest size is independent from the actual window size: when you resize the window, the quest image is scaled to fit the window. The pixel ratio is always preserved, possibly by adding black borders.

Solarus supports OpenGL shaders to modify the rendering, for example to apply a smoothing filter. See the [shader](./shaders.md) documentation for more details.

## Functions of `sol.video`

### `sol.video.get_window_title()`

Returns the text of the title bar of the window.

Return value (string)
: The window title.

### `sol.video.set_window_title(window_title)`

Sets the text of the title bar of the window.

By default, the window title is set to the title of your quest followed by its version. Both these properties are indicated in the [quest.dat](../files-specs/properties-file.md) file.

`window_title` (string)
: The window title to set.

### `sol.video.is_fullscreen()`

Returns whether the current video mode is fullscreen.

Return value (boolean)
: `true` if the video mode is fullscreen.

### `sol.video.set_fullscreen([fullscreen])`

Turns on or turns off fullscreen, keeping an equivalent video mode.

`fullscreen` (boolean, optional, default: `true`)
: `true` to set fullscreen.

!!! note "Note"

    When the quest is run from the Solarus GUI, the user can also
    switch fullscreen from the menus of the GUI.

### `sol.video.is_cursor_visible()`

Returns whether the mouse cursor is currently visible.

Return value (boolean)
: `true` if the mouse cursor is currently visible.

### `sol.video.set_cursor_visible([cursor_visible])`

Shows or hides the mouse cursor, keeping an equivalent video mode.

`visible_cursor` (boolean, optional, default: `true`)
: `true` to show mouse cursor.

### `sol.video.get_quest_size()`

Returns the logical size of the quest screen.

This quest size is fixed at runtime. It is always in the range of allowed quest sizes specified in [quest.dat](../files-specs/properties-file.md).

The quest size is independent from the actual window size.

Return value 1 (number)
: Width of the quest screen in pixels.

Return value 2 (number)
: Height of the quest screen in pixels.

### `sol.video.get_window_size()`

Returns the size of the window.

The quest image has a [fixed size](#solvideoget_quest_size) determined when the program starts. This quest image is then scaled to the window. The size of the window can be changed at any moment by the user or by [`sol.video.set_window_size()`](#solvideoset_window_sizewidth-height). Black borders are added if necessary to keep the correct pixel ratio.

When the window is in fullscreen, this function returns the size to be used when returning to windowed mode.

Return value 1 (number)
: Width of the window in pixels.

Return value 2 (number)
: Height of the window in pixels.

!!! note "Note"

    When Solarus starts, the size of the window is **twice** the quest size.

### `sol.video.set_window_size(width, height)`

Sets the size of the window. See [`sol.video.get_window_size()`](#solvideoget_window_size) for a detailed description of the window size.

When the window is in fullscreen, this function still works: the changes will be visible when returning to windowed mode.

`width` (number)
: Width of the window in pixels.

`height` (number)
: Height of the window in pixels.

### `sol.video.get_shader()`

Returns the shader currently applied to the rendering, if any.

Return value ([shader](./shaders.md))
: The current shader, or `nil` if no shader is applied.

### `sol.video.set_shader(shader)`

Sets a shader to be applied to the rendering.

`shader` ([shader](./shaders.md) or `nil`)
: The shader to set. It replaces the previous shader if any. A `nil` value means to apply no shader.

### `sol.video.set_geometry_mode(mode)`

Sets the geometry mode of the video module.

`mode` (string)
: Video geometry mode. Mode can be one of the following :

    - `"letter_boxing"`: This is the default geometry mode of the engine, the quest size dictates the resolution of the quest surface and the surface is then scaled, preserving aspect ratio and applying letter boxing.
    - `"dynamic_quest_size"`: This mode is resizing the quest surface when the output video surfaces changes aspect ratio. It matches the shape of the video output but preserves the quest size pixel density.
    - `"dynamic_absolute"`: This mode resizes the quest surface to match the video output 1:1. This means that the quest surface changes resolution to match the window size exactly.

## Events of `sol.video`

Events are callback methods automatically called by the engine if you define them.

### `sol.video:on_draw(screen)`

Called at the very end of the engine rendering. This event allows you to draw directly on the screen after the quest surface has been scaled to fit the size of the window (possibly using [shaders](./shaders.md)). One pixel in the surface parameter will correspond to one pixel on screen.

`screen` ([surface](./drawable-objects/surface.md))
: Special surface representing the window.

!!! note "Note"

    Note that the screen cannot be used as a source surface (i.e. you cannot draw it onto another surface). But you can still use [`get_pixels()`](./drawable-objects/surface.md#surfaceget_pixels) to get the screen content.

## Deprecated functions of sol.video

The following functions are deprecated and may be removed it future releases.

### `sol.video.get_mode()`

Returns the current video mode. Video modes are built-in software filters that can scale the rendering.

Return value (string)
: Name of the video mode. The following modes are available:

    - `"normal"`: The quest screen is unchanged. In windowed mode, the image is stretched by a factor of 2.
    - `"scale2x"`: The quest screen is scaled by a factor of 2 with the [Scale2X](http://scale2x.sourceforge.net/algorithm.html) algorithm.
    - `"hq2x"`: The quest screen is scaled by a factor of 2 with the [hq2x](http://en.wikipedia.org/wiki/Hqx) algorithm.
    - `"hq3x"`: The quest screen is scaled by a factor of 3 with the [hq3x](http://en.wikipedia.org/wiki/Hqx) algorithm.
    - `"hq4x"`: The quest screen is scaled by a factor of 4 with the [hq4x](http://en.wikipedia.org/wiki/Hqx) algorithm.

!!! warning "Deprecated"

    This method is deprecated since Solarus 1.6. Indeed, [shaders](./shaders.md) are the new recommended way to implement video filters. Use [`sol.video.get_shader()`](#solvideoget_shader) instead.

### `sol.video.set_mode(video_mode)`

Sets the current video mode. Video modes are built-in software filters that can scale the rendering.

The window size is reset to a default value that depends on the video mode.

`video_mode` (string, default: `"normal"`)
: Name of the video mode. The following modes are available:

    - `"normal"` (default): The quest screen is unchanged. In windowed mode, the image is stretched by a factor of 2.
    - `"scale2x"`: The quest screen is scaled by a factor of 2 with the [Scale2X](http://scale2x.sourceforge.net/algorithm.html) algorithm.
    - `"hq2x"`: The quest screen is scaled by a factor of 2 with the [hq2x](http://en.wikipedia.org/wiki/Hqx) algorithm.
    - `"hq3x"`: The quest screen is scaled by a factor of 3 with the [hq3x](http://en.wikipedia.org/wiki/Hqx) algorithm.
    - `"hq4x"`: The quest screen is scaled by a factor of 4 with the [hq4x](http://en.wikipedia.org/wiki/Hqx) algorithm.

!!! warning "Deprecated"

    This method is deprecated since Solarus 1.6. Indeed, [shaders](./shaders.md) are the new recommended way to implement video filters. Use [`sol.video.set_shader()`](#solvideoset_shadershader) instead.

### `sol.video.switch_mode()`

Sets the next video mode in the list of video modes supported. Video modes are built-in software filters that can scale the rendering.

You can use this function if you want to change the video mode without specifying which one to use.

!!! warning "Deprecated"

    This method is deprecated since Solarus 1.6. Indeed, [shaders](./shaders.md) are the new recommended way to implement video filters. Use [`sol.video.set_shader()`](#solvideoget_shader) instead.

### `sol.video.is_mode_supported(video_mode)`

Returns whether a video mode is supported.

`video_mode` (string, default: `"normal"`)
: Name of a video mode. The following modes are available:

    - `"normal"` (default): The quest screen is unchanged. In windowed mode, the image is stretched by a factor of 2.
    - `"scale2x"`: The quest screen is scaled by a factor of 2 with the [Scale2X](http://scale2x.sourceforge.net/algorithm.html) algorithm.
    - `"hq2x"`: The quest screen is scaled by a factor of 2 with the [hq2x](http://en.wikipedia.org/wiki/Hqx) algorithm.
    - `"hq3x"`: The quest screen is scaled by a factor of 3 with the [hq3x](http://en.wikipedia.org/wiki/Hqx) algorithm.
    - `"hq4x"`: The quest screen is scaled by a factor of 4 with the [hq4x](http://en.wikipedia.org/wiki/Hqx) algorithm.

!!! warning "Deprecated"

    This method is deprecated since Solarus 1.6. Indeed, [shaders](./shaders.md) are the new recommended way to implement video filters. Use [`sol.shader.create()`](./shaders.md#solshadercreateshader_id-solshadercreateproperties) instead and check if it fails.

### `sol.video.get_modes()`

Returns an array of all video modes supported.

Return value (table)
: An array of names of all video modes supported.

!!! warning "Deprecated"

    This method is deprecated since Solarus 1.6. Indeed, [shaders](./shaders.md) are the new recommended way to implement video filters. Use [`sol.main.get_resource_ids("shader")`](./general-features.md#solmainget_resource_idsresource_type) instead.

### `sol.video.reset_window_size()`

Restores the size of the window to a default value that depends on the current video mode.

!!! warning "Deprecated"

    This method is deprecated since Solarus 1.6. Indeed, [shaders](./shaders.md) are the new recommended way to implement video filters. Use [`sol.video.set_window_size()`](#solvideoset_window_sizewidth-height) instead.
