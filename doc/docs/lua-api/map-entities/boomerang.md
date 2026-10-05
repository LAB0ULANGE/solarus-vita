# Boomerang

## Overview

A boomerang [map entity](./index.md) is used to implement the [hero](./hero.md)'s boomerang state.

!!! note "Historical Feature"

    Boomerang entities exist for historical reasons and are not very customizable. In new projects, we recommend to use [custom entities](./custom-entity.md) instead.

A boomerang entity is created when you call [`hero:start_boomerang()`](./hero.md#herostart_boomerangmax_distance-speed-tunic_preparing_animation-sprite_name).

It can immobilize or hurt [enemies](./enemy.md) and activate mechanisms.

Note that there is no predefined boomerang [equipment item](../equipment-items.md). It is your responsability to make one if you want. If you decide to make one, you will typically call [`hero:start_boomerang()`](./hero.md#herostart_boomerangmax_distance-speed-tunic_preparing_animation-sprite_name) from the [`item:on_using()`](../equipment-items.md) callback of your boomerang equipment item.

## Methods Inherited from `entity`

A boomerang is a particular [map entity](./index.md). Therefore, it inherits all methods from the type map entity.

See [entity](./index.md#methods-of-all-entity-types) to know these methods.

## Methods of the type `boomerang`

None.

## Events Inherited from `entity`

Events are callback methods automatically called by the engine if you define them.

A boomerang is a particular [map entity](./index.md). Therefore, it inherits all events from the type map entity.

See [entity](./index.md#events-of-all-entity-types) to know these events.

## Events of the type `boomerang`

None.
