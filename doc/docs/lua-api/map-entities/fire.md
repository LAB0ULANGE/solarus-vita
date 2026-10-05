# Fire

## Overview

This is some fire whose sprite hurts [enemies](./enemy.md).

You can create this type of [map entity](./index.md) only dynamically with [`map:create_fire()`](../map.md#mapcreate_fireproperties). It cannot be declared in the [map data file](../map.md#map-files).

!!! note "Historical Feature"

    Fire entities exist for historical reasons and are not very customizable. In new projects, we recommend to use [custom entities](./custom-entity.md) instead.

Fire is typically created with an [equipment item](../equipment-items.md) like a lamp.

Fire hurts [enemies](./enemy.md) that are sensible to it. It does not hurt the [hero](./hero.md).

Fire can also be detected by [non-playing characters](./npc.md): this is useful to implement a torch as a generalized NPC that interacts with fire.

## Methods Inherited from `entity`

Fire entities are particular [map entities](./index.md). Therefore, they inherit all methods from the type map entity.

See [entity](./index.md#methods-of-all-entity-types) to know these methods.

## Methods of the type fire

None.

## Events Inherited from `entity`

Events are callback methods automatically called by the engine if you define them.

Fire entities are particular [map entities](./index.md). Therefore, they inherit all events from the type map entity.

See [entity](./index.md#events-of-all-entity-types) to know these events.

## Events of the type fire

None.
