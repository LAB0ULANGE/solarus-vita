# Switch

## Overview

A switch is a button that can be activated to trigger a mechanism.

This type of [map entity](./index.md) can be declared in the [map data file](../map.md#map-files). It can also be created dynamically with [`map:create_switch()`](../map.md#mapcreate_switchproperties).

A switch may be activated by the [hero](./hero.md), by a [block](./block.md) or by a projectile, depending on its subtype. The following subtypes of switches are available:

- Walkable, traversable pressure plate.
- Button to be activated by shooting an [arrow](./arrow.md) on it with the [bow](./hero.md#herostart_bow).
- Solid switch to be activated with the sword or other weapons.

When a switch is activated, the event [`switch:on_activated()`](#switchon_activatedentity) is called. Define that event to implement what happens: [opening a door](../map.md#mapopen_doorsprefix), [showing a chest](./index.md#entityset_enabledenabled), etc.

Some switches get inactivated when the hero (or the entity that activated them) leaves them. In this case, when the switch is inactivated, the event [`switch:on_inactivated()`](#switchon_inactivatedentity) is called.

The size of a switch is always 16×16 pixels. Its [sprite](../drawable-objects/sprite.md) can be defined at creation time, as well as the sound played when it gets activated.

## Methods Inherited from `entity`

Switches are particular [map entities](./index.md). Therefore, they inherit all methods from the type map entity.

See [entity](./index.md#methods-of-all-entity-types) to know these methods.

## Methods of the type switch

The following methods are specific to switches.

### `switch:get_subtype()`

Returns the subtype for this switch.

Return value (string)
: the subtype:

    - `"walkable"`
    - `"arrow_target"`
    - `"solid"`

### `switch:set_subtype(subtype)`

Sets the subtype for this switch.

`subtype` (string)
: the subtype:

    - `"walkable"`
    - `"arrow_target"`
    - `"solid"`

### `switch:is_walkable()`

Returns whether this is a walkable switch.

Return value (boolean)
: `true` if this switch is a walkable one.

### `switch:is_activated()`

Returns whether this switch is activated.

Return value (boolean)
: `true` if this switch is currently activated.

### `switch:set_activated([activated])`

Sets whether this switch is activated or not.

The change is quiet and immediate: no sound is played and no event is triggered.

`activated` (boolean, optional, default: `true`)
: `true` to make the switch activated, `false` to make is inactivated.

### `switch:is_locked()`

Returns whether this switch is current locked.

When a switch is locked, its state cannot change anymore: it can no longer be activated or inactivated by other entities. However, it can still changed programmatically by calling [`switch:set_activated()`](#switchset_activatedactivated).

Return value (boolean)
: `true` if this switch is currently activated.

### `switch:set_locked([locked])`

Locks this switch in its current state or unlocks it.

When a switch is locked, its state cannot change anymore: it can no longer be activated or inactivated by other entities. However, it can still changed programmatically by calling [`switch:set_activated()`](#switchset_activatedactivated).

`locked` (boolean, optional, default: `true`)
: `true` to lock the switch, `false` to unlock it.

!!! note "Note"

    The method [`switch:set_activated()`](#switchset_activatedactivated) works even on a locked switch.

### `switch:get_inactivate_when_leaving()`

Returns whether the switch becomes inactivated when the [hero](./hero.md) or the [block](./block.md) leaves it (only for a walkable switch).

Return value (boolean)
: `true` if the switch becomes inactivated when left, `false` if it stays activated.

### `switch:set_inactivate_when_leaving([inactivate_when_leaving])`

Sets whether the switch becomes inactivated when the [hero](./hero.md) or the [block](./block.md) leaves it (only for a walkable switch).

`inactivate_when_leaving` (boolean, optional, default: `true`)
: `true` to inactivate the switch when it is left, `false` to make it stay activated.

## Events Inherited from `entity`

Events are callback methods automatically called by the engine if you define them.

Switches are particular [map entities](./index.md). Therefore, they inherit all events from the type map entity.

See [entity](./index.md#events-of-all-entity-types) to know these events.

## Events of the type switch

The following events are specific to switches.

### `switch:on_activated([entity])`

Called when this switch has just been turned on.

`entity` ([entity](./index.md) or `nil`)
: The entity that activated the switch, or `nil`.

This is the right place to define the action that you want your switch to perform.

### `switch:on_inactivated([entity])`

Called when a switch has just been turned off.

`entity` ([entity](./index.md) or `nil`)
: The entity that inactivated the switch, or `nil`.

### `switch:on_left([entity])`

Called when an entity placed on a switch (like the [hero](./hero.md) or a [block](./block.md)) has just left the switch, regardless of whether the switch was activated or not.

`entity` ([entity](./index.md) or `nil`)
: The entity that left the switch, or `nil`.
