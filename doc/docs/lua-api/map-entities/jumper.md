# Jumper

## Overview

A jumper is an invisible detector that makes the [hero](./hero.md) jump into one of the 8 main directions when touching it.

This type of [map entity](./index.md) can be declared in the [map data file](../map.md#map-files). It can also be created dynamically with [`map:create_jumper()`](../map.md#mapcreate_jumperproperties).

The [hero](./hero.md) makes a jump when touching the jumper. Properties of the jumper include the distance and the direction of the jump.

During the jump, the hero cannot be controlled by the player, and he can traverse obstacles. You need to make sure the destination of the jump is a valid place for when the control is restored to the player.

Think of a jumper like an horizontal, vertical or diagonal line (depending on the direction of the jump: one of the 8 main directions). This line has actually a thickness of 8 pixels so that it can be handled more easily in the editor, but this thickness does not really matter: the jump starts as soon as the hero touches the jumper.

## Methods Inherited from `entity`

Jumpers are particular [map entities](./index.md). Therefore, they inherit all methods from the type map entity.

See [entity](./index.md#methods-of-all-entity-types) to know these methods.

## Methods of the type `jumper`

### `jumper:get_jump_length()`

Returns the length of the jump.

Return value (number)
: the length in pixels.

### `jumper:set_jump_length(length)`

Sets the length of the jump.

`jump` (number)
: the length in pixels.

## Events Inherited from `entity`

Events are callback methods automatically called by the engine if you define them.

Jumpers are particular [map entities](./index.md). Therefore, they inherit all events from the type map entity.

See [entity](./index.md#events-of-all-entity-types) to know these events.

## Events of the type `jumper`

None.
