# Crystal Block

## Overview

A crystal block is a colored low wall that may be raised or lowered in the ground.

This type of [map entity](./index.md) can be declared in the [map data file](../map.md#map-files). It can also be created dynamically with [`map:create_crystal_block()`](../map.md#mapcreate_crystal_blockproperties).

!!! note "Note"

    Crystal blocks provide very specific behavior for historical reasons and are not very customizable. If you need more flexibility, we recommend to use [custom entities](./custom-entity.md) instead and to script the behavior you want.

An entity of type crystal block is a pattern of size 16×16 pixels that may be repeated horizontally and vertically like a [tile](./tile.md).

When they are lowered in the ground, crystal blocks are traversable. When they are raised, they become obstacles. If the [hero](./hero.md) overlaps them while they get raised, then he can walk on them.

Crystal blocks exists in two colors. One of them is initially lowered and the other is initially raised. The state of crystal blocks is swapped when the [hero](./hero.md) activates a [crystal](./crystal.md) or when you call [`map:change_crystal_state()`](../map.md#mapchange_crystal_state).

This state persists accross maps of the same [world](../map.md#mapget_world). It is reset when the world changes and when the [savegame](../game.md) is reloaded.

## Methods Inherited from `entity`

Crystals are particular [map entities](./index.md). Therefore, they inherit all methods from the type map entity.

See [entity](./index.md#methods-of-all-entity-types) to know these methods.

## Methods of the type `crystal`

None.

## Events Inherited from `entity`

Events are callback methods automatically called by the engine if you define them.

Crystals are particular [map entities](./index.md). Therefore, they inherit all events from the type map entity.

See [entity](./index.md#events-of-all-entity-types) to know these events.

## Events of the type crystal

None.
