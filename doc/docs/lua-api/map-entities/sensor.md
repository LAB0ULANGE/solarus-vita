# Sensor

A sensor is an invisible detector that triggers something when the [hero](./hero.md) overlaps it.

This type of [map entity](./index.md) can be declared in the [map data file](../map.md#map-files). It can also be created dynamically with [`map:create_sensor()`](../map.md#mapcreate_sensorproperties).

## Overview

Unlike [switches](./switch.md), [sensors](#sensor) cannot be avoided by a jump. If you want the [hero](./hero.md) to be detected for sure when he enters somewhere, use a sensor rather than an invisible switch.

When a sensor is activated, the event [`sensor:on_activated()`](#sensoron_activatedhero) is called. Define that event to implement what happens: [closing a door](../map.md#mapclose_doorsprefix), [starting a dialog](../game.md#gamestart_dialogdialog_id-info-callback), etc.

Sensors can have any size, but like all entities, their width and height must be multiples of 8 pixels. The minimum size if 16×16 pixels (the size of the hero).

A sensor is activated only if the bounding box of the hero is entirely on the sensor.

## Methods Inherited from `entity`

Sensors are particular [map entities](./index.md). Therefore, they inherit all methods from the type map entity.

See [entity](./index.md#methods-of-all-entity-types) to know these methods.

## Methods of the type `sensor`

None.

## Events Inherited from `entity`

Events are callback methods automatically called by the engine if you define them.

Sensors are particular [map entities](./index.md). Therefore, they inherit all events from the type map entity.

See [entity](./index.md#events-of-all-entity-types) to know these events.

## Events of the type sensor

The following events are specific to sensors.

### `sensor:on_activated(hero)`

Called when the [hero](./hero.md) overlaps this sensor.

`hero` ([hero](./hero.md))
: The hero that activated the sensor.

The bounding box of the hero (of size 16×16 pixels) must fit entirely the sensor. This means that if the sensor has a size of 16×16, the hero and the sensor must overlap perfectly.

This event is the right place to define the action that you want your sensor to perform.

### `sensor:on_activated_repeat()`

Called continuously while the [hero](./hero.md) overlaps this sensor.

### `sensor:on_left()`

Called when the [hero](./hero.md) stops overlapping this sensor.

### `sensor:on_collision_explosion()`

Called when an [explosion](./explosion.md) touches this sensor.
