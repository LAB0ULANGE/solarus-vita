# Map

Maps are areas where the [game](./game.md) takes place. They may be rooms, houses, entire dungeon floors, parts of the outside world or any place. The active map contains many objects called [map entities](./map-entities/index.md) (or just "entities" to be short). Map entities are everything that has a position on the map, including the [hero](./map-entities/hero.md), the [tiles](./map-entities/tile.md), the [enemies](./map-entities/enemy.md), the [pickable treasures](./map-entities/pickable.md) and even the [camera](./map-entities/camera.md). See the [entity API](./map-entities/index.md) for more details.

## Overview

### Coordinates and layer

A map has a rectangular size in pixels. The width and the height are always multiples of 8 pixels, and most [map entities](./map-entities/index.md) usually stay aligned on a grid of squares of 8x8 pixels (except when they are moving).

Thus, each [entity](./map-entities/index.md) has some coordinates `X,Y` on the map. But its position is also defined by a third value: its layer. The map has a number of distinct layers that are stacked. Each layer has its own list of entities. Layers are identified by a number. There is always a layer `0`, and maps can add additional layers above or below `0`

Layers allow to implement maps with multi-level content, like a bridge between two higher platforms. The [hero](./map-entities/hero.md) (as well as [enemies](./map-entities/enemy.md) and any other [map entity](./map-entities/index.md)) is then able to walk either above or under the bridge, depending on his layer. Entities like [stairs](./map-entities/stairs.md) and [jumpers](./map-entities/jumper.md) can change the layer of the hero automatically, but you can also do that from your Lua scripts.

### Tileset

The graphic skin of a map is called a [tileset](../files-specs/tileset-data-file.md). The tileset defines the small patterns used to draw [tiles](./map-entities/tile.md) and also some other [entities](./map-entities/index.md) that may depend on the skin. For example, you can have a forest tileset, a castle tileset, etc. Tilesets have no special size limitation. It is possible to use multiple tilesets in the same map.

### Map files

A map can contain many types of [entities](./map-entities/index.md). Map entities can be either declared in the map data file or created dynamically. Thus, a map with id `XXXX` is managed by two distinct files:

- In the data file `maps/XXXX.dat` are declared all entities that initially compose your map (most importantly: the [tiles](./map-entities/tile.md)). You normally don't need to edit this file by hand: you can use Solarus Editor (but if you want to, [here is the syntax](../files-specs/map-data-file.md)).

- In the script file `"maps/XXXX.lua"` (which is optional), you define the dynamic behavior of your map, using the features of the `map` Lua type described on this page (and the rest of the Solarus API will also be very useful).

When the player enters a map, the engine first creates the [entities](./map-entities/index.md) declared in the map data file [maps/XXXX.dat](../files-specs/map-data-file.md), and then it runs your script `"maps/XXXX.lua"` The Lua map object is passed as parameter of your script (remember that any Lua script is implicitly a function and can have parameters). Use the Lua notation `...` to get this parameter and store it into a regular variable.

Here is a basic example of script for a map that does nothing special except playing a special music and showing an evil welcome dialog when the hero enters it.

```lua
-- First, we put the parameter into a variable called "map".
-- (In Lua, the notation "..." refers to the parameter(s) of the script.)
local map = ...
local game = map:get_game()

-- Event called when the player enters the map, at initialization time.
function map:on_started()

  -- Play an evil music.
  sol.audio.play_music("evil")
end

-- Event called when the player takes control, after the opening transition.
function map:on_opening_transition_finished()

  -- Show an evil welcome dialog.
  game:start_dialog("welcome_to_darkness")
end
```

In practice, many maps have short scripts like this or even no script at all. Sometimes, everything that you define in the data file (with the help of Solarus Editor) is enough. You need a script when there is something dynamic to program on your map: opening a [door](./map-entities/door.md) when a [pressure plate](./map-entities/switch.md) is pressed, making a [treasure chest](./map-entities/chest.md) appear when some [enemies](./map-entities/enemy.md) are killed, showing a dialog with an [NPC](./map-entities/npc.md) that depends on whether the player has accomplished a particular action before, etc.

### Lifetime of maps

At any moment, during the [game](./game.md), only one map is active: the one drawn on the game screen. That map is also the one where the [hero](./map-entities/hero.md) is currently located. Only the active map has its entities living in it. Other maps are not loaded. If the player leaves the map and comes back later, your Lua map object will be a new one the second time. Your entities will be reloaded as specified in the [map data file](../files-specs/map-data-file.md) and your map script will be executed again.

