# Teletransporter

## Overview

A teletransporter is a detector that sends the [hero](./hero.md) to another place when he walks on it.

This type of [map entity](./index.md) can be declared in the [map data file](../map.md#map-files). It can also be created dynamically with [`map:create_teletransporter()`](../map.md#mapcreate_teletransporterproperties).

A teletransporter can send the [hero](./hero.md) to one of the following kind of places:

- A [destination](./destination.md) entity, on the same [map](../map.md) or on another one.
- The same coordinates on another [map](../map.md).
- The side of an adjacent map (only possible for a teletransporter placed on a side of the current [map](../map.md)).

Teletransporters can have any size, but like all entities, their width and height must be multiples of 8 pixels.

!!! note "Note"

    You can also teletransport the [hero](./hero.md) explicitly with [`hero:teleport()`](./hero.md#heroteleportmap_id-destination_name-transition_style).

## Methods Inherited from `entity`

Teletransporters are particular [map entities](./index.md). Therefore, they inherit all methods from the type map entity.

See [entity](./index.md#methods-of-all-entity-types) to know these methods.

## Methods of the type `teletransporter`

The following methods are specific to teletransporters.

### `teletransporter:get_sound()`

Returns the sound to be played when the hero uses this teletransporter.

Return value (string or `nil`)
: Id of the teletransporter's sound. `nil` means that no sound will be played.

### `teletransporter:set_sound([sound_id])`

Sets the sound to be played when the hero uses this teletransporter.

`sound_id` (string or `nil`, optional)
: Id of the teletransporter's sound. `nil` or empty string means that no sound will be played.

### `teletransporter:get_transition()`

Returns the style of transition to play when the hero uses this teletransporter.

Return value (string)
: The transition style. Can be one of:

    - `"immediate"`: No transition.
    - `"fade"`: Fade-out and fade-in effect.
    - `"scrolling"`: Scrolling between maps.

### `teletransporter:set_transition(transition_style)`

Sets the style of transition to play when the hero uses this teletransporter.

`transition_style` (string)
: The transition style. Can be one of:

    - `"immediate"`: No transition.
    - `"fade"`: Fade-out and fade-in effect.
    - `"scrolling"`: Scrolling between maps.

### `teletransporter:get_destination_map()`

Returns the id of the destination [map](../map.md) of this teletransporter.

Return value (string)
: Id of the destination map.

### `teletransporter:set_destination_map(map_id)`

Sets the destination [map](../map.md) of this teletransporter.

`map_id` (string)
: Id of the destination map to set.

### `teletransporter:get_destination_name()`

Returns the name of the destination place on the destination map.

Return value (string or `nil`)
: Location on the destination map.

    Can be the name of a [destination](./destination.md) entity, the special value `"_same"` to keep the hero's coordinates, or the special value `"_side"` to place on hero on the corresponding side of an adjacent map (normally used with the scrolling transition style).

    `nil` means the default destination entity of the map.

### `teletransporter:set_destination_name(destination_name)`

Sets the destination place on the destination map.

`destination_name` (string or `nil`)
: Location on the destination map.

    Can be the name of a [destination](./destination.md) entity, the special value `"_same"` to keep the hero's coordinates, or the special value `"_side"` to place on hero on the corresponding side of an adjacent map (normally used with the scrolling transition style).

    `nil` means the default destination entity of the map.

## Events Inherited from `entity`

Events are callback methods automatically called by the engine if you define them.

Teletransporters are particular [map entities](./index.md). Therefore, they inherit all events from the type map entity.

See [entity](./index.md#events-of-all-entity-types) to know these events.

## Events of the type `teletransporter`

The following events are specific to teletransporters.

### `teletransporter:on_activated(hero)`

Called when the user takes this teletransporter, just before the map closing transition starts.

`hero` ([hero](./hero.md))
: The hero that activated the teletransporter

!!! note "Note"

    If you [disable](./index.md#entityset_enabledenabled) or [remove](./index.md#entityremove) the teletransporter during this event, then the teletransportation does not occur.
