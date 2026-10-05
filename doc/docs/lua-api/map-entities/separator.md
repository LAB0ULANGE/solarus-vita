# Separator

## Overview

Separators allow to visually separate different regions of a map like if there was several maps.

This type of [map entity](./index.md) can be declared in the [map data file](../map.md#map-files). It can also be created dynamically with [`map:create_separator()`](../map.md#mapcreate_separatorproperties).

A separator must be a horizontal bar with a height of 16 pixels, or a vertical bar with a width of 16 pixels. The separation is made in the middle of this bar: in other words, 8 pixels belong to each side.

When the [camera](./camera.md) touches the separation, it stops like if there was a limit of a map. If the [hero](./hero.md) touches the separation, he automatically scrolls to the other side.

It is your responsibility to make sure that your separators don't conflict. For example, if you make a separator too close to a limit of the map, or if you make two vertical separators too close to each other, it is impossible for the engine to respect both constraints. In such cases, the result will look weird. The good news is that you know the maximum size of the visible quest screen: it is a property of your quest, specified in ([quest.dat](../../files-specs/properties-file.md)). If the space between vertical separators is greater than the maximum quest width, you will be safe. The rule is the same for horizontal separators and the maximum quest height.

Enemies and other entities cannot cross separators. But you should take care of [enemies](./enemy.md) whose behavior is to attack the hero when they are close to him or to shoot projectiles at him: if the enemy is close to the hero but on the other side of a separator, you probably don't want to perform the attack. Use [`entity:is_in_same_region()`](./index.md#entityis_in_same_regionx-y-entityis_in_same_regionother_entity) to determine whether the enemy is in the same region than the hero.

To get all entities in a given region, see [`map:get_entities_in_region()`](../map.md#mapget_entities_in_regionx-y-mapget_entities_in_regionentity).

## Methods Inherited from `entity`

Separators are particular [map entities](./index.md). Therefore, they inherit all methods from the type map entity.

See [entity](./index.md#methods-of-all-entity-types) to know these methods.

## Methods of the type separator

None.

## Events Inherited from `entity`

Events are callback methods automatically called by the engine if you define them.

Separators are particular [map entities](./index.md). Therefore, they inherit all events from the type map entity.

See [entity](./index.md#events-of-all-entity-types) to know these events.

## Events of the type separator

The following events are specific to separators.

### `separator:on_activating(direction4)`

Called when the camera starts traversing on this separator.

`direction4` (number)
: Direction of the hero when traversing the separator, from `0` (East) to `3` (South).

### `separator:on_activated(direction4)`

Called when the camera has finished traversing this separator.

The hero is now on the other side.

`direction4` (number)
: Direction of the hero when traversing the separator, from `0` (East) to `3` (South).