If you want some data or behavior to persist when the player comes back, like the position of an [NPC](./map-entities/npc.md) or whether a puzzle was solved, you can store the information into the [savegame](./game.md#gameset_valuesavegame_variable-value). Alternatively, if you don't want these data to be saved, but you want to remember then during the current [game](./game.md) session, you can store the information as a field of the [game](./game.md) object. The game object persists until the player stops playing and comes back to the title screen or restarts the game.

!!! note "Note"

    Some entities save their state automatically if you want, like whether a [treasure chest](./map-entities/chest.md) is open, whether a [door](./map-entities/door.md) is open or whether an [enemy](./map-entities/enemy.md) is killed. Therefore, you have nothing to do for such entities: they keep their state.

### Accessing maps like tables

Like other essential Solarus types (including [game](./game.md), [items](./equipment-items.md) and [map entities](./map-entities/index.md)), a fundamental property of maps is that even if they are userdata, they can also be used like Lua tables.

This property is actually what allows you to define events on your map. But you can store any data in your map object, including new functions specific to your particular map. Here is an example that uses this feature:

```lua
-- Example of a map with 3 switches to activate in the correct order.
-- We assume that 3 switches exist with names "puzzle_switch_1", "puzzle_switch_2" and "puzzle_switch_3".
local map = ...
local game = map:get_game()

function map:on_started()
  map.next_switch_index = 1
  -- Equivalent to: map["next_switch_index"] = 1
end

-- Called when the player walks on the switch named "puzzle_switch_1".
function puzzle_switch_1:on_activated()
  -- Check that puzzle_switch_1 is the correct switch to activate.
  map:check_switch(self)
end

function puzzle_switch_2:on_activated()
  map:check_switch(self)
end

function puzzle_switch_3:on_activated()
  map:check_switch(self)
end

function map:check_switch(switch)
  if switch:get_name() == "puzzle_switch_" .. map.next_switch_index then
    -- Okay so far.
    map.next_switch_index = map.next_switch_index + 1
    if map.next_switch_index > 3 then
      -- Finished!
      sol.audio.play_sound("secret")
      game:start_dialog("congratulations")
    end
  else
    -- Wrong switch: reset the puzzle.
    sol.audio.play_sound("wrong")
    puzzle_switch_1:set_activated(false)
    puzzle_switch_2:set_activated(false)
    puzzle_switch_3:set_activated(false)
    map.next_switch_index = 1
  end
end
```

In this example, you see that we add three values to the map object, like if it was a table: `next_switch_index` (a number), `on_started` (a function) and `check_switch` (a function). Actually, we also use this feature on the three switches as well: we add a value `on_activated` on them.

!!! note "Note"

    You may wonder how we can access `puzzle_switch_1`,`puzzle_switch_2`and `puzzle_switch_3` without declaring them. There is a mechanism that makes all named [map entities](./map-entities/index.md) directly accessible in the environment of the map script. See [`map:get_entity()`](#mapget_entityname) for more details. But this example has some issues. The three [switches](./map-entities/switch.md) are managed by duplicated code. This is error-prone because one day, we will probably want to make a similar puzzle with 50 entities instead of just 3. To make your map easier to maintain, the following and equivalent version is much more preferred:

    ```lua
    local map = ...
    local game = map:get_game()

    function map:on_started()
      map.next_switch_index = 1
      map.nb_switches = map:get_entities_count("puzzle_switch_")
    end

    local function puzzle_switch_activated(switch)
      map:check_switch(switch)
    end

    -- Define the same on_activated() method for all entities whose name
    -- starts with "puzzle_switch_".
    for switch in map:get_entities("puzzle_switch_") do
      switch.on_activated = puzzle_switch_activated
    end

    function map:check_switch(switch)
      if switch:get_name() == "puzzle_switch_" .. map.next_switch_index then
        -- Okay so far.
        map.next_switch_index = map.next_switch_index + 1
        if map.next_switch_index > map.nb_switches then
          -- Finished!
          sol.audio.play_sound("secret")
          game:start_dialog("congratulations")
        end
      else
        -- Wrong switch: reset the puzzle.
        sol.audio.play_sound("wrong")
        for s in map:get_entities("puzzle_switch_") do
          s:set_activated(false)
        end
        map.next_switch_index = 1
      end
    end
    ```

    This improved version is more evolutive: it does not even hardcode the number of [switches](./map-entities/switch.md) in the puzzle. Thus, if you add a switch called `"puzzle_switch_4"` from Solarus Editor one day, you won't even have to change your scripts: it will directly take it into account in the puzzle.

## Methods of the type `map`

### `map:get_id()`

Returns the id of this map.

Return value (string)
: Id of the map.

!!! note "Note"

    This id appears in the name of [map files](#map-files).

### `map:get_game()`

Returns the current game.

Return value ([game](./game.md))
: The game that is currently running the map.

### `map:get_world()`

Returns the world name that was set on this map.

The world name is an optional property defined in the [map data file](#map-files). Worlds allow to group maps together. The world can be any arbitrary name. Maps that have the same world name are considered to be part of the same environment. For example, your map can be in a world named `"outside_world"`,`"dungeon_1"` or `"some_scary_cave"` A map that has no world is always considered to be alone in its own environment.

The world property is used to decide when to set the starting location of the player (the place where he starts when loading his [savegame](./game.md)). By default, the starting location is automatically set by the engine when the world changes (not when the map changes). This can be changed by defining the "Save starting location" property of destinations, from Solarus Editor or from a script (with [`destination:set_starting_location_mode()`](./map-entities/destination.md#destinationset_starting_location_modemode)).

Some other features may also rely on the world property, like the state of [crystal blocks](./map-entities/crystal-block.md). Their state persists between all maps of the current world and is reset when entering a map whose world is different.

Return value (string or `nil`)
: Name of the world of the current map. `nil` means no world.

### `map:set_world(world)`

Changes the world of this map.

The world property remains until the map is destroyed: If you reload the same map again later, the world is reset to the one defined in the [map data file](#map-files).

`world` (string or `nil`)
: The new world name to set, or `nil` to set no world.

### `map:get_floor()`

Returns the floor of the current map if any.

The floor is an optional property defined in the [map data file](#map-files).

The engine does not do anything particular with this floor property. But you can use it in scripts, for example to show the current floor on the HUD when it changes or to make a minimap [menu](./menus.md).

Return value (number or `nil`)
: The current floor. `0` is the first floor, `1` is the second floor, `-1` is the first basement floor, etc. `nil` means that this map is not part of a floor system.

### `map:set_floor(floor)`

Changes the floor of this map.

The floor property remains until the map is destroyed: If you reload the same map again later, the floor is reset to the one defined in the [map data file](#map-files).

`floor` (number or `nil`)
: The new floor number to set, or `nil` to set no floor.

### `map:get_min_layer()`

Returns the index of the lowest layer of this map.

Return value (number)
: The lowest layer (0 or less).

### `map:get_max_layer()`

Returns the index of the highest layer of this map.

Return value (number)
: The highest layer (0 or more).

### `map:get_size()`

Returns the size of this map in pixels.

Return value 1 (number)
: The width in pixels (always a multiple of 8).

Return value 2 (number)
: The height in pixels (always a multiple of 8).

### `map:get_location()`

Returns the x,y location of this map in its [world](#mapget_world).

The engine uses this information to implement scrolling between two adjacent maps.

For example, you can also use this property in scripts if you want to show the position of the hero on the minimap [menu](./menus.md) of your outside [world](#mapget_world). Indeed, your outside world is probably not a single map, but it is usually composed of several adjacent maps.

Return value 1 (number)
: X position of the top-left corner of this map relative to its world.

Return value 2 (number)
: Y position of the top-left corner of this map relative to its world.

### `map:get_tileset()`

Returns the name of the [tileset](#tileset) of the current map.

Return value (string)
: Id of the current tileset.

### `map:set_tileset(tileset_id)`

Changes the [tileset](#tileset) of the current map.

It is your responsability to make sure that the new tileset is be compatible with the previous one: every tile of the previous tileset must exist in the new one and have the exact same properties, and only the images can differ.

`tileset_id` (string)
: Id of the new tileset to set.

!!! note "Note"

    If the new tileset is not compatible with the previous one, tiles will be displayed with wrong graphics.

### `map:get_music()`

Returns the name of the music associated to this map.

This is the music to play when the map starts, as specified in the map file. It may be different from the music currently being played. To get the music currently being played, see [`sol.audio.get_music()`](./audio/index.md#solaudioplay_musicmusic_id-action).

Return value (string or `nil`)
: Name of the music of this map, relative to the `musics` directory and without extension. It can also be the special value `"same"` if the map specifies to keep the music unchanged, or `nil`. If the map specifies to play no music.

### `map:get_camera()`

Returns the main camera entity of the map.

Return value ([camera](./map-entities/camera.md))
: The main camera.

### `map:get_cameras()`

Returns all the camera entities of the map. By default, each camera is tracking a specific hero.
See [map:get_heroes](./map.md/#mapget_heroes).

Return value (function)
: An iterator to all camera entities of the map.

### `map:get_ground(x, y, layer)`

Returns the kind of ground (terrain) of a point.

The ground is defined by [tiles](./map-entities/tile.md) (and other entities that may change it like [dynamic tiles](./map-entities/dynamic-tile.md)) that overlap this point.

`x` (number)
: X coordinate of a point of the map.

`y` (number)
: Y coordinate of a point of the map.

`layer` (number)
: The layer, between [`map:get_min_layer()`](#mapget_min_layer) and [`map:get_max_layer()`](#mapget_max_layer).

Return value (string)
: The kind of ground. The possible values are the same as the `ground` property of the [tileset file](../files-specs/tileset-data-file.md):

    - `"empty"`
    - `"traversable"`
    - `"wall"`
    - `"low_wall"`
    - `"wall_top_right"`
    - `"wall_top_left"`
    - `"wall_bottom_left"`
    - `"wall_bottom_right"`
    - `"wall_top_right_water"`
    - `"wall_top_left_water"`
    - `"wall_bottom_left_water"`
    - `"wall_bottom_right_water"`
    - `"deep_water"`
    - `"shallow_water"`
    - `"grass"`
    - `"hole"`
    - `"ice"`
    - `"ladder"`
    - `"prickles"`
    - `"lava"`

### `map:draw_visual(drawable, x, y)`

Draws a [drawable object](./drawable-objects/index.md) ([surface](./drawable-objects/surface.md), [text surface](./drawable-objects/text-surface.md) or [sprite](./drawable-objects/sprite.md)) on the [camera](#mapget_camera) at the given map coordinates.

This function can be used as an alternative to [`drawable:draw()`](./drawable-objects/index.md#drawabledrawdst_surface-x-y) in order to draw the object relative to the map (instead of relative to the screen).

If the object to draw is a [sprite](./drawable-objects/sprite.md), its origin point will be displayed at the given location, relative to the the upper left corner of the map.

This function should only be called during the drawing phase of the map, for example from [`map:on_draw()`](#mapon_drawcamera_surface) or from [`custom_entity:on_post_draw()`](./map-entities/index.md#entityon_post_drawcamera).

`drawable` ([drawable](./drawable-objects/index.md))
: The visual object to draw on the map.

`x` (number)
: X coordinate of where to draw the object, in map coordinates.

`y` (number)
: Y coordinate of where to draw the object, in map coordinates.

### `map:draw_sprite(sprite, x, y)`

Draws a [sprite](./drawable-objects/sprite.md) on the screen at the given map coordinates.

!!! warning "Deprecated"

    This method is deprecated since Solarus 1.5. Use [`map:draw_visual()`](#mapdraw_visualdrawable-x-y) instead.

### `map:get_crystal_state()`

Returns the configuration of [crystal blocks](./map-entities/crystal-block.md).

Return value (boolean)
: `false` initially (orange blocks lowered), `true` otherwise (blue blocks lowered).

### `map:set_crystal_state(state)`

Sets the configuration of [crystal blocks](./map-entities/crystal-block.md).

This state persists accross maps of the same [world](#mapget_world). It is reset when the world changes and when the [savegame](./game.md) is reloaded.

`state` (boolean)
: `false` to set the initial configuration (orange blocks lowered), `true` to the modified one (blue blocks lowered).

### `map:change_crystal_state()`

Inverts the configuration of [crystal blocks](./map-entities/crystal-block.md).

!!! note "Note"

    Equivalent to `map:set_crystal_state(not map:get_crystal_state())`.

### `map:open_doors(prefix)`

Opens the [doors](./map-entities/door.md) whose name starts with the specified prefix, enables or disables relative [dynamic tiles](./map-entities/dynamic-tile.md) accordingly and plays the `"door_open"` [sound](./audio/index.md#solaudioplay_soundsound_id).

Opening a door may be more complex than just modifying a single [door entity](./map-entities/door.md). Indeed, there is often a corresponding door is the adjacent room that you also want to open (that corresponding door is another [entity](./map-entities/index.md)). Name both doors with the same prefix, and you can use this function to open both of them.

Furthermore, you sometimes want [dynamic tiles](./map-entities/dynamic-tile.md) to be shown or hidden depending on the state of a door. When a door is open, all dynamic tiles whose prefix is the door's name followed by `_open` or `_closed` are automatically enabled or disabled, respectively.

`prefix` (string)
: Prefix of the name of doors to open.

!!! note "Note"

    The doors will be really closed once the opening animation of their sprite is finished. However, they immediately become obstacles.

### `map:close_doors(prefix)`

Closes the [doors](./map-entities/door.md) whose name starts with the specified prefix, enables or disables relative [dynamic tiles](./map-entities/dynamic-tile.md) accordingly and plays the `"door_closed"` [sound](./audio/index.md#solaudioplay_soundsound_id).

Closing a door may be more complex than just modifying a single [door entity](./map-entities/door.md). Indeed, there is often a corresponding door is the adjacent room that you also want to open (that corresponding door is another [entity](./map-entities/index.md)). Name both doors with the same prefix, and you can use this function to close both of them.

Furthermore, you sometimes want [dynamic tiles](./map-entities/dynamic-tile.md) to be shown or hidden depending on the state of a door. When a door is closed, all dynamic tiles whose prefix is the door's name followed by `_open` or `_closed` are automatically disabled or enabled, respectively.

`prefix` (string)
: Prefix of the name of doors to close.

### `map:set_doors_open(prefix, [open])`

Like [`map:open_doors()`](#mapopen_doorsprefix) or [`map:close_doors()`](#mapclose_doorsprefix), but does not play any sound or any sprite animation.

This function is intended to be called when you don't want the player to notice the change, typically when your map starts (i.e. from the [`map:on_started()`](#mapon_starteddestination) event).

`prefix` (string)
: Prefix of the name of doors to set.

`open` (boolean, optional, default: `true`)
: `true` to open the doors, `false` to close them.

### `map:get_entity(name)`

Returns the [map entity](./map-entities/index.md) with the specified name if it exists on this map. Entity names are unique (two entities cannot exist on the map with the same name at the same time). The name is optional: some entities may have no name. In this case, you cannot access them from this function.

As a convenient feature, map entities can also be accessed directly through the environment of the [map script](#map-files). In other words, you can just write `bob:get_position()` as an equivalent to `map:get_entity("bob"):get_position()`.

`name` (string)
: Name of the map entity to get.

Return value ([entity](./map-entities/index.md) or `nil`)
: The corresponding entity, or `nil`. If there exists no entity with this name on the map.

!!! note "Note"

    Technical note for curious Lua experts: the mechanism that makes map entities directly accessible in the map script environment is lazy (it is implemented as an `__index` metamethod).

    Entities are imported to the Lua side only when your script requests them. If you have thousands of named entities in your map, you won't have thousands of useless objects living in Lua. Only the ones your script tries to access are imported.

### `map:has_entity(name)`

Returns whether there currently exists a [map entity](./map-entities/index.md) with the specified name on the map.

`name` (string)
: Name of the [map entity](./map-entities/index.md) to check.

Return value (boolean)
: `true` if such an entity exists.

!!! note "Note"

    Equivalent to `map:get_entity(name) ~= nil` (but a bit lighter because it avoids to export the entity to Lua).

### `map:get_entities([prefix])`

Returns an iterator to all [map entities](./map-entities/index.md) whose name has the specified prefix.

The typical usage of this function is:

```lua
for entity in map:get_entities("your_prefix") do
  -- some code related to the entity
end
```

`prefix` (string, optional)
: Prefix of the entities to get. No value means all entities (equivalent to an empty prefix).

Return value (function)
: An iterator to all entities with this prefix. Entities are returned in Z order (insertion order).

### `map:get_entities_count(prefix)`

Returns the number of [map entities](./map-entities/index.md) having the specified prefix.

`prefix` (string)
: Prefix of the entities to count.

Return value (number)
: The number of entities having this prefix on the map.

### `map:has_entities(prefix)`

Returns whether there exists at least one [map entity](./map-entities/index.md) having the specified prefix.

This function can be used for example to checker whether a group of [enemies](./map-entities/enemy.md) is dead.

`prefix` (string)
: Prefix of the entities to check.

Return value (boolean)
: `true` if at least one entity with this prefix exists on the map.

!!! note "Note"

    Equivalent to `map:get_entities_count(prefix) > 0` but faster when there are a lot of entities (because it stops searching as soon as there is a match).

### `map:get_entities_by_type(type)`

Returns an iterator to all [map entities](./map-entities/index.md) of the given type on the map.

The typical usage of this function is:

```lua
for entity in map:get_entities_by_type(type) do
  -- some code related to the entity
end
```

`type` (string)
: Name of an entity type. See [`entity:get_type()`](./map-entities/index.md#entityget_type) for the possible values.

### `map:get_entities_in_rectangle(x, y, width, height)`

Returns an iterator to all [map entities](./map-entities/index.md) whose maximum bounding box intersects the given rectangle. The maximum bounding box is the union of the entity's own [bounding box](./map-entities/index.md#entityget_bounding_box) and of the bounding boxes from its sprites.

The typical usage of this function is:

```lua
for entity in map:get_entities_in_rectangle(x, y, width, height) do
  -- some code related to the entity
end
```

`x` (number)
: X coordinate of the rectangle's upper-left corner.

`y` (number)
: Y coordinate of the rectangle's upper-left corner.

`width` (number)
: Width of the rectangle.

`height` (number)
: Height of the rectangle.

Return value (function)
: An iterator to all entities intersecting the rectangle. Entities are returned in Z order (insertion order).

### `map:get_entities_in_region(x, y), map:get_entities_in_region(entity)`

Returns an iterator to all [map entities](./map-entities/index.md) that are in a region. Regions of the map are defined by the position of [separators](./map-entities/separator.md) and map limits. The region of an entity is the one of its center point.

Regions should be rectangular. Non-convex regions, for example with an "L" shape, are not supported by this function.

The typical usage of this function is:

```lua
for entity in map:get_entities_in_region(my_entity) do
    -- some code related to the entity
end
```

To get entities in the same region as a point:

| With coordinates | With an [entity](./map-entities/index.md) |
|------------------|------------------------------|
| <dl><dt>`x` (number)</dt><dd>X coordinate of the region to get.</dd><dt>`y` (number)</dt><dd>Y coordinate of the region to get.</dd></dl> | <dl><dt>`entity` ([entity](./map-entities/index.md))</dt><dd>The entity whose region to get.</dd></dl>|

Return value (function)
: An iterator to all entities in the same region as the point.

### `map:get_hero()`

Returns the main [hero](./map-entities/hero.md) or the first hero in a multiplayer game.

Return value ([hero](./map-entities/hero.md))
: The main hero.

!!! note "Note"

    Equivalent to `map:get_entity("hero")` but shorter to write. This function is provided for convenience as getting the hero is often needed.

### `map:get_heroes()`

Returns all [hero](./map-entities/hero.md) entities for this map.

Return value (function)
: An iterator to all hero entities in the map.

### `map:set_entities_enabled(prefix, [enabled])`

Enables or disables all [map entities](./map-entities/index.md) having the specified prefix.

Disabled entities are not displayed and are not updated. Therefore, they don't move and their collisions are no longer detected. But they still exist and can be enabled back later.

`prefix` (string)
: Prefix of the entities to change.

`enable` (boolean, optional, default: `true`)
: `true` to enable them, `false` to disable them.

!!! note "Note"

    Equivalent to calling [`entity:set_enabled()`](./map-entities/index.md#entityset_enabledenabled) on a group of entities.

### `map:remove_entities(prefix)`

Removes and destroys all [map entities](./map-entities/index.md) having the specified prefix.

Once an entity is removed, it is destroyed and it no longer exists on the map. A good practice is to avoid keeping references to destroyed entities in your scripts so that they can be garbage-collected by Lua.

`prefix` (string)
: Prefix of the entities to remove from the map.

!!! note "Note"

    Equivalent to calling [`entity:remove()`](./map-entities/index.md#entityremove) on a group of entities.

### `map:create_hero(properties)`

Creates a new [hero](./map-entities/hero.md) entity on the map.
The hero will have no equipment or items, this is not a copy of the main hero but a new one.
You can change the hero controls with [hero:set_controls](./map-entities/hero.md#heroset_controlscontrols) method.

`properties` (table)
: A table that describles all properties of the hero to create. Its key-value pairs must be:

    `name` (string or `nil`, optional)
    : Name identifying the entity or `nil`. If the name is already used by another entity, a suffix (of the form `"_2"`, `"_3"`, etc.) will be automatically appended to keep entity names unique.

    `layer` (number)
    : The layer, between [`map:get_min_layer()`](#mapget_min_layer) and [`map:get_max_layer()`](#mapget_max_layer).

    `x` (number)
    : X coordinate on the map.

    `y` (number)
    : Y coordinate on the map.

    `enabled_at_start` (boolean, optional, default: `true`)
    : Whether the entity should be initially enabled.

    `properties` (table, optional)
    : Additional user-defined properties. See [`entity:set_properties()`](./map-entities/index.md#entityset_propertiesproperties) for the specification.

Return value ([hero](./map-entities/hero.md))
: The hero created.

### `map:create_destination(properties)`

Creates an entity of type [destination](./map-entities/destination.md) on the map.

`properties` (table)
: A table that describles all properties of the entity to create. Its key-value pairs must be:

    `name` (string or `nil`, optional)
    : Name identifying the entity or `nil`. If the name is already used by another entity, a suffix (of the form `"_2"`, `"_3"`, etc.) will be automatically appended to keep entity names unique.

    `layer` (number)
    : The layer, between [`map:get_min_layer()`](#mapget_min_layer) and [`map:get_max_layer()`](#mapget_max_layer).

    `x` (number)
    : X coordinate on the map.

    `y` (number)
    : Y coordinate on the map.

    `direction` (number)
    : Direction that the hero should take when arriving on the destination, between `0` (East) and `3` (South), or `-1` to keep his direction unchanged.

    `sprite` (string, optional)
    : Id of the animation set of a [sprite](./drawable-objects/sprite.md) to create for the destination. No value means no sprite (the destination will then be invisible).

    `save_location` (string, optional)
    : Whether to update the [starting location](./game.md#gameset_starting_locationmap_id-destination_name) of the player when arriving to this destination. If yes, when the player restarts his game, he will restart at this destination. Must be one of:

        - `"when_world_changes"` (default): Updates the starting location if the current [world](#mapget_world) has just changed when arriving to this destination.
        - `"yes"`: Updates the starting location.
        - `"no"`: Does not update the starting location.

    `default` (boolean, optional, default: `false`)
    : Sets this destination as the default one when teletransporting the hero to this map without destination specified. Only one destination can be the default one on a map. If no default destination is set, then the first one declared becomes the default one.

    `enabled_at_start` (boolean, optional, default: `true`)
    : Whether the entity should be initially enabled.

    `properties` (table, optional)
    : Additional user-defined properties. See [`entity:set_properties()`](./map-entities/index.md#entityset_propertiesproperties) for the specification.

Return value ([destination](./map-entities/destination.md))
: The destination created.

### `map:create_teletransporter(properties)`

Creates an entity of type [teletransporter](./map-entities/teletransporter.md) on the map.

`properties` (table)
: A table that describles all properties of the entity to create. Its key-value pairs must be:

    `name` (string or `nil`, optional)
    : Name identifying the entity or `nil`. If the name is already used by another entity, a suffix (of the form `"_2"`, `"_3"`, etc.) will be automatically appended to keep entity names unique.

    `layer` (number)
    : The layer, between [`map:get_min_layer()`](#mapget_min_layer) and [`map:get_max_layer()`](#mapget_max_layer).

    `x` (number)
    : X coordinate on the map.

    `y` (number)
    : Y coordinate on the map.

    `width` (number)
    : Width of the entity in pixels.

    `height` (number)
    : Height of the entity in pixels.

    `sprite` (string, optional)
    : Id of the animation set of a [sprite](./drawable-objects/sprite.md) to create for the teletransporter. No value means no sprite (the teletransporter will then be invisible).

    `sound` (string, optional)
    : Sound to [play](./audio/index.md#solaudioplay_soundsound_id) when the [hero](./map-entities/hero.md) uses the teletransporter. No value means no sound.

    `transition` (string, optional, default: `"fade"`)
    : Style of transition to play when the hero uses the teletransporter. Must be one of:

        - `"fade"` (default): Fade-out and fade-in effect.
        - `"immediate"`: No transition.
        - `"scrolling"`: Scrolling between maps.

    `destination_map` (string)
    : Id of the map to transport to (can be the id of the current map).

    `destination` (string, optional)
    : Location on the destination map. Can be the name of a [destination](./map-entities/destination.md) entity, the special value `"_same"` to keep the hero's coordinates, or the special value `"_side"` to place on hero on the corresponding side of an adjacent map (normally used with the scrolling transition style). No value means the default destination entity of the map.

    `enabled_at_start` (boolean, optional, default: `true`)
    : Whether the entity should be initially enabled.

    `properties` (table, optional)
    : Additional user-defined properties. See [`entity:set_properties()`](./map-entities/index.md#entityset_propertiesproperties) for the specification.

Return value ([teletransporter](./map-entities/teletransporter.md))
: The teletransporter created.

### `map:create_pickable(properties)`

Creates an entity of type [pickable treasure](./map-entities/pickable.md) on the map.

`properties` (table)
: A table that describles all properties of the entity to create. Its key-value pairs must be:

    `name` (string or `nil`, optional)
    : Name identifying the entity or `nil`. If the name is already used by another entity, a suffix (of the form `"_2"`, `"_3"`, etc.) will be automatically appended to keep entity names unique.

    `layer` (number)
    : The layer, between [`map:get_min_layer()`](#mapget_min_layer) and [`map:get_max_layer()`](#mapget_max_layer).

    `x` (number)
    : X coordinate on the map.

    `y` (number)
    : Y coordinate on the map.

    `treasure_name` (string, optional)
    : Kind of treasure to create (the name of an [equipment item](./equipment-items.md)). If this value is not set, or corresponds to a [non-obtainable](./equipment-items.md#itemis_obtainable) item, then no entity is created and `nil` is returned.

    `treasure_variant` (number, optional, default: `1`)
    : Variant of the treasure (because some [equipment items](./equipment-items.md) may have several variants). The default value is the first variant.

    `treasure_savegame_variable` (string, optional)
    : Name of the boolean value that stores in the [savegame](./game.md) whether this pickable treasure was found. No value means that the treasure is not saved. If the treasure is saved and the player already has it, then no entity is be created and `nil` is returned.

    `enabled_at_start` (boolean, optional, default: `true`)
    : Whether the entity should be initially enabled.

    `properties` (table, optional)
    : Additional user-defined properties. See [`entity:set_properties()`](./map-entities/index.md#entityset_propertiesproperties) for the specification.

Return value ([pickable treasure](./map-entities/pickable.md) or `nil`)
: The pickable treasure created, or `nil`. If the item is not set, not [obtainable](./equipment-items.md#itemis_obtainable), or if the pickable treasure is already found (for a saved one).

### `map:create_destructible(properties)`

Creates an entity of type [destructible object](./map-entities/destructible.md) on the map.

`properties` (table)
: A table that describes all properties of the entity to create. Its key-value pairs must be:

    `name` (string or `nil`, optional)
    : Name identifying the entity or `nil`. If the name is already used by another entity, a suffix (of the form `"_2"`, `"_3"`, etc.) will be automatically appended to keep entity names unique.

    `layer` (number)
    : The layer, between [`map:get_min_layer()`](#mapget_min_layer) and [`map:get_max_layer()`](#mapget_max_layer).

    `x` (number)
    : X coordinate on the map.

    `y` (number)
    : Y coordinate on the map.

    `treasure_name` (string, optional)
    : Kind of [pickable treasure](./map-entities/pickable.md) to hide in the destructible object (the name of an [equipment item](./equipment-items.md)). If this value is not set, then no treasure is placed in the destructible object. If the treasure is not obtainable when the object is destroyed, no pickable treasure is created.

    `treasure_variant` (number, optional, default: `1`)
    : Variant of the treasure if any (because some [equipment items](./equipment-items.md) may have several variants). The default value is the first variant.

    `treasure_savegame_variable` (string, optional)
    : Name of the boolean value that stores in the [savegame](./game.md) whether the [pickable treasure](./map-entities/pickable.md) hidden in the destructible object was found. No value means that the treasure (if any) is not saved. If the treasure is saved and the player already has it, then no treasure is put in the destructible object.

    `sprite` (string)
    : Name of the animation set of a [sprite](./drawable-objects/sprite.md) to create for the destructible object.

    `destruction_sound` (string, optional)
    : Sound to [play](./audio/index.md#solaudioplay_soundsound_id) when the destructible object is cut or broken after being thrown. No value means no sound.

    `weight` (number, optional, default: `0`)
    : Level of `"lift"` [ability](./game.md#gameget_abilityability_name) required to lift the object. `0` allows the player to lift the object unconditionally. The special value `-1` means that the object can never be lifted.

    `can_be_cut` (boolean, optional, default: `false`)
    : Whether the hero can cut the object with the sword.

    `can_explode` (boolean, optional, default: `false`)
    : Whether the object should explode when it is cut, hit by a weapon and after a delay when the hero lifts it.

    `can_regenerate` (boolean, optional, default: `false`)
    : Whether the object should automatically regenerate after a delay when it is destroyed.

    `damage_on_enemies` (number, optional, default: `1`)
    : Number of life points to remove from an enemy that gets hit by this object after the [hero](./map-entities/hero.md) throws it. If the value is `0`, enemies will ignore the object.

    `ground` (string, optional, default: `"wall"`)
    : Ground defined by this entity. The ground is usually `"wall"`, but you may set `"traversable"` to make the object traversable, or for example `"grass"` to make it traversable too but with an additional grass sprite below the hero.

    `enabled_at_start` (boolean, optional, default: `true`)
    : Whether the entity should be initially enabled.

    `properties` (table, optional)
    : Additional user-defined properties. See [`entity:set_properties()`](./map-entities/index.md#entityset_propertiesproperties) for the specification.

Return value ([destructible object](./map-entities/destructible.md))
: The destructible object created.

!!! note "Note"

    The state of the [pickable treasure](./map-entities/pickable.md) placed in the destructible object (obtained or not) and the possessed variant of the [item](./equipment-items.md) (a number) are two independent values that have different meanings and are saved separately.

### `map:create_chest(properties)`

Creates an entity of type [treasure chest](./map-entities/chest.md) on the map.

`properties` (table)
: A table that describes all properties of the entity to create. Its key-value pairs must be:

    `name` (string or `nil`, optional)
    : Name identifying the entity or `nil`. If the name is already used by another entity, a suffix (of the form `"_2"`, `"_3"`, etc.) will be automatically appended to keep entity names unique.

    `layer` (number)
    : The layer, between [`map:get_min_layer()`](#mapget_min_layer) and [`map:get_max_layer()`](#mapget_max_layer).

    `x` (number)
    : X coordinate on the map.

    `y` (number)
    : Y coordinate on the map.

    `treasure_name` (string, optional)
    : Kind of treasure to place in the chest (the name of an [equipment item](./equipment-items.md)). If this value is not set, then the chest will be empty. If the treasure is not obtainable when the hero opens the chest, it becomes empty.

    `treasure_variant` (number, optional, default: `1`)
    : Variant of the treasure (because some [equipment items](./equipment-items.md) may have several variants). The default value is the first variant.

    `treasure_savegame_variable` (string, optional)
    : Name of the boolean value that stores in the [savegame](./game.md) whether this chest is open. No value means that the state of the treasure is not saved. If the treasure is saved and the player already has it, then no treasure is placed in the chest (the chest will appear open).

    `sprite` (string)
    : Name of the animation set of the [sprite](./drawable-objects/sprite.md) to create for the chest. The sprite must have animations `"open"` and `"closed"`

    `opening_method` (string, optional, default: `"interaction"`)
    : Specifies the permissions for the hero to open the chest. Must be one of:

        - `"interaction"` (default): Can be opened by pressing the [action command](./game.md#game-commands) in front of it.
        - `"interaction_if_savegame_variable"`: Can be opened by pressing the [action command](./game.md#game-commands) in front of it, provided that a specific savegame variable is set.
        - `"interaction_if_item"`: Can be opened by pressing the [action command](./game.md#game-commands) in front of it, provided that the player has a specific [equipment item](./equipment-items.md).

    `opening_condition` (string, optional)
    : The condition required to open the chest. Only for opening methods `"interaction_if_savegame_variable"` and `"interaction_if_item"`

        - For opening method `"interaction_if_savegame_variable"`, it must be the name of a savegame variable. The [hero](./map-entities/hero.md) will be allowed to open the chest if this saved value is either `true`, an integer greater than zero or a non-empty string.
        - For opening method `"interaction_if_item"`, it must be the name of an [equipment item](./equipment-items.md). The hero will be allowed to open the chest if he has that item and, for items with an amount, if the amount is greater than zero.
        - For the default opening method (`"interaction"`), this setting has no effect.

    `opening_condition_consumed` (boolean, optional)
    : Whether opening the chest should consume the savegame variable or the [equipment item](./equipment-items.md) that was required. The default setting is `false` If you set it to `true`, the following rules are applied when the [hero](./map-entities/hero.md) successfully opens the chest:

        - For opening method `"interaction_if_savegame_variable"`, the savegame variable that was required is reset to `false`, `0` or `""` (depending on its type).
        - For opening method is `"interaction_if_item"`, the equipment item that was required is removed. This means setting its [possessed variant](./equipment-items.md#itemset_variantvariant) to `0`, unless it has an associated amount: in this case, the amount is decremented.

    `cannot_open_dialog` (string, optional)
    : Id of the dialog to show if the hero fails to open the chest. If you don't set this value, no dialog is shown.

    `enabled_at_start` (boolean, optional, default: `true`)
    : Whether the entity should be initially enabled.

    `properties` (table, optional)
    : Additional user-defined properties. See [`entity:set_properties()`](./map-entities/index.md#entityset_propertiesproperties) for the specification.

Return value ([chest](./map-entities/chest.md))
: The treasure chest created.

!!! note "Note"

    The state of the [treasure chest](./map-entities/chest.md) (obtained or not) and the possessed variant of its [item](./equipment-items.md) are two independent values that have different meanings and are saved separately.

### `map:create_jumper(properties)`

Creates an entity of type [jumper](./map-entities/jumper.md) on the map.

`properties` (table)
: A table that describes all properties of the entity to create. Its key-value pairs must be:

    `name` (string or `nil`, optional)
    : Name identifying the entity or `nil`. If the name is already used by another entity, a suffix (of the form `"_2"`, `"_3"`, etc.) will be automatically appended to keep entity names unique.

    `layer` (number)
    : The layer, between [`map:get_min_layer()`](#mapget_min_layer) and [`map:get_max_layer()`](#mapget_max_layer).

    `x` (number)
    : X coordinate on the map.

    `y` (number)
    : Y coordinate on the map.

    `width` (number)
    : Width of the entity in pixels.

    `height` (number)
    : Height of the entity in pixels.

    `direction` (number)
    : Direction of the jump, between `0` (East) and `7` (South-East). If the direction is horizontal, the width must be `8` pixels. If the direction is vertical, the height must be `8` pixels. If the direction is diagonal, the size must be square.

    `jump_length` (number)
    : Length of the baseline of the jump in pixels (see the [jump movement](./movements/jump-movement.md) page for details).

    `enabled_at_start` (boolean, optional, default: `true`)
    : Whether the entity should be initially enabled.

    `properties` (table, optional)
    : Additional user-defined properties. See [`entity:set_properties()`](./map-entities/index.md#entityset_propertiesproperties) for the specification.

Return value ([jumper](./map-entities/jumper.md))
: The jumper created.

### `map:create_enemy(properties)`

Creates an entity of type [enemy](./map-entities/enemy.md) on the map.

`properties` (table)
: A table that describes all properties of the entity to create. Its key-value pairs must be:

    `name` (string or `nil`, optional)
    : Name identifying the entity or `nil`. If the name is already used by another entity, a suffix (of the form `"_2"`, `"_3"`, etc.) will be automatically appended to keep entity names unique.

    `layer` (number)
    : The layer, between [`map:get_min_layer()`](#mapget_min_layer) and [`map:get_max_layer()`](#mapget_max_layer).

    `x` (number)
    : X coordinate on the map.

    `y` (number)
    : Y coordinate on the map.

    `direction` (number)
    : Initial direction of the enemy, between `0` (East) and `3` (South).

    `breed` (string)
    : Model of enemy to create.

    `savegame_variable` (string, optional)
    : Name of the boolean value that stores in the [savegame](./game.md) whether this enemy is dead. No value means that the enemy is not saved. If the enemy is saved and was already killed, then no enemy is created. Instead, its [pickable treasure](./map-entities/pickable.md) is created if it is a saved one.

    `treasure_name` (string, optional)
    : Kind of [pickable treasure](./map-entities/pickable.md) to drop when the enemy is killed (the name of an [equipment item](./equipment-items.md)). If this value is not set, then the enemy won't drop anything. If the treasure is not obtainable when the enemy is killed, then nothing is dropped either.

    `treasure_variant` (number, optional, default: `1`)
    : Variant of the treasure (because some [equipment items](./equipment-items.md) may have several variants). The default value is the first variant.

    `treasure_savegame_variable` (string, optional)
    : Name of the boolean value that stores in the [savegame](./game.md) whether the [pickable treasure](./map-entities/pickable.md) of this enemy was obtained. No value means that the state of the treasure is not saved. If the treasure is saved and the player already has it, then the enemy won't drop anything.

    `enabled_at_start` (boolean, optional, default: `true`)
    : Whether the entity should be initially enabled.

    `properties` (table, optional)
    : Additional user-defined properties. See [`entity:set_properties()`](./map-entities/index.md#entityset_propertiesproperties) for the specification.

Return value ([enemy](./map-entities/enemy.md) or [pickable treasure](./map-entities/pickable.md) or `nil`)
: The enemy created, except when it is a saved enemy that is already dead. In this case, if the enemy dropped a saved treasure that is not obtained yet, this [pickable treasure](./map-entities/pickable.md) is created and returned. Otherwise, `nil` is returned.

!!! note "Note"

    The state of the [enemy](./map-entities/enemy.md) (alive or dead), the state of its [treasure dropped](./map-entities/pickable.md) (obtained or not) and the possessed variant of the [item](./equipment-items.md) dropped (a number) are three independent values that have different meanings and are saved separately.

### `map:create_npc(properties)`

Creates an entity of type [non-playing character](./map-entities/npc.md) (NPC) on the map.

`properties` (table)
: A table that describes all properties of the entity to create. Its key-value pairs must be:

    `name` (string or `nil`, optional)
    : Name identifying the entity or `nil`. If the name is already used by another entity, a suffix (of the form `"_2"`, `"_3"`, etc.) will be automatically appended to keep entity names unique.

    `layer` (number)
    : The layer, between [`map:get_min_layer()`](#mapget_min_layer) and [`map:get_max_layer()`](#mapget_max_layer).

    `x` (number)
    : X coordinate on the map.

    `y` (number)
    : Y coordinate on the map.

    `direction` (number)
    : Initial direction of the NPC's sprite, between `0` (East) and `3` (South).

    `subtype` (number)
    : Kind of NPC to create: `1` for a usual NPC who the player can talk to, `0` for a generalized NPC (not necessarily a person). See the [NPC documentation](./map-entities/npc.md) for more details.

    `sprite` (string, optional)
    : Name of the animation set of a [sprite](./drawable-objects/sprite.md) to create for the NPC. No value means no sprite (the NPC will then be invisible).

    `behavior` (string, optional, default: `"map"`)
    : What to do when there is an interaction with the NPC.

        - `"map"` (default): Forwards events to the map script (for example, calls the [`on_interaction()`](./map-entities/npc.md#npcon_interaction) event of the NPC).
        - `"dialog#XXXX"`: Starts the dialog with id `XXXX` when the player talks to this NPC.
        - `"item#XXXX"`: Forwards events to an [equipment item](./equipment-items.md) script (for example, calls the [`on_interaction()`](./equipment-items.md#itemon_npc_interactionnpc) event of the equipment item with id `XXXX`)

    `enabled_at_start` (boolean, optional, default: `true`)
    : Whether the entity should be initially enabled. The default value is `true`.

    `properties` (table, optional)
    : Additional user-defined properties. See [`entity:set_properties()`](./map-entities/index.md#entityset_propertiesproperties) for the specification.

Return value ([npc](./map-entities/npc.md))
: The NPC created.

### `map:create_block(properties)`

Creates an entity of type [block](./map-entities/block.md) on the map.

`properties` (table)
: A table that describes all properties of the entity to create. Its key-value pairs must be:

    `name` (string or `nil`, optional)
    : Name identifying the entity or `nil`. If the name is already used by another entity, a suffix (of the form `"_2"`, `"_3"`, etc.) will be automatically appended to keep entity names unique.

    `layer` (number)
    : The layer, between [`map:get_min_layer()`](#mapget_min_layer) and [`map:get_max_layer()`](#mapget_max_layer).

    `x` (number)
    : X coordinate on the map.

    `y` (number)
    : Y coordinate on the map.

    `direction` (number or `nil`, optional, default: `nil`)
    : The only direction where the block can be moved, between `0` (East) and `3` (South). `nil` means no restriction and allows the block to be moved in any of the four main directions.

    `sprite` (string)
    : Name of the animation set of a [sprite](../files-specs/sprite-data-file.md) to create for the block.

    `pushable` (boolean)
    : `true` to allow the block to be pushed.

    `pullable` (boolean)
    : `true` to allow the block to be pulled.

    `max_moves` (number or `nil`, optional, default: `nil`)
    : `How` many times the block can be moved (`nil` means unlimited).

    `maximum_moves` (number, optional, deprecated: `1.6`)
    : Like `max_moves`, but for historical reasons, only supports `0`, `1` or the special value `2` to mean infinite. New scripts should only use `max_moves` It is an error to set both values.

    `enabled_at_start` (boolean, optional, default: `true`)
    : Whether the entity should be initially enabled.

    `properties` (table, optional)
    : Additional user-defined properties. See [`entity:set_properties()`](./map-entities/index.md#entityset_propertiesproperties) for the specification.

Return value ([block](./map-entities/block.md))
: The block created.

### `map:create_dynamic_tile(properties)`

Creates an entity of type [dynamic tile](./map-entities/dynamic-tile.md) on the map.

`properties` (table)
: A table that describes all properties of the entity to create. Its key-value pairs must be:

    `name` (string or `nil`, optional)
    : Name identifying the entity or `nil`. If the name is already used by another entity, a suffix (of the form `"_2"`, `"_3"`, etc.) will be automatically appended to keep entity names unique.

    `layer` (number)
    : The layer, between [`map:get_min_layer()`](#mapget_min_layer) and [`map:get_max_layer()`](#mapget_max_layer).

    `x` (number)
    : X coordinate of the top-left corner of the dynamic tile on the map.

    `y` (number)
    : Y coordinate of the top-left corner of the dynamic tile on the map.

    `width` (number)
    : Width of the dynamic tile in pixels. The tile pattern will be repeated horizontally to fit to this width.

    `height` (number)
    : Height of the entity in pixels. The tile pattern will be repeated vertically to fit to this height.

    `pattern` (string)
    : Id of the tile pattern to use from the tileset.

    `enabled_at_start` (boolean, optional, default: `true`)
    : Whether the entity should be initially enabled.

    `properties` (table, optional)
    : Additional user-defined properties. See [`entity:set_properties()`](./map-entities/index.md#entityset_propertiesproperties) for the specification.

Return value ([dynamic tile](./map-entities/dynamic-tile.md))
: The dynamic tile created.

### `map:create_switch(properties)`

Creates an entity of type [switch](./map-entities/switch.md) on the map.

`properties` (table)
: A table that describes all properties of the entity to create. Its key-value pairs must be:

    `name` (string or `nil`, optional)
    : Name identifying the entity or `nil`. If the name is already used by another entity, a suffix (of the form `"_2"`, `"_3"`, etc.) will be automatically appended to keep entity names unique.

    `layer` (number)
    : The layer, between [`map:get_min_layer()`](#mapget_min_layer) and [`map:get_max_layer()`](#mapget_max_layer).

    `x` (number)
    : X coordinate on the map.

    `y` (number)
    : Y coordinate on the map.

    `subtype` (string)
    : Kind of switch to create:

        - `"walkable"`: A traversable pressure plate that gets activated when the hero walks on it.
        - `"solid"`: A non-traversable, solid switch that can be activated in various conditions: by the sword, by an explosion or by a projectile (a thrown object, an arrow, the boomerang or the hookshot).
        - `"arrow_target"` A switch that can be only activated by shooting an arrow on it.

    `sprite` (string)
    : Name of the animation set of a [sprite](../files-specs/sprite-data-file.md) to create for the switch. The animation set must at least contain animations `"activated"` and `"inactivated"` No value means no sprite.

    `sound` (string, optional)
    : Sound to play when the switch is activated. No value means no sound.

    `needs_block` (boolean)
    : If `true`, the switch can only be activated by a [block](./map-entities/block.md) (only for a walkable switch).

    `inactivate_when_leaving` (boolean)
    : If `true`, the switch becomes inactivated when the [hero](./map-entities/hero.md) or the [block](./map-entities/block.md) leaves it (only for a walkable switch).

    `enabled_at_start` (boolean, optional, default: `true`)
    : Whether the entity should be initially enabled.

    `properties` (table, optional)
    : Additional user-defined properties. See [`entity:set_properties()`](./map-entities/index.md#entityset_propertiesproperties) for the specification.

Return value ([switch](./map-entities/switch.md))
: The switch created.

### `map:create_wall(properties)`

Creates an entity of type [wall](./map-entities/wall.md) on the map.

`properties` (table)
: A table that describes all properties of the entity to create. Its key-value pairs must be:

    `name` (string or `nil`, optional)
    : Name identifying the entity or `nil`. If the name is already used by another entity, a suffix (of the form `"_2"`, `"_3"`, etc.) will be automatically appended to keep entity names unique.

    `layer` (number)
    : The layer, between [`map:get_min_layer()`](#mapget_min_layer) and [`map:get_max_layer()`](#mapget_max_layer).

    `x` (number)
    : X coordinate on the map.

    `y` (number)
    : Y coordinate on the map.

    `width` (number)
    : Width of the entity in pixels.

    `height` (number)
    : Height of the entity in pixels.

    `stops_hero` (boolean, optional, default: `false`)
    : `true` to make the wall stop the [hero](./map-entities/hero.md).

    `stops_npcs` (boolean, optional, default: `false`)
    : `true` to make the wall stop [non-playing characters](./map-entities/npc.md).

    `stops_enemies` (boolean, optional, default: `false`)
    : `true` to make the wall stop [enemies](./map-entities/enemy.md).

    `stops_blocks` (boolean, optional, default: `false`)
    : `true` to make the wall stop [blocks](./map-entities/block.md).

    `stops_projectiles` (boolean, optional, default: `false`)
    : `true` to make the wall stop projectiles: [thrown objects](./map-entities/carried-object.md), [arrows](./map-entities/arrow.md), the [hookshot](./map-entities/hookshot.md) and [the boomerang](./map-entities/boomerang.md).

    `enabled_at_start` (boolean, optional, default: `true`)
    : Whether the entity should be initially enabled.

    `properties` (table, optional)
    : Additional user-defined properties. See [`entity:set_properties()`](./map-entities/index.md#entityset_propertiesproperties) for the specification.

Return value ([wall](./map-entities/wall.md))
: The wall created.

### `map:create_sensor(properties)`

Creates an entity of type [sensor](./map-entities/sensor.md) on the map.

`properties` (table)
: A table that describes all properties of the entity to create. Its key-value pairs must be:

    `name` (string or `nil`, optional)
    : Name identifying the entity or `nil`. If the name is already used by another entity, a suffix (of the form `"_2"`, `"_3"`, etc.) will be automatically appended to keep entity names unique.

    `layer` (number)
    : The layer, between [`map:get_min_layer()`](#mapget_min_layer) and [`map:get_max_layer()`](#mapget_max_layer).

    `x` (number)
    : X coordinate on the map.

    `y` (number)
    : Y coordinate on the map.

    `width` (number)
    : Width of the entity in pixels.

    `height` (number)
    : Height of the entity in pixels.

    `enabled_at_start` (boolean, optional, default: `true`)
    : Whether the entity should be initially enabled.

    `properties` (table, optional)
    : Additional user-defined properties. See [`entity:set_properties()`](./map-entities/index.md#entityset_propertiesproperties) for the specification.

Return value ([sensor](./map-entities/sensor.md))
: The sensor created.

### `map:create_crystal(properties)`

Creates an entity of type [crystal](./map-entities/crystal.md) on the map.

`properties` (table)
: A table that describes all properties of the entity to create. Its key-value pairs must be:

    `name` (string or `nil`, optional)
    : Name identifying the entity or `nil`. If the name is already used by another entity, a suffix (of the form `"_2"`, `"_3"`, etc.) will be automatically appended to keep entity names unique.

    `layer` (number)
    : The layer, between [`map:get_min_layer()`](#mapget_min_layer) and [`map:get_max_layer()`](#mapget_max_layer).

    `x` (number)
    : X coordinate on the map.

    `y` (number)
    : Y coordinate on the map.

    `enabled_at_start` (boolean, optional, default: `true`)
    : Whether the entity should be initially enabled.

    `properties` (table, optional)
    : Additional user-defined properties. See [`entity:set_properties()`](./map-entities/index.md#entityset_propertiesproperties) for the specification.

Return value ([crystal](./map-entities/crystal.md))
: The crystal created.

### `map:create_crystal_block(properties)`

Creates an entity of type [crystal block](./map-entities/crystal-block.md) on the map.

`properties` (table)
: A table that describes all properties of the entity to create. Its key-value pairs must be:

    `name` (string or `nil`, optional)
    : Name identifying the entity or `nil`. If the name is already used by another entity, a suffix (of the form `"_2"`, `"_3"`, etc.) will be automatically appended to keep entity names unique.

    `layer` (number)
    : The layer, between [`map:get_min_layer()`](#mapget_min_layer) and [`map:get_max_layer()`](#mapget_max_layer).

    `x` (number)
    : X coordinate on the map.

    `y` (number)
    : Y coordinate on the map.

    `width` (number)
    : Width of the entity in pixels.

    `height` (number)
    : Height of the entity in pixels.

    `subtype` (number)
    : Kind of crystal block to create: 
        
        - `0`: a block initially lowered (orange)
        - `1`: a block initially raised (blue).

    `enabled_at_start` (boolean, optional, default: `true`)
    : Whether the entity should be initially enabled.

    `properties` (table, optional)
    : Additional user-defined properties. See [`entity:set_properties()`](./map-entities/index.md#entityset_propertiesproperties) for the specification.

Return value ([crystal block](./map-entities/crystal-block.md))
: The crystal block created.

### `map:create_shop_treasure(properties)`

Creates an entity of type [shop treasure](./map-entities/shop-treasure.md) on the map.

`properties` (table)
: A table that describes all properties of the entity to create. Its key-value pairs must be:

    `name` (string or `nil`, optional)
    : Name identifying the entity or `nil`. If the name is already used by another entity, a suffix (of the form `"_2"`, `"_3"`, etc.) will be automatically appended to keep entity names unique.

    `layer` (number)
    : The layer, between [`map:get_min_layer()`](#mapget_min_layer) and [`map:get_max_layer()`](#mapget_max_layer).

    `x` (number)
    : X coordinate on the map.

    `y` (number)
    : Y coordinate on the map.

    `price` (number)
    : Money amount required to buy the treasure.

    `font` (string, optional)
    : Id of the font to use to display to price. The default value is the first one in alphabetical order.

    `dialog` (string)
    : Id of the dialog to show when the [hero](./map-entities/hero.md) asks for information about the treasure.

    `treasure_name` (string)
    : Kind of treasure to sell (the name of an [equipment item](./equipment-items.md)). If this value or corresponds to a [non-obtainable](./equipment-items.md#itemis_obtainable) item, then the shop treasure is not created and `nil` is returned.

    `treasure_variant` (number, optional, default: `1`)
    : Variant of the treasure (because some [equipment items](./equipment-items.md) may have several variants). The default value is the first variant.

    `treasure_savegame_variable` (string, optional)
    : Name of the boolean value that stores in the [savegame](./game.md) whether the player has purchased this treasure. No value means that the state of the treasure is not saved. If the treasure is saved and the player already has it, then the shop treasure is not created and `nil` is returned.

    `enabled_at_start` (boolean, optional, default: `true`)
    : Whether the entity should be initially enabled.

    `properties` (table, optional)
    : Additional user-defined properties. See [`entity:set_properties()`](./map-entities/index.md#entityset_propertiesproperties) for the specification.

Return value ([shop treasure](./map-entities/shop-treasure.md) or `nil`)
: The shop treasure created, or `nil`. If the item is not [obtainable](./equipment-items.md#itemis_obtainable), or if the shop treasure was already purchased (for a saved one).

!!! note "Note"

    The state of the [shop treasure](./map-entities/shop-treasure.md) (purchased or not) and the possessed variant of its [item](./equipment-items.md) (a number) are two independent values that have different meanings and are saved separately.

### `map:create_stream(properties)`

Creates an entity of type [stream](./map-entities/stream.md) on the map.

`properties` (table)
: A table that describes all properties of the entity to create. Its key-value pairs must be:

    `name` (string or `nil`, optional)
    : Name identifying the entity or `nil`. If the name is already used by another entity, a suffix (of the form `"_2"`, `"_3"`, etc.) will be automatically appended to keep entity names unique.

    `layer` (number)
    : The layer, between [`map:get_min_layer()`](#mapget_min_layer) and [`map:get_max_layer()`](#mapget_max_layer).

    `x` (number)
    : X coordinate on the map.

    `y` (number)
    : Y coordinate on the map.

    `direction` (number)
    : Direction where the stream moves the [hero](./map-entities/hero.md), between `0` (East) and `7` (South-East).

    `sprite` (string, optional)
    : Id of the animation set of a [sprite](../files-specs/sprite-data-file.md) to create for the stream. No value means no sprite (the stream will then be invisible).

    `speed` (number, optional, default: `64`)
    : Speed of the movement applied to the hero by the stream, in pixels per second.

    `allow_movement` (boolean, optional, default: `true`)
    : Whether the player can still move the hero when he is on the stream.

    `allow_attack` (boolean, optional, default: `true`)
    : Whether the player can use the sword when he is on the stream.

    `allow_item` (boolean, optional, default: `true`)
    : Whether the player can use equipment items when he is on the stream.

    `enabled_at_start` (boolean, optional, default: `true`)
    : Whether the entity should be initially enabled.

    `properties` (table, optional)
    : Additional user-defined properties. See [`entity:set_properties()`](./map-entities/index.md#entityset_propertiesproperties) for the specification.

Return value ([stream](./map-entities/stream.md))
: The stream created.

### `map:create_door(properties)`

Creates an entity of type [door](./map-entities/door.md) on the map.

`properties` (table)
: A table that describes all properties of the entity to create. Its key-value pairs must be:

    `name` (string or `nil`, optional)
    : Name identifying the entity or `nil`. If the name is already used by another entity, a suffix (of the form `"_2"`, `"_3"`, etc.) will be automatically appended to keep entity names unique.

    `layer` (number)
    : The layer, between [`map:get_min_layer()`](#mapget_min_layer) and [`map:get_max_layer()`](#mapget_max_layer).

    `x` (number)
    : X coordinate on the map.

    `y` (number)
    : Y coordinate on the map.

    `direction` (number)
    : Direction of the door, between `0` (East of the room) and `3` (South of the room).

    `sprite` (string)
    : Name of the animation set of the [sprite](./drawable-objects/sprite.md) to create for the door. The sprite must have an animation `"closed"`, that will be shown while the door is closed. When the door is open, no sprite is displayed. Optionally, the sprite can also have animations `"opening"` and `"closing"`, that will be shown (if they exist) while the door is being opened or closed, respectively. If they don't exist, the door will open close instantly.

    `savegame_variable` (string, optional)
    : Name of the boolean value that stores in the [savegame](./game.md) whether this door is open. No value means that the door is not saved. If the door is saved as open, then it appears open.

    `opening_method` (string, optional, default: `"none"`)
    : How the door is supposed to be opened by the player. Must be one of:

        - `"none"` (default): Cannot be opened by the player. You can only open it from Lua.
        - `"interaction"`: Can be opened by pressing the [action command](./game.md#game-commands) in front of it.
        - `"interaction_if_savegame_variable"`: Can be opened by pressing the [action command](./game.md#game-commands) in front of it, provided that a specific savegame variable is set.
        - `"interaction_if_item"`: Can be opened by pressing the [action command](./game.md#game-commands) in front of it, provided that the player has a specific [equipment item](./equipment-items.md).
        - `"explosion"`: Can be opened by an explosion.

    `opening_condition` (string, optional)
    : The condition required to open the door. Only for opening methods `"interaction_if_savegame_variable"` and `"interaction_if_item"`

        - For opening method `"interaction_if_savegame_variable"`, it must be the name of a savegame variable. The [hero](./map-entities/hero.md) will be allowed to open the door if this saved value is either `true`, an integer greater than zero or a non-empty string.
        - For opening method `"interaction_if_item"`, it must be the name of an [equipment item](./equipment-items.md). The hero will be allowed to open the door if he has that item and, for items with an amount, if the amount is greater than zero.
        - For other opening methods, this setting has no effect.

    `opening_condition_consumed` (boolean, optional)
    : Whether opening the door should consume the savegame variable or the [equipment item](./equipment-items.md) that was required. The default setting is `false` If you set it to `true`, the following rules are applied when the [hero](./map-entities/hero.md) successfully opens the door:

        - For opening method `"interaction_if_savegame_variable"`, the savegame variable that was required is reset to `false`, `0` or `""` (depending on its type).
        - For opening method is `"interaction_if_item"`, the equipment item that was required is removed. This means setting its [possessed variant](./equipment-items.md#itemset_variantvariant) to `0`, unless it has an associated amount: in this case, the amount is decremented.
        - With other opening methods, this setting has no effect.

    `cannot_open_dialog` (string, optional)
    : Id of the dialog to show if the hero fails to open the door. If you don't set this value, no dialog is shown.

    `enabled_at_start` (boolean, optional, default: `true`)
    : Whether the entity should be initially enabled.

    `properties` (table, optional)
    : Additional user-defined properties. See [`entity:set_properties()`](./map-entities/index.md#entityset_propertiesproperties) for the specification.

Return value ([door](./map-entities/door.md))
: The [door](./map-entities/door.md) created.

### `map:create_stairs(properties)`

Creates an entity of type [stairs](./map-entities/stairs.md) on the map.

`properties` (table)
: A table that describes all properties of the entity to create. Its key-value pairs must be:

    `name` (string or `nil`, optional)
    : Name identifying the entity or `nil`. If the name is already used by another entity, a suffix (of the form `"_2"`, `"_3"`, etc.) will be automatically appended to keep entity names unique.

    `layer` (number)
    : The layer, between [`map:get_min_layer()`](#mapget_min_layer) and [`map:get_max_layer()`](#mapget_max_layer).

    `x` (number)
    : X coordinate on the map.

    `y` (number)
    : Y coordinate on the map.

    `direction` (number)
    : Direction where the stairs should be turned between `0` (East of the room) and `3` (South of the room). For stairs inside a single floor, this is the direction of going upstairs.

    `subtype` (number)
    : Kind of stairs to create:

        - `0`: Spiral staircase going upstairs.
        - `1`: Spiral staircase going downstairs.
        - `2`: Straight staircase going upstairs.
        - `3`: Straight staircase going downstairs.
        - `4`: Small stairs inside a single floor (change the layer of the [hero](./map-entities/hero.md)).

    `enabled_at_start` (boolean, optional, default: `true`)
    : Whether the entity should be initially enabled.

    `properties` (table, optional)
    : Additional user-defined properties. See [`entity:set_properties()`](./map-entities/index.md#entityset_propertiesproperties) for the specification.

Return value ([stairs](./map-entities/stairs.md))
: The stairs created.

### `map:create_bomb(properties)`

Creates an entity of type [bomb](./map-entities/bomb.md) on the map.

`properties` (table)
: A table that describes all properties of the entity to create. Its key-value pairs must be:

    `name` (string or `nil`, optional)
    : Name identifying the entity or `nil`. If the name is already used by another entity, a suffix (of the form `"_2"`, `"_3"`, etc.) will be automatically appended to keep entity names unique.

    `layer` (number)
    : The layer, between [`map:get_min_layer()`](#mapget_min_layer) and [`map:get_max_layer()`](#mapget_max_layer).

    `x` (number)
    : X coordinate on the map.

    `y` (number)
    : Y coordinate on the map.

    `enabled_at_start` (boolean, optional, default: `true`)
    : Whether the entity should be initially enabled.

    `properties` (table, optional)
    : Additional user-defined properties. See [`entity:set_properties()`](./map-entities/index.md#entityset_propertiesproperties) for the specification.

Return value ([bomb](./map-entities/bomb.md))
: The bomb created.

### `map:create_explosion(properties)`

Creates an entity of type [explosion](./map-entities/explosion.md) on the map.

`properties` (table)
: A table that describes all properties of the entity to create. Its key-value pairs must be:

    `name` (string or `nil`, optional)
    : Name identifying the entity or `nil`. If the name is already used by another entity, a suffix (of the form `"_2"`, `"_3"`, etc.) will be automatically appended to keep entity names unique.

    `layer` (number)
    : The layer, between [`map:get_min_layer()`](#mapget_min_layer) and [`map:get_max_layer()`](#mapget_max_layer).

    `x` (number)
    : X coordinate on the map.

    `y` (number)
    : Y coordinate on the map.

    `enabled_at_start` (boolean, optional, default: `true`)
    : Whether the entity should be initially enabled.

    `properties` (table, optional)
    : Additional user-defined properties. See [`entity:set_properties()`](./map-entities/index.md#entityset_propertiesproperties) for the specification.

Return value ([explosion](./map-entities/explosion.md))
: The explosion created.

### `map:create_fire(properties)`

Creates an entity of type [fire](./map-entities/fire.md) on the map.

`properties` (table)
: A table that describes all properties of the entity to create. Its key-value pairs must be:

    `name` (string or `nil`, optional)
    : Name identifying the entity or `nil`. If the name is already used by another entity, a suffix (of the form `"_2"`, `"_3"`, etc.) will be automatically appended to keep entity names unique.

    `layer` (number)
    : The layer, between [`map:get_min_layer()`](#mapget_min_layer) and [`map:get_max_layer()`](#mapget_max_layer).

    `x` (number)
    : X coordinate on the map.

    `y` (number)
    : Y coordinate on the map.

    `enabled_at_start` (boolean, optional, default: `true`)
    : Whether the entity should be initially enabled.

    `properties` (table, optional)
    : Additional user-defined properties. See [`entity:set_properties()`](./map-entities/index.md#entityset_propertiesproperties) for the specification.

Return value ([fire](./map-entities/fire.md))
: The fire created.

### `map:create_separator(properties)`

Creates an entity of type [separator](./map-entities/separator.md) on the map.

`properties` (table)
: A table that describles all properties of the entity to create. Its key-value pairs must be:

    `name` (string or `nil`, optional)
    : Name identifying the entity or `nil`. If the name is already used by another entity, a suffix (of the form `"_2"`, `"_3"`, etc.) will be automatically appended to keep entity names unique.

    `layer` (number)
    : The layer, between [`map:get_min_layer()`](#mapget_min_layer) and [`map:get_max_layer()`](#mapget_max_layer).

    `x` (number)
    : X coordinate on the map.

    `y` (number)
    : Y coordinate on the map.

    `width` (number)
    : Width of the entity in pixels.

    `height` (number)
    : Height of the entity in pixels. One of `width` or `height` must be 16 pixels.

    `enabled_at_start` (boolean, optional, default: `true`)
    : Whether the entity should be initially enabled.

    `properties` (table, optional)
    : Additional user-defined properties. See [`entity:set_properties()`](./map-entities/index.md#entityset_propertiesproperties) for the specification.

Return value ([separator](./map-entities/separator.md))
: The separator created.

### `map:create_custom_entity(properties)`

Creates an entity of type [custom entity](./map-entities/custom-entity.md) on the map.

`properties` (table)
: A table that describles all properties of the entity to create. Its key-value pairs must be:

    `name` (string or `nil`, optional)
    : Name identifying the entity or `nil`. If the name is already used by another entity, a suffix (of the form `"_2"`, `"_3"`, etc.) will be automatically appended to keep entity names unique.

    `direction` (number)
    : Direction of the custom entity, between `0` (East) and `3` (South). This direction will be applied to the entity's sprites if possible.

    `layer` (number)
    : The layer, between [`map:get_min_layer()`](#mapget_min_layer) and [`map:get_max_layer()`](#mapget_max_layer).

    `x` (number)
    : X coordinate on the map.

    `y` (number)
    : Y coordinate on the map.

    `width` (number, optional, default: `16`)
    : Width of the entity in pixels.

    `height` (number, optional, default: `16`)
    : Height of the entity in pixels.

    `origin_x` (number, optional, default: `8`)
    : X coordinate of the origin point, relative to the entity's upper left corner.

    `origin_y` (number, optional, default: `13`)
    : Y coordinate of the origin point, relative to the entity's upper left corner.

    `sprite` (string, optional)
    : Name of the animation set of a [sprite](./drawable-objects/sprite.md) to create for the custom entity.

    `tiled` (boolean, optional, default: `false`)
    : Whether to repeat the sprite with tiling to fit the size of the custom entity, when the size is bigger than the sprite.

    `model` (string, optional)
    : Model of custom entity or `nil` The model is the name of a Lua script in the `"entities"` directory of your quest. It will define the behavior of your entity. This script will be called with the entity as parameter. Models are useful when you need to create lots of similar entities, especially in different maps. `nil` means no model: in this case, no particular script will be called but you can still define the behavior of your entity in the map script.

    `enabled_at_start` (boolean, optional, default: `true`)
    : Whether the entity should be initially enabled.

    `properties` (table, optional)
    : Additional user-defined properties. See [`entity:set_properties()`](./map-entities/index.md#entityset_propertiesproperties) for the specification.

Return value ([custom entity](./map-entities/custom-entity.md))
: The custom entity created.

## Events of the type `map`

Events are callback methods automatically called by the engine if you define them. In the case of maps, they are only called on the current map.

### `map:on_started(destination)`

Called when this map starts (when the player enters it).

`destination` ([destination](./map-entities/destination.md) or `nil`)
: The destination entity from where the [hero](./map-entities/hero.md) arrives on the map, or `nil`. If he used another way than a destination entity (like the side of the map or direct coordinates).

### `map:on_finished()`

Called when this map stops (when the player leaves it).

### `map:on_update()`

Called at each cycle of the main loop while this map is the current one.

!!! note "Note"

    As this function is called at each cycle, it is recommended to use other solutions when possible, like [timers](./timers.md) and other events.

### `map:on_draw(camera_surface)`

Called when the map has just been redrawn by the engine.

The engine has already drawn the map contents, but not the [menus](./menus.md) of this map if any. Use this event if you want to draw some additional content on the map before the menus, for example an overlay.

`camera_surface` ([surface](./drawable-objects/surface.md))
: The surface of the camera being drawn. This surface represents the visible part of the camera, not the whole map.

!!! note "Note"

    If the map has multiple cameras, this event is called for every camera.

### `map:on_suspended(suspended)`

Called when the map has just been suspended or resumed.

The map is suspended by the engine in a few cases, like when the [game](./game.md) is paused or when a dialog is active. When this happens, all [map entities](./map-entities/index.md) stop moving and most [sprites](./drawable-objects/sprite.md) stop their animation.

`suspended` (boolean)
: `true` if the map was just suspended, `false` if it was resumed.

### `map:on_opening_transition_finished(destination)`

When the map begins, called when the opening transition effect finishes.

`destination` ([destination](./map-entities/destination.md) or `nil`)
: The destination entity from where the [hero](./map-entities/hero.md) arrived on the map, or `nil` if he used another way than a destination entity (like the side of the map or direct coordinates).

### `map:on_obtaining_treasure(treasure_item, treasure_variant, treasure_savegame_variable, hero)`

Called when the [hero](./map-entities/hero.md) is obtaining a treasure on this map, before the treasure's dialog (if any).

`treasure_item` ([item](./equipment-items.md))
: Equipment item being obtained.

`treasure_variant` (number)
: Variant of the treasure (because some [equipment items](./equipment-items.md) may have several variants).

`treasure_savegame_variable` (string or `nil`)
: Name of the boolean value that stores in the [savegame](./game.md) whether this treasure is found, or `nil` if this treasure is not saved.

`hero` ([hero](./map-entities/hero.md))
: The hero who is obtaining this treasure.

### `map:on_obtained_treasure(treasure_item, treasure_variant, treasure_savegame_variable, hero)`

Called after the [hero](./map-entities/hero.md) has obtained a treasure on this map.

In the case of a brandished treasure, this event is called once the treasure's dialog is finished. Otherwise, it is called immediately after [`map:on_obtaining_treasure()`](#mapon_obtaining_treasuretreasure_item-treasure_variant-treasure_savegame_variable-hero).

`treasure_item` ([item](./equipment-items.md))
: Equipment item being obtained.

`treasure_variant` (number)
: Variant of the treasure (because some [equipment items](./equipment-items.md) may have several variants).

`treasure_savegame_variable` (string or `nil`)
: Name of the boolean value that stores in the [savegame](./game.md) whether this treasure is found, or `nil` if this treasure is not saved.

`hero` ([hero](./map-entities/hero.md))
: The hero who obtained the treasure.

### `map:on_command_pressed(command)`

Called when the player presses a [game command](./game.md#game-commands) (a keyboard key or a joypad action mapped to a built-in game behavior) while this map is active. You can use this event to override the normal built-in behavior of the game command.

`command` (string)
: Name of the built-in game command that was pressed. Possible commands are:

    - `"action"`
    - `"attack"`
    - `"pause"`
    - `"item_1"`
    - `"item_2"`
    - `"right"`
    - `"up"`
    - `"left"`
    - `"down"`

Return value (boolean)
: Indicates whether the event was handled. If you return `true`, the event won't be propagated to other objects (you are overriding the built-in behavior of pressing this game command).

!!! note "Note"

    This event is not triggered if you already handled its underlying low-level keyboard or joypad event.

### `map:on_command_released(command)`

Called when the player released a [game command](./game.md#game-commands) (a keyboard key or a joypad action mapped to a built-in game behavior). while this map is active. You can use this event to override the normal built-in behavior of the game command.

`command` (string)
: Name of the built-in game command that was released. Possible commands are:

    - `"action"`
    - `"attack"`
    - `"pause"`
    - `"item_1"`
    - `"item_2"`
    - `"right"`
    - `"up"`
    - `"left"`
    - `"down"`

Return value (boolean)
: Indicates whether the event was handled. If you return `true`, the event won't be propagated to other objects (you are overriding the built-in behavior of releasing this game command).

!!! note "Note"

    This event is not triggered if you already handled its underlying low-level keyboard or joypad event.

## Events as an Input Handler

In addition to its own events a map is also an input handler and receives all of those events.

The current map is checked after the game and before the hero's state.

See [inputs](./controls/inputs.md) to see all of these events.

## Deprecated methods of the type `map`

The following methods are deprecated and may be removed it future releases.

### `map:get_camera_position()`

Returns the currently visible area of the map.

!!! warning "Deprecated"

    This method is deprecated since Solarus 1.5. The [camera](./map-entities/camera.md) is now an entity. Therefore, you can get its coordinates and size like any other entity. Use [`map:get_camera():get_bounding_box()`](./map-entities/index.md#entityget_bounding_box) instead.

### `map:move_camera(x, y, speed, callback, [delay_before], [delay_after])`

Starts a camera moving sequence.

```lua
function map:move_camera(x, y, speed, callback, delay_before, delay_after)

  local camera = map:get_camera()
  local game = map:get_game()
  local hero = map:get_hero()

  delay_before = delay_before or 1000
  delay_after = delay_after or 1000

  local back_x, back_y = camera:get_position_to_track(hero)
  game:set_suspended(true)
  camera:start_manual()

  local movement = sol.movement.create("target")
  movement:set_target(camera:get_position_to_track(x, y))
  movement:set_ignore_obstacles(true)
  movement:set_speed(speed)
  movement:start(camera, function()
    local timer_1 = sol.timer.start(map, delay_before, function()
      callback()
      local timer_2 = sol.timer.start(map, delay_after, function()
        local movement = sol.movement.create("target")
        movement:set_target(back_x, back_y)
        movement:set_ignore_obstacles(true)
        movement:set_speed(speed)
        movement:start(camera, function()
          game:set_suspended(false)
          camera:start_tracking(hero)
          if map.on_camera_back ~= nil then
            map:on_camera_back()
          end
        end)
      end)
      timer_2:set_suspended_with_map(false)
    end)
    timer_1:set_suspended_with_map(false)
  end)
end
```

This function temporarily moves the camera somewhere else, like to a place where a [chest](./map-entities/chest.md) or an [enemy](./map-entities/enemy.md) will appear.

The camera first moves towards a target point. When the target is reached, after a first delay, your callback function is called. Then, after a second delay, the camera moves back towards the hero. The game is suspended during the whole sequence.

`x` (number)
: X coordinate of the target, relative to the map's top-left corner.

`y` (number)
: Y coordinate of the target, relative to the map's top-left corner.

`speed` (number)
: Speed of the camera movement in pixels per second.

`callback` (function)
: A function to be called when the camera reaches the target (after `delay_before`)

`delay_before` (number or `nil`)
: A delay in milliseconds before calling your function once the target is reached (default `1000`)

`delay_after` (number, optional)
: A delay in milliseconds after calling your function, before the camera goes back (default `1000`)

!!! warning "Deprecated"

    This method is deprecated since Solarus 1.5. The [camera](./map-entities/camera.md) is now an entity. Therefore, you can move the camera like any other entity and have much more customization possibilities. It can be rewritten in pure Lua as follows.

## Deprecated events of the type `map`

The following events are deprecated and may be removed it future releases.

### `map:on_camera_back()`

After a camera sequence initiated by [`map:move_camera()`](#mapmove_camerax-y-speed-callback-delay_before-delay_after), this event is triggered when the camera gets back to the [hero](./map-entities/hero.md).

!!! warning "Deprecated"

    This event is deprecated since Solarus 1.5. Indeed, [`camera:move()`](#mapmove_camerax-y-speed-callback-delay_before-delay_after) is deprecated. The [camera](./map-entities/camera.md) is now an entity and now has much more customization possibilities. Use [`camera:on_state_changed()`](./map-entities/camera.md#cameraon_state_changednew_state_name) instead, or the callback parameter of [`movement:start()`](./movements/index.md#movementstartobject_to_move-callback)
