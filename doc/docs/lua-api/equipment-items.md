# Equipment Items

## Overview

An equipment item represents something that the player can obtain (one or more times, in various forms and with several variants) and possibly keep. The Lua item type described in this page provides various functions to:

- Get and the possession state of a equipment item,
- Set its properties, like whether the item is saved and has an amount,
- Control its dynamic behavior, like what happens when the player uses this item (for items that can be used).

A Lua item object represents a kind of treasure, and not a particular instance of treasures. Individual treasures may then be represented as [pickable treasures](./map-entities/pickable.md), [chests](./map-entities/chest.md), [shop treasures](./map-entities/shop-treasure.md), and may be brandished by the [hero](./map-entities/hero.md). For example, multiple treasures of the kind `"rupee"` may exist at the same time during the [game](./game.md), but only one Lua item object manages them.

The script file `items/XXXX.lua` defines the item named `XXXX` The corresponding Lua item object is passed as parameter of that script. Use the Lua notation `...` to get this parameter and store it into a regular variable.

Here is a basic example of script for the `rupee` item, an item whose only role is to increase the money of the player when he obtains a rupee.

```lua
-- First, we put the parameter into a variable called "rupee".
-- (In Lua, the notation "..." refers to the parameter(s) of the script.)
local rupee = ...

-- Event called when the hero obtains a rupee treasure.
function rupee:on_obtaining()
  self:get_game():add_money(1)  -- Increase the money of 1 unit.
end
```

