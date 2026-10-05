# Hero

## Overview

The hero is the character controlled by the player. There is always exactly one hero on the current map. The hero is automatically created by the engine: you cannot create or remove him.

The name of the hero (as returned by [`entity:get_name()`](./index.md#entityget_name)) is always `"hero"` Therefore, from a script, you can access the hero with `map:get_entity("hero")` or, if you are in a map script, directly with the `hero` variable (see [`map:get_entity()`](../map.md#mapget_entityname) for more details). There are also quick accessors [`map:get_hero()`](../map.md#mapget_hero) and [`game:get_hero()`](../game.md#gameget_hero) to retrieve the hero more easily.

His size is always `16×16` pixels.

The hero entity persists accross map changes. In other words, the effect of functions like [`hero:set_walking_speed()`](#heroset_walking_speedwalking_speed) persists when the player goes to another map. Similarly, events that you define on the hero (like [`hero:on_taking_damage()`](#heroon_taking_damagedamage)) continue to get called on any map while the game is active.

We describe here the Lua API that you can use to change the hero's state.

### Hero sprites

Multiple sprites for the hero are automatically created by the engine. You can access them like for any other entity, specifying their name in [`entity:get_sprite([name])`](./index.md#entityget_spritename).

Here is the list of hero sprites created by the engine and their names:

| Sprite          | Role                                                                                                                                         |
| --------------- | -------------------------------------------------------------------------------------------------------------------------------------------- |
| `"tunic"`       | Main sprite of the hero. It always exists.</br>This is the default one in [`entity:get_sprite([name])`](./index.md#entityget_spritename). |
| `"shield"`      | Shield if any.                                                                                                                               |
| `"sword"`       | Sword if any.                                                                                                                                |
| `"sword_stars"` | Small stars sparkling near the sword in some states.                                                                                         |
| `"shadow"`      | Shadow displayed under the hero in some states.                                                                                              |
| `"trail"`       | Trail of dust displayed under the hero when running.                                                                                         |
| `"ground"`      | Ground displayed under the hero when walking on special terrain like shallow water or grass.                                                 |

Keep in mind that depending on the hero's equipment, his state and the ground below him, all of these built-in sprites don't always exist, and some of them may exist at some point but without being currently displayed.

## Methods Inherited from `entity`

The hero is a particular [map entity](./index.md). Therefore, he inherits all methods from the type map entity.

See [entity](./index.md#methods-of-all-entity-types) to know these methods.

## Methods of the type hero

The following methods are specific to the hero.

### `hero:get_controls()`

Returns the controls currently applied for this hero.

Return value ([controls](../controls/index.md))
: The Controls object for this hero.

### `hero:set_controls(controls)`

Sets the controls for this hero.

`controls` ([controls](../controls/index.md))
: The controls to apply to this hero entity.

### `hero:teleport(map_id, [destination_name, [transition_style]])`

Teletransports the hero to a different place.

`map_id` (string)
: Id of the [map](../map.md) to go to (may be the same map or another one). If the map does not exist, the teletransportation fails and this function generates a Lua error. If the map exists, then the teletransportation is always successful even if there is no viable destination (see below).

`destination_name` (string or `nil`, optional)
: Name of the [destination entity](./destination.md) where to go on that map, or the special keyword `"_same"` to keep the same coordinates. Can also be the special keyword `"_side0"`, `"_side1"`, `"_side2"` or `"_side3"` to arrive near the East, North, West or South frontier of the map respectively. But the hero should be near the corresponding side of the original map for this to look okay. This is usually used in combination with scrolling transitions. No value means the default destination entity of the map. If the destination does not exist, a debugging message is logged and the default destination is used as a fallback. Finally, if there is no destination at all, then no default destination can be used. In this case, another debugging message is logged and the hero is placed at coordinates `(0,0)`

`transition_style` (string, optional, requires: `destination_name`)
: `"immediate"` (no transition effect), `"fade"` (fade-out and fade-in effect) or `"scrolling"`. No value means [`game:get_transition_style()`](../game.md#gameget_transition_style), which is `"fade"` by default.

!!! note "Note"

    The transportation will occur at the next cycle of the engine's main loop. Therefore, your map is not unloaded immediately and your map script continues to work.

### `hero:get_direction()`

Returns the direction of the hero's [sprites](../drawable-objects/sprite.md).

Return value (number)
: The direction of the hero's sprites, between `0` (East) and `3` (South).

!!! note "Note"

    The direction of the hero's sprites may be different from both the direction pressed by the [player's commands](../game.md#gameget_commands_direction) and from the actual direction of the hero's [movement](../movements/index.md).

### `hero:set_direction(direction4)`

Sets the direction of the hero's [sprites](../drawable-objects/sprite.md).

`direction4` (number)
: The direction of the hero's sprites, between `0` (East) and `3` (South).

!!! note "Note"

    The direction of the hero's sprites may be different from both the direction pressed by the [player's commands](../game.md#gameget_commands_direction) and from the actual direction of the hero's [movement](../movements/index.md).

### `hero:get_walking_speed()`

Returns the speed of the normal walking movement of hero.

This speed is automatically reduced when the hero walks on special ground like grass, ladders or holes.

Return value (number)
: The speed of normal walk in pixels per second.

### `hero:set_walking_speed(walking_speed)`

Sets the speed of the normal walking movement of hero.

The default walking speed is 88 pixels per second. This speed is automatically reduced when the hero walks on special ground like grass, ladders or holes.

`walking_speed` (number)
: The speed of normal walk in pixels per second.

### `hero:get_swimming_speed()`

Returns the speed of the swimming movement of hero.

Return value (number)
: The swimming speed in pixels per second.

### `hero:set_swimming_speed(swimming_speed)`

Sets the swimming speed movement of hero.

The default swimming speed is 44 pixels per second. When the player perform the swim boost, the speed is doubled.

`swimming_speed` (number)
: The swimming speed n pixels per second.

### `hero:get_can_swim_faster()`

Returns true if the hero is allowed to perform a swim boost in deep water.

Return value (boolean)
: The ability to swim faster.

### `hero:set_can_swim_faster([can_swim_faster])`

Sets the ability for the player to perform a swim boost in deep water.

`can_swim_faster` (boolean, optional, default: `true`)
: true if the hero is allowed to swim faster.

### `hero:get_push_delay()`

Returns the delay between the moment the hero starts punshing onto a wall (i.e. the hero is facing an obstacle and the player keeps pressing the direction toward the wall) and the moment its state and animation are set to "pushing".

Return value (number)
: The push delay, in ms.

### `hero:set_push_delay(delay)`

Sets the hero's [push delay](#heroget_push_delay).

The default delay is `800` ms.

`delay` (number)
: The delay before starting the pushing state, in ms.

### `hero:get_carry_height()`

Returns the height carried objects will be displayed at, i.e. carried objects will be displayed this many pixels above the hero's origin on screen. Carried object can add their own offset to this height.

Return value (number)
: The carry height.

### `hero:set_carry_height(height)`

Sets the height carried objects will be displayed at, i.e. carried objects will be displayed this many pixels above the hero's origin on screen. Carried object can add their own offset to this height. The default height is 18 pixels.

`height` (number)
: The new height.

### `hero:save_solid_ground([x, y, layer])`

Sets a position to go back to if the hero falls into a hole or other bad ground.

This replaces the usual behavior which is going back to the last solid position before the fall.

The position can be specified either as coordinates and a layer, or as a function that returns coordinates and a layer. Using a function allows to decide the position at the last moment.

To memorize a position directly:

`x` (number, optional, requires: `y` and `layer`)
: X coordinate to memorize (no value means the current position).

`y` (number, optional, requires: `x` and `layer`)
: Y coordinate to memorize (no value means the current position).

`layer` (number, optional, requires: `x` and `y`)
: Layer to memorize (no value means the current position).

To set a function that indicates the position to go back to:

!!! note "Note"

    You can instead pass a callback function to be called whenever the hero falls into bad ground. The function should return 2 or 3 values: x, y and optionally the layer (no layer value means keeping the layer unchanged). A `nil` value unsets any position or function that was previously set.

### `hero:reset_solid_ground()`

Forgets a position that was previously memorized by [`hero:save_solid_ground()`](#herosave_solid_groundx-y-layer) (if any).

The initial behavior is restored: the hero will now get back to where he was just before falling, instead going to of a memorized position.

This is equivalent to `hero:save_solid_ground(nil)`.

### `hero:get_solid_ground_position()`

Returns the position where the hero gets back if he falls into a hole or other bad ground now.

This is the position that was previously memorized by the last call to [`hero:save_solid_ground()`](#herosave_solid_groundx-y-layer), if any. If the position was passed to [`hero:save_solid_ground()`](#herosave_solid_groundx-y-layer) as a function, then this function is called to get a position.

Otherwise, this is the position of the hero the last time he was on solid ground.

Return value 1 (number or `nil`)
: X coordinate where to get back to solid ground, or `nil` if the hero never went on solid ground on this map yet.

Return value 2 (number)
: Y coordinate where to get back to solid ground, or no value if the hero never went on solid ground on this map yet.

Return value 3 (number)
: Layer where to get back to solid ground, or no value if the hero never went on solid ground on this map yet.

### `hero:get_animation()`

Returns the current animation of the hero's sprites.

The hero may have several sprites (see [`hero:set_animation()`](#heroset_animationanimation-callback)). This function always returns the animation of the tunic sprite.

Return value (string)
: The animation name of the tunic sprite.

### `hero:set_animation(animation, [callback])`

Changes the animation of the hero's sprites.

The hero has several [sprites](../drawable-objects/sprite.md), that are normally displayed or not depending on his [state](#heroget_state) and his [abilities](../game.md#gameget_abilityability_name):

- the body (the tunic),
- the shield,
- the sword,
- the sword stars,
- the trail of dust (by default, only shown when running).

The animation of all these sprites is usually managed by the engine to represent the current state of the hero. You can use this function to customize the animation. This allows you to implement custom actions that are not provided by the built-in states. Make sure that you don't call this function when the hero is not already doing a particular action (like pushing something or attacking), unless you know what you are doing.

All sprites of the hero that have an animation with the specified name take the animation. The ones that don't have such an animation are not displayed.

`animation` (string)
: Name of the animation to set to hero sprites.

`callback` (function, optional)
: A function to call when the animation ends (on the tunic sprite). If the animation loops, or is replaced by another animation before it ends, then the function will never be called.

### `hero:is_invincible()`

Returns whether the hero is currently invincible.

The hero is temporarily invincible after being hurt or after you called [`hero:set_invincible()`](#heroset_invincibleinvincible-duration). In this situation, [enemies](./enemy.md) cannot attack the hero, but you can still hurt him manually with [`hero:start_hurt()`](#herostart_hurtsource_x-source_y-damage-herostart_hurtsource_entity-source_sprite-damage).

Return value (boolean)
: `true` if the hero is currently invincible.

### `hero:set_invincible([invincible, [duration]])`

Sets or unsets the hero temporarily invincible.

When the hero is invincible, [enemies](./enemy.md) cannot attack him, but you can still hurt him manually with [`hero:start_hurt()`](#herostart_hurtsource_x-source_y-damage-herostart_hurtsource_entity-source_sprite-damage).

`invincible` (boolean, optional, default: `true`)
: `true` to make the hero invincible, or `false` to stop the invincibility.

`duration` (number, optional, requires: `invincible`)
: Duration of the invincibility in milliseconds. Only possible when you set `invincible` to `true` No value means unlimited.

### `hero:is_blinking()`

Returns whether the hero's sprites are currently blinking.

The sprites are temporarily blinking after the hero was hurt or after you called [`hero:set_blinking()`](#heroset_blinkingblinking-duration).

Return value (boolean)
: `true` if the hero's sprite are currently blinking.

!!! note "Note"

    The visibility property of the hero is independent from this. Even when the sprites are blinking, the result of [`hero:is_visible()`](./index.md#entityis_visible) is unchanged.

### `hero:set_blinking([blinking, [duration]])`

Makes the hero's sprites temporarily blink or stop blinking.

This only affects displaying: see [`hero:set_invincible()`](#heroset_invincibleinvincible-duration) if you also want to make the hero invincible.

`blinking` (boolean, optional, default: `true`)
: `true` to to make the sprites blink, or `false` to stop the blinking.

`duration` (number, optional, requires: `blinking`)
: Duration in milliseconds before stopping the blinking. Only possible when you set `blinking` to `true` No value means unlimited.

### `hero:get_carried_object()`

Returns the [carried object](./carried-object.md) the hero is currently lifting, carrying or throwing, if any.

Return value ([carried object](./carried-object.md) or `nil`)
: The current carried object or `nil`.

### `hero:freeze()`

Prevents the player from moving the hero until you call [`hero:unfreeze()`](#herounfreeze).

After you call this method, the [state](#heroget_state) of the hero is `"frozen"`

### `hero:unfreeze()`

Restores the control to the player. The control may have been lost for example by a call to [`hero:freeze()`](#herofreeze) or to [`some_movement:start(hero)`](../movements/index.md#movementstartobject_to_move-callback).

### `hero:walk(path, [loop, [ignore_obstacles]])`

Makes the hero move with the specified path and a walking animation. The player cannot control him during the movement.

`path` (string)
: The path as a string sequence of integers. Each value is a number between `0` and `7` that represents a step (move of 8 pixels) in the path. `0` is East, `1` is North-East, etc.

`loop` (boolean, optional, default: `false`)
: `true` to repeat the path once it is done.

`ignore_obstacles` (boolean, optional, requires: `loop`, default: `false`)
: `true` to allow the hero to traverse obstacles during this movement. Make sure the movement does not end inside an obstacle.

### `hero:start_jumping(direction8, distance, [ignore_obstacles])`

Makes the hero jump towards the specified direction.

`direction8` (number)
: Direction of the jump, between `0` and `7` (see [`jump_movement:set_direction8()`](../movements/jump-movement.md#jump_movementset_direction8direction8)).

`distance` (number)
: Distance of the jump in pixels (see [`jump_movement:set_distance()`](../movements/jump-movement.md#jump_movementset_distancedistance)).

`ignore_obstacles` (boolean, optional, default: `false`)
: `true` to allow the hero to traverse obstacles during this movement. Make sure the movement does not end inside an obstacle.

### `hero:start_attack()`

Makes the hero perform his main attack (swinging his sword).

This function does the same as what happens when the player presses the `"attack"` game [command](../game.md#game-commands). You can use it to trigger the attack from your script instead of from a game command.

If the player is not allowed to perform the attack now (because he does not have the sword [ability](#heroget_abilityability_name) or because the hero is currently busy in another state that does not allow to use the sword), then nothing happens.

### `hero:start_attack_loading([spin_attack_delay])`

Makes the hero start loading his sword.

This function does the same as what happens when the player keeps pressing the `"attack"` game [command](../game.md#game-commands) after using the sword.

If the player is not allowed to perform this attack now (because he does not have the sword [ability](#heroget_abilityability_name) or because the hero is currently busy in another state that does not allow to use the sword), then nothing happens.

`spin_attack_delay` (number, optional, default: `1000`)
: Delay in milliseconds before the sword is loaded, allowing a spin attack then. A value of `0` allows the spin attack immediately. The special value `-1` means infinite: then, no spin attack will be possible.

### `hero:start_item(item)`

Makes the hero use an [equipment item](../equipment-items.md).

The [`item:on_using()`](../equipment-items.md#itemon_usinghero) event will be called and the player won't be able to control the hero until you call [`item:set_finished()`](../equipment-items.md#itemset_finished). See the documentation of [equipment items](../equipment-items.md) for more information.

This function does the same as what happens when the player presses a game [command](../game.md#game-commands) corresponding to this equipment item. You can use it to trigger the item from your script instead of from a game command.

If the player is not allowed to use the item now (because he does not have it, because the item cannot be used explicitly, or because the hero is currently busy in another state), then nothing happens.

`item` ([item](../equipment-items.md))
: The equipment item to start using.

### `hero:start_grabbing()`

Makes the hero grab the obstacle he is facing.

This function does the same as what happens when the player presses the `"action"` game [command](../game.md#game-commands) while facing an obstacle. You can use it to start the grabbing state from your script instead of from a game command.

### `hero:start_treasure(treasure_name, [treasure_variant, [treasure_savegame_variable]], [callback])`

Gives a treasure to the player. The hero will brandish the treasure as follows.

The hero sprites take the animation `"treasure"` The treasure is displayed above the hero, as the sprite `"entities/items"` with animation `treasure_name`and direction `treasure_variant - 1`. If this sprite, this animation or this direction do not exist, then no treasure sprite is displayed above the hero.

If a dialog called `"_treasure.treasure_name.treasure_variant"` exists, and if there is no currently active dialog already, then this dialog is displayed during the brandishing animation. For example, calling `hero:start_treasure("heart", 1)` will automatically show the dialog `_treasure.heart.1`.

`treasure_name` (string, optional)
: Kind of treasure to give (the name of an [equipment item](../equipment-items.md)). The treasure must be an [obtainable](../equipment-items.md#itemis_obtainable) item.

`treasure_variant` (number, optional)
: Variant of the treasure (because some [equipment items](../equipment-items.md) may have several variants). The default value is `1` (the first variant).

`treasure_savegame_variable` (string, optional, requires: `treasure_variant`)
: Name of the boolean value that stores in the [savegame](../game.md) whether this treasure is found. No value means that the state of the treasure is not saved. It is allowed (though strange) to give the same saved treasure twice.

`callback` (function, optional)
: A function that will be called when the treasure's dialog finishes, or after a delay of 3 seconds if there is no dialog.

!!! note "Note"

    If there is no callback, or if the callback does not change the state of the hero, then he automatically stops the brandishing animation.

### `hero:start_victory([callback])`

Makes the hero brandish his sword for a victory.

`callback` (function, optional)
: A function to call when the victory sequence finishes. If you don't define it, the default behavior is to restore control to the player. If you define it, you can do other things, like teletransporting the hero somewhere else. To restore the control to the player, call [`hero:unfreeze()`](#herounfreeze).

### `hero:start_boomerang(max_distance, speed, tunic_preparing_animation, sprite_name)`

Makes the hero shoot a [boomerang](./boomerang.md).

`max_distance` (number)
: Maximum distance of the boomerang's movement in pixels.

`speed` (number)
: Speed of the boomerang's movement in pixels per second.

`tunic_preparing_animation` (string)
: Name of the animation that the hero's tunic sprite should take while preparing the boomerang.

`sprite_name` (string)
: Sprite animation set to use to draw the boomerang then.

### `hero:start_bow()`

Makes the hero shoot an [arrow](./arrow.md) with a bow.

### `hero:start_hookshot()`

Makes the hero throw a [hookshot](./hookshot.md).

### `hero:start_running()`

Makes the hero run.

### `hero:start_hurt(source_x, source_y, damage), hero:start_hurt([source_entity, [source_sprite]], damage)`

Hurts the hero, exactly like when he is touched by an [enemy](./enemy.md). The hurting animations and sounds of the hero are played.

This method hurts the hero even if enemies cannot, including when the hero is temporarily [invincible](#herois_invincible) (because he was already hurt recently) or when the hero is in a state where he cannot be hurt (for example when he is brandishing a treasure).

| With coordinates | With a source [entity](./index.md) |
|------------------|------------------------------|
| <dl><dt>`source_x` (number)</dt><dd>X coordinate of whatever hurts the hero. Used to push the hero away from that source.</dd><dt>`source_y` (number)</dt><dd>Y coordinate of whatever hurts the hero. Used to push the hero away from that source.</dd></dl> | <dl><dt>`source_entity` ([map entity](./index.md), optional)</dt><dd>Whatever hurts the hero. The coordinates of this source entity are used to push the hero away from that source. No value means that the hero will not be pushed away.</dd><dt>`source_sprite` ([sprite](../drawable-objects/sprite.md), optional, requires: `source_entity`)</dt><dd>Which sprite of the source entity is hurting the hero. If you set this value, the hero will be pushed away from the origin of this sprite instead of from the origin of the source entity. Most of the time, you don't need to set this parameter.</dd></dl> |

`damage` (number)
: Base number of life points to remove (possibly `0`) This number will be divided by the [tunic](#heroget_abilityability_name) level of the player, unless you override this default calculation in [`hero:on_taking_damage()`](#heroon_taking_damagedamage).

!!! note "Note"

    If you just want to remove some life to the player, without making the hurting animations and sounds of the hero, see [`game:remove_life()`](../game.md#gameremove_lifelife).

### `hero:get_state()`

Returns the name of the current state of the hero, and possibly the corresponding [custom state](../custom-states.md) object if any.

Return value 1 (`string`)
: The current state. Can be one of:

    - `"back_to_solid_ground"`
    - `"boomerang"`
    - `"bow"`
    - `"carrying"`
    - `"falling"`
    - `"forced_walking"`
    - `"free"`
    - `"frozen"`
    - `"grabbing"`
    - `"hookshot"`
    - `"hurt"`
    - `"jumping"`
    - `"lifting"`
    - `"plunging"`
    - `"pulling"`
    - `"pushing"`
    - `"running"`
    - `"stairs"`
    - `"swimming"`
    - `"sword_loading"`
    - `"sword_spin_attack"`
    - `"sword_swinging"`
    - `"sword_tapping"`
    - `"treasure"`
    - `"using_item"`
    - `"victory"`
    - `"custom"`

Return value 2 ([state](../custom-states.md))
: The custom state object, in case the state name is `"custom"`. Otherwise, no value is returned.

### `hero:start_state(state)`

Starts a [custom state](../custom-states.md) on the hero.

Custom states allow advanced customization of the hero's behavior. After you call this method, the hero state string as returned by [`hero:get_state()`](#heroget_state) is `"custom"` Use [`hero:get_state_object()`](#heroget_state_object) to get the actual custom state object.

`state` ([custom state](../custom-states.md))
: The custom state to start.

### `hero:get_state_object()`

Returns the current custom state object of the hero, if any.

Return value ([state](../custom-states.md) or `nil`)
: The custom state, or `nil` if the current state is not a custom one.

### `hero:get_life()`

Returns the current level of life of the player.

Return value (number)
: The current life.

### `hero:set_life(life)`

Sets the level of life of the player.

A negative value will be replaced by zero. A value greater than than the maximum level of life will be replaced by the maximum value.

`life` (number)
: Number of life points to set.

### `hero:add_life(life)`

Adds some life to the player.

`life` (number)
: Number of life points to add. Must be a positive number or `0`.

!!! note "Note"

    Equivalent to `hero:set_life(hero:get_life() + life)`.

### `hero:remove_life(life)`

Removes some life from the player.

`life` (number)
: Number of life points to remove. Must be a positive number or `0`.

!!! note "Note"

    Equivalent to `hero:set_life(hero:get_life() - life)`.

### `hero:get_max_life()`

Returns the maximum level of life of the player.

Return value (number)
: The maximum number of life points.

### `hero:set_max_life(life)`

Sets the maximum level of life of the player.

`life` (number)
: Maximum number of life points to set. Must be a positive number.

### `hero:add_max_life(life)`

Increases the maximum level of life of the player.

`life` (number)
: Maximum number of life points to add to the maximum. Must be a positive number or `0`.

!!! note "Note"

    Equivalent to `hero:set_max_life(hero:get_max_life() + life)`.

### `hero:get_money()`

Returns the amount of money of the player.

Return value (number)
: The current amount of money.

### `hero:set_money(money)`

Sets the amount of money of the player.

A negative value will be replaced by zero. A value greater than than the maximum amount of money will be replaced by the maximum amount.

`money` (number)
: The amount of money to set.

### `hero:add_money(money)`

Adds some money to the player.

`money` (number)
: Amount of money to add. Must be a positive number or `0`.

!!! note "Note"

    Equivalent to `hero:set_money(hero:get_money() + money)`.

### `hero:remove_money(money)`

Removes some money from the player.

`money` (number)
: Amount of money to remove. Must be a positive number or `0`.

!!! note "Note"

    Equivalent to `hero:set_money(hero:get_money() - money)`.

### `hero:get_max_money()`

Returns the maximum amount of money of the player.

Return value (number)
: The maximum money.

### `hero:set_max_money(money)`

Sets the maximum amount of money of the player.

`money` (number)
: Maximum money to set. Must be a positive number or `0`.

### `hero:get_magic()`

Returns the current number of magic points.

Return value (number)
: The current number of magic points.

### `hero:set_magic(magic)`

Sets the amount of magic points of the player.

A negative value will be replaced by zero. A value greater than than the maximum number of magic points will be replaced by that maximum.

`magic` (number)
: The number of magic points to set.

### `hero:add_magic(magic)`

Adds some magic points to the player.

`magic` (number)
: Number of magic points to add. Must be a positive number or `0`.

!!! note "Note"

    Equivalent to `hero:set_magic(hero:get_magic() + magic)`.

### `hero:remove_magic(magic)`

Removes some magic points from the player.

`magic` (number)
: Number of magic points to remove. Must be a positive number or `0`.

!!! note "Note"

    Equivalent to `hero:set_magic(hero:get_magic() - magic)`.

### `hero:get_max_magic()`

Returns the maximum number of magic points.

Return value (number)
: The maximum number of magic points.

### `hero:set_max_magic(magic)`

Sets the maximum number of magic points.

`magic` (number)
: The maximum number of magic points to set. Must be a positive number or `0`.

### `hero:has_ability(ability_name)`

Returns whether the player has a built-in ability.

`ability_name` (string)
: Name of the ability to get (see [`hero:get_ability()`](#heroget_abilityability_name) for the list of valid ability names).

Return value (boolean)
: `true` if the player has this ability.

!!! note "Note"

    Equivalent to `hero:get_ability(ability_name) > 0`.

### `hero:get_ability(ability_name)`

Returns the level of a built-in ability.

Built-in ability levels indicate whether the hero can perform some built-in actions like attacking, swimming or running. The initial value is `0` unless specified otherwise.

`ability_name` (string)
: Name of the ability to get. Valid ability names are:

    - `"sword"`: Using the sword when pressing the attack command. Determines the default sword sprite.
    - `"sword_spin_attack"`: Ability to make a spin attack. 1 is a normal spin attack, 2 is a super spin attack where the hero makes circles.
    - `"tunic"`: Resistance level that reduces the damage received by the hero. Determines the default sprite used for the hero's body. The initial value is `1`.
    - `"shield"`: Protection against enemies. Allows to avoid some attacks. Determines the default shield sprite.
    - `"lift"`: Ability to [lift other entities](./index.md#entityget_weight).
    - `"swim"`: Ability to swim in deep water.
    - `"jump_over_water"`: Automatically jumping when arriving into water without the `"swim"` ability.
    - `"run"`: Running when pressing the action command.
    - `"push"`: Trying to push when walking towards an obstacle. The initial value is `1`.
    - `"grab"`: Grabbing the faced obstacle when pressing the action command. The initial value is `1`.
    - `"pull"`: Pulling the faced obstacle the hero is grabbing. The initial value is `1`.
    - `"detect_weak_walls"`: Notifying the player with a sound when a weak wall is nearby.

Return value (number)
: Level of this ability (`0` means not having this ability yet).

### `hero:set_ability(ability_name, level)`

Sets the level of an ability.

`ability_name` (string)
: Name of the ability to set (see [`hero:get_ability()`](#heroget_abilityability_name) for the list of valid ability names).

`level` (number)
: Level of this ability to set (`0` removes the ability).

### `hero:get_item(item_name)`

Returns an equipment item.

`item_name` (string)
: Name of the item to get.

Return value ([item](../equipment-items.md))
: The corresponding equipment item.

### `hero:has_item(item_name)`

Returns whether the player has the specified [equipment item](../equipment-items.md) (only for a saved item).

`item_name` (string)
: Name of the item to check.

Return value (boolean)
: `true` if the player has at least the first variant of this item.

!!! note "Note"

    Equivalent to `hero:get_item(item_name):get_variant() > 0`.

### `hero:get_item_assigned(slot)`

Returns the equipment item assigned to a slot.

`slot` (number)
: The slot to get:

    - `1`
    - `2`

Return value ([item](../equipment-items.md) or `nil`)
: The equipment item associated to this slot (`nil` means none).

### `hero:set_item_assigned(slot, item)`

Assigns an equipment item to a slot.

`slot` (number)
: The slot to set:

    - `1`
    - `2`

`item` ([item](../equipment-items.md) or `nil`)
: The equipment item to associate to this slot, or `nil` to make the slot empty.

### `hero:get_tunic_sprite_id()`

Returns the name of the sprite representing the hero's body.

Return value (string)
: The [sprite](../drawable-objects/sprite.md) animation set id of the hero's tunic.

### `hero:set_tunic_sprite_id(sprite_id)`

Changes the sprite representing the hero's body.

By default, the sprite used for the body is `"hero/tunicX"`,where X is the [tunic level](#heroget_abilityability_name).

You can use this function if you want to use another sprite.

`sprite_id` (string)
: The [sprite](../drawable-objects/sprite.md) animation set id of the hero's tunic.

### `hero:get_sword_sprite_id()`

Returns the name of the sprite representing the hero's sword.

Return value (string)
: The [sprite](../drawable-objects/sprite.md) animation set id of the hero's sword.

### `hero:set_sword_sprite_id(sprite_id)`

Changes the sprite representing the hero's sword.

By default, the sprite used for the sword is `"hero/swordX"`,where X is the [sword level](#heroget_abilityability_name), or no sprite if the sword level is `0`.

You can use this function if you want to use another sprite.

`sprite_id` (string)
: The [sprite](../drawable-objects/sprite.md) animation set id of the hero's sword. An empty string means no sword sprite.

### `hero:get_sword_sound()`

Returns the name of the sound played when the hero uses the sword.

Return value (string or `nil`)
: The sound id of the hero's sword. `nil` means no sound.

### `hero:set_sword_sound([sound_id])`

Changes the sound to play when the hero uses the sword.

By default, the sound used for the sword is `"swordX"`,where X is the [sword level](#heroget_abilityability_name), or no sound if the sword level is `0`.

You can use this function if you want another sound to be played.

`sound_id` (string or `nil`, optional)
: The sound id of the hero's sword. `nil` or an empty string means no sword sound.

### `hero:get_shield_sprite_id()`

Returns the name of the sprite representing the hero's shield.

Return value (string)
: The [sprite](../drawable-objects/sprite.md) animation set id of the hero's shield.

### `hero:set_shield_sprite_id(sprite_id)`

Changes the sprite representing the hero's shield.

By default, the sprite used for the shield is `"hero/shieldX"`,where X is the [shield level](#heroget_abilityability_name), or no sprite if the shield level is `0`.

You can use this function if you want to use another sprite.

`sprite_id` (string)
: The [sprite](../drawable-objects/sprite.md) animation set id of the hero's shield. An empty string means no shield sprite.

### `hero:get_falling_sound()`

Returns the name of the sound played when the hero is falling in bad grounds like a hole.

Return value (string or `nil`)
: The falling sound id. `nil` means no sound.

### `hero:set_falling_sound([sound_id])`

Changes the sound to play when the hero is falling in bad ground like a hole.

By default, the falling sound is `"hero_falls"`

You can use this function if you want another sound to be played.

`sound_id` (string or `nil`, optional)
: The falling sound id. `nil` or an empty string means no sound.

### `hero:get_respawn_sound()`

Returns the name of the sound played when the hero gets back to solid ground after falling.

Return value (string or `nil`)
: The respawning sound id. `nil` means no sound.

### `hero:set_respawn_sound([sound_id])`

Changes the sound to play when the hero gets back to solid ground after falling.

By default, this sound is `"message_end"` for historical reasons.

You can use this function if you want another sound to be played.

`sound_id` (string or `nil`, optional)
: The respawning sound id. `nil` or an empty string means no sound.

### `hero:get_landing_sound()`

Returns the name of the sound played when the hero is landing after juming from a jumper or from an upper layer.

Return value (string or `nil`)
: The landing sound id. `nil` means no sound.

### `hero:set_landing_sound([sound_id])`

Changes the sound to play when the hero is landing after juming from a jumper or from an upper layer.

By default, this sound is `"hero_lands"` for historical reasons.

You can use this function if you want another sound to be played.

`sound_id` (string or `nil`, optional)
: The landing sound id. `nil` or an empty string means no sound.

### `hero:get_jumping_sound()`

Returns the name of the sound played when the hero is jumping from a jumper or using the built-in feather item.

Return value (string or `nil`)
: The jumping sound id. `nil` means no sound.

### `hero:set_jumping_sound([sound_id])`

Changes the sound to play when the hero is jumping from a jumper or using the built-in feather item.

By default, this sound is `"jump"` for historical reasons.

You can use this function if you want another sound to be played.

`sound_id` (string or `nil`, optional)
: The jumping sound id. `nil` or an empty string means no sound.

### `hero:get_hurt_sound()`

Returns the name of the sound played when the hero is getting damage.

Return value (string or `nil`)
: The hurt sound id. `nil` means no sound.

### `hero:set_hurt_sound([sound_id])`

Changes the sound to play when the hero is getting damage.

By default, this sound is `"hero_hurt"` for historical reasons.

You can use this function if you want another sound to be played.

`sound_id` (string or `nil`, optional)
: The hurt sound id. `nil` or an empty string means no sound.

### `hero:get_sinking_sound()`

Returns the name of the sound played when the hero is sinking into deep water or lava.

Return value (string or `nil`)
: The sinking sound id. `nil` means no sound.

### `hero:set_sinking_sound([sound_id])`

Changes the sound to play when the hero is sinking into deep water or lava.

By default, this sound is `"splash"` for historical reasons.

You can use this function if you want another sound to be played.

`sound_id` (string or `nil`, optional)
: The sinking sound id. `nil` or an empty string means no sound.

### `hero:get_swimming_sound()`

Returns the name of the sound played when the hero is swimming.

Return value (string or `nil`)
: The swimming sound id. `nil` means no sound.

### `hero:set_swimming_sound([sound_id])`

Changes the sound to play when the hero is swimming.

By default, this sound is `"swim"` for historical reasons.

You can use this function if you want another sound to be played.

`sound_id` (string or `nil`, optional)
: The swimming sound id. `nil` or an empty string means no sound.

### `hero:get_walking_on_grass_sound()`

Returns the name of the sound played when the hero is walking on tall grass.

Return value (string or `nil`)
: The walking_on_grass sound id. `nil` means no sound.

### `hero:set_walking_on_grass_sound([sound_id])`

Changes the sound to play when the hero is walking on tall grass.

By default, this sound is `"walk_on_grass"` for historical reasons.

You can use this function if you want another sound to be played.

`sound_id` (string or `nil`, optional)
: The walking_on_grass sound id. `nil` or an empty string means no sound.

### `hero:get_walking_on_water_sound()`

Returns the name of the sound played when the hero is walking on shallow water.

Return value (string or `nil`)
: The walking_on_water sound id. `nil` means no sound.

### `hero:set_walking_on_water_sound([sound_id])`

Changes the sound to play when the hero is walking on shallow water.

By default, this sound is `"walk_on_water"` for historical reasons.

You can use this function if you want another sound to be played.

`sound_id` (string or `nil`, optional)
: The walking_on_water sound id. `nil` or an empty string means no sound.

### `hero:get_lifting_sound()`

Returns the name of the sound played when the hero is lifting an entity (if liftable).

Return value (string or `nil`)
: The lifting sound id. `nil` means no sound.

### `hero:set_lifting_sound([sound_id])`

Changes the sound to play when the hero is lifting an entity (if liftable).

By default, this sound is `"lift"` for historical reasons.

You can use this function if you want another sound to be played.

`sound_id` (string or `nil`, optional)
: The lifting sound id. `nil` or an empty string means no sound.

### `hero:get_running_sound()`

Returns the name of the sound played when the hero is running.

Return value (string or `nil`)
: The running sound id. `nil` means no sound.

### `hero:set_running_sound([sound_id])`

Changes the sound to play when the hero is running.

By default, this sound is `"running"` for historical reasons.

You can use this function if you want another sound to be played.

`sound_id` (string or `nil`, optional)
: The running sound id. `nil` or an empty string means no sound.

### `hero:get_running_obstacle_sound()`

Returns the name of the sound played when the hero is colliding with an obstacle while running.

Return value (string or `nil`)
: The running_obstacle sound id. `nil` means no sound.

### `hero:set_running_obstacle_sound([sound_id])`

Changes the sound to play when the hero is colliding with an obstacle while running.

By default, this sound is `"running_obstacle"` for historical reasons.

You can use this function if you want another sound to be played.

`sound_id` (string or `nil`, optional)
: The running_obstacle sound id. `nil` or an empty string means no sound.

### `hero:get_spin_attack_load_sound()`

Returns the name of the sound played when the hero is loading the spin attack.

Return value (string or `nil`)
: The spin_attack_load sound id. `nil` means no sound.

### `hero:set_spin_attack_load_sound([sound_id])`

Changes the sound to play when the hero is loading the spin attack.

By default, this sound is `"sword_spin_attack_load"` for historical reasons.

You can use this function if you want another sound to be played.

`sound_id` (string or `nil`, optional)
: The spin_attack_load sound id. `nil` or an empty string means no sound.

### `hero:get_spin_attack_release_sound()`

Returns the name of the sound played when the hero is releasing the spin attack.

Return value (string or `nil`)
: The spin_attack_release sound id. `nil` means no sound.

### `hero:set_spin_attack_release_sound([sound_id])`

Changes the sound to play when the hero is releasing the spin attack.

By default, this sound is `"sword_spin_attack_release"` for historical reasons.

You can use this function if you want another sound to be played.

`sound_id` (string or `nil`, optional)
: The spin_attack_load sound id. `nil` or an empty string means no sound.

### `hero:get_victory_sound()`

Returns the name of the sound played when the hero is performing the victory animation.

Return value (string or `nil`)
: The victory sound id. `nil` means no sound.

### `hero:set_victory_sound([sound_id])`

Changes the sound to play when the hero is performing the victory animation.

By default, this sound is `"victory"` for historical reasons.

You can use this function if you want another sound to be played.

`sound_id` (string or `nil`, optional)
: The victory sound id. `nil` or an empty string means no sound.

## Deprecated methods of Hero

### `hero:get_sword_sound_id()`

Returns the name of the sound played when the hero uses the sword.

Return value (string or `nil`)
: The sound id of the hero's sword. `nil` means no sound.

!!! warning "Deprecated"

    This method is deprecated since Solarus 2.0. Use hero:get_sword_sound instead.

### `hero:set_sword_sound_id([sound_id])`

Changes the sound to play when the hero uses the sword.

By default, the sound used for the sword is `"swordX"`,where X is the [sword level](#heroget_abilityability_name), or no sound if the sword level is `0`.

You can use this function if you want another sound to be played.

`sound_id` (string or `nil`, optional)
: The sound id of the hero's sword. `nil` or an empty string means no sword sound.

!!! warning "Deprecated"

    This method is deprecated since Solarus 2.0. Use hero:set_sword_sound instead.

## Events Inherited from `entity`

Events are callback methods automatically called by the engine if you define them.

The hero is a particular [map entity](./index.md). Therefore, he inherits all events from the type map entity.

See [entity](./index.md#events-of-all-entity-types) to know these events.

## Events of the type hero

The following events are specific to the hero.

Recall that the hero persists when the player goes to another map, and so do the events defined on the hero.

### `hero:on_state_changing(state_name, next_state_name)`

Called when the state of the hero is about to change.

`state_name` (string)
: Name of the current built-in state. See [`hero:get_state()`](#heroget_state) for the list of possible built-in states.

`next_state_name` (string)
: Name of the built-in state about to start. See [`hero:get_state()`](#heroget_state) for the list of possible built-in states.

### `hero:on_state_changed(new_state_name)`

Called when the state of the hero has just changed.

`new_state_name` (string)
: Name of the new state. See [`hero:get_state()`](#heroget_state) for the list of possible state names.

!!! note "Note"

    This event is called even for the initial state of the hero, right after [`game:on_started()`](../game.md#gameon_started). This initial state is always `"free"`

### `hero:on_taking_damage(damage)`

Called when the hero is hurt and should take damages.

This happens usually after a collision with an [enemy](./enemy.md) or when you call [`hero:start_hurt()`](#herostart_hurtsource_x-source_y-damage-herostart_hurtsource_entity-source_sprite-damage).

This event allows you to override what happens when the hero takes damage. By default, if you don't define this event, the hero loses some life as follows. The life lost is the damage inflicted by the attacker divided by the [tunic level](../game.md#gameset_abilityability_name-level) of the player, with a minimum of `1` (unless the initial damage was already `0`).

You can define this event if you need to change how the hero takes damage, for example if you want the [shield level](../game.md#gameget_abilityability_name) to give better resistance to injuries.

`damage` (number)
: Damage inflicted by the attacker, no matter if this was an enemy or a call to [`hero:start_hurt()`](#herostart_hurtsource_x-source_y-damage-herostart_hurtsource_entity-source_sprite-damage).

### `hero:on_attacking_hero(other_hero, our_sprite)`

Called when this hero is attacking another hero.

`other_hero` ([hero](#hero))
: The hero this hero is attacking

`our_sprite` ([sprite](../drawable-objects/sprite.md))
: The sprite of this hero that attacked

This can return `true` without doing anything to tell the engine to avoid friendly fire.
