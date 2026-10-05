# Map Entities

## Overview

Objects placed on the [map](../map.md) are called map entities (or just entities).

There exists many types of entities. They can be either declared in the [map data file](../../files-specs/map-data-file.md), or created dynamically by using the `map:create_*` methods of the [map API](../map.md).

All entities have a position on the map (X, Y and layer) and a size. Depending on their type, they can be visible or not. When they are visible, they are usually represented by one or several [sprites](../drawable-objects/sprite.md). Some entities are fixed, others move according to a [movement](../movements/index.md) object.

Entities can also have a name that uniquely identifies them on the map. This is useful to access them from the [map API](../map.md). The name is optional, but if an entity has a name, it must be unique on the map.

Here are the existing types of entities.

| Entity                                   | Description                                                                                                                       |
| ---------------------------------------- | --------------------------------------------------------------------------------------------------------------------------------- |
| [Hero](./hero.md)                        | The character controlled by the player.                                                                                           |
| [Tile](./tile.md)                        | A small brick that composes a piece of the [map](../map.md), with a pattern picked from the tileset.                              |
| [Dynamic tile](./dynamic-tile.md)        | A special [tile](./tile.md) that can be enabled or disabled dynamically ([usual tiles](./tile.md) are optimized away at runtime). |
| [Teletransporter](./teletransporter.md)  | When walking on it, the [hero](./hero.md) is transported somewhere, possibly on the same [map](../map.md) or another map.         |
| [Destination](./destination.md)          | A possible destination place for [teletransporters](./teletransporter.md).                                                        |
| [Pickable Treasure](./pickable.md)       | A treasure placed on the ground and that the [hero](./hero.md) can pick up.                                                       |
| [Destructible Object](./destructible.md) | An entity that can be cut or lifted by [hero](./hero.md), and that may hide a [pickable treasure](./pickable.md).                 |
| [Carried Object](./carried-object.md)    | Something lifted and carried by the [hero](./hero.md).                                                                            |
| [Chest](./chest.md)                      | A chest that contains a treasure.                                                                                                 |
| [Shop treasure](./shop-treasure.md)      | A treasure that the [hero](./hero.md) can buy for a price.                                                                        |
| [Enemy](./enemy.md)                      | A bad guy (possibly a boss) who may also drop a [pickable treasure](./pickable.md) when killed.                                   |
| [Non-Playing Character](./npc.md)        | Somebody or something the [hero](./hero.md) can interact with. Often abbreviated as NPC.                                          |
| [Block](./block.md)                      | An entity that the [hero](./hero.md) can push or pull.                                                                            |
| [Jumper](./jumper.md)                    | When walking on it, the [hero](./hero.md) jumps into a direction.                                                                 |
| [Switch](./switch.md)                    | A button or another mechanism that the [hero](./hero.md) can activate.                                                            |
| [Sensor](./sensor.md)                    | An invisible detector that detects the presence of the [hero](./hero.md).                                                         |
| [Separator](./separator.md)              | An horizontal or vertical separation between two parts of the map.                                                                |
| [Wall](./wall.md)                        | An invisible object that stops some kinds of entities.                                                                            |
| [Crystal](./crystal.md)                  | A switch that lowers or raises [crystal blocks](./crystal-block.md).                                                              |
| [Crystal Block](./crystal-block.md)      | A low wall that can be lowered (travserable) or raised (obstacle) using a [crystal](./crystal.md).                                |
| [Stream](./stream.md)                    | When walking on it, the [hero](./hero.md) automatically moves into a direction.                                                   |
| [Door](./door.md)                        | A door to open with an [equipment item](../equipment-items.md) or another condition.                                              |
| [Stairs](./stairs.md)                    | Stairs between two [maps](../map.md) or to a platform of a single map.                                                            |
| [Bomb](./bomb.md)                        | A bomb that will explode after a few seconds and that may be lifted by the [hero](./hero.md).                                     |
| [Explosion](./explosion.md)              | An explosion that can hurt the [hero](./hero.md) and the [enemies](./enemy.md).                                                   |
| [Fire](./fire.md)                        | A flame that can hurt [enemies](./enemy.md) and interact with other entities.                                                     |
| [Arrow](./arrow.md)                      | An arrow shot by the bow.                                                                                                         |
| [Hookshot](./hookshot.md)                | A hookshot shot by the [hero](./hero.md).                                                                                         |
| [Boomerang](./boomerang.md)              | A boomerang shot by the [hero](./hero.md).                                                                                        |
| [Camera](./camera.md)                    | A rectangle that determines the visible area of the map.                                                                          |
| [Custom Entity](./custom-entity.md)      | An entity fully controlled by your Lua scripts.                                                                                   |

!!! note "Note"

    Note that [sprites](../drawable-objects/sprite.md) and [movements](../movements/index.md) are not map entities, but they can be attached to map entities (to display them and to move them, respectively). Sprites and movements can also be used outside a [map](../map.md), for example in your title screen or in other [menus](../menus.md).

