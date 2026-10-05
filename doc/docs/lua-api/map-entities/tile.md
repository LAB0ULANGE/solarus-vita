# Tile

## Overview

Tiles are the small fixed bricks that compose the [map](../map.md).

Each tile has a pattern coming from the tileset of the map. A tile has an adjustable size: you can repeat that pattern several times instead of placing many times the same tile on the map. Tiles can also overlap each other.

For some reasons explained below, tiles are not accessible from Lua. When you need to access a tile from Lua (typically, to make it appear or disappear), use a [dynamic tile](./dynamic-tile.md) instead.

### Tiles are designed for performance

Tiles are fixed: they are only declared in the map data file. Actually, most of them don't even exist at runtime. You cannot create them dynamically: unlike other types of map entities, there is no method `map:create_tile()`. You cannot access them dynamically either.

Most tiles don't exist individually at runtime, because the engine makes a special performance treatment to them. When the map is loaded, the engine builds once for all, for each layer, a big image of all tiles (except animated tiles that have to be displayed separately). It also builds once for all the obstacle property of every 8×8 square that compose each layer. Then, drawing the tiles or detecting whether an entity collides with obstacle tiles is very fast. You can declare a huge number of tiles on your map without degrading performances at runtime.

!!! note "Note"

    There exists situations where you want to create, remove, enable or disable a tile from your script. [Dynamic tiles](./dynamic-tile.md) are made for that. Dynamic tiles are like normal tiles, except that they are not optimized as explained above: they do exist at runtime. You can access them from your script and call methods such as [`dynamic_tile:set_enabled()`](./index.md#entityset_enabledenabled).
