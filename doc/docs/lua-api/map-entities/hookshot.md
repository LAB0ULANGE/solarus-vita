# Hookshot

## Overview

A hookshot [map entity](./index.md) is used to implement the [hero](./hero.md)'s hookshot state.

!!! note "Historical Feature"

    Hookshot entities exist for historical reasons and are not very customizable. In new projects, we recommend to use [custom entities](./custom-entity.md) instead.

A hookshot entity is created when you call [`hero:start_hookshot()`](./hero.md#herostart_hookshot).

It can immobilize or hurt [enemies](./enemy.md) and transport the [hero](./hero.md) to distant places.

Note that there is no predefined hookshot [equipment item](../equipment-items.md). It is your responsability to make one if you want. If you decide to make one, you will typically call [`hero:start_hookshot()`](./hero.md#herostart_hookshot) from the [`item:on_using()`](../equipment-items.md#itemon_usinghero) callback of your hookshot equipment item.

## Methods Inherited from `entity`

A hookshot is a particular [map entity](./index.md). Therefore, it inherits all methods from the type map entity.

See [entity](./index.md#methods-of-all-entity-types) to know these methods.

## Methods of the type `hookshot`

None.

## Events Inherited from `entity`

Events are callback methods automatically called by the engine if you define them.

A hookshot is a particular [map entity](./index.md). Therefore, it inherits all events from the type map entity.

See [entity](./index.md#events-of-all-entity-types) to know these events.

## Events of the type `hookshot`

None.
