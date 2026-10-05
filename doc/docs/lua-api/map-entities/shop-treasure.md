# Shop Treasure

## Overview

A shop treasure is a treasure that can be purchased by the [hero](./hero.md) for money.

This type of [map entity](./index.md) can be declared in the [map data file](../map.md#map-files). It can also be created dynamically with [`map:create_shop_treasure()`](../map.md#mapcreate_shop_treasureproperties).

A shop treasure entity shows the [sprite](../drawable-objects/sprite.md) of the treasure and its price. Its size is always 32×32 pixels and other entities cannot traverse it.

The [hero](./hero.md) can interact with this a shop treasure to buy it. A dialog is then automatically shown and the player can choose to buy the item or not. If he buys the treasure, he obtains it and brandishes it (just like when opening a [chest](./chest.md)). If the treasure is saved, it disappears from the [map](../map.md), otherwise it stays and can be bought again.

Shop treasures allow to make shops very easily from Solarus Editor. They provide a built-in process that handles entirely the dialogs, the price and the treasure. This API does not provide much control on this process. If you need to implement more customized interactions, you can use a [generalized NPC](./npc.md).

## Dialogs of shop treasures

Dialogs of shop treasures need special care because everything is built-in with shop treasures, but dialogs are entirely customizable. You can skip this section if you are not making your own dialog box system.

Dialogs that are displayed when interacting with a shop treasure are quite elaborate because they have both a parameter (the price: a number) and a result (the decision of the player: a boolean). If you implement your own dialog box in Lua, which is recommended, you need to respect the mechanism described in this section for the particular dialog of shop treasures.

First, when the hero interacts with a shop treasure, a dialog shows the description of the treasure. There is nothing special about this dialog.

Then, a second dialog shows the price of the treasure and asks if the player wants to buy it. The player can accept or refuse. This second dialog has the id `"_shop.question"`

Since the price of the treasure is only known at runtime ([dialogs.dat](../../files-specs/languages/dialogs.md) cannot know it), you should put a special sequence in the text of the dialog instead (like `"$v"`), and replace this special sequence by the actual price at runtime. At runtime, the price is the `info` argument of the [`game:on_dialog_started()`](../game.md#gameon_dialog_starteddialog-info) event, so you can perform the substitution there.

Furthermore, when the dialog finishes, you have to tell the engine the decision of the player. This value to return can be `true` to buy the treasure, or anything else to refuse and do nothing. Use the `status` parameter of [`game:stop_dialog(status)`](../game.md#gamestop_dialogstatus) to indicate it.

!!! note "Note"

    Recall that when the [`game:on_dialog_started()`](../game.md#gameon_dialog_starteddialog-info) event is not defined, the engine uses by default a built-in dialog box with minimal features. This built-in dialog box works correctly with shop items, providing that the text of the `"_shop.question"` dialog has the expected format. It should have three lines. The `"$v"` sequence (if any) is subsituted by the actual price like suggested above, and the user can select one of the last two lines: the first one is yes and the second one is no.

## Methods Inherited from `entity`

Shop treasures are particular [map entities](./index.md). Therefore, they inherit all methods from the type map entity.

See [entity](./index.md#methods-of-all-entity-types) to know these methods.

## Methods of the type shop treasure

### `shop_treasure:get_cannot_buy_sound()`

Returns the sound id defined for this shop treasure when the player is unable to buy it.

Return value (string)
: The sound id.

### `shop_treasure:set_cannot_open_sound([sound_id])`

Changes the sound to play when the player is unable to buy the shop treasure. By default, the sound used is "wrong". You can use this function if you want another sound to be played.

`sound_id` (string or `nil`, optional)
: The sound id. An empty string or `nil` means no sound.

## Events Inherited from `entity`

Events are callback methods automatically called by the engine if you define them.

Shop treasures are particular [map entities](./index.md). Therefore, they inherit all events from the type map entity.

See [entity](./index.md#events-of-all-entity-types) to know these events.

## Events of the type shop treasure

The following events are specific to shop treasures.

### `shop_treasure:on_buying()`

Called when the player is about to buy this treasure.

He already accepted to buy it and validated the dialog. At this point, you may cancel the deal for some reason by returning `false`(for example if you realize that the player has no bottle and therefore cannot buy the potion he wanted).

Return value (boolean)
: `true` if the player is allowed to buy the treasure.

### `shop_treasure:on_bought()`

Called when the player has just bought this treasure.

Return value (boolean)
: `true` if the player is allowed to buy the treasure.

!!! note "Note"

    This event is called right after the more general events [`item:on_obtaining()`](../equipment-items.md#itemon_obtainingvariant-savegame_variable) and [`map:on_obtaining_treasure()`](../map.md#mapon_obtaining_treasuretreasure_item-treasure_variant-treasure_savegame_variable-hero). Those two events are called no matter how the treasure is being obtained: from a [chest](./chest.md), from a [pickable treasure](./pickable.md), from a shop treasure or explicitly with [`hero:start_treasure()`](./hero.md#herostart_treasuretreasure_name-treasure_variant-treasure_savegame_variable-callback).
