# Stream

## Overview

When walking on a stream, the [hero](./hero.md) automatically moves into one of the eight main directions.

This type of [map entity](./index.md) can be declared in the [map data file](../map.md#map-files). It can also be created dynamically with [`map:create_stream()`](../map.md#mapcreate_streamproperties).

A stream is a [map entity](./index.md) of size 16×16 pixels (the size of the [hero](./hero.md)). When the hero overlaps a significant part of it, he moves into a specific direction (one of the 8 main directions).

During this process, the player may or may not continue to control the hero, use his sword and his equipment items, depending on the stream properties.

The hero is not the only map entity that can follow a stream: [Bombs](./bomb.md) also have this ability.

## Streams and holes

If you make a stream that moves toward a hole or other bad ground, it is your responsibility to make sure to call [`hero:save_solid_ground()`](./hero.md#herosave_solid_groundx-y-layer) before (typically, when entering the room), otherwise the [hero](./hero.md) will reappear on the stream, one pixel before the hole and he will fall again repeatedly.

## Methods Inherited from `entity`

Streams are particular [map entities](./index.md). Therefore, they inherit all methods from the type map entity.

See [entity](./index.md#methods-of-all-entity-types) to know these methods.

## Methods of the type stream

The following methods are specific to streams.

### `stream:get_direction()`

Returns the direction of this stream.

This direction will be applied to entities that follow the stream.

Return value (number)
: The direction between `0` (East) and `7` (South-East).

### `stream:set_direction(direction)`

Sets the direction of this stream.

This direction will be applied to entities that follow the stream.

`direction` (number)
: The direction to set, between `0` (East) and `7` (South-East).

### `stream:get_speed()`

Returns the speed applied by this stream.

Return value (number)
: The speed of the stream in pixels per second.

### `stream:set_speed(speed)`

Sets the speed applied by this stream.

`speed` (number)
: The speed to set in pixels per second. The default value is `40`

### `stream:get_allow_movement()`

Returns whether the player can still move the hero while being on this stream.

Return value (boolean)
: `true` if the player can still move, `false` if this is a blocking stream.

### `stream:set_allow_movement([allow_movement])`

Sets whether the player can still move the hero while being on this stream.

`allow_movement` (boolean, optional, default: `true`)
: `true` to allow the player to move, `false` to make a blocking stream.

### `stream:get_allow_attack()`

Returns whether the player can still use the sword while being on this stream.

Return value (boolean)
: `true` if the player use his sword.

### `stream:set_allow_attack([allow_attack])`

Sets whether the player can still use the sword while being on this stream.

`allow_attack` (boolean, optional, default: `true`)
: `true` to allow the player to use the sword.

### `stream:get_allow_item()`

Returns whether the player can still use equipment items while being on this stream.

Return value (boolean)
: `true` if the player can still use equipment itens.

### `stream:set_allow_item([allow_item])`

Sets whether the player can still use equipment items while being on this stream.

`allow_item` (boolean, optional, default: `true`)
: `true` to allow the player to use equipment items.

## Events Inherited from `entity`

Events are callback methods automatically called by the engine if you define them.

Streams are particular [map entities](./index.md). Therefore, they inherit all events from the type map entity.

See [entity](./index.md#events-of-all-entity-types) to know these events.

## Events of the type stream

None.