!!! note "Note"

      All item scripts are loaded when you create a [savegame](./game.md) object. Indeed, equipment of a particular savegame. As shown in the example above, you can retrieve that savegame with [`item:get_game()`](#itemget_game).

!!! note "Note"

    A [sprite](../files-specs/sprite-data-file.md) animation named `XXXX` must also exist in the sprite `entities/items`: it is used by the engine whenever it needs to draw your item on the [map](./map.md) (for example, when a [pickable treasure](./map-entities/pickable.md) of this kind is created on the map).

## Methods of the type `item`

### `item:get_name()`

Returns the name of this item.

Return value (string)
: The name that identifies this item. It is also the file name of the item script (without the extension).

### `item:get_game()`

Returns the game where this item belongs.

Return value ([game](./game.md))
: The game that contains this item.

!!! note "Note"

    Items only exist in the context of a game, but the game is not necessarily running.

### `item:get_map()`

Returns the current map.

Return value ([map](./map.md) or `nil`)
: The current map, or `nil` if the [game](./game.md) is not running.

### `item:get_savegame_variable()`

Returns the name of the integer savegame value that stores the possession state of this item.

Return value (string or `nil`)
: The savegame variable that stores the possessed variant of this item, or `nil` if this item is not saved.

### `item:set_savegame_variable(savegame_variable)`

Sets the name of the integer savegame value that stores the possession state of this item.

You should call this function at initialization time if you want your item to be saved.

`savegame_variable` (string or `nil`)
: The savegame variable that should stored the possessed variant of this item, or `nil` to make this item unsaved.

### `item:get_amount_savegame_variable()`

Returns the name of the integer savegame value that stores the amount associated to this item.

Return value (string or `nil`)
: The savegame variable that stores the possessed amount of this item, or `nil` if this item has no associated amount.

### `item:set_amount_savegame_variable(amount_savegame_variable)`

Sets the name of the integer savegame value that stores the amount of this item.

You should call this function at initialization time if you want your item to store an amount (in addition to its possessed variant). This is typically used for items like the bow and the counter of bombs.

`amount_savegame_variable` (string or `nil`)
: The savegame variable that should store the possessed amount of this item, or `nil` to make this item have no associated amount.

### `item:is_obtainable()`

Returns whether the player is allowed to obtain this item.

If not, any treasure representing this item is automatically replaced by an empty treasure.

Return value (boolean)
: `true` if this item is obtainable.

### `item:set_obtainable([obtainable])`

Sets whether the player is allowed to obtain this item.

If not, any treasure representing this item is automatically replaced by an empty treasure. There is no risk that the player can obtain it or even see it during the [game](./game.md). You can use this feature to hide some items while the player has not the necessary equipment. For example, you can make arrows unobtainable until the player has the bow. You can also make magic jars unobtainable until the player has a magic bar.

`obtainable` (boolean, optional, default: `true`)
: `true` to make this item obtainable.

### `item:is_assignable()`

Returns whether this item can be assigned to an item slot.

When the item is assigned to a slot, the player can use it by pressing the [game command](./game.md#game-commands) of that slot. Some items are meant to be used by pressing a command (like the bow), other are not supposed to (like a key or a rupee). When the player uses your item, the event [`item:on_using()`](#itemon_usinghero) is triggered.

Return value (boolean)
: `true` if this item is assignable.

### `item:set_assignable([assignable])`

Sets whether this item should be assignable to an item slot.

When the item is assigned to a slot, the player can use it by pressing the [game command](./game.md#game-commands) of this slot. Some items are meant to be used by pressing a command (like the bow), other are not supposed to (like a key or a rupee). When the player uses your item, the event [`item:on_using()`](#itemon_usinghero) is triggered.

By default, an item is not assignable. Call this function at initialization time if you want your item to be assignable.

`assignable` (boolean, optional, default: `true`)
: `true` to make this item assignable.

### `item:get_can_disappear()`

Returns whether [pickable treasures](./map-entities/pickable.md) of this kind disappears after a few seconds when they are dropped by an [enemy](./map-entities/enemy.md) or a [destructible entity](./map-entities/destructible.md).

Return value (boolean)
: `true` if pickable treasures of this kind can disappear.

### `item:set_can_disappear([can_disappear])`

Sets whether [pickable treasures](./map-entities/pickable.md) of this kind should disappear after a few seconds when they are dropped by an [enemy](./map-entities/enemy.md) or a [destructible entity](./map-entities/destructible.md).

By default, an item cannot disappear. Call this function at initialization time if you want your item to be ephemeral.

`can_disappear` (boolean, optional, default: `true`)
: `true` to make such pickable treasures disappear after a few seconds.

!!! note "Note"

    This property only applies to [pickable treasures](./map-entities/pickable.md) dropped dynamically (by [enemies](./map-entities/enemy.md) and [destructible entities](./map-entities/destructible.md)).

    Pickable treasures already present on the [map](./map.md) when the map starts don't disappear with time.

### `item:get_brandish_when_picked()`

Returns whether the hero brandishes treasures of this kind when he [picks](./map-entities/pickable.md) them on the ground.

Return value (boolean)
: `true` if the hero brandish such treasures.

### `item:set_brandish_when_picked([brandish_when_picked])`

Sets whether the hero should brandish treasures of this kind when he [picks](./map-entities/pickable.md) them on the ground.

Treasures coming from a [chest](./map-entities/chest.md) are always brandished, even the most basic ones like simple rupees. However, when treasures are [picked](./map-entities/pickable.md) on the ground (like rupees dropped by an [enemy](./map-entities/enemy.md)), you may want the hero not to brandish them.

Call this function if you don't want your item to be brandished when it is picked on the ground.

`brandish_when_picked` (boolean, optional, default: `true`)
: `true` if the hero should brandish such treasures.

### `item:get_shadow()`

Returns the name of the animation representing the shadow of this item in the sprite `"entities/shadow"`

Return value (string or `nil`)
: Name of the shadow animation adapted to this item in the sprite `"entities/shadow"`. `nil` means no shadow displayed.

### `item:set_shadow(shadow_animation)`

Sets the name of the animation that should represent the shadow of this item in the sprite `"entities/shadow"`

When the engine needs to show a treasure representing your item, it sometimes also wants to display a shadow (in addition of the treasure's main sprite). For example, [pickable treasures](./map-entities/pickable.md) dropped by enemies normally have a shadow.

You should call this function at initialization time if your item sprite is larger or smaller than usual.

`shadow_animation` (string or `nil`, default: `"big"`)
: Name of the shadow animation in the sprite `"entities/shadow"` to set for this item. `nil` means that no shadow will be displayed.

!!! note "Note"

    To draw a treasure, the engine relies on two sprites: the treasure's main sprite (`entities/item`) and the shadow's sprite (`entities/shadow`). Both sprites and their appropriate animations must exist so that treasures can be displayed correctly.

### `item:get_sound_when_picked()`

Returns the sound played when the hero [picks a treasure](./map-entities/pickable.md) of this kind.

Return value (string or `nil`)
: Name of the sound played when the hero picks a treasure of this kind (`nil` means no sound).

### `item:set_sound_when_picked(sound_when_picked)`

Sets the sound to play when the hero [picks a treasure](./map-entities/pickable.md) of this kind.

`sound_when_picked` (string or `nil`, default: `"picked_item"`)
: Name of the sound to play (as in [`sol.audio.play_sound()`](./audio/index.md#solaudioplay_soundsound_id)) when the hero picks a treasure of this kind (`nil` means no sound).

!!! note "Note"

    This sound is always played, even if the treasure is also brandished then (i.e. if [`item:get_brandish_when_picked()`](#itemget_brandish_when_picked) returns `true`)

### `item:get_sound_when_brandished()`

Returns the sound played when the hero brandishes a treasure of this kind.

Return value (string or `nil`)
: Name of the sound played when the hero brandishes a treasure of this kind (`nil` means no sound).

### `item:set_sound_when_brandished(sound_when_brandished)`

Sets the sound to play when the hero brandishes a treasure of this kind.

The hero can brandish treasures in various situations: when opening a [chest](./map-entities/chest.md), when buying a [shop treasure](./map-entities/shop-treasure.md), when picking up a [pickable treasure](./map-entities/pickable.md) (unless you called [`item:set_brandish_when_picked(false)`](#itemset_brandish_when_pickedbrandish_when_picked)), and also when you call [`hero:start_treasure()`](./map-entities/hero.md#herostart_treasuretreasure_name-treasure_variant-treasure_savegame_variable-callback) directly.

`sound_when_brandished` (string or `nil`, default: `"treasure"`)
: Name of the sound to play (as in [`sol.audio.play_sound()`](./audio/index.md#solaudioplay_soundsound_id)) when the hero brandishes a treasure of this kind (`nil` means no sound).

### `item:has_variant([variant])`

Returns whether the player owns at least the specified variant of this item (only for a saved item).

`variant` (number, optional, default: `1`)
: The variant to check.

Return value (boolean)
: `true` if the player has at least this variant.

!!! note "Note"

    Equivalent to `item:get_variant() >= variant`.

### `item:get_variant()`

Returns the possession state of this item (only for a saved item).

Return value (number)
: The possession state (`0`: not possessed, `1`: first variant, `2`: second variant, etc.).

!!! note "Note"

    Equivalent to `item:get_game():get_value(item:get_savegame_variable())`.

### `item:set_variant(variant)`

Sets the possession state of this item (only for a saved item).

`variant` (number)
: The new possession state to set (`0`: not possessed, `1`: first variant, `2`: second variant, etc.)

!!! note "Note"

    Equivalent to `item:get_game():set_value(item:get_savegame_variable(), variant)`.

### `item:has_amount([amount])`

Returns whether this item has an associated amount, or whether the player has at least the specified value.

`amount` (number, optional)
: The amount to check. If this parameter is not set, this function only tests whether an amount value exists for the item.

Return value (boolean)
: If an amount is specified, return `true` if the player has at least that amount. Otherwise, returns `true` if the item has an amount value.

!!! note "Note"

    If an amount is specified, this method is equivalent to `item:get_amount() >= amount`. Otherwise, this method is equivalent to `item:get_amount_savegame_variable() ~= nil`.

### `item:get_amount()`

Returns the amount associated to this item (only for an item with an amount value).

Return value (number)
: The associated amount.

!!! note "Note"

    Equivalent to `item:get_game():get_value(item:get_amount_savegame_variable())`.

### `item:set_amount(amount)`

Sets the amount associated to this item (only for an item with an amount value). A negative amount will be replaced by `0` An amount greater than [`item:get_max_amount()`](#itemget_max_amount) will be replaced by that maximum value.

`amount` (number)
: The amount to set.

### `item:add_amount(amount)`

Increases the amount associated to this item (only for an item with an amount value), without exceeding the maximum amount.

`amount` (number)
: The amount to add.

### `item:remove_amount(amount)`

Decreases the amount associated to this item (only for an item with an amount value), without going below `0`.

`amount` (number)
: The amount to remove.

### `item:get_max_amount()`

Returns the maximum amount associated to this item (only for an item with an amount value).

Return value (number)
: The maximum amount.

### `item:set_max_amount(max_amount)`

Sets the maximum amount associated to this item (only for an item with an amount value). This maximum value is used in [`item:set_amount()`](#itemset_amountamount) and [`item:add_amount()`](#itemadd_amountamount) to make a limit.

`max_amount` (number, default: `1000`)
: The maximum amount to set.

!!! note "Note"

    The maximum amount of an item is not saved automatically. Only the current variant (see [`item:set_savegame_variable()`](#itemset_savegame_variablesavegame_variable)) and the current amount (see [`item:set_amount_savegame_variable()`](#itemset_amount_savegame_variableamount_savegame_variable)) are saved by the engine.

    Therefore, you have to set the maximum amount of appropriate items when they are loaded (typically from event [`item:on_started()`](#itemon_started)).

### `item:is_being_used()`

Returns whether the item is currently being used by the [hero](./map-entities/hero.md).

Return value (boolean)
: `true` if the item is being used, `false` otherwise.

### `item:set_finished()`

Notifies the engine that using this item is finished and that the hero can get back to a normal state.

When the player uses this item (by pressing an item [game command](./game.md#game-commands)), your item script takes full control of the [hero](./map-entities/hero.md) (event [`item:on_using()`](#itemon_usinghero) is called) and you have to program the item's behavior. When it is finished, call this function to restore normal control to the player.

This method should only be called when the hero is using this item.

## Events of the type `item`

Events are callback methods automatically called by the engine if you define them. In the case of a game, they are only called on the game currently running, if any.

### `item:on_created()`

Called when your item script is being created, that is, when the [game](./game.md) object is being created.

In this event, you should initialize your item.

!!! note "Note"

    All items scripts are not loaded yet at this time. Your item is being loaded, but you don't know the status of the other ones. Unless your item is the last one to be initialized, other items are not ready.

    If your item needs to get some properties from other items in its initialization, use [`item:on_started()`](#itemon_started) instead.

### `item:on_started()`

Called when your item starts, that is, once all items are created.

At this time, all items scripts are loaded and their [`on_created()`](#itemon_created) events have been called.

You can use this event to make initializations that depend on other items. Such initializations could not be done earlier from [`item:on_created()`](#itemon_created), because all items did not exist at that time.

### `item:on_finished()`

Called when the [game](./game.md) stops running.

### `item:on_update()`

Called at each cycle of the main loop while the [game](./game.md) is running.

!!! note "Note"

    As this function is called at each cycle, it is recommended to use other solutions when possible, like [timers](./timers.md) and other events.

### `item:on_suspended(suspended)`

Called when the [map](./map.md) is being suspended or resumed. Only possible when the [game](./game.md) is running.

The map is suspended by the engine in a few cases, like when the game is paused or when the camera is being moved by a script. When this happens, all [map entities](./map-entities/index.md) stop moving and most [sprites](./drawable-objects/sprite.md) stop their animation. Your item may need to be notified.

`suspended` (boolean)
: `true` if the map is being suspended, `false` if it is being resumed.

### `item:on_map_changed(map)`

Called when the player has just entered a [map](./map.md). Only possible when the [game](./game.md) is running.

The new map is already started at this point.

`map` ([map](./map.md))
: The new active map.

!!! note "Note"

    This event is also called for the first map (when your [game](./game.md) starts).

### `item:on_pickable_created(pickable)`

Called when a [pickable treasure](./map-entities/pickable.md) of your item's kind is created on the [map](./map.md). Only possible when the [game](./game.md) is running.

If you need to do something special when a pickable treasure of this kind appears on the [map](./map.md), like setting a particular [movement](./movements/index.md), you can use this event.

`pickable` ([pickable treasure](./map-entities/pickable.md))
: The pickable treasure just created.

### `item:on_obtaining(variant, savegame_variable)`

Called when the hero is obtaining a treasure of this kind of item. Only possible when the [game](./game.md) is running.

`variant` (number)
: The variant of item (because some items may have several variants). `1` is the first variant, `2` is the second variant, etc.

`savegame_variable` (string or `nil`)
: Name of the boolean saved value that stores the state of the treasure being obtained. `nil` means that the treasure is not saved.

!!! note "Note"

    If the treasure is brandished, this function is called immediately (before the dialog starts). If you want to do something after the treasure's dialog, see [`item:on_obtained()`](#itemon_obtainedvariant-savegame_variable).

### `item:on_obtained(variant, savegame_variable).`

Like [`item:on_obtaining()`](#itemon_obtainingvariant-savegame_variable), but called right after the treasure is obtained. Only possible when the [game](./game.md) is running.

In the case of a brandished treasure, this event is called once the treasure's dialog is finished. Otherwise, it is called immediately after [`item:on_obtaining()`](#itemon_obtainingvariant-savegame_variable).

`variant` (number)
: The variant of item (because some items may have several variants). `1` is the first variant, `2` is the second variant, etc.

`savegame_variable` (string or `nil`)
: Name of the boolean saved value that stores the state of the treasure just obtained. `nil` means that the treasure is not saved.

### `item:on_variant_changed(variant)`

Called when the possessed variant of this item has just changed. Only for saved items.

This event is triggered whenever the variant changes: it can be when the hero obtains a treasure of this item's kind, or when you call [`item:set_variant()`](#itemset_variantvariant) explicitly.

`variant` (number)
: The new variant possessed (possibly `0`)

### `item:on_amount_changed(amount)`

Called when the possessed amount of this item has just changed. Only for items with an associated amount.

This event is triggered when you call [`item:set_amount()`](#itemset_amountamount), [`item:add_amount()`](#itemadd_amountamount), [`item:remove_amount()`](#itemremove_amountamount),

`amount` (number)
: The new amount possessed (possibly `0`)

### `item:on_using(hero)`

Called when a player is using this item. Only possible when the [game](./game.md) is running, and only for [assignable items](#itemset_assignableassignable).

The player is using your item (by pressing an item [game command](./game.md#game-commands)). You now have full control of the [hero](./map-entities/hero.md). From this event, you have to program the item's behavior. For example, your item can remove some magic points and perform a special attack that kills all enemies nearby. When you have finished, call [`item:set_finished()`](#itemset_finished) to restore normal control to the player.

`hero` ([hero](./map-entities/hero.md))
: The hero entity that uses the item.

!!! note "Note"

    There is another event for the special case of giving an item to an [non-playing character](./map-entities/npc.md). If the [hero](./map-entities/hero.md) uses an item in front of an [NPC](./map-entities/npc.md) whose property is to notify your item, then event [`item:on_npc_interaction_item()`](#itemon_npc_interaction_itemnpc-item_used) is triggered first. If that event is defined and returns `true`, then [`item:on_using()`](#itemon_usinghero) is not called (the interaction is considered done).

### `item:on_ability_used(ability_name, hero)`

Called when a hero has just performed a built-in ability. Only possible when the [game](./game.md) is running.

Built-in abilities indicate whether the hero can perform some built-in actions like attacking, swimming or running. See [`game:get_ability()`](./game.md#gameget_abilityability_name) for more details.

`ability_name` (string)
: Name of the ability that was used.

`hero` ([hero](./map-entities/hero.md))
: The hero entity that just used the ability.

### `item:on_npc_interaction(npc)`

Called when the [hero](./map-entities/hero.md) interacts (the player pressed the [action command](./game.md#game-commands)) in front of an [NPC](./map-entities/npc.md) whose property is to notify your item. Only possible when the [game](./game.md) is running.

`npc` ([npc](./map-entities/npc.md))
: A non-playing character.

### `item:on_npc_interaction_item(npc, item_used)`

Called when the [hero](./map-entities/hero.md) uses any item (the player pressed an [item command](./game.md#game-commands)) with an [NPC](./map-entities/npc.md) whose property is to notify your item. Only possible when the [game](./game.md) is running.

`npc` ([npc](./map-entities/npc.md))
: A non-playing character.

`item_used` (item)
: The item currently used by the player. This is not necessarily your item. The reason why your item gets notified is because the NPC is related to your item. But the player may be using another item.

Return value (boolean)
: `true` if an interaction happened. If you return `false` or nothing, then [`item_used:on_using()`](#itemon_usinghero) will be called (just like if there was no NPC in front of the hero).

### `item:on_npc_collision_fire(npc)`

Called when an [NPC](./map-entities/npc.md) whose property is to notify your item touches [fire](./map-entities/fire.md). Only possible when the [game](./game.md) is running.

`npc` ([npc](./map-entities/npc.md))
: A non-playing character that touches fire and has the property to notify your item.
