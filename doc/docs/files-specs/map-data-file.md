# Map Definition File

## Overview

The `maps` directory contains the full description and the script of each map.

Maps are areas where the game takes place. They may be rooms, houses, entire dungeon floors, parts of the outside world or any place.

A map with id `xx` is defined with two files:

| File      | Description                                                                                                                                                                                                                                                |
| --------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `xx.dat:` | Definition of all entities of this map (tiles, enemies, non-playing characters, chests, teletransporters, etc.) and the properties of the map. This page will describe the syntax of this file.                                                            |
| `xx.lua:` | Lua script of the map. It defines the dynamic events that happen on your map at runtime, such as opening a door, making enemies appear, moving a non-playing character, etc. See the [map API](../lua-api/map.md) page for more details about map scripts. |

A map is composed of the following information:

- A size in pixels.
- A [tileset](./tileset-data-file.md) (the graphical skin used to draw its tiles).
- A background music.
- The world it belongs to (optional). Allows to groups maps together.
- Its location relative to its world.
- The floor it belongs to (if any).
- All entities of the map (tiles, enemies, chests, pots, switches, etc.), separated in different layers.

All of this is stored in the map data file `xx.dat` We now specify its syntax.

## Syntax of the Map Data File

Solarus Editor fully supports the edition of maps. You should not have to edit map data files by hand unless you know what you are doing.

The sequence of characters `--` (two dashes) marks the beginning of a comment. After them, the rest of the line is ignored by the engine. Empty lines are also ignored.

### Map Properties

The first element in the map data file describes the properties of the map. The definition of these properties starts with `properties{` and ends with `}` Properties are declared with the syntax `key = value` and separated with commas. It is allowed to have an extra comma after the last property. String values should be enclosed within double quotes. The following properties must be defined:

