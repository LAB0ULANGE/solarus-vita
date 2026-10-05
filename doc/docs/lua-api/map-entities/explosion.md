# Explosion

## Overview

This is an explosion whose sprite hurts the [hero](./hero.md) and [enemies](./enemy.md).

You can create this type of [map entity](./index.md) only dynamically with [`map:create_explosion()`](../map.md#mapcreate_explosionproperties). It cannot be declared in the [map data file](../map.md#map-files).

!!! note "Historical Feature"

    Explosion entities exist for historical reasons and are not very customizable. In new projects, we recommend to use [custom entities](./custom-entity.md) instead.

Explosions are automatically created by [bombs](./bomb.md) that explode, and by dying [enemies](./enemy.md) whose [hurting style](./enemy.md#enemyget_hurt_style) is `"boss"`.

Explosions hurt the [hero](./hero.md) and [enemies](./enemy.md) that are sensible to them. But they may also blast [doors](./door.md) and can be detected by [sensors](./sensor.md).

## Methods Inherited from `entity`

Explosions are particular [map entities](./index.md). Therefore, they inherit all methods from the type map entity.

See [entity](./index.md#methods-of-all-entity-types) to know these methods.

## Methods of the type explosion

None.

## Events Inherited from `entity`

Events are callback methods automatically called by the engine if you define them.

Explosions are particular [map entities](./index.md). Therefore, they inherit all events from the type map entity.

See [entity](./index.md#events-of-all-entity-types) to know these events.

## Events of the type explosion

None.
