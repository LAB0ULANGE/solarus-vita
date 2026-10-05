# Sprite Data File

## Overview

A sprite is an animated image that can be displayed on a surface or attached to an entity of the map.

A sprite displays images from an animation set with a current animation, a current direction and a current frame. Several sprites displayed at the same time can share the same animation set and have different states (i.e. different animations, directions and frames).

An animation set has at least two data files:

- One or more PNG images that contain the images of the animation set.
- A sprite definition file like `sprites/xx.dat,` where `xx` is the id of the sprite animation set. This file contains the definition of each animation, direction and frame of the animation set.

The sprite animation sets are represented as in Multimedia Fusion:

- A sprite animation set is composed of one or several animations. For instance, the animation set of a basic enemy may have four animations: `"stopped",` `"walking",` `"hurt"` and `"shaking"`.
- Each animation is composed of one or several directions, numbered from 0. Non-playing characters and usual enemies have four directions.
- Each direction is a sequence of frames, where a frame is a static image. Frames are numbered from 0. Once the animation is finished, the sequence may loop, that is, come back to a specific frame (often, but not necessarily, the first one).

The definition file `sprites/xx.dat` contains those information.

## Origin Point

A crucial concept that comes with sprites is the notion of origin point.

When a sprite is drawn at some coordinates, the origin point determines what exact point of the sprite's rectangle is at those coordinates. This point is not necessarily the upper-left corner of the rectangle.

The origin point is essential when your sprite has several animations of different frame sizes, and also when your sprite represents a map entity whose collision box has a different size (usually, the sprite of a character is larger than its collision box).

Let's take an exemple: the animation `"walking"` of a character's sprite has frames of size `24x32` in the PNG image, while its animation `"plunging"` has frames of size `64x24`(for instance, because some water splashing is displayed next to the body).

In such a case, how can you indicate to the engine the relative position of frames of both animations? When switching from `"walking"` to `"plunging",` you most likely don't want the upper-left corner of the `64x24` rectangle to be simply displayed where the upper-left corner of the `24x32` rectangle was displayed.

The solution is to specify a point of `"walking"` frames that should match a point of `"plunging"` frames. We call this point the origin point. You can specify it on each animation, relative to the upper-left corner of a frame. In the example above, the origin point might be `12,29` for animation `"walking"` and `32,21` for animation `"plunging"`

Furthermore, when the sprite is used to represent a character or an enemy on the map, the collision box of that character or enemy is a rectangle whose size is usually `16×16`(used to avoid obstacles when moving). The sprite is often larger than this collision box, so in this situation, we also need the notion of origin point. Therefore, map entities also have an origin point property that allows to correctly anchor sprites of any size on their collision box.

We normally choose as origin point the central point of contact between the entity and the ground. For example, the hero and the non-playing characters always have a collision box of `16×16,` and their origin point is always `8,13` (the center of their shadow). Sprites have to take care of this setting to obtain correct results.

!!! note "Note"

    The origin point is the same thing as the "hotspot" property in Multimedia Fusion, except that in Solarus, it is the same for all frames of a direction.

## Syntax of a Sprite Sheet File

Solarus Editor fully supports the edition of sprites. You should not have to edit sprite data files by hand unless you know what you are doing.

The sequence of characters `--` (two dashes) marks the beginning of a comment. After them, the rest of the line is ignored by the engine. Empty lines are also ignored.

The definition of each animation starts with `animation{` and ends with `}` Between the braces are defined the properties of the animation. Properties are declared with the syntax `key = value` and are separated with commas. It is allowed to have an extra comma after the last property. String values are enclosed within double quotes. The properties must be:

`name` (string)
: A name identifying this animation.

`src_image` (string)
: Name of a PNG image that contains all frames of all directions of this animation. It cannot contain spaces. Its path is relative to the directory `sprites` Alternatively, `src_image` may also be the keyword `"tileset":` in this case, it means that the sprite is tileset-dependent. Indeed, you may want to make sprites that change with the skin of the current map (like animated doors). The PNG file is then `tilesets/yy.entities.png` where `yy` is the id of the current tileset.

`frame_delay` (number, optional)
: Delay in milliseconds between two frames of the animation. Zero or no value means an infinite delay. In this case, the sprite will show a fixed image and will never finish its animation.