## Methods of all entity types

These methods exist in all entity types.

### `entity:get_type()`

Returns the type of entity.

Return value (string)
: The type of this entity. Can be one of:

    - `"hero"`
    - `"dynamic_tile"`
    - `"teletransporter"`
    - `"destination"`
    - `"pickable"`
    - `"destructible"`
    - `"carried_object"`
    - `"chest"`
    - `"shop_treasure"`
    - `"enemy"`
    - `"npc"`
    - `"block"`
    - `"jumper"`
    - `"switch"`
    - `"sensor"`
    - `"separator"`
    - `"wall"`
    - `"crystal"`
    - `"crystal_block"`
    - `"stream"`
    - `"door"`
    - `"stairs"`
    - `"bomb"`
    - `"explosion"`
    - `"fire"`
    - `"arrow"`
    - `"hookshot"`
    - `"boomerang"`
    - `"custom_entity"`

!!! note "Note"

    The type `"tile"` is not is this list because [tiles](./tile.md) don't exist at runtime for optimization reasons.

### `entity:get_map()`

Returns the map this entity belongs to.

Return value ([map](../map.md))
: The map that contains this entity.

### `entity:get_game()`

Returns the game that is running the map this entity belongs to.

Return value ([game](../game.md))
: The current game.

### `entity:get_name()`

Returns the name of this map entity.

The name uniquely identifies the entity on the map.

Return value (string or `nil`)
: The name of this entity, or `nil` if the entity has no name (because the name is optional).

### `entity:set_name(name)`

Sets the name of this map entity.

The name uniquely identifies the entity on the map. If there is already another entity on the same map with this name, then a numbered suffix is automatically appended to make sure that entity names are unique.

`name` (string or `nil`)
: The name of this entity, or `nil` to set no name (because the name is optional).

### `entity:exists()`

Returns whether this entity still exists on the map.