`width` (number)
: Width of the map in pixels. Must be a multiple of 8.
`height` (number)
: Height of the map in pixels. Must be a multiple of 8.
`min_layer` (number)
: Index of the lowest layer (0 or less).
`max_layer` (number)
: Index of the highest layer (0 or less).
`world` (string, optional)
: A name that identifies a group of maps this map belongs to. Worlds allow to group maps together. The world can be any arbitrary name. Maps that have the same non-empty world name are considered to be part of the same environment. The starting location of the savegame is automatically set by the engine when the world changes (not when the map changes). See [`map:get_world()`](../lua-api/map.md#mapget_world) for more details.

`x` (number, optional)
: X coordinate of the top-left corner of the map in its world. Useful to store the location of this map if it belongs to a group of adjacent maps. The engine uses this information to implement scrolling between two adjacent maps. The default value is `0` See [`map:get_location()`](../lua-api/map.md#mapget_location) for more details.

`y` (number, optional)
: Y coordinate of the top-left corner of the map in it world. The default is `0`

`floor` (number, optional)
: The floor of this map if it is part of a floor system. This property is optional. The engine does not do anything particular with the floor, but your quest can use it in scripts, for example to show the current floor on the HUD when it changes or to make a minimap menu. `0` is the first floor, `1` is the second floor, `-1` is the first basement floor, etc.

`tileset` (string)
: Id of the [tileset](./tileset-data-file.md) to use as a skin for this map.

`music` (string, optional)
: Id of the music to play when entering this map. It can be a music file name relative to the `"musics"` directory (without extension), or the special value `"none"` to play no music on this map, or the special value `"same"` to keep the music unchanged. No value means no music.

Example of map properties definition:

```lua
properties {
  width = 2048,
  height = 2048,
  world = "outside",
  x = 0,
  y = 0,
  tileset = "overworld",
  music = "village",
}
```

### Declaration of Map Entities

The rest of the map data file declares all entities (tiles, enemies, chests, etc.) initially present on the map when this map is loaded during the game.

!!! note "Note"

    Recall that during the game, after this loading phase, map entities can also be created or destroyed dynamically using the [Lua map scripting API](../lua-api/map.md).

There exists many types of entities and most of them can be declared in the map data file. Here is their list (if you want all types of map entities, including the ones that cannot be declared in the map data file, see the [map entity API](../lua-api/map-entities/index.md)):

- [Tile](./map-data-file.md#tile): A small brick that composes a piece of the map, with a pattern picked from the [tileset](./tileset-data-file.md).
- [Dynamic tile](./map-data-file.md#dynamic-tile): A special tile that can be enabled or disabled dynamically (usual tiles are optimized away at runtime).
- [Teletransporter](./map-data-file.md#teletransporter): When walking on it, the hero is transported somewhere, possibly on the same map or another map.
- [Destination](./map-data-file.md#destination): A possible destination place for teletransporters.
- [Pickable treasure](./map-data-file.md#pickable-treasure): A treasure placed on the ground and that the hero can pick up.
- [Destructible object](./map-data-file.md#destructible-object): An entity that can be cut or lifted by hero, and that may hide a pickable treasure.
- [Chest](./map-data-file.md#chest): A chest that contains a treasure.
- [Shop treasure](./map-data-file.md#shop-treasure): A treasure that the hero can buy for a price.
- [Enemy](./map-data-file.md#enemy): A bad guy (possibly a boss) who may also drop a pickable treasure when killed.
- [Non-playing character (NPC)](./map-data-file.md#non-playing-character): Somebody or something the hero can interact with.
- [Block](./map-data-file.md#block): An entity that the hero can push or pull.
- [Jumper](./map-data-file.md#jumper): When walking on it, the hero jumps into a direction.
- [Switch](./map-data-file.md#switch): A button or another mechanism that the hero can activate.
- [Sensor](./map-data-file.md#sensor): An invisible detector that detects the presence of the hero.
- [Wall](./map-data-file.md#wall): An invisible object that stops some kinds of entities.
- [Crystal](./map-data-file.md#crystal): A switch that lowers or raises crystal blocks.
- [Crystal block](./map-data-file.md#crystal-block): A low wall that can be lowered (travserable) or raised (obstacle) using a crystal.
- [Stream](./map-data-file.md#stream): When walking on it, the hero automatically moves into a direction.
- [Door](./map-data-file.md#door): A door to open with an equipment item or another condition.
- [Stairs](./map-data-file.md#stairs): Stairs between two maps or to a platform of a single map.
- [Separator](./map-data-file.md#separator): A visual separation between parts of the map.
- [Custom entity](./map-data-file.md#custom-entity): An entity fully customizable, with no built-in behavior.

The definition of a map entity starts with `entity_type{` and ends with `},` where `entity_type` is a type of entity (see below). Inside the braces, the properties of the entity are specified. Properties are declared with the syntax `key = value` and separated with commas. It is allowed to have an extra comma after the last property. String values should be enclosed within double quotes and cannot have line breaks. Table values are enclosed within braces: `{` and `}`and contain elements separated by commas (an extra comma is allowed after the last element).

In each layer, entities are ordered like they are declared in the map data file: the first entity in the map data file is the most to the back, the last one is the most to the front. But [tiles](./map-data-file.md#tile) are always below all other types of entities because they are optimized at runtime. Therefore, tiles are always declared first in the map data file for each layer.

### Common Properties

The following properties exist for all types of entities (with some exceptions for tiles, see below):

`name` (string, optional)
: Name identifying the entity on the map. The name is optional: you will only need it if you need to refer to this entity individually, for example to make a [switch](./map-data-file.md#switch) open a [door](./map-data-file.md#door). If you define a name to an entity, it should be unique in the map. If the name is already used by another entity, a suffix (of the form `"_2"`, `"_3"`, etc.) will be automatically appended to keep entity names unique.

`layer` (number)
: Layer where the entity is on the map.

`x` (number)
: X coordinate of the entity relative to the upper-left corner of the map. The [origin point](../lua-api/map-entities/index.md#entityget_origin) of the entity will be placed at these coordinates (for tiles, it is the upper-left corner).

`y` (number)
: Y coordinate of the entity relative to the upper-left corner of the map.

`enabled_at_start` (boolean, optional)
: Whether the entity should initially be enabled on the map. The default value is `true`.

`locked` (boolean, optional)
: Whether the entity is locked. This is only used in Solarus Editor and has no effect at runtime. The default value is `false`.

`group` (number, optional)
: Group number of the entity. This is only used in Solarus Editor for grouping and has no effect at runtime. The default value is `0`.

`properties` (table, optional)
: Array of additional properties. The engine does nothing special with them, but your scripts can use them to store some custom extra information. Each property is itself a table with the following two elements, where an element has the syntax `x = something`.

`key` (string)
: The name of the property. It must be a valid identifier (only alphanumeric ASCII characters or `'_'`).

`value` (string)
: Value of the property.

`name,` `x,` `y,` `layer,` `enabled_at_start` and `properties`are common to all types of entities, except `name` and `enabled_at_start` that do not exist for [tiles](./map-data-file.md#tile). This is because tiles don't exist individually at runtime for performance reasons.

Example of an entity that includes additional properties:

```lua
sensor {
  layer = 0,
  x = 912,
  y = 453,
  width = 16,
  height = 16,
  properties = {
    { key = "sound",  value = "fanfare" },
    { key = "image", value = "smiley.png" },
  }
}
```

## Map Entities Types

We now detail the specific properties of each type of entity.

### Tile

Tiles are the small fixed bricks that compose the map.

The engine makes a special performance treatment for them. For this reason, this map data file is the only place where they can be declared: they can never be accessed or created at runtime. If you want to access a tile at runtime (typically, to make it appear or disappear), use a [dynamic tile](./map-data-file.md#dynamic-tile) instead.

A tile must be declared in the map data file with `tile{ ... }`.

Additional properties:

`pattern` (string)
: Id of the [tile pattern](./tileset-data-file.md#tile-patterns) to use.

`tileset` (string, optional)
: Id of the [tileset](./tileset-data-file.md) to use. The default value is the tileset of the map.

`width` (number)
: Width of the tile in pixels. It must be a multiple of the width of the pattern. The pattern will be repeated horizontally to fit this width.

`height` (number)
: Height of the tile in pixels. It must be a multiple of the height of the pattern. The pattern will be repeated vertically to fit this height.

Example of tile:

```lua
tile {
  layer = 0,
  x = 1656,
  y = 104,
  width = 96,
  height = 16,
  pattern = "dark_wall",
}
```

### Dynamic Tile

Dynamic tiles are like [tiles](./map-data-file.md#tile), except that they are not optimized at runtime. Therefore, they can be enabled, disabled, created and deleted using the [Lua API](../lua-api/map-entities/dynamic-tile.md).

A dynamic tile can be declared in the map data file with `dynamic_tile{ ... }`.

Additional properties:

`pattern` (string)
: Id of the [tile pattern](./tileset-data-file.md#tile-patterns) to use.

`tileset` (string, optional)
: Id of the [tileset](./tileset-data-file.md) to use. The default value is the tileset of the map.

`width` (number)
: Width of the tile in pixels. It must be a multiple of the width of the pattern. The pattern will be repeated horizontally to fit this width.

`height` (number)
: Height of the tile in pixels. It must be a multiple of the height of the pattern. The pattern will be repeated vertically to fit this height.

Example of dynamic tile:

```lua
dynamic_tile {
  layer = 0,
  x = 552,
  y = 488,
  width = 48,
  height = 8,
  name = "bridge",
  pattern = "bridge_path",
}
```

### Teletransporter

A teletransporter is a detector that sends the hero to another place when he walks on it.

A teletransporter can be declared in the map data file with `teletransporter{ ... }`.

Additional properties:

`width` (number)
: Width of the teletransporter in pixels.

`height` (number)
: Height of the teletransporter in pixels.

`sprite` (string, optional)
: Id of the animation set of a [sprite](./sprite-data-file.md) to create for the teletransporter. No value means no sprite (the teletransporter will then be invisible).

`sound` (string, optional)
: Sound to play when the hero uses the teletransporter. No value means no sound.

`transition` (string, optional)
: Style of transition to play when the hero uses the teletransporter. Must be one of:

    - `"immediate":` No transition.
    - `"fade":` Fade-out and fade-in effect.
    - `"scrolling":` Scrolling between maps. The default value is `"fade"`

`destination_map` (string)
: Id of the map to transport to (can be the id of the current map).

`destination` (string, optional)
: Location on the destination map. Can be the name of a [destination](./map-data-file.md#destination) entity, the special value `"_same"` to keep the hero's coordinates, or the special value `"_side"` to place on hero on the corresponding side of an adjacent map (normally used with the scrolling transition style). No value means the default destination entity of the map.

Example of teletransporter:

```lua
teletransporter {
  layer = 1,
  x = 112,
  y = 600,
  width = 16,
  height = 16,
  transition = "fade",
  destination_map = "dungeon_1_1f",
  destination = "from_outside",
}
```

### Destination

A destination is a possible arrival place for [teletransporters](./map-data-file.md#teletransporter).

A destination can be declared in the map data file with `destination{ ... }`.

Additional properties:

`direction` (number)
: Direction that the hero should take when arriving on the destination, between `0` (East) and `3` (South), or `-1` to keep his direction unchanged.

`sprite` (string, optional)
: Name of the animation set of a [sprite](./sprite-data-file.md) to create for the destination. No value means no sprite (the destination will then be invisible).

`starting_location_mode` (string, optional)
: Whether to update the [starting location](../lua-api/game.md#gameset_starting_locationmap_id-destination_name) of the player when arriving to this destination. If yes, when the player restarts his game, he will restart at this destination. Must be one of:

`"when_world_changes"` (default)
: Updates the starting location if the current [world](../lua-api/map.md#mapget_world) has just changed when arriving to this destination.

    - `"yes":` Updates the starting location.
    - `"no":` Does not update the starting location.

`default` (boolean, optional)
: Sets this destination as the default one when teletransporting the hero to this map without destination specified. No value means `false` Only one destination can be the default one on a map. If no default destination is set, then the lowest one in Z order is set as the default one.

Example of destination:

```lua
destination {
  name = "from_outside",
  layer = 0,
  x = 160,
  y = 37,
  direction = 1,
  default = true,
}
```

### Pickable Treasure

A pickable treasure is a treasure on the ground that the hero can pick up.

A pickable treasure can be declared in the map data file with `pickable{ ... }`.

Additional properties:

`treasure_name` (string, optional)
: Kind of treasure to create (the id of an equipment item). If this value is not set, or corresponds to a non-obtainable item, then the pickable treasure won't be created.

`treasure_variant` (number, optional)
: Variant of the treasure (because some equipment items may have several variants). The default value is `1` (the first variant).

`treasure_savegame_variable` (string, optional)
: Name of the boolean value that stores in the savegame whether this pickable treasure was found. No value means that the treasure is not saved. If the treasure is saved and the player already has it, then the pickable treasure won't be created.

Example of pickable treasure:

```lua
pickable{
  layer = 0,
  x = 216,
  y = 845,
  treasure_name = "rupee",
  treasure_variant = 3,
  treasure_savegame_variable = "castle_rupee_1_found",
}
```

### Destructible Object

A destructible object is an entity that can be cut or lifted by the hero and that may hide a [pickable treasure](./map-data-file.md#pickable-treasure).

A destructible object can be declared in the map data file with `destructible{ ... }`.

Additional properties:

`treasure_name` (string, optional)
: Kind of [pickable treasure](./map-data-file.md#pickable-treasure) to hide in the destructible object (the id of an equipment item). If this value is not set, then no treasure is placed in the destructible object. If the treasure is not obtainable when the object is destroyed, no pickable treasure is be created.

`treasure_variant` (number, optional)
: Variant of the treasure if any (because some equipment items may have several variants). The default value is `1` (the first variant).

`treasure_savegame_variable` (string, optional)
: Name of the boolean value that stores in the savegame whether the [pickable treasure](./map-data-file.md#pickable-treasure) hidden in the destructible object was found. No value means that the treasure (if any) is not saved. If the treasure is saved and the player already has it, then no treasure is put in the destructible object.

`sprite` (string)
: Name of the animation set of a [sprite](../lua-api/drawable-objects/sprite.md) to create for the destructible object.

`destruction_sound` (string, optional)
: Sound to [play](../lua-api/audio/index.md#solaudioplay_soundsound_id) when the destructible object is cut or broken after being thrown. No value means no sound.

`weight` (number, optional)
: Level of `"lift"` [ability](../lua-api/game.md#gameset_abilityability_name-level) required to lift the object. `0` allows the player to lift the object unconditionally. The special value `-1` means that the object can never be lifted. The default value is `0`

`can_be_cut` (boolean, optional)
: Whether the hero can cut the object with the sword. No value means `false`

`can_explode` (boolean, optional)
: Whether the object should explode when it is cut, hit by a weapon and after a delay when the hero lifts it. The default value is `false`

`can_regenerate` (boolean, optional)
: Whether the object should automatically regenerate after a delay when it is destroyed. The default value is `false`

`damage_on_enemies` (number, optional)
: Number of life points to remove from an enemy that gets hit by this object after the [hero](../lua-api/map-entities/hero.md) throws it. If the value is `0,` enemies will ignore the object. The default value is `1`.

`ground` (string, optional)
: Ground defined by this entity. The ground is usually `"wall"`, but you may set `"traversable"` to make the object traversable, or for example `"grass"` to make it traversable too but with an additional grass sprite below the hero. See [`map:get_ground()`](../lua-api/map.md#mapget_groundx-y-layer) for the list of grounds. The default value is `"wall"`

Examples of destructible objects:

```lua
destructible {
  layer = 0,
  x = 232,
  y = 165,
  treasure_name = "heart",
  sprite = "entities/white_stone",
  destruction_sound = "stone",
  weight = 1,
  damage_on_enemies = 2,
}
```

```lua
destructible {
  layer = 0,
  x = 264,
  y = 165,
  treasure_name = "heart",
  sprite = "entities/grass",
  can_be_cut = true,
  ground = "grass",
}
```

### Chest

A chest is a box that contains a treasure.

A chest can be declared in the map data file with `chest{ ... }`.

Additional properties:

`treasure_name` (string, optional)
: Kind of treasure to place in the chest (the name of an equipment item). If this value is not set, then the chest will be empty. If the treasure is not obtainable when the hero opens the chest, it becomes empty.

`treasure_variant` (number, optional)
: Variant of the treasure (because some equipment items may have several variants). The default value is `1` (the first variant).

`treasure_savegame_variable` (string, optional)
: Name of the boolean value that stores in the savegame whether this chest is open. No value means that the state of the treasure is not saved. If the treasure is saved and the player already has it, then no treasure is placed in the chest (the chest will appear open).

`sprite` (string)
: Id of the animation set of the [sprite](./sprite-data-file.md) to create for the chest. The sprite must have animations `"open"` and `"closed"`.

`opening_method` (string, optional)
: Specifies the permissions for the hero to open the chest. Must be one of:

`"interaction"` (default)
: Can be opened by pressing the action command in front of the chest.

    - `"interaction_if_savegame_variable"`: Can be opened by pressing the action command in front of the chest, provided that a specific savegame variable is set.
    - `"interaction_if_item"`: Can be opened by pressing the action command in front of the chest, provided that the player has a specific equipment item.

`opening_condition` (string, optional)
: The condition required to open the chest. Only for opening methods `"interaction_if_savegame_variable"` and `"interaction_if_item"`.

    - For opening method `"interaction_if_savegame_variable"`, it must be the name of a savegame variable. The hero will be allowed to open the chest if this saved value is either `true,` an integer greater than zero or a non-empty string.
    - For opening method `"interaction_if_item"`, it must be the name of an equipment item. The hero will be allowed to open the chest if he has that item and, for items with an amount, if the amount is greater than zero.
    - For the default opening method (`"interaction"`), this setting has no effect.

`opening_condition_consumed` (boolean, optional)
: Whether opening the chest should consume the savegame variable or the equipment item that was required. The default setting is `false` If you set it to `true,` the following rules are applied when the hero successfully opens the chest:

    - For opening method `"interaction_if_savegame_variable"`, the savegame variable that was required is reset to `false,` `0` or `""` (depending on its type).
    - For opening method is `"interaction_if_item"`, the equipment item that was required is removed. This means setting its possessed variant to `0,` unless it has an associated amount: in this case, the amount is decremented.

`cannot_open_dialog` (string, optional)
: Id of the dialog to show if the hero fails to open the chest. If you don't set this value, no dialog is shown.

Example of chest:

```lua
chest {
  layer = 1,
  x = 168,
  y = 645,
  treasure_name = "sword",
  treasure_variant = 2,
  treasure_savegame_variable = "dungeon_6_sword_2_found",
  sprite = "entities/chest",
}
```

### Shop Treasure

A shop treasure is a treasure that can be purchased by the hero for money.

A shop treasure can be declared in the map data file with `shop_item{ ... }`.

Additional properties:

`price` (number)
: Money amount required to buy the treasure.

`font` (string, optional)
: Id of the font to use to display to price. The default value is the first one in alphabetical order.

`dialog` (string)
: Id of the dialog to show when the player asks for information about the treasure.

`treasure_name` (string)
: Kind of treasure to sell (the name of an equipment item). If this value or corresponds to a non-obtainable item, then the shop treasure is not created.

`treasure_variant` (number, optional)
: Variant of the treasure (because some equipment items may have several variants). The default value is `1` (the first variant).

`treasure_savegame_variable` (string, optional)
: Name of the boolean value that stores in the savegame whether the player has purchased this treasure. No value means that the state of the treasure is not saved. If the treasure is saved and the player already has it, then the shop treasure is not created.

Example of shop treasure:

```lua
shop_item {
  layer = 1,
  x = 200,
  y = 104,
  treasure_name = "health_potion",
  price = 160,
  dialog = "witch_shop.health_potion",
}
```

### Enemy

An enemy is a bad guy that hurts the hero when touching him.

An enemy can be declared in the map data file with `enemy{ ... }`.

Additional properties:

`direction` (number)
: Initial direction of the enemy, between `0` (East) and `3` (South).

`breed` (string)
: Model of enemy to create.

`savegame_variable` (string, optional)
: Name of the boolean value that stores in the savegame whether this enemy is dead. No value means that the enemy is not saved. If the enemy is saved and was already killed, then no enemy is created. Instead, its [pickable treasure](./map-data-file.md#pickable-treasure) is created if it is a saved one.

`treasure_name` (string, optional)
: Kind of [pickable treasure](./map-data-file.md#pickable-treasure) to drop when the enemy is killed (the name of an equipment item). If this value is not set, then the enemy won't drop anything. If the treasure is not obtainable when the enemy is killed, then nothing is dropped either.

`treasure_variant` (number, optional)
: Variant of the treasure (because some equipment items may have several variants). The default value is `1` (the first variant).

`treasure_savegame_variable` (string, optional)
: Name of the boolean value that stores in the savegame whether the [pickable treasure](./map-data-file.md#pickable-treasure) of this enemy was obtained. No value means that the state of the treasure is not saved. If the treasure is saved and the player already has it, then the enemy won't drop anything.

Example of enemy:

```lua
enemy {
  layer = 0,
  x = 912,
  y = 453,
  direction = 3,
  breed = "knight_soldier",
  treasure_name = "rupee",
}
```

### Non-Playing Character

A non-playing character (NPC) is somebody or something that the [hero](../lua-api/map-entities/hero.md) can interact with by pressing the action command or by using an equipment item just in front of it.

A non-playing character can be declared in the map data file with `npc{ ... }`.

Additional properties:

`direction` (number)
: Initial direction of the NPC's sprite, between `0` (East) and `3` (South).

`subtype` (number)
: Kind of NPC to create: `1` for a usual NPC who the player can talk to, `0` for a generalized NPC (not necessarily a person). See the [non-playing characters API](../lua-api/map-entities/npc.md) for more details.

`sprite` (string, optional)
: Id of the animation set of a [sprite](./sprite-data-file.md) to create for the NPC. No value means no sprite (the NPC will then be invisible).

`behavior` (string, optional)
: What to do when there is an interaction with the NPC.

    - `"dialog#XXXX":` Start the dialog with id `"XXXX"` when the player talks to this NPC.

`"map"` (default)
: Notify the [map Lua script](../lua-api/map.md).

    - `"item#XXXX":` Notify an [equipment item Lua script](../lua-api/equipment-items.md).

Example of non-playing character:

```lua
npc {
  layer = 0,
  x = 648,
  y = 389,
  direction = 3,
  subtype = "0",
  sprite = "entities/sign",
  behavior = "dialog#outside_world.old_castle_sign",
}
```

### Block

Blocks are solid entities that may be pushed or pulled by the hero.

A block can be declared in the map data file with `block{ ... }`.

Additional properties:

`direction` (number, optional)
: The only direction where the block can be moved, between `0` (East) and `3` (South). `nil` means no restriction and allows the block to be moved in any of the four main directions. The default value is `nil`.

`sprite` (string)
: Name of the animation set of a [sprite](./sprite-data-file.md) to create for the block.

`pushable` (boolean)
: `true` to allow the block to be pushed.

`pullable` (boolean)
: `true` to allow the block to be pulled.

`max_moves` (number, optional)
: `How` many times the block can be moved (`nil` means unlimited). The default value is `nil`.

`maximum_moves` (number, optional, deprecated)
: Like `max_moves,` but for historical reasons, only supports `0,` `1` or the special value `2` to mean infinite. New map files should only use `max_moves` It is an error to set both values.

Example of block:

```lua
block {
  layer = 1,
  x = 584,
  y = 69,
  direction = 3,
  sprite = "entities/block",
  pushable = true,
  pullable = false,
  max_moves = 1,
}
```

### Jumper

A jumper is an invisible detector that makes the hero jump into one of the 8 main directions when touching it.

A jumper can be declared in the map data file with `jumper{ ... }`.

Additional properties:

`width` (number)
: Width of the jumper in pixels.

`height` (number)
: Height of the jumper in pixels.

`direction` (number)
: Direction of the jump, between `0` (East) and `7` (South-East). If the direction is horizontal, the width must be `8` pixels. If the direction is vertical, the height must be `8` pixels. If the direction is diagonal, the size must be square.

`jump_length` (number)
: Length of the baseline of the jump in pixels (see the [jump movement](../lua-api/movements/jump-movement.md) page for details).

Example of jumper:

```lua
jumper {
  layer = 0,
  x = 192,
  y = 824,
  width = 8,
  height = 32,
  direction = 6,
  jump_length = 40,
}
```

### Switch

A switch is a button that can be activated to trigger a mechanism.

A switch can be declared in the map data file with `switch{ ... }`.

Additional properties:

`subtype` (string)
: Kind of switch to create:

    - `"walkable"`: A traversable pressure plate that gets activated when the hero walks on it.
    - `"solid"`: A non-traversable, solid switch that can be activated in various conditions by the sword, by an explosion or by a projectile (a thrown object, an arrow, the boomerang or the hookshot).
    - `"arrow_target"` A switch that can be only activated by shooting an arrow on it.

`sprite` (string)
: Name of the animation set of a [sprite](./sprite-data-file.md) to create for the switch. The animation set must at least contain animations `"activated"` and `"inactivated"` No value means no sprite.

`sound` (string, optional)
: Sound to play when the switch is activated. No value means no sound.

`needs_block` (boolean)
: If `true,` the switch can only be activated by a [block](./map-data-file.md#block) (only for a walkable switch).

`inactivate_when_leaving` (boolean)
: If `true,` the switch becomes inactivated when the hero or the block leaves it (only for a walkable switch).

Example of switch:

```lua
switch {
  name = "open_door_2_switch",
  layer = 1,
  x = 376,
  y = 152,
  subtype = "walkable",
  sprite = "entities/switch",
  sound = "switch_pressed",
  needs_block = false,
  inactivate_when_leaving = true,
}
```

### Sensor

A sensor is an invisible detector that triggers something when the hero overlaps it.

A sensor can be declared in the map data file with `sensor{ ... }`.

Additional properties:

`width` (number)
: Width of the sensor in pixels.

`height` (number)
: Height of the sensor in pixels.

Example of sensor:

```lua
sensor {
  name = "start_boss_sensor",
  layer = 0,
  x = 392,
  y = 301,
  width = 16,
  height = 16,
}
```

### Wall

A wall is an invisible obstacle that stops some specific types of map entities.

A wall can be declared in the map data file with `wall{ ... }`.

Additional properties:

`width` (number)
: Width of the wall in pixels.

`height` (number)
: Height of the wall in pixels.

`stops_hero` (boolean, optional)
: `true` to make the wall stop the hero. No value means `false`

`stops_npcs` (boolean, optional)
: `true` to make the wall stop [non-playing characters](./map-data-file.md#non-playing-character). No value means `false`

`stops_enemies` (boolean, optional)
: `true` to make the wall stop [enemies](./map-data-file.md#enemy). No value means `false`

`stops_blocks` (boolean, optional)
: `true` to make the wall stop [blocks](./map-data-file.md#block). No value means `false`

`stops_projectiles` (boolean, optional)
: `true` to make the wall stop projectiles: thrown objects, arrows, the hookshot and the boomerang. No value means `false`

Example of wall:

```lua
wall {
  layer = 1,
  x = 416,
  y = 256,
  width = 24,
  height = 8,
  stops_hero = false,
  stops_enemies = true,
  stops_npcs = true,
  stops_blocks = true,
  stops_projectiles = true,
}
```

### Crystal

A crystal is a switch that lowers or raises alternatively some special colored blocks in the ground called [crystal blocks](./map-data-file.md#crystal-block).

A crystal can be declared in the map data file with `crystal{ ... }`.

Additional properties: none.

Example of crystal:

```lua
crystal{
  layer = 0,
  x = 176,
  y = 965,
}
```

### Crystal Block

A crystal block is a colored low wall that may be raised or lowered in the ground.

A crystal block can be declared in the map data file with `crystal_block{ ... }`.

Additional properties:

`width` (number)
: Width of the crystal block in pixels (must be a multiple of 16 pixels). The crystal block pattern will be repeated to fit the size.

`height` (number)
: Height of the crystal block in pixels (must be a multiple of 16 pixels). The crystal block pattern will be repeated to fit the size.

`subtype` (number)
: Kind of crystal block to create: `0` for a block initially lowered (orange), `1` for a block initially raised (blue).

Example of crystal block:

```lua
crystal_block {
  layer = 0,
  x = 120,
  y = 856,
  width = 112,
  height = 16,
  subtype = 1,
}
```

### Stream

When walking on a stream, the hero automatically moves into one of the 8 main directions. The stream can allow or not the hero to move and use items. Streams can be used for example to make conveyor belts, water streams or air streams.

A stream can be declared in the map data file with `stream{ ... }`.

Additional properties:

`direction` (number)
: Direction where the stream moves the hero, between `0` (East) and `7` (South-East).

`sprite` (string, optional)
: Id of the animation set of a [sprite](./sprite-data-file.md) to create for the stream. No value means no sprite (the stream will then be invisible).

`speed` (number, optional)
: Speed of the movement applied to the hero by the stream, in pixels per second. The default value is `64` A speed of `0` means that the hero will never be moved by the stream.

`allow_movement` (boolean, optional)
: Whether the player can still move the hero when he is on the stream. The default value is `true`.

`allow_attack` (boolean, optional)
: Whether the player can use the sword when he is on the stream. The default value is `true`.

`allow_item` (boolean, optional)
: Whether the player can use equipment items when he is on the stream. The default value is `true`.

Example of stream:

```lua
stream {
  layer = 1,
  x = 200,
  y = 157,
  direction = 5,
  sprite = "entities/air_stream",
  allow_movement = false,
  allow_attack = true,
  allow_item = true,
}
```

### Door

A door is an obstacle that can be opened by Lua scripts and optionally by the hero under some conditions.

A door can be declared in the map data file with `door{ ... }`.

Additional properties:

`direction` (number)
: Direction of the door, between `0` (East of the room) and `3` (South of the room).

`sprite` (string)
: Id of the animation set of the [sprite](./sprite-data-file.md) to create for the door. The sprite must have an animation `"closed"`, that will be shown while the door is closed. When the door is open, no sprite is displayed. Optionally, the sprite can also have animations `"opening"` and `"closing"`, that will be shown (if they exist) while the door is being opened or closed, respectively. If they don't exist, the door will open or close instantly.

`savegame_variable` (string, optional)
: Name of the boolean value that stores in the savegame whether this door is open. No value means that the door is not saved. If the door is saved as open, then it appears open.

`opening_method` (string, optional)
: How the door is supposed to be opened by the player. Must be one of:

    - `"none"` (default) : Cannot be opened by the player. You can only open it from a Lua script.
    - `"interaction":` Can be opened by pressing the action command in front of the door.
    - `"interaction_if_savegame_variable":` Can be opened by pressing the action command in front of the door, provided that a specific savegame variable is set.
    - `"interaction_if_item":` Can be opened by pressing the action command in front of the door, provided that the player has a specific equipment item.
    - `"explosion":` Can be opened by an explosion.

`opening_condition` (string, optional)
: The condition required to open the door. Only for opening methods `"interaction_if_savegame_variable"` and `"interaction_if_item"`.

    - For opening method `"interaction_if_savegame_variable"`, it must be the name of a savegame variable. The hero will be allowed to open the door if this saved value is either `true,` an integer greater than zero or a non-empty string.
    - For opening method `"interaction_if_item"`, it must be the id of an equipment item. The hero will be allowed to open the door if he has that item and, for items with an amount, if the amount is greater than zero.
    - For other opening methods, this setting has no effect.

`opening_condition_consumed` (boolean, optional)
: Whether opening the door should consume the savegame variable or the equipment item that was required. The default setting is `false` If you set it to `true,` the following rules are applied when the hero successfully opens the door:

    - For opening method `"interaction_if_savegame_variable"`, the savegame variable that was required is reset to `false,` `0` or `""` (depending on its type).
    - For opening method is `"interaction_if_item"`, the equipment item that was required is removed. This means setting its possessed variant to `0,` unless it has an associated amount: in this case, the amount is decremented.
    - With other opening methods, this setting has no effect.

`cannot_open_dialog` (string, optional)
: Id of the dialog to show if the hero fails to open the door. If you don't set this value, no dialog is shown.

Example of door:

```lua
door {
  layer = 1,
  x = 872,
  y = 184,
  direction = 2,
  sprite = "entities/door_small_key",
  savegame_variable = "dungeon_6_locked_door_1_open",
  opening_method = "interaction_if_savegame_variable",
  opening_condition = "dungeon_6_small_keys",
  opening_condition_consumed = true,
  cannot_open_dialog = "small_key_required",
}
```

### Stairs

Stairs make fancy animations, movements and sounds when the hero takes them between two maps or to a platform of a single map.

Stairs can be declared in the map data file with `stairs{ ... }`.

Additional properties:

`direction` (number)
: Direction where the stairs should be turned between `0` (East of the room) and `3` (South of the room). For stairs inside a single floor, this is the direction of going upstairs.

`subtype` (number)
: Kind of stairs to create:

    - `0:` Spiral staircase going upstairs.
    - `1:` Spiral staircase going downstairs.
    - `2:` Straight staircase going upstairs.
    - `3:` Straight staircase going downstairs.
    - `4:` Small stairs inside a single floor (change the layer of the hero).

Example of stairs:

```lua
stairs {
  layer = 0,
  x = 384,
  y = 272,
  direction = 3,
  subtype = "1",
}
```

### Separator

Separators allow to visually separate different regions of a map like if they were several maps. When the camera touches the separation, it stops like if there was a limit of a map. If the hero touches the separation, he scrolls to the other side.

A separator can be declared in the map data file with `separator{ ... }`.

Additional properties:

`width` (number)
: Width of the separator in pixels.

`height` (number)
: Height of the separator in pixels. One of `width` or `height` must be 16 pixels.

Example of separator:

```lua
separator {
  layer = 2,
  x = 320,
  y = 0,
  width = 16,
  height = 1280,
}
```

### Custom entity

Custom entities have no special properties or behavior. You can define them entirely in your scripts. Optionally, a custom entity may be managed by model. The model is the name of a Lua script that will be applied to all custom entities referring to it. This works exactly like the breed of enemies, except that it is optional. The model is useful if you have a lot of identical (or very similar) custom entities in your game, like for example torches.

A custom entity can be declared in the map data file with `custom_entity{ ... }`.

Additional properties:

`direction` (number)
: Direction of the custom entity, between `0` (East) and `3` (South). This direction will be applied to the entity's sprite if possible.

`width` (number)
: Width of the entity in pixels.

`height` (number)
: Height of the entity in pixels.

`origin_x` (number, optional)
: X coordinate of the origin point, relative to the entity's upper left corner. The default origin is `8, 13`.

`origin_y` (number, optional)
: Y coordinate of the origin point, relative to the entity's upper left corner. The default origin is `8, 13`.

`sprite` (string, optional)
: Id of the animation set of a [sprite](./sprite-data-file.md) to create for the custom entity. No value means no sprite (the custom entity will then be invisible).

`tiled` (boolean, optional)
: Whether to repeat the sprite with tiling to fit the size of the custom entity, when the size is bigger than the sprite. No value means `false`

`model` (string, optional)
: Model of custom entity to create. The model is the name of a Lua script in the `"entities"` directory of your quest. It will define the behavior of your entity. No value means no model: in this case, no particular script will be called but you can still define the behavior of your entity in the map script.

Examples of custom entities:

```lua
-- A cannon that exists only at this place of the game.
custom_entity {
  name = "cannon,
  layer = 0,
  x = 496,
  y = 232,
  width = 16,
  height = 16,
  sprite = "entities/cannon",
}
```

```lua
-- A minecart.
-- Managed by a model because several minecarts may exist in the game.
custom_entity {
  layer = 0,
  x = 152,
  y = 272,
  width = 16,
  height = 16,
  model = "minecart",
}
```
