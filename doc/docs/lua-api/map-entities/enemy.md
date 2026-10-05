# Enemy

## Overview

An enemy is a bad guy that hurts the [hero](./hero.md) when touching him.

This type of [map entity](./index.md) can be declared in the [map data file](../map.md#map-files). It can also be created dynamically with [`map:create_enemy()`](../map.md#mapcreate_enemyproperties).

Enemies can exist in various breeds. Each breed corresponds to a model of enemy with its behavior, its [sprites](../drawable-objects/sprite.md) and its [movements](../movements/index.md).

The script file `enemies/XXXX.lua` defines the enemy breed `XXXX` This script is executed every time an enemy of that model is created. The corresponding Lua enemy object is passed as parameter of that script. Use the Lua notation `...` to get this parameter and store it into a regular variable.

Here is a basic example of script for the enemy breed `"tentacle"`, a simple model of enemy that just follows the hero.

```lua
-- First, we put the parameter into a variable called "enemy".
-- (In Lua, the notation "..." refers to the parameter(s) of the script.)
local enemy = ...

-- Called when the enemy was just created on the map.
function enemy:on_created()

  -- Define the properties of this enemy.
  self:set_life(1)
  self:set_damage(2)
  self:create_sprite("enemies/tentacle")
  self:set_size(16, 16)
  self:set_origin(8, 13)
end

-- Called when the enemy should start or restart its movement
-- (for example if the enemy has just been created or was just hurt).
function enemy:on_restarted()

  -- Create a movement that walks toward the hero.
  local m = sol.movement.create("target")
  m:set_speed(32)
  m:start(self)
end
```

Such a script is all what you need to define a model of enemy. The engine handles for you the detection of collisions with the [hero](./hero.md) or his weapons, hurts the hero or the enemy when appropriate, removes the enemy when it gets killed, etc. But you can customize everything using the API described on this page, like what kind of attacks can hurt the enemy. You can also make complex enemies composed of several sprites, and set different behavior to each sprite. This is very useful to program a boss.

[Timers](../timers.md) are handy to script some repeated behavior on enemies, like performing a particular attack every 5 seconds. Create your timers from the [`enemy:on_restarted()`](#enemyon_restarted) event. Timers of an enemy are automatically destroyed when the enemy is hurt or immobilized, so that they don't get triggered during these special states. They are also destroyed when you call [`enemy:restart()`](#enemyrestart). When [`enemy:on_restarted()`](#enemyon_restarted) is called, you are guaranteed that no timers exist on your enemy. Thus, it is safe to create them there.

Basic enemies often have the same behavior. To avoid duplication of code, you can factorize some code into a generic file and call it from each enemy breed script with `require()`,[`sol.main.do_file()`](../general-features.md#solmaindo_filescript_name) or [`sol.main.load_file()`](../general-features.md#solmainload_filescript_name).

## Methods Inherited from `entity`

Enemies are particular [map entities](./index.md). Therefore, they inherit [all methods](./index.md#methods-of-all-entity-types) from the type map entity.

## Methods of the type enemy

The following methods are specific to enemies.

### `enemy:get_breed()`

Returns the breed (the model) of this enemy.

Return value (string)
: The enemy's breed.

### `enemy:get_life()`

Returns the current life of this enemy.

Return value (number)
: Number of health points of the enemy.

### `enemy:set_life(life)`

Sets the current life of this enemy.

The initial value is `1`.

`life` (number)
: Number of health points to set. A value of `0` or lower kills the enemy.

### `enemy:add_life(life)`

Adds some life to the enemy.

`life` (number)
: Number of health points to add.

!!! note "Note"

    Equivalent to `enemy:set_life(enemy:get_life() + life)`.

### `enemy:remove_life(life)`

Removes some life from the enemy.

`life` (number)
: Number of health points to remove.

!!! note "Note"

    Equivalent to `enemy:set_life(enemy:get_life() - life)`.

### `enemy:get_damage()`

Returns the number of life points that the enemy removes from the [hero](./hero.md) when touching him. This number will be divided by the level of resistance ability of the player (his tunic).

Return value (number)
: Damage inflicted to the hero.

### `enemy:set_damage(damage)`

Sets the number of life points that the enemy removes from the [hero](./hero.md) when touching him. This number will be divided by the [tunic](../game.md#gameget_abilityability_name) level of the player, unless you override this default calculation in [`hero:on_taking_damage()`](./hero.md#heroon_taking_damagedamage).

The default value is `1`.

`damage` (number)
: Damage inflicted to the hero.

### `enemy:is_pushed_back_when_hurt()`

Returns whether the enemy is pushed away when it is hurt.

Return value (boolean)
: `true` if the enemy is pushed away when hurt.

### `enemy:set_pushed_back_when_hurt([pushed_back_when_hurt])`

Sets whether the enemy should be pushed away when it is hurt. The default value is `true`.

`pushed_back_when_hurt` (boolean, optional, default: `true`)
: `true` to make the enemy pushed away when hurt.

### `enemy:get_push_hero_on_sword()`

Returns whether the [hero](./hero.md) is pushed away when he hits this enemy with his sword.

Return value (boolean)
: `true` if the hero is pushed away when hitting this enemy with his sword.

### `enemy:set_push_hero_on_sword([push_hero_on_sword])`

Sets whether the [hero](./hero.md) should be pushed away when he hits this enemy with his sword.

The default value is `false`.

`push_hero_on_sword` (boolean, optional, default: `true`)
: `true` to push the hero away when hitting this enemy with his sword.

### `enemy:get_can_hurt_hero_running()`

Returns whether this enemy can hurt the [hero](./hero.md) even when the hero is running.

Return value (boolean)
: `true` if the hero can be hurt by this enemy even when running.

### `enemy:set_can_hurt_hero_running([can_hurt_hero_running])`

Sets whether this enemy can hurt the [hero](./hero.md) even when the hero is running.

The default value is `false`.

`can_hurt_hero_running` (boolean, optional, default: `true`)
: `true` so that the hero can be hurt by this enemy even when running.

### `enemy:get_hurt_style()`

Returns the style of sounds and animations to play when this enemy is hurt.

Return value (string)
: Possible values are:

    - `"normal"`
    - `"monster"`
    - `"boss"`

### `enemy:set_hurt_style(hurt_style)`

Sets the style of sounds and animations to play when this enemy is hurt. The default values is `"normal"`.

`hurt_style` (string)
: Possible values are:

    - `"normal"`
    - `"monster"`
    - `"boss"`

### `enemy:get_dying_sprite_id()`

Returns the id of the sprite to show when this enemy is dying.

This sprite is displayed during the usual dying animation. It is not used when the dying animation is a special one, for example if the [hurt style](#enemyget_hurt_style) is `"boss"` (in which case explosions are displayed instead) or if the enemy was killed by bad ground like water or holes.

The default dying sprite id is `"enemies/enemy_killed"`.

Return value (string or `nil`)
: Id of the sprite to use for the dying animation, or `nil` to show no dying animation.

### `enemy:set_dying_sprite_id(dying_sprite_id)`

Sets the id of the sprite to show when this enemy is dying.

See [`enemy:get_dying_sprite_id()`](#enemyget_dying_sprite_id) for more details.

`dying_sprite_id` (string or `nil`)
: Id of the sprite to use for the dying animation, or `nil` to show no dying animation.

### `enemy:get_can_attack()`

Returns whether this enemy can currently attack the [hero](./hero.md).

Return value (boolean)
: `true` if the enemy can currently attack the hero.

### `enemy:set_can_attack([can_attack])`

Sets whether this enemy can currently attack the [hero](./hero.md).

When the enemy restarts after being hurt, `can_attack` is always set to `true`.

`can_attack` (boolean, optional, default: `true`)
: `true` to allow the enemy to attack the hero..

### `enemy:get_minimum_shield_needed()`

Returns the level of protection (if any) that stops attacks from this enemy.

If the player has a protection [ability](../game.md#gameget_abilityability_name) greater than or equal to this value, he will stop attacks from this enemy if he is facing the direction of the enemy. The special value of `0` means that attacks cannot be stopped with the protection ability. Returns the required level of protection to stop attacks from this enemy.

Return value (number)
: The level of protection that stops attacks from this enemy. A value of `0` means that the hero cannot stop the attacks.

### `enemy:set_minimum_shield_needed(minimum_shield_needed)`

Sets a level of protection that stops attacks from this enemy.

If the player has a protection [ability](../game.md#gameget_abilityability_name) greater than or equal to this value, he will stop attacks from this enemy if he is facing the direction of the enemy. The special value of `0` means that attacks cannot be stopped with the protection ability. The default value is `0`.

`minimum_shield_needed` (number)
: The level of protection that stops attacks from this enemy. A value of `0` means that the hero cannot stop the attacks.

### `enemy:is_traversable()`

Returns whether this enemy can be traversed by other entities.

Return value (boolean)
: `true` if this enemy is traversable.

### `enemy:set_traversable([traversable])`

Sets whether this enemy can be traversed by other entities.

By default, the enemy is traversable. For example, if you want to prevent the [hero](./hero.md) to pass without killing the enemy, you can use this function to make the enemy become an obstacle.

`traversable` (boolean, optional, default: `true`)
: `true` to make this enemy traversable.

!!! note "Note"

    When the enemy is not traversable, the enemy can no longer hurt the hero since their sprites cannot overlap anymore (unless if the enemy's sprite is bigger than its bounding box). You can use [`enemy:set_attacking_collision_mode("touching")`](#enemyset_attacking_collision_modecollision_mode) to make the enemy still hurt the hero when touching him without overlapping.

### `enemy:get_attacking_collision_mode()`

Returns the kind of collision test performed to detect when the hero should be hurt by this enemy.

Return value (string)
: A collision mode name. See [`enemy:set_attacking_collision_mode()`](#enemyset_attacking_collision_modecollision_mode) for the possible values.

### `enemy:set_attacking_collision_mode(collision_mode)`

Sets the kind of collision test performed to detect when the hero should be hurt by this enemy.

collision_mode (string)
: A collision mode name. This may be one of:

    - `"overlapping"`: Collision if the [bounding box](./index.md#entityget_bounding_box) of the enemy and the hero overlap.
    - `"containing"`: Collision if the bounding box of the hero is fully inside the bounding box of this enemy.
    - `"origin"`: Collision if the [origin point](./index.md#entityget_origin) or the hero is inside the bounding box of this enemy.
    - `"center"`: Collision if the [center point](./index.md#entityget_center_position) of the hero is inside the bounding box of this enemy.
    - `"facing"`: Collision if the [facing position](./index.md#entityget_facing_position) of the hero's bounding box is touching this enemy's bounding box. Bounding boxes don't necessarily overlap, but they are in contact: there is no space between them. When you consider the bounding box of the hero, which is a rectangle with four sides, the facing point is the middle point of the side the hero is oriented to.
    - `"touching"`: Like `"facing"`, but accepts all four sides of the hero's bounding box, no matter its direction. This `"touching"` collision test is useful when the hero cannot traverse your enemy (see [`enemy:set_traversable()`](#enemyset_traversabletraversable)).
    - `"sprite"`: Collision if the tunic sprite of the hero overlaps a sprite of this enemy. The collision test is pixel precise. This is the default value.

### `enemy:get_attack_consequence(attack)`

Returns how this enemy reacts when he receives an attack.

Recall that enemies may have several [sprites](../drawable-objects/sprite.md). This attack consequence applies to all sprites of the enemy, unless you override some of them with [`enemy:set_attack_consequence_sprite()`](#enemyset_attack_consequence_spritesprite-attack-consequence).

`attack` (string)
: Name of an attack against the enemy:

    - `"sword"`
    - `"thrown_item"`
    - `"explosion"`
    - `"arrow"`
    - `"hookshot"`
    - `"boomerang"`
    - `"fire"`

`consequence` (number or string or function)
: Indicates what happens when this enemy receives the attack. The possible values are the same as in [`enemy:set_attack_consequence()`](#enemyset_attack_consequenceattack-consequence).

### `enemy:set_attack_consequence(attack, consequence)`

Sets how this enemy reacts when he receives an attack.

Recall that enemies may have several [sprites](../drawable-objects/sprite.md). This attack consequence applies to all sprites of the enemy, unless you override some of them with [`enemy:set_attack_consequence_sprite()`](#enemyset_attack_consequence_spritesprite-attack-consequence).

`attack` (string)
: Name of an attack against the enemy:

    - `"sword"`
    - `"thrown_item"`
    - `"explosion"`
    - `"arrow"`
    - `"hookshot"`
    - `"boomerang"`
    - `"fire"`

`consequence` (number or string or function)
: Indicates what happens when this enemy receives the attack. It may be:

    - A positive integer: The enemy is hurt and loses this number of life points. In the particular case of a sword attack, this number will by default be increased by the level of the sword (see [`enemy:on_hurt_by_sword()`](#enemyon_hurt_by_swordhero-enemy_sprite)).
    - `"ignored"`: Nothing happens. The weapon (if any) traverses the enemy.
    - `"protected"`: The enemy stops the attack. An attack failure sound is played.
    - `"immobilized"`: The enemy is immobilized for a few seconds.
    - `"custom"`: Event [`enemy:on_custom_attack_received()`](#enemyon_custom_attack_receivedattack-sprite) is called. Note: Since Solarus 1.6, we recommend to use a function parameter instead for more simplicity (see below).
    - A function to be called when this enemy receives the attack.

### `enemy:get_attack_consequence_sprite(sprite, attack)`

Returns how this enemy reacts when one of his [sprites](../drawable-objects/sprite.md) receives an attack.

This method returns the same result as [`enemy:get_attack_consequence()`](#enemyget_attack_consequenceattack), unless you override the reaction of the enemy for a particular sprite with [`enemy:set_attack_consequence_sprite()`](#enemyset_attack_consequence_spritesprite-attack-consequence).

`sprite` ([sprite](../drawable-objects/sprite.md))
: A sprite of this enemy.

`attack` (string)
: Name of an attack against the enemy:

    - `"sword"`
    - `"thrown_item"`
    - `"explosion"`
    - `"arrow"`
    - `"hookshot"`
    - `"boomerang"`
    - `"fire"`

`consequence` (number or string or function)
: Indicates what happens when this sprite receives the attack. The possible values are the same as in [`enemy:set_attack_consequence()`](#enemyset_attack_consequenceattack-consequence).

### `enemy:set_attack_consequence_sprite(sprite, attack, consequence)`

Sets how this enemy reacts when one of his [sprites](../drawable-objects/sprite.md) receives an attack.

This method overrides for a particular sprite the attack consequences defined by [`enemy:set_attack_consequence()`](#enemyset_attack_consequenceattack-consequence).

`sprite` ([sprite](../drawable-objects/sprite.md))
: A sprite of this enemy.

`attack` (string)
: Name of an attack against the enemy:

    - `"sword"`
    - `"thrown_item"`
    - `"explosion"`
    - `"arrow"`
    - `"hookshot"`
    - `"boomerang"`
    - `"fire"`

`consequence` (number or string or function)
: Indicates what happens when this sprite receives the attack. The possible values are the same as in [`enemy:set_attack_consequence()`](#enemyset_attack_consequenceattack-consequence).

### `enemy:set_default_attack_consequences()`

Restores the default attack consequences for this enemy and its sprites.

### `enemy:set_default_attack_consequences_sprite(sprite)`

Restores the default attack consequences for a particular sprite of this enemy.

`sprite` ([sprite](../drawable-objects/sprite.md))
: A sprite of this enemy.

### `enemy:get_savegame_variable()`

Returns the name of the savegame variable where the state of this enemy is stored.

Return value (string or `nil`)
: the name of the variable (or `nil` if the enemy is not saved).

### `enemy:set_invincible()`

Makes this enemy ignore all attacks.

Equivalent to calling [`enemy:set_attack_consequence(attack, "ignored")`](#enemyset_attack_consequenceattack-consequence) for each attack.

### `enemy:set_invincible_sprite(sprite)`

Makes a sprite of this enemy ignore all attacks.

Equivalent to calling [`enemy:set_attack_consequence(sprite, attack, "ignored")`](#enemyset_attack_consequence_spritesprite-attack-consequence) for each attack.

`sprite` ([sprite](../drawable-objects/sprite.md))
: A sprite of this enemy.

### `enemy:has_layer_independent_collisions()`

Returns whether this enemy can detect collisions with entities even if they are not on the same layer.

By default, enemies can only have collisions with entities on the same layer.

Return value (boolean)
: `true` if this enemy can detect collisions even with entities on other layers.

### `enemy:set_layer_independent_collisions([independent])`

Sets whether this enemy can detect collisions with entities even if they are not on the same layer.

By default, enemies can only have collisions with entities on the same layer. If you set this property to `true`, this enemy will be able to hurt the [hero](./hero.md) even from a different layer.

`independent` (boolean, optional, default: `true`)
: `true` to make this enemy detect collisions even with entities on other layers.

### `enemy:get_treasure()`

Returns the [pickable treasure](./pickable.md) that will drop this enemy when killed.

Return value 1 (string or `nil`)
: Name of an [equipment item](../equipment-items.md). `nil` means no item dropped (in this case, other return values are `nil` too).

Return value 2 (number)
: Variant of this equipment item (`1` means the first variant).

Return value 3 (string or `nil`)
: Name of the boolean value that stores in the [savegame](../game.md) whether the treasure dropped is found. `nil` means that the treasure is not saved.

### `enemy:set_treasure([item_name, [variant, [savegame_variable]]])`

Sets the [pickable treasure](./pickable.md) that will drop this enemy when killed.

`item_name` (string or `nil`, optional)
: Name of an [equipment item](../equipment-items.md). `nil` or no value means no item.

`variant` (number, optional, requires: `item_name`, default: `1`)
: Variant of this equipment item (`1` means the first variant).

`savegame_variable` (string or `nil`, optional, requires: `variant`)
: Name of the boolean value that stores in the [savegame](../game.md) whether the treasure dropped is found. `nil` or no value means that the treasure is not saved.

### `enemy:get_obstacle_behavior()`

Returns how the enemy behaves with obstacles.

Return value (string)
: Can be one of:

    - `"normal"`
    - `"flying"`
    - `"swimming"`

### `enemy:set_obstacle_behavior(obstacle_behavior)`

Sets how this enemy should behave with obstacles. The default value is `"normal"` `"swimming"` allow the enemy to traverse water. `"flying"` allows the enemy to traverse holes, water and lava.

`obstacle_behavior` (string)
: Possible values are:

    - `"normal"`
    - `"flying"`
    - `"swimming"`

### `enemy:get_immobilization_duration()`

Returns the immobilization duration for this enemy.

Return value (integer)
: the duration in milliseconds. Default value is `5000` (5 seconds).

### `enemy:set_immobilization_duration(duration)`

Sets the immobilization duration for this enemy.

`duration` (integer)
: the duration in milliseconds.

### `enemy:get_attack_failure_sound()`

Returns the sound id when an attack against the enemy fails.

Return value (string)
: The sound id.

### `enemy:set_attack_failure_sound([sound_id])`

Changes the sound to play when an attack against the enemy fails. By default, the sound used is `"sword_tapping"`. You can use this function if you want another sound to be played.

`sound_id` (string or `nil`, optional)
: The sound id. An empty string or `nil` means no sound.

### `enemy:get_falling_sound()`

Returns the sound id when the enemy is falling into a hole.

Return value (string)
: The sound id.

### `enemy:set_falling_sound([sound_id])`

Changes the sound to play when the enemy is falling into a hole. By default, the sound used is `"jump"`. You can use this function if you want another sound to be played.

`sound_id` (string or `nil`, optional)
: The sound id. An empty string or `nil` means no sound.

### `enemy:get_sinking_sound()`

Returns the sound id when the enemy is sinking into deep water or lava.

Return value (string)
: The sound id.

### `enemy:set_sinking_sound([sound_id])`

Changes the sound to play when the enemy is sinking into deep water or lava. By default, the sound used is `"splash"`. You can use this function if you want another sound to be played.

`sound_id` (string or `nil`, optional)
: The sound id. An empty string or `nil` means no sound.

### `enemy:get_dying_sound()`

Returns the sound id when the enemy is dying.

Return value (string)
: The sound id.

### `enemy:set_dying_sound([sound_id])`

Changes the sound to play when the enemy is dying. By default, the sound used is `"enemy_killed"`. You can use this function if you want another sound to be played.

`sound_id` (string or `nil`, optional)
: The sound id. An empty string or `nil` means no sound.

### `enemy:get_exploding_sound()`

Returns the sound id when the enemy is exploding.

Return value (string)
: The sound id.

### `enemy:set_exploding_sound([sound_id])`

Changes the sound to play when the enemy is exploding. By default, the sound used is `"explosion"`. You can use this function if you want another sound to be played.

`sound_id` (string or `nil`, optional)
: The sound id. An empty string or `nil` means no sound.

### `enemy:get_hurt_sound()`

Returns the sound id when the enemy is hurt.

Return value (string)
: The sound id.

### `enemy:set_hurt_sound([sound_id])`

Changes the sound to play when the enemy is hurt. You can use this function if you want another sound to be played. Calling this function disable sounds related to `hurt_style`.

`sound_id` (string or `nil`, optional)
: The sound id. An empty string or `nil` means no sound.

### `enemy:restart()`

Restarts this enemy.

This plays animation `"walking"` on its [sprites](../drawable-objects/sprite.md), destroys any timer of the enemy and calls the event [`enemy:on_restarted()`](#enemyon_restarted).

This function has no effect if the enemy is dying.

### `enemy:hurt(life_points)`

Hurts this enemy if he is currently vulnerable.

If the enemy is vulnerable, the hurting animation and sound start and the given number of life points are removed.

Nothing happens if the enemy is currently invulnerable (for example because he is already being hurt).

`life_points` (number)
: Number of life points to remove from the enemy.

!!! note "Note"

    If you just want to silently remove some life, call [`enemy:remove_life()`](#enemyremove_lifelife) instead.

### `enemy:is_immobilized()`

Returns whether the enemy is currently immobilized.

Return value (boolean)
: `true` if the enemy is immobilized.

### `enemy:immobilize()`

Immobilizes this enemy for a while if possible.

After a few seconds, the enemy shakes and then restarts.

### `enemy:create_enemy(properties)`

Creates another enemy on the [map](../map.md), specifying its coordinates as relative to the current enemy.

This function is similar to [`map:create_enemy()`](../map.md#mapcreate_enemyproperties) but the coordinates are relative to the current enemy, and the layer is the one of the current enemy by default.

`properties` (table)
: A table that describes all properties of the enemy to create. Its key-value pairs must be:

    `name` (string, optional)
    : Name identifying the entity. If the name is already used by another entity, a suffix (of the form `"_2"`, `"_3"`, etc.) will be automatically appended to keep entity names unique. No value means no name.

    `layer` (number, optional)
    : The layer, between [`map:get_min_layer()`](../map.md#mapget_min_layer) and [`map:get_max_layer()`](../map.md#mapget_max_layer). No value means the same layer as the current enemy.

    `x` (number, optional, default: `0`)
    : X coordinate on the map, relative to the current enemy.

    `y` (number, optional, default: `0`)
    : Y coordinate on the map, relative to the current enemy.

    `direction` (number, optional, default: `3`)
    : Initial direction of the enemy, between `0` (East) and `3` (South).

    `breed` (string)
    : Model of enemy to create.

    `savegame_variable` (string, optional)
    : Name of the boolean value that stores in the [savegame](../game.md) whether this enemy is dead. No value means that the enemy is not saved. If the enemy is saved and was already killed, then no enemy is created. Instead, its [pickable treasure](./pickable.md) is created if it is a saved one.

    `treasure_name` (string, optional)
    : Kind of [pickable treasure](./pickable.md) to drop when the enemy is killed (the name of an [equipment item](../equipment-items.md)). If this value is not set, or corresponds to a [non obtainable](../equipment-items.md#itemis_obtainable) item, then the enemy won't drop anything.

    `treasure_variant` (number, optional, default: `1`)
    : Variant of the treasure (because some [equipment items](../equipment-items.md) may have several variants). The default value is the first variant.

    `treasure_savegame_variable` (string, optional)
    : Name of the boolean value that stores in the [savegame](../game.md) whether the [pickable treasure](./pickable.md) of this enemy was obtained. No value means that the state of the treasure is not saved. If the treasure is saved and the player already has it, then the enemy won't drop anything.

Return value (enemy or [pickable treasure](./pickable.md) or `nil`)
: The enemy created, except when it is a saved enemy that is already dead. In this case, if the enemy dropped a saved treasure that is not obtained yet, this [pickable treasure](./pickable.md) is created and returned. Otherwise, `nil` is returned.

## Events Inherited from `entity`

Events are callback methods automatically called by the engine if you define them.

Enemies are particular [map entities](./index.md). Therefore, they inherit all events from the type map entity.

See [](./index.md#events-of-all-entity-types) to know these events.

## Events of the type enemy

The following events are specific to enemies.

### `enemy:on_update()`

Called at each cycle while this enemy is alive.

!!! note "Note"

    As this function is called at each cycle, it is recommended to use other solutions when possible, like [timers](../timers.md) and other events.

### `enemy:on_restarted()`

Called when this enemy should start or restart its [movement](../movements/index.md) and [timers](../timers.md) because something happened. For example, the enemy has just been created, or it was just hurt or immobilized, or you called [`enemy:restart()`](#enemyrestart). If your enemy should move, this is the right place to create its movement.

[Timers](../timers.md) associated to the enemy were automatically destroyed. Thus, you should also recreate them from this event.

### `enemy:on_collision_enemy(other_enemy, other_sprite, my_sprite)`

Called when a [sprite](../drawable-objects/sprite.md) of this enemy overlaps another enemy's sprite.

`other_enemy` (enemy)
: Another enemy.

`other_sprite` ([sprite](../drawable-objects/sprite.md))
: A sprite of that other enemy.

`my_sprite` ([sprite](../drawable-objects/sprite.md))
: A sprite of the current enemy.

### `enemy:on_custom_attack_received(attack, sprite)`

Called when this enemy receives an attack with a custom effect.

This function is called if you have set [consequence of the attack](#enemyget_attack_consequenceattack) to `"custom"` You have to define what happens, for example hurting the enemy, making a special reaction, etc.

`attack` (string)
: The attack that was received:

    - `"sword"`
    - `"thrown_item"`
    - `"explosion"`
    - `"arrow"`
    - `"hookshot"`
    - `"boomerang"`
    - `"fire"`

`sprite` ([sprite](../drawable-objects/sprite.md) or `nil`)
: The sprite of this enemy that receives the attack, or `nil` if the attack does not come from a pixel-precise collision.

!!! note "Note"

    In the particular case of a `"sword"` attack, the sword state is stopped and the hero goes back to state `"free"`.

!!! note "Note"

    Instead of using this event and the attack consequence `"custom"`, you can now simply pass a callback parameter to [`enemy:set_attack_consequence()`](#enemyset_attack_consequenceattack-consequence) or to [`enemy:set_attack_consequence_sprite()`](#enemyset_attack_consequence_spritesprite-attack-consequence).

### `enemy:on_hurt_by_sword(hero, enemy_sprite)`

Called when this enemy is successfully hurt by the sword of the hero.

You should define this event to customize the damage inflicted by the sword.

This event can only be called if the [reaction](#enemyget_attack_consequenceattack) to the `"sword"` attack is hurting the enemy.

At this point, the enemy is in the state of being hurt. His hurting animation and sound have just started. This is a good time to remove some life points with [`enemy:remove_life()`](#enemyremove_lifelife).

By default, if you don't define this event, the enemy loses a number of life points computed as [his reaction to sword attacks](#enemyget_attack_consequenceattack) multiplied by the sword [ability level](../game.md#gameget_abilityability_name) of the hero, and doubled during a spin attack.

`hero` ([hero](./hero.md))
: The hero who used the sword.

`enemy_sprite` ([sprite](../drawable-objects/sprite.md))
: The sprite of this enemy that was hit. You may use this information if your enemy has several sprites with different behaviors.

### `enemy:on_hurt(attack)`

Called when this enemy is successfully hurt by any attack.

This event can only be called if the [reaction](#enemyget_attack_consequenceattack) to the attack is hurting the enemy.

At this point, the enemy is in the state of being hurt. His hurting animation and sound have just started and he has just lost some life.

`attack` (string)
: The attack that was received:

    - `"sword"`
    - `"thrown_item"`
    - `"explosion"`
    - `"arrow"`
    - `"hookshot"`
    - `"boomerang"`
    - `"fire"`
    - `"script"`

!!! note "Note"

    In the case of a `"sword"` attack, this event is called right after [`enemy:on_hurt_by_sword()`](#enemyon_hurt_by_swordhero-enemy_sprite).

### `enemy:on_dying()`

Called when the enemy's life comes to `0`.

When the life comes to `0`, the movement of the enemy is stopped, its timers are stopped too, the dying animation starts and a sound is played. The details of the dying animation and the sound played depend on the [hurt style](#enemyset_hurt_stylehurt_style) property.

- If the hurt style is `"enemy"` or `"monster"`, any sprite of the enemy is automatically removed and replaced by the sprite indicated by [`enemy:get_dying_sprite_id()`](#enemyget_dying_sprite_id) if any (which is by default `"enemies/enemy_killed"`)
- If the hurt style is `"boss"`, your sprites continue to exist and to play animation "hurt", while explosions appear on the enemy.

In all cases, the enemy will be removed from the map when the dying animation ends.

!!! note "Note"

    This event is called right after [`enemy:on_hurt()`](#enemyon_hurtattack).

### `enemy:on_dead()`

Called when the enemy's dying animation is finished.

At this point, the enemy no longer exists on the map. In other words, [`enemy:exists()`](./index.md#entityexists) returns `false`,trying to get the enemy from its name returns `nil`,and functions like [`map:get_entities(prefix)`](../map.md#mapget_entities_countprefix) won't find this enemy.

This means that you can safely use [`map:has_entities(prefix)`](../map.md#maphas_entitiesprefix) from `enemy:on_dead()` to detect when all enemies with a common prefix are dead.

### `enemy:on_immobilized()`

Called when the enemy is immobilized.

### `enemy:on_attacking_hero(hero, enemy_sprite)`

Called when the [hero](./hero.md) is successfully touched by this enemy.

This event is not called if the hero was protected by his shield, or if he currently cannot be hurt for some reason, like when he is already being hurt, when he is [temporarily invincible](./hero.md#heroset_invincibleinvincible-duration), or when he is in a special [state](./hero.md#heroget_state) like brandishing a treasure.

Your script can define this event to customize what bad things happen to the hero. If you define this event, the engine does absolutely nothing and lets you handle this.

If you don't define this event, the hero is hurt with the predefined behavior as follows. The hero goes to the state `"hurt"` where is pushed away from the enemy. He loses some life depending on the enemy's [damage](#enemyget_damage) property, and on the hero's [tunic](../game.md#gameget_abilityability_name) and on [`hero:on_taking_damage()`](./hero.md#heroon_taking_damagedamage) if defined. Then, he recovers and his sprites blink for a while. During this short period, he is temporarily invincible.

`hero` ([hero](./hero.md))
: The hero being attacked.

`enemy_sprite` ([sprite](../drawable-objects/sprite.md) or `nil`)
: The sprite of the enemy that caused the collision with the hero. You may use this information if your enemy has several sprites with different behaviors. This value is `nil` if the attack does not come from a sprite collision test (see [`enemy:set_attacking_collision_mode()`](#enemyset_attacking_collision_modecollision_mode)).

!!! note "Note"

    If you call [`hero:start_hurt()`](./hero.md#herostart_hurtsource_x-source_y-damage-herostart_hurtsource_entity-source_sprite-damage), you will obtain something equivalent to the default behavior. But if you don't, keep in mind that if the hero can still be hurt after your call, this event will continue to be called while there is a collision with the enemy. To avoid this, see for example [`hero:set_invincible()`](./hero.md#heroset_invincibleinvincible-duration) to make the hero temporarily invincible.
