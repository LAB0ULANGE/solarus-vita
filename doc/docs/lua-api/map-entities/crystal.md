# Crystal

## Overview

A crystal is a switch that lowers or raises alternatively some special colored blocks in the ground called [crystal blocks](./crystal-block.md).

This type of [map entity](./index.md) can be declared in the [map data file](../map.md#map-files). It can also be created dynamically with [`map:create_crystal()`](../map.md#mapcreate_crystalproperties).

!!! note "Note"

    Crystals provide very specific behavior for historical reasons and are not very customizable. If you need more flexibility, we recommend to use [custom entities](./custom-entity.md) instead and to script the behavior you want.

A crystal is essentially a solid switch that [inverts the configuration](../map.md#mapchange_crystal_state) of [crystal blocks](./crystal-block.md) when activated.

### Crystal sprites

Two sprites for a crystal are automatically created by the engine. You can access them like for any other entity, specifying their name in [`entity:get_sprite([name])`](./index.md#entityget_spritename).

- `"main"`: Main sprite representing the crystal. Its animation set is `"entities/crystal"` This is the default one in [`entity:get_sprite([name])`](./index.md#entityget_spritename).
- `"star"`: Star twinkling over the crystal. Its animation set is `"entities/star"`.

## Methods Inherited from `entity`

Crystals are particular [map entities](./index.md). Therefore, they inherit all methods from the type map entity.

See [entity](./index.md#methods-of-all-entity-types) to know these methods.

## Methods of the type `crystal`

None.

## Events Inherited from `entity`

Events are callback methods automatically called by the engine if you define them.

Crystals are particular [map entities](./index.md). Therefore, they inherit all events from the type map entity.

See [entity](./index.md#events-of-all-entity-types) to know these events.

## Events of the type `crystal`

None.
