# Bomb

## Overview

A bomb is an entity that explodes after a few seconds.

You can create this type of [map entity](./index.md) only dynamically with [`map:create_bomb()`](../map.md#mapcreate_bombproperties). It cannot be declared in the [map data file](../map.md#map-files).

!!! note "Historical Feature"

    Bomb entities exist for historical reasons and are not very customizable. In new projects, we recommend to use [custom entities](./custom-entity.md) instead.

A bomb can be lifted by the [hero](./hero.md) before it explodes. When it explodes, it is removed from the [map](../map.md), a new entity of type [explosion](./explosion.md) is created at the same position and the sound `"explosion"` is played.

If a bomb is placed on a [stream](./stream.md), it follows a movement corresponding to this stream.

## Methods Inherited from `entity`

Bombs are particular [map entities](./index.md). Therefore, they inherit all methods from the type map entity.

See [entity](./index.md#methods-of-all-entity-types) to know these methods.

## Methods of the type `bomb`

None.

## Events Inherited from `entity`

Events are callback methods automatically called by the engine if you define them.

Bombs are particular [map entities](./index.md). Therefore, they inherit all events from the type map entity.

See [entity](./index.md#events-of-all-entity-types) to know these events.

## Events of the type `bomb`

None.
