# Destination

## Overview

A destination is a possible arrival place for [teletransporters](./teletransporter.md).

This type of [map entity](./index.md) can be declared in the [map data file](../map.md#map-files). It can also be created dynamically with [`map:create_destination()`](../map.md#mapcreate_destinationproperties).

Destinations may either have a sprite or be invisible. They may also have a direction: in this case, the [hero](./hero.md) takes that direction when arriving on the destination. Otherwise, the hero keeps his current direction.

The size of a destination is the one of the [hero](./hero.md) (16×16 pixels).

## Methods Inherited from `entity`

Destinations are particular [map entities](./index.md). Therefore, they inherit all methods from the type map entity.

See [entity](./index.md#methods-of-all-entity-types) to know these methods.

## Methods of the type `destination`

### `destination:get_starting_location_mode()`

Returns whether this destination updates the [starting location](../game.md#gameset_starting_locationmap_id-destination_name) of the player when arriving on it. If yes, when the player restarts his game, he will restart at this destination. The default value is `"when_world_changes"`

Return value (string)
: The starting location mode. Can be one of:

    - `"when_world_changes"`: Updates the starting location if the current [world](../map.md#mapget_world) has just changed when arriving to this destination.
    - `"yes"`: Updates the starting location.
    - `"no"`: Does not update the starting location.

### `destination:is_default()`

Returns whether this destination is the default destination.

Return value (boolean)
: `true` if this destination is the default one of its map.

### `destination:set_starting_location_mode(mode)`

Sets whether this destination updates the [starting location](../game.md#gameset_starting_locationmap_id-destination_name) of the player when arriving on it. If yes, when the player restarts his game, he will restart at this destination. The default value is `"when_world_changes"`.

`mode` (string)
: The starting location mode. Can be one of:

    - `"when_world_changes"`: Updates the starting location if the current [world](../map.md#mapget_world) has just changed when arriving to this destination.
    - `"yes"`: Updates the starting location.
    - `"no"`: Does not update the starting location.

### `destination:get_direction()`

Returns the direction this destination gives to heroes using it.

Return value (number or nil)
: a direction between `0` (East) and `3` (South), or `nil` if this destination does not change the hero's direction.

## Events Inherited from `entity`

Events are callback methods automatically called by the engine if you define them.

Destinations are particular [map entities](./index.md). Therefore, they inherit all events from the type map entity.

See [entity](./index.md#events-of-all-entity-types) to know these events.

## Events of the type `destination`

The following events are specific to destinations.

### `destination:on_activated(hero)`

Called when the [hero](./hero.md) arrives on this destination.

`hero` ([hero](./hero.md))
: The hero that activated the destination

The map opening transition is about to start at this point.

He may come from a [teletransporter](./teletransporter.md), from [`hero:teleport()`](./hero.md#heroteleportmap_id-destination_name-transition_style) or from the [saved starting location](../game.md#gameget_starting_location).
