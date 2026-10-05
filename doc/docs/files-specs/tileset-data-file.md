# Tileset definition file

## Overview

Maps are composed of small bricks called tiles. Tiles are static entities: they cannot be moved or removed dynamically during the game. They are opposed to all dynamic entities such as enemies, doors, bushes, switches, etc. Each tile is a graphical element whose width and height are multiple of 8 pixels. When placed on a map, tiles can have different sizes and can overlap each other.

A tileset is a list of patterns that tiles on a map can use. You can call it the skin of the map. For example, there may exist a tileset for the outside world, another one for houses, another one for dungeons, etc.

The `tilesets` directory contains all tilesets and their images. For a tileset with id `xx,`there are three files:

- `xx.tiles.png:` The PNG image containing all tile patterns of this tileset.
- `xx.dat:` The definition of each tile pattern in `xx.tiles.png` and their properties. We detail the syntax of this file below.

`xx.entities.png` (optional)
: A PNG image containing sprite sheets of dynamic entities (i.e. entities other that tiles) whose graphics depend on the tileset. For instance, doors and block can have different colors for each tileset. This optional PNG file allows to more easily make [sprites](./sprite-data-file.md) that depend on the current tileset.

Tile patterns defined in `xx.tiles.png` can be fixed or animated with several frames. Examples of uses of multi-frame patterns are water, flowers and lava.

Tile patterns can also have special scrolling effects like parallax scrolling.

## Syntax of the Tileset Data File

Solarus Editor fully supports the edition of tilesets. You should not have to edit tileset data files by hand unless you know what you are doing.

We now specify the syntax of a tileset data file.

The sequence of characters `--` (two dashes) marks the beginning of a comment. After them, the rest of the line is ignored by the engine. Empty lines are also ignored.

### Background Color

The first element in the tileset file is the background color of the maps using this tileset. The definition of this background color starts with `background_color{` and ends with `}` Inside the braces, the background color is specified in RGB format: three integers between 0 and 255, separated by commas. It is allowed to have an extra comma after the last integer value.

Example of background color definition:

```lua
background_color { 104, 184, 104 }
```

### Tile Patterns

After the background color definition, the tileset data file defines all tile patterns of the tileset. The definition of a tile pattern starts with `tile_pattern{` and ends with `}` Inside the braces, the properties of the tile pattern are specified. Properties are declared with the syntax `key = value` and separated with commas. It is allowed to have an extra comma after the last property. String values should be enclosed within double quotes. Each tile pattern must have the following properties:

`id` (string)
: Id of the tile pattern. Maps refer to this id when they want to use the tile pattern. It must be unique among tile patterns in the tileset.

`ground` (string)
: Kind of ground. Must be one of:

    - `"empty":` There is no ground, this tile is purely decorative. Keep the ground of the tile below instead (if any).
    - `"traversable":` Normal ground without obstacle.
    - `"wall":` Cannot be traversed.
    - `"low_wall":` Cannot only be traversed by projectiles like thrown items, arrows, the boomerang or flying enemies.
    - `"wall_top_right":` The upper-right half of the tile is an obstacle.
    - `"wall_top_left":` The upper-left halt of the tile is an obstacle.
    - `"wall_bottom_left":` The lower-left half of the tile is an obstacle.
    - `"wall_bottom_right":` The lower-right half of the tile is an obstacle.
    - `"wall_top_right_water":` The upper-right half of the tile is an obstacle, the rest is deep water.
    - `"wall_top_left_water":` The upper-left half of the tile is an obstacle, the rest is deep water.
    - `"wall_bottom_left_water":` The lower-left half of the tile is an obstacle, the rest is deep water.
    - `"wall_bottom_right_water":` The lower-right half of the tile is an obstacle, the rest is deep water.
    - `"deep_water":` Deep water (the hero drowns or swims).
    - `"shallow_water":` Shallow water (the hero can walk on it).
    - `"grass":` Displays some grass under the hero.
    - `"hole":` A hole (the hero falls into it).
    - `"ice":` Ice (the hero slides).
    - `"ladder":` A ladder (the hero walks slowly on it).
    - `"prickles":` Untraversable spikes (it hurts the hero).
    - `"lava":` Lava (the hero drowns and gets hurt).

