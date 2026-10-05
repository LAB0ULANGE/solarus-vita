# Non-Playing Character

## Overview

A _non-playing character_ (NPC) is somebody or something that the [hero](./hero.md) can interact with by pressing the [action command](../game.md#game-commands) or by using an [equipment item](../equipment-items.md) just in front of it.

This type of [map entity](./index.md) can be declared in the [map data file](../map.md#map-files). It can also be created dynamically with [`map:create_npc()`](../map.md#mapcreate_npcproperties).

An NPC is not necessarily a person: it can also be a more general, solid interactive entity. In this case, it is called a generalized NPC.

Interactions with an NPC may start a dialog or be forwarded to Lua. More precisely, what happens can be one of the following.

- A dialog starts.
- A Lua event is called on the NPC itself, for instance [`npc:on_interaction()`](#npcon_interaction) or [`npc:on_interaction_item()`](#npcon_interaction_itemitem_used).
- A Lua event is called on an [equipment item](../equipment-items.md), for instance [`item:on_npc_interaction()`](../equipment-items.md#itemon_npc_interactionnpc) or [`item:on_npc_interaction_item()`](../equipment-items.md#itemon_npc_interaction_itemnpc-item_used).

An NPC can move if you apply a movement to it by calling [`movement:start(npc)`](../movements/index.md#movementstartobject_to_move-callback).

The size of an NPC is always 16×16 pixels (like the [hero](./hero.md)) and by default, the NPC is an obstacle for most [map entities](./index.md), including the hero.

Usual NPCs (i.e. non-generalized ones) are suited to usual interactions with people. They must have a sprite with at least four directions, and with animations named `"stopped"` and (if you move them) `"walking"` These predefined animations are automatically started by the engine when you make the NPC move. When the hero talks to them, their sprite automatically stops its animation and looks into his direction.

**Example of use of a usual NPC:** a non-playing character walking randomly in a town.

Generalized NPCs are more customizable. They are solid entities that the hero can interact with, and there is essentially nothing more predefined by the engine. They may even have no sprite. You can choose to allow interactions with the hero from any of the four directions or from a precise direction only.

**Example of use of a generalized NPC:** a stone with something to read on it.

## Methods Inherited from `entity`

Non-playing characters are particular [map entities](./index.md). Therefore, they inherit all methods from the type map entity.

See [entity](./index.md#methods-of-all-entity-types) to know these methods.

## Methods of the type `npc`

The following methods are specific to non-playing characters.

### `npc:get_subtype()`

Returns the subtype for this NPC.

Return value (number)
: the subtype number, 0 for generalized, 1 for usual.

### `npc:set_subtype([subtype])`

Sets the subtype for this NPC.

`subtype` (number, optional, default: `1`)
: the subtype number, 0 for generalized, 1 for usual.

### `npc:is_traversable()`

Returns whether this NPC can be traversed by other entities.

By default, NPCs are not traversable. However, be aware that some entities can override this setting. Indeed, other NPCs, [enemies](./enemy.md) and projectiles ([thrown objects](./carried-object.md), [arrows](./arrow.md), [boomerang](./boomerang.md)) can traverse usual NPCs but cannot traverse generalized NPCs. And [custom entities](./custom-entity.md) can have finer customization.

Return value (boolean)
: `true` if this NPC is traversable.

### `npc:set_traversable([traversable])`

Sets whether this NPC can be traversed by other entities.

By default, NPCs are not traversable. However, be aware that some entities can override this setting. Indeed, other NPCs, [enemies](./enemy.md) and projectiles ([thrown objects](./carried-object.md), [arrows](./arrow.md), [boomerang](./boomerang.md)) can traverse usual NPCs but cannot traverse generalized NPCs. And [custom entities](./custom-entity.md) can have finer customization.

If you want to allow the [hero](./hero.md) to be able to traverse this NPC, you can use this function.

`traversable` (boolean, optional, default: `true`)
: `true` to make this NPC traversable.

## Events Inherited from `entity`

Events are callback methods automatically called by the engine if you define them.

Non-playing characters are particular [map entities](./index.md). Therefore, they inherit all events from the type map entity.

See [entity](./index.md#events-of-all-entity-types) to know these events.

## Events of the type non-playing character

The following events are specific to non-playing characters.

### `npc:on_interaction()`

Called when the [hero](./hero.md) interacts (the player pressed the [action command](../game.md#game-commands)) in front of this NPC, if the NPC has the property to notify its own Lua script.

`hero` ([hero](./hero.md))
: The hero who is triggering the interaction.

### `npc:on_interaction_item(item_used)`

Called when the [hero](./hero.md) uses any [equipment item](../equipment-items.md) (the player pressed an [item command](../game.md#game-commands)) with this NPC, if the NPC has the property to notify its own Lua script.

`item_used` ([item](../equipment-items.md))
: The item currently used by the player.

Return value (boolean)
: `true` if an interaction happened. If you return `false` or nothing, then [`item_used:on_using()`](../equipment-items.md#itemon_usinghero) will be called (just like if there was no NPC in front of the hero).

### `npc:on_collision_fire()`

Called when [fire](./fire.md) touches this NPC, if the NPC has the property to notify its own Lua script.
