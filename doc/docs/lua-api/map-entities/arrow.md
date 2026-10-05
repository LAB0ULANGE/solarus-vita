# Arrow

## Overview

When the [hero](./hero.md) uses a [bow](./hero.md#herostart_bow), an arrow is created.

This type of entity can only be created by the engine.

An arrow entity is created when you call [`hero:start_bow()`](./hero.md#herostart_bow), after the bow animation. The arrow flies for a few seconds and disappears later.

It can hurt [enemies](./enemy.md) and activate [switches](./switch.md) that are sensible to arrows.

Note that there are no predefined bow and arrows [equipment items](../equipment-items.md). It is your responsability to make them if you want. If you decide to make them, you will typically call [`hero:start_bow()`](./hero.md#herostart_bow) from the [`item:on_using()`](../equipment-items.md#itemon_usinghero) callback of your bow equipment item.

## Methods Inherited from `entity`

Arrows are particular [map entities](./index.md). Therefore, they inherit all methods from the type map entity.

See [entity](./index.md#methods-of-all-entity-types) to know these methods.

## Methods of the type `arrow`

None.

## Events Inherited from `entity`

Events are callback methods automatically called by the engine if you define them.

Arrows are particular [map entities](./index.md). Therefore, they inherit all events from the type map entity.

See [entity](./index.md#events-of-all-entity-types) to know these events.

## Events of the type `arrow`

None.