An entity gets destroyed when you call [`entity:remove()`](#entityremove) or when the engine removes it (for example an [enemy](./enemy.md) that gets killed or a [pickable treasure](./pickable.md) that gets picked up). If you refer from Lua to an entity that no longer exists in the C++ side, this method returns `false`.

Return value (boolean)
: `true` if the entity exists, `false` if it was destroyed.

### `entity:remove()`

Removes this entity from the map and destroys it.

After the entity is destroyed, [`entity:exists()`](#entityexists) returns `false` and there is no reason to keep a reference to it in the Lua side (though it is harmless).

### `entity:is_enabled()`

Returns whether this entity is enabled.

When an entity is disabled, it is not displayed on the map, it does not move and does not detect collisions. But it still exists, it still has a position and it can be enabled again later.

Return value (boolean)
: `true` if this entity is enabled.

### `entity:set_enabled([enabled])`

Enables or disables this entity.

When an entity is disabled, it is not displayed on the map, it does not move and does not detect collisions. Its [movement](../movements/index.md), its [sprites](../drawable-objects/sprite.md) and its [timers](../timers.md) if any are suspended and will be resumed when the entity gets enabled again. While the entity is disabled, it still exists, it still has a position and it can be enabled again later.

`enabled` (boolean, optional, default: `true`)
: `true` to enable the entity, `false` to disable it.

### `entity:get_size()`

Returns the size of the bounding box of this entity.

The [bounding box](#entityget_bounding_box) is a rectangle that determines the position of the entity on the map. The bounding box is used to detect whether the entity overlaps obstacles or other entities.

Return value 1 (number)
: Width of the entity in pixels.

Return value 2 (number)
: Height of the entity in pixels.

### `entity:set_size(width, height)`

Sets the size of the [bounding box](#entityget_bounding_box) of this entity.

This is the effective size used to detect obstacles when moving, but the [`sprite(s)`](../drawable-objects/sprite.md) of the entity may be larger.

The default value depends on the type of entity and is often `16×16` pixels.

`width` (number)
: Width of the entity in pixels.

`height` (number)
: Height of the entity in pixels.

!!! note "Note"

    Note that the [sprites](../drawable-objects/sprite.md) of an entity may have a different size than the entity itself. See [`sprite:get_size()`](../drawable-objects/sprite.md#spriteget_sizeanimation_name-direction) to know it.

### `entity:get_origin()`

Returns the origin point of this entity, relative to the upper left corner of its [bounding box](#entityget_bounding_box).

When an entity is located at some coordinates on the [map](../map.md), the origin points determines what exact point of the entity's bounding box is at those coordinates. It is not necessarily the upper left corner of the entity's bounding box.

The default origin point depends on the type of entity. By convention, it is usually be the central point of contact between the entity and the soil. For most entities, including the [hero](./hero.md), [enemies](./enemy.md), [non-playing characters](./npc.md) and [custom entities](./custom-entity.md), the default origin point is `8, 13` as their default size is `16×16`. More generally, the convention is to have an origin point of `width / 2, height - 3`.

This origin point property allows entities of different sizes to have comparable reference points. Indeed, when two entities to be drawn in Y order overlap, the engine needs to determine which one has to be displayed first (it is always the one with the lowest Y coordinate). Using the upper left corner Y coordinate for this would not work well with entities of different sizes.

Similarly, if you need to compute an [angle](#entityget_anglex-y-entityget_angleother_entity) between two entities to move an entity away from another one, the calculation uses the origin point of both entities. Using the upper left corner of their bounding box would not give the accurate angle with entities of different sizes.

The origin point is also the point of synchronization of an entity with its [sprites](../drawable-objects/sprite.md) (because again, an entity that has a given size may have sprites with different sizes).

Return value 1 (number)
: X coordinate of the origin point in pixels, relative to the upper left corner of the entity's bounding box.

Return value 2 (number)
: Y coordinate of the origin point in pixels, relative to the upper left corner of the entity's bounding box.

### `entity:set_origin(origin_x, origin_y)`

Sets the origin point of this entity, relative to the upper left corner of its [bounding box](#entityget_bounding_box).

See [`entity:get_origin()`](#entityget_origin) for details about the origin point.

`origin_x` (number)
: X coordinate of the origin point in pixels, relative to the upper left corner of the entity's bounding box.

`origin_y` (number)
: Y coordinate of the origin point in pixels, relative to the upper left corner of the entity's bounding box.

!!! note "Note"

    When you call this method, the bounding box of the entity is moved so that coordinates (as returned by [`entity:get_position()`](#entityget_position)) do not change. If this is not what you need, you can call [`entity:set_position()`](#entityset_positionx-y-layer) then to adjust the entity's position to the coordinates you want.

### `entity:get_position()`

Returns the position of this entity on the [map](../map.md) (coordinates and layer).

Return value 1 (number)
: X coordinate of the [origin point](#entityget_origin) of the entity, relative to the upper left corner of the map.

Return value 2 (number)
: Y coordinate of the [origin point](#entityget_origin) of the entity, relative to the upper left corner of the map.

Return value 3 (number)
: Layer where the entity is on the map, between [`map:get_min_layer()`](../map.md#mapget_min_layer) and [`map:get_max_layer()`](../map.md#mapget_max_layer).

### `entity:set_position(x, y, [layer])`

Changes instantly the position of this entity on the map (coordinates and layer). The [origin point](#entityget_origin) of the entity gets placed at these coordinates, relative to the map's upper left corner. Any previous movement or other action performed by the entity continues normally.

`x` (number)
: X coordinate to set.

`y` (number)
: Y coordinate to set.

`layer` (number, optional)
: Layer to set, between [`map:get_min_layer()`](../map.md#mapget_min_layer) and [`map:get_max_layer()`](../map.md#mapget_max_layer). By default, the layer is unchanged.

!!! note "Note"

    Be careful: this function does not check collisions with obstacles.

### `entity:get_center_position()`

Returns the coordinates of the center point of this entity on the [map](../map.md).

Return value 1 (number)
: X coordinate of the center of this entity's bounding box, relative to the upper left corner of the map.

Return value 2 (number)
: Y coordinate of the center of this entity's bounding box, relative to the upper left corner of the map.

Return value 3 (number)
: Layer of the entity.

### `entity:get_facing_position()`

Returns the coordinates of the point this entity is looking at. This point depends on the direction of the main sprite if any. If the entity has no sprite, or if the main sprite has not 4 directions, then the movement is considered. If there is no movement either, the entity is assumed to look to the North.

Return value 1 (number)
: X coordinate of the facing point of this entity, relative to the upper left corner of the map.

Return value 2 (number)
: Y coordinate of the facing point of this entity, relative to the upper left corner of the map.

Return value 3 (number)
: Layer of the entity.

### `entity:get_facing_entity()`

Returns the entity this entity is looking at, if any.

This is an entity overlapping the [facing position](#entityget_facing_position) of this entity. If several entities are overlapping the facing position, the first one in Z order is returned.

Return value (entity or `nil`)
: The facing entity, or `nil` if there is no entity in front of this entity.

### `entity:get_ground_position()`

Returns the coordinates of the point used for ground detection for this entity on the [map](../map.md).

The ground position is the point tested by all features related to the ground, like all effects of various grounds on the [hero](./hero.md), the result of [`entity:get_ground_below()`](#entityget_ground_below) and the event [`custom_entity:on_ground_below_changed()`](./custom-entity.md#custom_entityon_ground_below_changedground_below).

Return value 1 (number)
: X coordinate of the ground point of this entity, relative to the upper left corner of the map.

Return value 2 (number)
: Y coordinate of the ground point of this entity, relative to the upper left corner of the map.

Return value 3 (number)
: Layer of the entity.

!!! note "Note"

    The ground point of an entity is slightly (2 pixels) above its origin point as returned by [`entity:get_position()`](#entityget_position).

### `entity:get_ground_below()`

Returns the map's ground below this entity.

The ground is defined by the topmost [tile](./tile.md) below this entity, plus potential dynamic entities that may affect the ground, like [dynamic tiles](./dynamic-tile.md), [destructibles](./destructible.md) and [custom entities](./custom-entity.md).

The exact point tested is the one returned by [`entity:get_ground_position()`](#entityget_ground_position), and it is slightly different from [`entity:get_position()`](#entityget_position).

Return value (string)
: The ground below this entity. See [`map:get_ground()`](../map.md#mapget_groundx-y-layer) for the list of possible grounds.

### `entity:get_bounding_box()`

Returns the rectangle representing the [coordinates](#entityget_position) and [size](#entityget_size) of this entity on the [map](../map.md).

The [bounding box](#entityget_bounding_box) is a rectangle that determines the position of the entity on the map. The bounding box is used to detect whether the entity overlaps obstacles or other entities.

Return value 1 (number)
: X coordinate of the upper left corner of the bounding box.

Return value 2 (number)
: Y coordinate of the upper left corner of the bounding box.

Return value 3 (number)
: Width of the bounding box.

Return value 4 (number)
: Height of the bounding box.

!!! note "Note"

    The sprites of this entity (if any) may exceed the bounding box.

    See [`entity:get_max_bounding_box()`](#entityget_max_bounding_box).

### `entity:get_max_bounding_box()`

Returns the rectangle surrounding the bounding box of this entity plus the bounding boxes of its sprites in all their possible animations and directions.

This is usually larger than [`entity:get_bounding_box()`](#entityget_bounding_box), because the sprite of an entity often exceeds its bounding box.

Return value 1 (number)
: X coordinate of the upper left corner of the sprites bounding box.

Return value 2 (number)
: Y coordinate of the upper left corner of the sprites bounding box.

Return value 3 (number)
: Width of the sprites bounding box.

Return value 4 (number)
: Height of the sprites bounding box.

### `entity:get_layer()`

Returns the layer of this entity on the [map](../map.md).

Return value (number)
: Layer where the entity is on the map, between [`map:get_min_layer()`](../map.md#mapget_min_layer) and [`map:get_max_layer()`](../map.md#mapget_max_layer).

### `entity:set_layer(layer)`

Changes the layer of this entity on the map. The X and Y coordinates of the entity are unchanged. Any previous movement or action performed by the entity continues normally.

`layer` (number)
: Layer to set, between [`map:get_min_layer()`](../map.md#mapget_min_layer) and [`map:get_max_layer()`](../map.md#mapget_max_layer).

!!! note "Note"

    Be careful: this function does not check collisions with obstacles.

### `entity:overlaps(x, y, [width, height]), entity:overlaps(other_entity, [collision_mode, [entity_sprite, [other_entity_sprite]]])`

Returns whether the [bounding box](#entityget_bounding_box) of this entity overlaps the specified rectangle or point, or whether another entity collides with this entity according to the specified collision test.

| With a rectangle or a point (a point is a rectangle of size `1x1`) | With another entity |
|--------------------------------------------------------------------|---------------------|
| <dl><dt>`x` (number)</dt><dd>X coordinate of the upper left corner of the rectangle to check.</dd><dt>`y` (number)</dt><dd>Y coordinate of the upper left corner of the rectangle to check.</dd><dt>`width` (number, optional, requires: `height`, default: `1`)</dt><dd>Width of the rectangle.</dd><dt>`height` (number, optional, requires: `width`, default: `1`)</dt><dd>Height of the rectangle.</dd></dl> | <dl><dt>`other_entity` (entity)</dt><dd>Another entity.</dd><dt>`collision_mode` (string, optional)</dt><dd><p>Specifies what kind of collision you want to test. This may be one of:</p><ul><li>`"overlapping"` (default): Collision if the [bounding box](#entityget_bounding_box) of both entities overlap.</li><li>`"containing"`: Collision if the bounding box of the other entity is fully inside the bounding box of this entity.</li><li>`"origin"`: Collision if the [origin point](#entityget_origin) or the other entity is inside the bounding box of this entity.</li><li>`"center"`: Collision if the [center point](#entityget_center_position) of the other entity is inside the bounding box of this entity.</li><li>`"facing"`: Collision if the [facing position](#entityget_facing_position) of the other entity's bounding box is touching this entity's bounding box. Bounding boxes don't necessarily overlap, but they are in contact: there is no space between them. When you consider the bounding box of an entity, which is a rectangle with four sides, the facing point is the middle point of the side the entity is oriented to. This `"facing"` collision test is useful when the other entity cannot traverse your custom entity. For instance, if the other entity has direction "east", there is a collision if the middle of the east side of its bounding box touches (but does not necessarily overlap) this entity's bounding box. This is typically what you need to let the hero interact with this entity when he is looking at it.</li><li>`"touching"`: Like `"facing"`, but accepts all four sides of the other entity's bounding box, no matter its direction.</li><li>`"sprite"`: Collision if a sprite of the other entity overlaps a sprite of this entity. The collision test is pixel precise. The last two optional sprite parameters can then indicate which sprite of both entities you want to test. If you don't set them, all sprites of both entities will be tested.</li></ul></dd><dt>`entity_sprite` ([sprite](../drawable-objects/sprite.md) or `nil`, optional, requires: `collision_mode`)</dt><dd>Sprite of this entity you want to test (only with collision mode `"sprite"`) `nil` or no value means to test all sprites of this entity.</dd><dt>`other_entity_sprite` ([sprite](../drawable-objects/sprite.md) or `nil`, optional, requires: `entity_sprite`)</dt><dd>Sprite of the other entity you want to test (only with collision mode `"sprite"`) `nil` or no value means to test all sprites of the other entity.</dd></dl>|

Return value (boolean)
: `true` if a collision is detected with this collision test.

!!! note "Note"

    For custom entities, see also [`custom_entity:add_collision_test()`](./custom-entity.md#custom_entityadd_collision_testcollision_mode-callback) to be automatically notified when a collision is detected.

### `entity:get_distance(x, y), entity:get_distance(other_entity)`

Returns the distance in pixels between this map entity and a point or another map entity.

To compute the distance to a specified point:

| With coordinates | With another entity |
|------------------|------------------------------|
| <dl><dt>`x` (number)</dt><dd>X coordinate of the point.</dd><dt>`y` (number)</dt><dd>Y coordinate of the point.</dd></dl> | <dl><dt>`other_entity` (entity)</dt><dd>The entity to compute the distance to.</dd></dl>|

Return value (number)
: The Euclidean distance in pixels between the origin point of this entity and the point. The result is rounded down to the nearest integer.

### `entity:get_angle(x, y), entity:get_angle(other_entity)`

Returns the angle between the X axis and the vector that joins this entity to a point.

To compute the angle to a specified point:

| With coordinates | With another entity |
|------------------|------------------------------|
| <dl><dt>`x` (number)</dt><dd>X coordinate of the point.</dd><dt>`y` (number)</dt><dd>Y coordinate of the point.</dd></dl> | <dl><dt>`other_entity` (entity)</dt><dd>The entity to compute the angle to.</dd></dl>|

Return value (number)
: The angle in radians between the origin point of this entity and the specified point. The angle is between `0` and `2 * math.pi`.

### `entity:get_direction4_to(x, y), entity:get_direction4_to(other_entity)`

Like [`entity:get_angle()`](#entityget_anglex-y-entityget_angleother_entity), but instead of an angle in radians, returns the closest direction among the 4 main directions.

This is a utility function that essentially rounds the result of [`entity:get_angle()`](#entityget_anglex-y-entityget_angleother_entity).

To compute the direction to a specified point:

| With coordinates | With another entity |
|------------------|------------------------------|
| <dl><dt>`x` (number)</dt><dd>X coordinate of the point.</dd><dt>`y` (number)</dt><dd>Y coordinate of the point.</dd></dl> | <dl><dt>`other_entity` (entity)</dt><dd>The entity to compute the direction to.</dd></dl>|

Return value (number)
: The direction this entity should take to look at this point, between 0 (East) and 3 (South).

### `entity:get_direction8_to(x, y), entity:get_direction8_to(other_entity)`

Like [`entity:get_angle()`](#entityget_anglex-y-entityget_angleother_entity), but instead of an angle in radians, returns the closest direction among the 8 main directions.

This is a utility function that essentially rounds the result of [`entity:get_angle()`](#entityget_anglex-y-entityget_angleother_entity).

To compute the direction to a specified point:

| With coordinates | With another entity |
|------------------|------------------------------|
| <dl><dt>`x` (number)</dt><dd>X coordinate of the point.</dd><dt>`y` (number)</dt><dd>Y coordinate of the point.</dd></dl> | <dl><dt>`other_entity` (entity)</dt><dd>The entity to compute the direction to.</dd></dl>|

Return value (number)
: The direction this entity should take to look at this point, between `0` (East) and `7` (South-East).

### `entity:snap_to_grid()`

Makes sure this entity's upper left corner is aligned with the `8x8` grid of the [map](../map.md).

!!! note "Note"

    Be careful: this function does not check collisions with obstacles.

### `entity:bring_to_front()`

Places this entity in front of all other entities on the same layer.

Since entities that are on the same layer can overlap, you can use this function to change their Z order.

!!! note "Note"

    Some entities can have have the property to be [drawn in Y order](#entityis_drawn_in_y_order) rather than in Z order. This function only has an effect on entities drawn in Z order.

### `entity:bring_to_back()`

Places this entity behind all other entities on the same layer.

Since entities that are on the same layer can overlap, you can use this function to change their Z order.

!!! note "Note"

    Some entities can have have the property to be [drawn in Y order](#entityis_drawn_in_y_order) rather than in Z order. This function only has an effect on entities drawn in Z order.

### `entity:is_drawn_in_y_order()`

Returns whether this entity should be drawn in Y order or in Z order.

The map is drawn layer by layer, and each layer is drawn in two passes: first, entities displayed in Z order, and then, entities displayed in Y order.

The Z order is the creation order of entities unless you call [`entity:bring_to_front()`](#entitybring_to_front) or [`entity:bring_to_back()`](#entitybring_to_back).

The Y order compares the Y coordinate of entities on the map. Entities drawn in Y order are displayed from the one the most to the north to the one the most to the south.

Usually, entities representing a character or something that is standing should be drawn in Y order, so that it looks correct when some of them overlap with each other, and flat entities should be drawn in Z order.

The default setting depends on the type of entities.

Return value (boolean)
: `true` if this entity is displayed in Y order, `false` if it is displayed in Z order.

### `entity:set_drawn_in_y_order([y_order])`

Sets whether this entity should be drawn in Y order or in Z order.

See [`entity:is_drawn_in_y_order()`](#entityis_drawn_in_y_order) for details about the Y order and the Z order.

`y_order` (boolean, optional, default: `true`)
: `true` to display this entity in Y order, `false` to display it in Z order.

### `entity:get_optimization_distance()`

Returns the optimization threshold hint of this map entity.

Above this distance from the camera, the engine may decide to skip updates or drawings. This is only a hint: the engine is responsible of the final decision. A value of `0` means an infinite distance (the entity is never optimized away).

Return value (number)
: The optimization distance hint in pixels.

### `entity:set_optimization_distance(optimization_distance)`

Sets the optimization threshold hint of this map entity.

Above this distance from the camera, the engine may decide to skip updates or drawings. This is only a hint: the engine is responsible of the final decision.

A value of `0` means an infinite distance (the entity is never optimized away). The default value is `0`.

`optimization_distance` (number)
: The optimization distance hint to set in pixels.

### `entity:is_in_same_region(x, y), entity:is_in_same_region(other_entity)`

Returns whether this entity is in the same region as a position or another entity.

Regions of the map are defined by the position of [separators](./separator.md) and map limits. The region of an entity is the one of its center point.

Regions should be rectangular. Non-convex regions, for example with an "L" shape, are not supported by this function.

You can use this function to make sure that an entity is in the same region as an arbitrary point.

| With coordinates | With another entity |
|------------------|------------------------------|
| <dl><dt>`x` (number)</dt><dd>X coordinate of the point.</dd><dt>`y` (number)</dt><dd>Y coordinate of the point.</dd></dl> | <dl><dt>`other_entity` (entity)</dt><dd>The entity to get the region from.</dd></dl>|

Return value (boolean)
: `true` if the entity is in the same region as the position.

You can use this function to make sure that an [enemy](./enemy.md) close to the [hero](./hero.md) but in the other side of a separator won't attack the hero.

### `entity:test_obstacles([dx, dy, [layer]])`

Returns whether there would be a collision with obstacles if this map entity was placed at a given offset from its current position.

`dx` (number, optional, requires: `dy`, default: `0`)
: X offset in pixels (`0` means the current X position).

`dy` (number, optional, requires: `dx`, default: `0`)
: Y offset in pixels (`0` means the current Y position).=

`layer` (number, optional, requires: `dx` and `dy`)
: Layer to test. No value means the current layer.

Return value (boolean)
: `true` if there would be a collision in this position.

### `entity:get_sprite([name])`

Returns a [sprite](../drawable-objects/sprite.md) representing this entity.

To manage entities with multiple sprites, you can set names when you create sprites with [`entity:create_sprite()`](#entitycreate_spriteanimation_set_id-sprite_name). However, it is easier to just leave the names blank and simply store the result of these sprite creation methods. The name is more useful for built-in entities that have multiple sprites automatically created by the engine. Such entities are the [hero](./hero.md), [pickable treasures](./pickable.md), [carried objects](./carried-object.md) and [crystals](./crystal.md). See the documentation pages of these entities to know their exact sprites, the name of these sprites and which one is their main sprite. For [enemies](./enemy.md) and [custom entities](./custom-entity.md), the main sprite is the first one in Z order, which is the sprite creation order unless you call [`entity:bring_sprite_to_front()`](#entitybring_sprite_to_frontsprite) or [`entity:bring_sprite_to_back()`](#entitybring_sprite_to_backsprite).

`name` (string, optional)
: Name of the sprite to get. Only useful for entities that have multiple sprites. No value means the main sprite.

Return value ([sprite](../drawable-objects/sprite.md) or `nil`)
: The entity sprite with this name, or its main sprite if no name is specified. Returns `nil` if the entity has no such sprite.

### `entity:get_sprites()`

Returns an iterator to all [sprites](../drawable-objects/sprite.md) of this entity.

At each step, the iterator provides two values: the name of a sprite (which is an empty string if the sprite has no name) and the sprite itself. See [`entity:get_sprite()`](#entityget_spritename) for more details about named sprites.

Sprites are returned in their displaying order. Note that this order can be changed with [`entity:bring_sprite_to_front()`](#entitybring_sprite_to_frontsprite) and [`entity:bring_sprite_to_back()`](#entitybring_sprite_to_backsprite).

The typical usage of this function is:

```lua
for sprite_name, sprite in entity:get_sprites() do
  -- some code related to the sprite
end
```

Return value (function)
: An iterator to all sprites of this entity.

### `entity:create_sprite(animation_set_id, [sprite_name])`

Creates a [sprite](../drawable-objects/sprite.md) for this entity.

`animation_set_id` (string)
: Animation set to use for the sprite.

`sprite_name` (string, optional)
: An optional name to identify the created sprite. Only useful for entities with multiple sprites (see [`entity:get_sprite()`](#entityget_spritename)).

Return value ([sprite](../drawable-objects/sprite.md))
: The sprite created.

### `entity:remove_sprite([sprite])`

Removes and destroys a [sprite](../drawable-objects/sprite.md) of this entity.

`sprite` ([sprite](../drawable-objects/sprite.md), optional)
: The sprite to remove. The default value is the first sprite that was created.

### `entity:bring_sprite_to_front(sprite)`

Reorders a [sprite](../drawable-objects/sprite.md) of this entity to be displayed after other sprites (displayed to the front).

This function is only useful for entities that have multiple sprites.

`sprite` ([sprite](../drawable-objects/sprite.md))
: The sprite to reorder. It must belong to this entity.

### `entity:bring_sprite_to_back(sprite)`

Reorders a [sprite](../drawable-objects/sprite.md) of this entity to be displayed before other sprites (displayed to the back).

This function is only useful for entities that have multiple sprites.

`sprite` ([sprite](../drawable-objects/sprite.md))
: The sprite to reorder. It must belong to this entity.

### `entity:is_visible()`

Returns whether this entity is visible.

When the entity is hidden, its sprites (if any) are not displayed, but everything else continues normally, including collisions.

Return value (boolean)
: `true` if the entity is visible.

### `entity:set_visible([visible])`

Hides or shows the entity.

When the entity is hidden, its sprites (if any) are not displayed, but everything else continues normally, including collisions.

`visible` (boolean, optional, default: `true`)
: `true` to show the entity, `false` to hide it.

### `entity:get_draw_override()`

Returns the draw function of this entity.

See [`entity:set_draw_override()`](#entityset_draw_overridedraw_override) for more details.

Return value (function or `nil`)
: The draw function, or `nil` if the draw function was not overridden.

### `entity:set_draw_override(draw_override)`

Changes how this entity is drawn.

You can use this to replace the built-in draw implementation of the engine by your own function, if the default behavior does not fit your needs. To do so, your function can either call [`map:draw_visual()`](../map.md#mapdraw_visualdrawable-x-y) or draw on [`camera:get_surface()`](./camera.md).

`draw_override` (function or `nil`)
: The draw function, or `nil` to restore the built-in drawing. Your function will receive the following parameters:

    `entity` (entity)
    : The entity to draw.

    `camera` ([camera](./camera.md))
    : Camera where this entity is drawn.

!!! note "Note"

    Even when you set a draw override, events [`entity:on_pre_draw()`](#entityon_pre_drawcamera) and [`entity:on_post_draw()`](#entityon_post_drawcamera) are still called.

### `entity:get_weight()`

Returns the weight of this entity, if any.

If the entity has a weight, then the [hero](./hero.md) is allowed to lift it if his [lift ability](../game.md#gameget_abilityability_name) is greater than or equal to that weight. You can use [`entity:on_lifting()`](#entityon_liftingcarrier-carried_object) to know when an entity starts being lifted.

The default weight depends on the type of entity. Most entities cannot be lifted by default.

Return value (number)
: The level of `"lift"` ability required to lift this entity. `0` allows the [hero](./hero.md) to lift the entity unconditionally. The special value `-1` means that the entity can never be lifted.

### `entity:set_weight(weight)`

Sets the weight of this entity.

See [`entity:get_weight()`](#entityget_weight) for more details.

`weight` (number)
: The level of `"lift"` ability required to lift this entity. `0` allows the [hero](./hero.md) to lift the entity unconditionally. The special value `-1` means that the entity can never be lifted.

### `entity:get_controlling_stream()`

Returns the [stream](./stream.md) that is currently controlling this entity, if any.

Return value ([stream](./stream.md) or `nil`)
: The current stream, or `nil` if this entity is not being controlled by a stream.

### `entity:get_movement()`

Returns the current movement of this map entity.

Return value ([movement](../movements/index.md) or `nil`)
: The current movement, or `nil` if the entity has currently no movement.

!!! note "Note"

    Even when the entity is not moving, it can still have a movement of speed `0`.

### `entity:stop_movement()`

Stops the current movement of this map entity if any.

### `entity:get_property(key)`

Returns the value of a user-defined property of this entity.

User-defined properties are arbitrary key-value pairs that you can set to any entity. The engine does nothing special with them, but you can use them in your scripts to store extra information.

`key` (string)
: Name of the property to get.

Return value (string or `nil`)
: The corresponding value, or `nil` if there is no such property.

### `entity:set_property(key, value)`

Sets a user-defined property for this entity.

If the property does not exist yet, it will be created.

User-defined properties are arbitrary key-value pairs that you can set to any entity. The engine does nothing special with them, but you can use them in your scripts to store extra information.

`key` (string)
: Name of the property to set. It must be a valid identifier (only alphanumeric ASCII characters or `'_'`).

`value` (string or `nil`)
: The value to set, or `nil` to remove the property.

### `entity:get_properties()`

Returns the user-defined properties of this entity.

Return value (table)
: An array of properties. Each property is a table with two fields:

`key` (string)
: Name of the property.

`value` (string)
: Value of the property.

### `entity:set_properties(properties)`

Sets the user-defined properties of this entity.

Existing properties if any are removed.

`properties` (table)
: An array of properties. Each property is a table with two fields:

`key` (string)
: Name of the property. It must be a valid identifier (only alphanumeric ASCII characters or `'_'`).

`value` (string)
: Value of the property.

## Events of all entity types

Events are callback methods automatically called by the engine if you define them.

### `entity:on_created()`

Called when this entity has just been created on the map.

### `entity:on_removed()`

Called when this entity is about to be removed from the map (and therefore destroyed).

### `entity:on_enabled()`

called when this entity has just been [enabled](#entityis_enabled).

### `entity:on_disabled()`

called when this entity has just been [disabled](#entityis_enabled).

### `entity:on_suspended(suspended)`

Called when the entity has just been suspended or resumed.

The entity is suspended by the engine in a few cases, like when the [game](../game.md) is paused or when a dialog is active. When this happens, all [map entities](#overview) stop moving and most [sprites](../drawable-objects/sprite.md) stop their animation.

`suspended` (boolean)
: `true` if the entity was just suspended, `false` if it was resumed.

### `entity:on_position_changed(x, y, layer)`

Called when the coordinates of this entity have just changed.

`x` (number)
: The new X coordinate of the entity.

`y` (number)
: The new Y coordinate of the entity.

`layer` (number)
: The new layer of the entity.

### `entity:on_obstacle_reached(movement)`

Called when the [movement](../movements/index.md) of this entity was stopped because of an obstacle.

When an obstacle is reached, this event is called instead of [`entity:on_position_changed()`](#entityon_position_changedx-y-layer).

`movement` ([movement](../movements/index.md))
: The movement of the entity.

### `entity:on_movement_started(movement)`

Called when a [movement](../movements/index.md) is started on this entity.

`movement` ([movement](../movements/index.md))
: The movement that was just started on this entity.

### `entity:on_movement_changed(movement)`

Called when some characteristics of this entity's [movement](../movements/index.md) (like the speed or the angle) have just changed.

`movement` ([movement](../movements/index.md))
: The movement of the entity.

### `entity:on_movement_finished()`

Called when the [movement](../movements/index.md) of the entity is finished (if there is an end).

### `entity:on_lifting(carrier, carried_object)`

Called when this entity starts being lifted.

At this point, the [hero](./hero.md) is in [state](./hero.md#heroget_state) `"lifting"` The animation `"lifting"` of his sprites is playing and the player cannot control the hero.

This entity no longer exists (unless it is a destructible object that can [regenerate](./destructible.md#destructibleget_can_regenerate)). It is replaced by a [carried object](./carried-object.md) with the same sprite.

`carrier` ([entity](#overview))
: Entity that is lifting this destructible object (can only be the [hero](./hero.md) for now).

`carried_object` ([carried object](./carried-object.md))
: The carried object that was created.

### `entity:on_pre_draw(camera)`

Called just before the entity is drawn on the map.

You may display additional things below the entity. To do so, you can either call [`map:draw_visual()`](../map.md#mapdraw_visualdrawable-x-y) or draw on [`camera:get_surface()`](./camera.md).

`camera` ([camera](./camera.md))
: The camera where this entity is being drawn.

### `entity:on_post_draw(camera)`

Called just after the entity is drawn on the map.

You may display additional things above the entity. To do so, you can either call [`map:draw_visual()`](../map.md#mapdraw_visualdrawable-x-y) or draw on [`camera:get_surface()`](./camera.md).

`camera` ([camera](./camera.md))
: The camera where this entity is being drawn.