`frame_to_loop_on` (number, optional)
: Index of the frame where you want the animation to come back when the last frame finishes (the first frame is `0)` No value means no loop (the sprite stops being displayed after the last frame). Note that when `delay` is `0,` this can never happen since the first frame can never finish.

`directions` (table)
: List of all directions (sequences of frames) of this animation. The list of directions is enclosed between `{` and `},` where directions are separated by commas. Each direction defines a sequence of individual frames. In the PNG image, all frames of a direction are rectangles of the same size, placed in adjacent locations from left to right and possibly from top to bottom (in other words, organized in several columns and several rows). Therefore, instead of defining the frames one by one, we just specify the location of the first frame, the number of frames and the number of columns by row. This is enough to determine the exact position of each frame.

The syntax of a direction is as follows: the direction is enclosed between `{` and `}` and contains some properties declared with the syntax `key = value` and separated by commas. A direction has the following properties:

`x` (number)
: X coordinate of the top-left corner of area containing the frames in the PNG image.

`y` (number)
: Y coordinate of the top-left corner of area containing the frames in the PNG image.

`frame_width` (number)
: Width of each frame in the PNG image.

`frame_height` (number)
: Height of each frame in the PNG image. Thus, the first frame is the rectangle of coordinates (`x,` `y`) and size (`frame_width,` `frame_height`)

`origin_x` (number, optional)
: X coordinate of the [origin point](#origin-point) of the sprite, relative to the upper-left rectangle of a frame. When a sprite is drawn at some coordinates, the origin point determines what exact point of the sprite's rectangle is at those coordinates. It is not necessarily the top-left corner of the rectangle. No values means `0`

`origin_y` (number, optional)
: Y coordinate of the [origin point](#origin-point) of the sprite, relative to the upper-left rectangle of a frame. No values means `0`

`num_frames` (number, optional)
: Number of frames of this direction. Most of the time, this number is the same for all directions. The default number of frames is `1`

`num_columns` (number, optional)
: Number of columns of the grid containing the frames of this direction in the PNG image. The number of rows is then deduced from `num_frames` and `num_columns` Note that the last row of the grid may be incomplete. No value means `num_frames,` meaning that all frames are arranged in the only one row in the PNG image.

## Example

Examples of a sprite animation set definition file:

```lua
-- This animation is actually a fixed image (only one frame, no delay property).
animation {
  name = "stopped",
  src_image = "hero/stopped.tunic.png",
  directions = {
    {
      x =  0,
      y = 0,
      frame_width = 24,
      frame_height = 24,
      origin_x = 12,
      origin_y = 21
    },
    {
      x = 24,
      y = 0,
      frame_width = 24,
      frame_height = 24,
      origin_x = 12,
      origin_y = 21
    },
    {
      x = 72,
      y = 0,
      frame_width = 24,
      frame_height = 24,
      origin_x = 12,
      origin_y = 21
    },
    {
      x = 96,
      y = 0,
      frame_width = 24,
      frame_height = 24,
      origin_x = 12,
      origin_y = 21
    },
  }
}
```

```lua
-- This animation loops on the first frame.
animation {
  name = "walking",
  src_image = "hero/walking.tunic.png",
  frame_delay = 100,
  frame_to_loop_on = 0,
  directions = {
    {
      x = 0,
      y =   0,
      frame_width = 24,
      frame_height = 32,
      origin_x = 12,
      origin_y = 29,
      num_frames = 8
    },
    {
      x = 0,
      y =  32,
      frame_width = 24,
      frame_height = 32,
      origin_x = 12,
      origin_y = 29,
      num_frames = 8
    },
    {
      x = 0,
      y =  96,
      frame_width = 24,
      frame_height = 32,
      origin_x = 12,
      origin_y = 29,
      num_frames = 8
    },
    {
      x = 0,
      y = 128,
      frame_width = 24,
      frame_height = 32,
      origin_x = 12,
      origin_y = 29,
      num_frames = 8
    },
  }
}
```

```lua
-- This animation stops after its last frame (no frame_to_loop_on property).
animation{
  name = "sword",
  src_image = "hero/sword.tunic.png",
  frame_delay = 30,
  directions = {
    {
      x = 0,
      y =  0,
      frame_width = 32,
      frame_height = 32,
      origin_x = 16,
      origin_y = 29,
      num_frames = 12
   },
   {
      x = 0,
      y = 32,
      frame_width = 32,
      frame_height = 32,
      origin_x = 16,
      origin_y = 29,
      num_frames = 12
    },
    {
      x = 0,
      y = 64,
      frame_width = 32,
      frame_height = 32,
      origin_x = 16,
      origin_y = 29,
      num_frames = 12
    },
    {
      x = 0,
      y = 96,
      frame_width = 32,
      frame_height = 32,
      origin_x = 16,
      origin_y = 29,
      num_frames = 12
    },
  }
}
```
