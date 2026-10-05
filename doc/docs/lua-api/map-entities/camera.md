# Camera

## Overview

The camera is a rectangular shape that determines the visible part of the [map](../map.md). There is always exactly one camera on the current map. The camera is automatically created by the engine when loading a map. You cannot create or remove it. To access the camera of the map from one of your scripts, you can use [`map:get_camera()`](../map.md#mapget_camera).

The camera can work in two states. It can be either centered on an [entity](./index.md) and track it automatically, or it can be controlled manually by your scripts or by the engine.

When a map starts, its camera is initially set to track the [hero](./hero.md).

Use [`camera:start_manual()`](#camerastart_manual) to switch to manual state, and [`camera:start_tracking()`](#camerastart_trackingentity) to switch to tracking state. Starting a [movement](../movements/index.md) on the camera also switches it to manual state.

### Movements and constraints

[Separators](./separator.md) and map limits are obstacles for the camera.

It means that if the camera is close to a separator or to a limit of the map, its movement stops as you would expect, independently of its state.

Remember however that obstacles can be explicitly ignored with [`movement:set_ignore_obstacles(true)`](../movements/index.md#movementset_ignore_obstaclesignore_obstacles). For a camera, it means that you can ignore separators and even map limits if you want. Parts of the camera that are outside the map are displayed with the background color of the tileset.

### Camera size

By default, the size of the camera is the [quest size](../video.md#solvideoget_quest_size), meaning that the camera occupies the whole screen.

It is possible to set a smaller size to the camera, in order to show the map only on a subpart of the screen. You can then use the rest of the screen to display some information like a HUD. Use [`camera:set_size()`](./index.md#entityset_sizewidth-height) and [`camera:set_position_on_screen()`](#cameraset_position_on_screenx-y) to indicate the exact subpart of the screen where you want the camera to be displayed.

### Suspending the game

Moving the camera does not automatically suspends the game, except when it is scrolling on a [separator](./separator.md) in tracking state. You can call [`game:set_suspended()`](../game.md#gameset_suspendedsuspended) if you want the game to be suspended during your camera sequence. Note that unlike most entities, the camera can still move when the game is suspended.

## Methods Inherited from `entity`

A camera is a particular [map entity](./index.md). Therefore, it inherits all methods from the type map entity.

See [entity](./index.md#methods-of-all-entity-types) to know these methods.

## Methods of the type camera

The following methods are specific to cameras.

### `camera:get_position_on_screen()`

Returns where the camera is displayed on the quest screen.

The default position is `0, 0`, meaning that the upper left corner of the camera is displayed on the upper left corner of the screen.

Return value 1 (number)
: X coordinate of the camera on the screen, in quest screen coordinates.

Return value 2 (number)
: Y coordinate of the camera on the screen, in quest screen coordinates.

### `camera:set_position_on_screen(x, y)`

Sets where the camera is displayed on the quest screen.

You can use this function in conjunction with [`camera:set_size()`](./index.md#entityset_sizewidth-height) to display the camera only on a subpart of the screen and for example keep the rest of the space for the HUD.

The default position is `0, 0`, meaning that the upper left corner of the camera is displayed on the upper left corner of the screen.

`x` (number)
: X coordinate of the camera on the screen, in quest screen coordinates.

`y` (number)
: Y coordinate of the camera on the screen, in quest screen coordinates.

### `camera:get_position_to_track(x, y), camera:get_position_to_track(entity)`

Returns the coordinates this camera should have in order to track the given entity or point, respecting constraints of map limits and separators.

The returned coordinates make their best to have the entity or point centered in the camera, but make sure that the camera does not cross [separators](./separator.md) or map limits. This function can be used to compute legal coordinates for the camera, and for example pass them to [`camera:set_position()`](./index.md#entityset_positionx-y-layer) or start a [movement](../movements/index.md#movementstartobject_to_move-callback).

| With coordinates | With an [entity](./index.md) |
|------------------|------------------------------|
| <dl><dt>`x` (number)</dt><dd>X coordinate of the point to center the camera on.</dd><dt>`y` (number)</dt><dd>Y coordinate of the point to center the camera on.</dd></dl> | <dl><dt>`entity` ([entity](./index.md))</dt><dd>Entity to center the camera on.</dd></dl>|

Return value 1 (number)
: X coordinate this camera should have.

Return value 2 (number)
: Y coordinate this camera should have.

### `camera:get_state()`

Returns the name of the current state of the camera.

Return value (string)
: The current camera state:

    - `"tracking"`
    - `"manual"`

### `camera:start_tracking(entity)`

Switches the camera to tracking state.

The camera will be focused on an entity to track, and follow it when it moves.

When the tracked entity crosses a separator, the engine automatically starts a scrolling movement on the camera. The game is automatically suspended during the scrolling. After the scrolling, the camera continues to normally track the entity on the other side of the separator.

`entity` ([entity](./index.md))
: The entity to track.

### `camera:get_tracked_entity()`

Returns the entity currently tracked by this camera, if any.

Return value ([entity](./index.md) or `nil`)
: The tracked entity if the camera is in tracking state, or `nil` if the camera is not in tracking state.

### `camera:start_manual()`

Switches the camera to manual state.

!!! note "Note"

    The camera automatically switches to manual state if you start a [movement](../movements/index.md#movementstartobject_to_move-callback) on it.

### `camera:get_surface()`

Returns the [surface](../drawable-objects/surface.md) this camera draws its content to.

The size of this surface is the size of the camera.

Return value ([surface](../drawable-objects/surface.md))
: The camera's surface.

### `camera:set_viewport(x, y, w, h)`

Sets the viewport of the camera, only has an effect if [`sol.video.set_geometry_mode()`](../video.md#solvideoset_geometry_modemode) is set to "dynamic_quest_size" or "dynamic_absolute".

`x` (number)
: Relative horizontal position of the viewport on screen `[0, 1]`.

`y` (number)
: Relative vertical position of the viewport on screen `[0, 1]`.

`w` (number)
: Relative width of the viewport on screen `[0, 1]`.

`h` (number)
: Relative height of the viewport on screen `[0, 1]`.

### `camera:get_viewport(0)`

Returns the viewport of the camera

Return value 1 (number)
: Relative horizontal position of the viewport on screen `[0, 1]`.

Return value 2 (number)
: Relative vertical position of the viewport on screen `[0, 1]`.

Return value 3 (number)
: Relative width of the viewport on screen `[0, 1]`.

Return value 4 (number)
: Relative height of the viewport on screen `[0, 1]`.

### `camera:set_zoom(zoomx, zoomy)`

Sets the zoom factors of the camera. Values greater than `1` zoom in.

`zoomx` (number)
: Scaling of the camera along x axis.

`zoomy` (number)
: Scaling of the camera along y axis.

### `camera:get_zoom()`

Gets the zoom factors of the camera.

Return value 1 (number)
: Scaling of the camera along x axis.

Return value 2 (number)
: Scaling of the camera along y axis.

### `camera:set_rotation(angle)`

Sets the camera rotation around its center.

`angle` (number)
: Rotation angle, positive is counter-clockwise.

### `camera:get_rotation(angle)`

Gets the camera rotation around its center.

Return value (number)
: Rotation angle, positive is counter-clockwise.

### `camera:teleport(map_id, [destination_name, [transition_style]])`

Teleports the camera to another map and destination.

`map_id` (string)
: Id of the map to teleport the camera into.

`destination_name` (string or `nil`, optional)
: Name of the destination to place the camera on on arrival.

`transition_style` (string, optional, requires: `destination_name`)
: No value means [`game:get_transition_style()`](../game.md#gameget_transition_style), which is `"fade"` by default. Possible values are:

    - `"immediate"`: no transition effect
    - `"fade"`: fade-out and fade-in effect
    - `"scrolling"`

## Events Inherited from `entity`

Events are callback methods automatically called by the engine if you define them.

A camera is a particular [map entity](./index.md). Therefore, it inherits all events from the type map entity.

See [entity](./index.md#events-of-all-entity-types) to know these events.

## Events of the type camera

The following events are specific to cameras.

### `camera:on_state_changing(state_name, next_state_name)`

Called when the state of the camera is about to change.

`state_name` (string)
: Name of the current state. See [`camera:get_state()`](#cameraget_state) for the list of possible state names.

`next_state_name` (string)
: Name of the state about to start. See [`camera:get_state()`](#cameraget_state) for the list of possible state names.

### `camera:on_state_changed(new_state_name)`

Called when the state of the camera has just changed.

`new_state_name` (string)
: Name of the new state. See [`camera:get_state()`](#cameraget_state) for the list of possible state names.

!!! note "Note"

    When the map starts, the initial state of the camera always `"tracking"`, because the camera initially tracks the hero.