`default_layer` (number)
: The initial layer to be used when creating a tile with this pattern on a map with a map editor. This information is only useful for map editors (the engine does not use it).

`x` (number or table)
: X coordinate(s) of the upper-left corner of the pattern's rectangle(s) in the tileset image file. The rectangle of a tile pattern must not overlap other tile patterns in the tileset. If it is a number, the tile pattern will be a single-frame tile pattern. If it is a table, it must have the syntax `{ x1, x2, … }` and the tile pattern will be a multi-frame tile pattern (an animated one). Multi-frame tile patterns play a sequence of successive frames. The successive rectangles of multi-frame tile patterns must be adjacent in the tileset image, and placed horizontally (from left to right) or vertically (from top to bottom).

`y` (number or table)
: Y coordinate(s) of the upper-left corner of the pattern's rectangle(s) in the tileset image file. Same rules as `x` Note that in the case of a multi-frame tile pattern, the table must have the same number of elements as `x,` and that due to the horizontal or vertical placement of the frames in the tileset image, one of `x` or `y` will necessarily be a table with identical values.

`width` (number)
: Width of the pattern's rectangle(s) in pixels in the tileset image. In the case of a multi-frame tile pattern, this is the width of an individual rectangle. All rectangles have the same size. Note that there is a redundancy: since the rectangles must be adjacent, the size is actually implied by `x` and `y`.

`height` (number)
: Height of the pattern's rectangle(s) in pixels in the tileset image. In the case of a multi-frame tile pattern, this is the height of an individual rectangle. All rectangles have the same size. Note that there is a redundancy: since the rectangles must be adjacent, the size is actually implied by `x` and `y`.

`frame_delay` (number, optional)
: Delay in milliseconds between two frames. Only allowed for multi-frame patterns. The default value is `250` ms.

`mirror_loop` (boolean, optional)
: For animated patterns, whether to continue the animation in reverse after the last frame is played. Only allowed for multi-frame patterns. If `false` (the default), the animation repeats normally from the first frame (for example with 4 frames: 1→2→3→4→1→2→3→4…). If `true,` the animation is played backwards before it repeats (for example with 4 frames: 1→2→3→4→3→2→1→2→3→4→3→2…).

`scrolling` (string, optional)
: Applies a scrolling effect on the tile pattern. No value means no special scrolling. If this value is defined, it must be one of:

    - `"parallax":` Parallax scrolling. When the camera moves, the place where the pattern is displayed also moves, but twice slower, to give an illusion of depth. Placing such tiles on your map can be tricky because at runtime, they are displayed somewhere else (except when the camera is at the upper-left corner of the map). Note that collisions properties remain applied to the real position of the tiles and not to where they are displayed. Usually, parallax scrolling is used for decoration only. Parallax scrolling tile pattern can also be multi-frame. Example of use : a rich background panorama.

    - `"self"`: When the camera moves, the pattern scrolls on itself, but twice slower than the camera. This achieves the same effect as parallax scrolling, but the tile remains displayed at its position. However, this is limited to simple, repeatable patterns  you cannot make big scrolling backgrounds composed of different tiles (use parallax patterns for this) because nothing actually moves with the camera, tiles just scroll on themselves. Self-scrolling tile patterns cannot be multi-frame. Example of use: holes in dungeons.

`repeat_mode` (string, optional)
: How the pattern is supposed to be repeatable. This property is only useful for map editors (the engine does not use it). Map editors can take advantage of it to resize patterns only in the way they are intended. It can be one of:

`"all"` (default)
: The pattern can be repeated both horizontally and vertically.

    - `"horizontal":` The pattern can only be repeated horizontally.
    - `"vertical":` The pattern can only be repeated vertically.
    - `"none":` The pattern cannot be repeated.

Example of a tile pattern:

```lua
tile_pattern {
  id = "wall.1",
  ground = "wall",
  default_layer = 0,
  x = 64,
  y = 48,
  width = 24,
  height = 16,
  repeat_mode = "vertical",
}
```

### Border Sets

After the definition of all tile patterns, the tileset data file may contain the definition of border sets. Border sets are useful for auto-tiling. Solarus Editor uses them to allow the user to automatically generate borders around some selected entities in the map editor. Border sets currently have no effect in the engine itself: they are only used by Solarus Editor and they generate regular tiles.

A border set is a list of 12 tile pattern ids that correspond to 4 edges, 4 convex corners and 4 concave corners.

The definition of a border set starts with `border_set{`and ends with `}` Inside the braces, the properties of the border set are specified. Properties are declared with the syntax `key = value` and separated with commas. It is allowed to have an extra comma after the last property. String values should be enclosed within double quotes. Each border set has the following properties:

`id` (string)
: Id of the border set. It must be unique among border sets in the tileset.

`inner` (boolean, optional)
: Whether borders should be generated inside the current selection or outside it in Solarus Editor. No value means `false`

`pattern_0` (string, optional)
: Id of the East border pattern.

`pattern_1` (string, optional)
: Id of the North border pattern

`pattern_2` (string, optional)
: Id of the West border pattern.

`pattern_3` (string, optional)
: Id of the South border pattern.

`pattern_4` (string, optional)
: Id of the North-East convex corner pattern.

`pattern_5` (string, optional)
: Id of the North-West convex corner pattern.

`pattern_6` (string, optional)
: Id of the South-West convex corner pattern.

`pattern_7` (string, optional)
: Id of the South-East convex corner pattern.

`pattern_8` (string, optional)
: Id of the North-East concave corner pattern.

`pattern_9` (string, optional)
: Id of the North-West concave corner pattern.

`pattern_10` (string, optional)
: Id of the South-West concave corner pattern.

`pattern_11` (string, optional)
: Id of the South-East concave corner pattern.

The properties `pattern_0` to `pattern_11,` if defined, should all be ids of existing patterns in this tileset. These properties are all optional: if some of them are unset, it means that no border tile should be generated for the corresponding edges or corners. It is possible that several of these properties refer to the same pattern: it means that the same pattern can be used for several different edges or corners.

Example of a border set:

```lua
border_set {
  id = "wall",
  pattern_0 = "wall.4",
  pattern_1 = "wall.1",
  pattern_2 = "wall.3",
  pattern_3 = "wall.2",
  pattern_4 = "wall.corner.2",
  pattern_5 = "wall.corner.1",
  pattern_6 = "wall.corner.3",
  pattern_7 = "wall.corner.4",
  pattern_8 = "wall.corner_reverse.2",
  pattern_9 = "wall.corner_reverse.1",
  pattern_10 = "wall.corner_reverse.3",
  pattern_11 = "wall.corner_reverse.4",
}
```

### Tileset Data File Example

Here is an example of a valid tileset file:

```lua
background_color { 104, 184, 104 }

tile_pattern {
  id = "path_dirt",
  ground = "traversable",
  default_layer = 0,
  x = 32,
  y = 0,
  width = 16,
  height = 16,
}

tile_pattern {
  id = "ocean",
  ground = "deep_water",
  default_layer = 0,
  x = { 0, 8, 16 },
  y = { 32, 32, 32 },
  width = 8,
  height = 8,
}

tile_pattern {
  id = "parallax_tree",
  ground = "traversable",
  default_layer = 0,
  x = 320,
  y = 448,
  width = 16,
  height = 24,
  scrolling = "parallax",
}
```
