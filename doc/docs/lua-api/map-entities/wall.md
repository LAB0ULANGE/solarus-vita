# Wall

A wall is an invisible obstacle that stops some specific types of [map entities](./index.md).

This type of [map entity](./index.md) can be declared in the [map data file](../map.md#map-files). It can also be created dynamically with [`map:create_wall()`](../map.md#mapcreate_wallproperties).

## Overview

If you want to prevent a kind of entity to leave a delimited area while allowing others to pass, you can use walls.

For example, at the entrance of a village, you can make a wall that blocks enemies [enemies](./enemy.md) and lets the [hero](./hero.md) pass.

Walls can have any size, but like all entities, their width and height must be multiples of 8 pixels.

## Methods Inherited from `entity`

Walls are particular [map entities](./index.md). Therefore, they inherit all methods from the type map entity.

See [entity](./index.md#methods-of-all-entity-types) to know these methods.

## Methods of the type `wall`

None.

## Events Inherited from `entity`

Events are callback methods automatically called by the engine if you define them.

Walls are particular [map entities](./index.md). Therefore, they inherit all events from the type map entity.

See [entity](./index.md#events-of-all-entity-types) to know these events.

## Events of the type `wall`

None.
