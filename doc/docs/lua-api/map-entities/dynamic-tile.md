# Dynamic Tile

## Overview

Dynamic tiles are [tiles](./tile.md) that can be hidden, shown, created and deleted at runtime.

This type of [map entity](./index.md) can be declared in the [map data file](../map.md#map-files). It can also be created dynamically with [`map:create_dynamic_tile()`](../map.md#mapcreate_dynamic_tileproperties).

Dynamic tiles exist because normal [tiles](./tile.md) are optimized away at runtime. See [tile performances](./tile.md#tiles-are-designed-for-performance) for more details.

A typical usage of dynamic tiles is to make appear of disappear parts of the map because something happens: a puzzle is solved, an [non-playing characters](./npc.md) opens a path for you, water disappears, etc. Use [`dynamic_tile:set_enabled()`](./index.md#entityset_enabledenabled) to this end.

!!! note "Note"

    Dynamic tiles can get enabled and disabled automatically to reflect the state of a [door](./door.md). If you give the appropriate name to your dynamic tile, the engine will enable or disable it depending on whether the door is open or closed. See the documentation of [`map:open_doors()`](../map.md#mapopen_doorsprefix) for more information.

## Methods Inherited from `entity`

Dynamic tiles are particular [map entities](./index.md). Therefore, they inherit all methods from the type map entity.

See [entity](./index.md#methods-of-all-entity-types) to know these methods.

## Methods of the type dynamic tile

The following methods are specific to dynamic tiles.

### `dynamic_tile:get_pattern_id()`

Returns the id of the tile pattern of this dynamic tile.

Return value (string)
: The tile pattern id in the tileset.

### `dynamic_tile:get_tileset()`

Returns the id of the tileset of this dynamic tile.

Return value (string or `nil`)
: The tileset id, or `nil` if no tileset is set for the dynamic tile (in this case, the tileset of the map is used).

### `dynamic_tile:set_tileset(tileset_id)`

Sets the tileset of this dynamic tile.

`tileset_id` (string or `nil`)
: The tileset id, or `nil` to clear the tileset for the dynamic tile (in this case, the tileset of the map is used).

### `dynamic_tile:get_modified_ground()`

Returns the ground defined by this dynamic tile on the map.

The presence of a dynamic tile can modify the ground of the map. This is determined by the ground property of the tile pattern.

Return value (string)
: The ground defined by this dynamic tile. `"empty"` means that this dynamic tile does not modify the ground of the map. See [`map:get_ground()`](../map.md#mapget_groundx-y-layer) for the list of possible grounds.

## Events Inherited from `entity`

Events are callback methods automatically called by the engine if you define them.

Dynamic tiles are particular [map entities](./index.md). Therefore, they inherit all events from the type map entity.

See [entity](./index.md#events-of-all-entity-types) to know these events.

## Events of the type dynamic tile

None.
