# Custom States

## Overview

This module provides a datatype `state` that represents a custom state allowing advanced customization of the [hero](./map-entities/hero.md).

Custom states can be used to precisely control everything that can happen to the [hero](./map-entities/hero.md) in particular situations.

The engine provides a number of built-in states like `"free"`, `"frozen"`, `"hurt"`, `"swimming"`, `"pushing"` and more (see [`hero:get_state()`](./map-entities/hero.md#heroget_state) for the whole list). In every state, the hero can or cannot do specific actions (like moving), and can react or ignore other entities.

Most of the time, you don't need custom states. Custom states are for advanced users who want more control. Remember that in any built-in state, you can already customize the essential features like sprites and movements. More often that not, the `"frozen"` built-in state is enough to make a lot of things, like cutscenes.

Sometimes however, you may want more advanced customization. For instance, if the built-in `"swimming"` state does not fit your needs, you can make a custom state instead with your own swimming implementation. Or sometimes, you just want a completely new behavior that is not covered by any existing built-in state.

### How to implement a custom state

The [hero](./map-entities/hero.md) always has only one current state. It can either be a built-in state or a custom state.

To make a custom state, first create a custom state object with [`sol.state.create()`](#solstatecreatedescription). Then, use some of the many methods and events documented below, and finally start your state on the [hero](./map-entities/hero.md) with [`hero:start_state(state)`](./map-entities/hero.md#herostart_statestate).

After this operation, the current state string of the hero (as returned by [`hero:get_state()`](./map-entities/hero.md#heroget_state)) is `"custom"`, and no longer a usual built-in state like `"free"`, `"frozen"`, `"swimming"`, etc. You can then use [`hero:get_state_object()`](./map-entities/hero.md#heroget_state_object) to retrieve the actual custom state object that you created.

When your state finishes, because you or the engine started another state (built-in or custom), it is your responsibility to clean things that you may have done on objects other than the state. For instance, if you [added a sprite](./map-entities/index.md#entitycreate_spriteanimation_set_id-sprite_name) to the hero for your custom state, maybe you want to remove it when the state finishes. The event [`state:on_finished()`](#stateon_finishednext_state_name-next_state) is the appropriate place to do so, because it is called no matter who is changing the state.

### Example

This short example is a state that allows the [hero](./map-entities/hero.md) to move, but keeps his sprites direction fixed.

```lua
 -- Create a custom state object.
local state = sol.state.create()

 -- Set up its properties.
state:set_can_control_movement(true)
state:set_can_control_direction(false)

-- Associate it to the hero.
hero:start_state(state)
```

There are other ways to implement such a feature, but custom states make this easier.

## Functions of `sol.state`

### `sol.state.create([description])`

Creates a custom state but does not start it yet.

`description` (string, optional)
: An optional description of your state. The engine does nothing special with this description, but it may help you distinguish states.

Return value (state)
: The custom state created.

## Methods of the type `state`

### `state:get_description()`

Returns the description of this state.

Return value (string or `nil`)
: The description, or `nil` if no description was set.

### `state:set_description(description)`

Sets the description of this state.

The engine does nothing special with this description, but it may help you distinguish states.

`description` (string or `nil`)
: The description to set, or `nil` to set no description.

### `state:get_entity()`

Returns the [entity](./map-entities/index.md) controlled by this state.

Return value ([entity](./map-entities/index.md) or `nil`)
: The entity controlled by this state (which can only by the [hero](./map-entities/hero.md)), or `nil` if the state is not associated to an entity yet.

!!! note "Note"

    Even if the state is finished, this method still returns the entity that was controlled.

### `state:get_map()`

Returns the [map](./map.md) of the entity controlled by this state.

Return value ([map](./map.md) or `nil`)
: The map, or `nil` if the state is not associated to an entity yet.

### `state:get_game()`

Returns the [game](./game.md) of the entity controlled by this state.

Return value ([game](./game.md) or `nil`)
: The game, or `nil` if the state is not associated to an entity yet.

### `state:is_started()`

Returns whether this state is started, that is, if it was activated on an entity (see [`hero:start_state()`](./map-entities/hero.md#herostart_statestate)) and is not finished yet.

Return value (boolean)
: `true` if the state is started.

### `state:is_visible()`

Returns whether the entity is visible during this state.

Return value (boolean)
: `true` if the entity is visible during this state, `false` if it is hidden.

### `state:set_visible(visible)`

Returns whether the entity should be visible during this state.

`visible` (boolean, default: `true`)
: `true` to make the entity visible during this state, `false` to hide it.

### `state:get_draw_override()`

Returns the draw function of this state.

See [`state:set_draw_override()`](#stateset_draw_overridedraw_override) for more details.

Return value (function or `nil`)
: The draw function, or `nil` if the draw function was not overridden.

### `state:set_draw_override(draw_override)`

Changes how this entity is drawn during this state.

You can use this to replace the built-in draw implementation of the engine by your own function, if the default behavior does not fit your needs. To do so, your function can either call [`map:draw_visual()`](./map.md#mapdraw_visualdrawable-x-y) or draw on [`camera:get_surface()`](./map-entities/camera.md#cameraget_surface).

`draw_override` (function or `nil`)
: The draw function, or `nil` to restore the built-in drawing. Your function will receive the following parameters:

    `state` (state)
    : The custom state of the entity to draw.

    `camera` ([camera](./map-entities/camera.md))
    : Camera where the entity is drawn.

!!! note "Note"

    Even when you set a draw override, events [`state:on_pre_draw()`](#stateon_pre_drawcamera) and [`state:on_post_draw()`](#stateon_post_drawcamera) are still called.

### `state:get_can_control_direction()`

Returns whether the player controls the direction of entity's sprites during this state.

Return value (boolean)
: `true` if the player controls the sprites direction.

### `state:set_can_control_direction(can_control_direction)`

Sets whether the player controls the direction of entity's sprites during this state.

If you set this to `false`, then the entity's sprites no longer automatically take the direction pressed by the player.

`can_control_direction` (boolean, default: `true`)
: `true` to let the player control the sprites direction.

### `state:get_can_control_movement()`

Returns whether the player controls the movement of the entity during this state.

Return value (boolean)
: `true` if the player controls the movement.

### `state:set_can_control_movement(can_control_movement)`

Sets whether the player controls the movement of the entity during this state.

If this setting is `true`, a movement is automatically created on the entity and this movement reacts to the player's input. If you set this to `false`, then the entity has no automatic movement during this state.

`can_control_movement` (boolean, default: `true`)
: `true` to let the player control the movement.

### `state:set_can_traverse([entity_type], traversable)`

Sets whether the entity can traverse other entities in this state.

By default, this depends on the other entities: for example, [sensors](./map-entities/sensor.md) can be traversed by default while [doors](./map-entities/door.md) cannot unless they are open.

`entity_type` (string, optional)
: A type of entity. See [`entity:get_type()`](./map-entities/index.md#entityget_type) for the possible values. If not specified, the setting will be applied to all entity types for which you don't override this setting.

`traversable` (boolean or function or `nil`)
: Whether the entity controlled by this state can traverse the other entity type. This can be:

    - A boolean: `true` to allow your entity to traverse entities of the specified type, `false` otherwise.
    - A function: Custom test. This allows you to decide dynamically. The function takes your entity (the one controlled by this state) and then the other entity as parameters, and should return `true` if you allow your entity to traverse the other entity. When your entity has a [movement](./movements/index.md), this function will be called every time it is about to overlap an entity of the specified type.
    - `nil`: Clears any previous setting for this entity type and therefore restores the default value.

### `state:get_can_traverse_ground(ground)`

Returns whether the entity can traverse the given kind of ground during this state.

`ground` (string)
: A kind of ground. See [`map:get_ground()`](./map.md#mapget_groundx-y-layer) for the possible values.

Return value (boolean)
: `true` if the ground can be traversed during this state.

### `state:set_can_traverse_ground(ground, traversable)`

Sets whether the entity can traverse the given kind of ground during this state.

By default, this depends on the the ground: for example, the `"grass"` ground can be traversed by default while the `"low` wall" ground cannot.

`ground` (string)
: A kind of ground. See [`map:get_ground()`](./map.md#mapget_groundx-y-layer) for the possible values.

`traversable` (boolean)
: Whether the entity can traverse this kind of ground during this state.

### `state:is_gravity_enabled()`

Returns whether the entity is affected by gravity during this state.

If yes, the entity will fall to the lower layer when the ground below it is `"empty"`

Return value (boolean)
: `true` if the entity is affected by gravity during this state.

### `state:set_gravity_enabled(gravity_enabled)`

Sets whether the entity is affected by gravity during this state.

If yes, the entity will fall to the lower layer when the ground below it is `"empty"`

You should typically set this to `false` when the entity is jumping of flying.

`gravity_enabled` (boolean, default: `true`)
: `true` to make the entity affected by gravity during this state.

### `state:is_affected_by_ground(ground)`

Returns whether the given kind of ground affects the entity during this state.

`ground` (string)
: A kind of ground. See [`map:get_ground()`](./map.md#mapget_groundx-y-layer) for the possible values.

Return value (boolean)
: `true` if this ground affects the entity during this state.

### `state:set_affected_by_ground(ground, affected)`

Sets whether a kind of ground affects the entity during this state.

`ground` (string)
: A kind of ground. See [`map:get_ground()`](./map.md#mapget_groundx-y-layer) for the possible values.

`affected` (boolean)
: `true` to make this ground affect the entity during this state.

### `state:get_can_come_from_bad_ground()`

Returns whether this state remembers the last solid position of the entity as a place to come back to later if it falls into bad ground like holes or lava.

Return value (boolean)
: `true` if solid positions in this state are considered as places to come back to when falling into bad grounds.

### `state:set_can_come_from_bad_ground(can_come_from_bad_ground)`

Sets whether this state remembers the last solid position of the entity as a place to come back to later if it falls into bad ground like holes or lava.

`can_come_from_bad_ground` (boolean, default: `true`)
: `true` if solid positions in this state should be considered as places to come back to when falling into bad grounds.

### `state:get_can_be_hurt()`

Returns whether the entity can be hurt during this state.

Return value (boolean)
: `true` if the entity can be hurt during this state.

!!! note "Note"

    If you passed a function to [`state:set_can_be_hurt()`](#stateset_can_be_hurtcan_be_hurt), then your function will be evaluated with a `nil` parameter.

### `state:set_can_be_hurt(can_be_hurt)`

Sets whether the entity can be hurt during this state.

`can_be_hurt` (boolean or function, default: `true`)
: Whether the entity controlled by this state can be hurt. You can pass a function if you want to decide this depending at the last moment, for example depending on the attacker. In this case, your function should accept the following parameters and return value:

    `state` (state)
    : The current state itself.

    `attacker` ([entity](./map-entities/index.md) or `nil`)
    : The attacker entity, or `nil` if the attack does not come from an entity.

    Return value (boolean)
    : `true` to allow your entity to get hurt by this attacker.

### `state:get_can_use_sword()`

Returns whether the entity can swing the sword during this state.

Return value (boolean)
: `true` if the entity can use the sword.

### `state:set_can_use_sword(can_use_sword)`

Sets whether the entity can swing the sword during this state.

`can_use_sword` (boolean, default: `true`)
: `true` to allow to use the sword.

### `state:get_can_cut()`

Returns whether the entity can cut another [entity](./map-entities/index.md) with the sword during this state.

Return value (boolean)
: `true` if the entity can cut a destructible entity during this state.

!!! note "Note"

    If you passed a function to [`state:set_can_cut()`](#stateset_can_cutcan_cut), then your function will be evaluated with a `nil` parameter.

### `state:set_can_cut(can_cut)`

Sets whether the entity can cut another [entity](./map-entities/index.md) with the sword during this state.

When the sword sprite overlaps a destructible object that [`destructible:get_can_be_cut()`](./map-entities/destructible.md#destructibleget_can_be_cut) can be cut, this setting decides if the destructible will actually be cut.

`can_cut` (boolean or function, default: `true`)
: Whether the sword can cut another entity during this state. You can pass a function if you want to decide this at the last moment, for example depending on the exact position of the entity about to be cut. In this case, your function should support the following parameters and return value:

    `state` (state)
    : The current state itself.

    `entity` ([entity](./map-entities/index.md) or `nil`)
    : The entity that would be cut, or `nil` if there is no entity about to be cut.

Return value (boolean)
: `true` to allow to cut the entity with the sword.

### `state:get_can_use_shield()`

Returns whether the entity can stop attacks with the shield during this state.

Return value (boolean)
: `true` if the entity can stop attacks with the shield.

### `state:set_can_use_shield(can_use_shield)`

Sets whether the entity can stop attacks with the shield during this state.

`can_use_shield` (boolean, default: `true`)
: `true` if the entity can stop attacks with the shield.

### `state:get_can_use_item([item_id])`

Returns whether an [equipment item](./equipment-items.md) can be used during this state.

`item_id` (string, optional)
: Name of the item to test, or leave empty to mean items in general.

Return value (boolean)
: `true` if the player can use an equipment item during this state.

### `state:set_can_use_item([item_id], can_use_item)`

Sets whether an [equipment item](./equipment-items.md) can be used during this state.

`item_id` (string or `nil`, optional)
: Name of the item to allow or disallow, or `nil` to mean items in general.

`can_use_item` (boolean, default: `true`)
: `true` to allow the player to use an equipment item during this state.

### `state:get_can_interact()`

Returns whether the entity can interact with the entities it is facing.

Return value (boolean)
: `true` if interactions are allowed in this state.

### `state:set_can_interact(can_interact)`

Sets whether the entity can interact with the entities it is facing.

If `true`, when the action game command is pressed while facing an entity that reacts to interactions (like an [NPC](./map-entities/npc.md)), then an interaction will occur.

`can_interact` (boolean, default: `true`)
: `true` to allow to interact in this state.

### `state:get_can_grab()`

Returns whether the entity can grab the obstacles it is facing.

Return value (boolean)
: `true` if grab is allowed in this state.

### `state:set_can_grab(can_grab)`

Sets whether the entity can start grabbing the obstacles it is facing.

If `true`, the entity will go to state `"grabbing"` when the action command is pressed while facing an obstacle.

The default value is `true`.

`can_grab` (boolean)
: `true` to allow to grab in this state.

!!! note "Note"

    Ability [grab](./game.md#gameget_abilityability_name) is necessary so that the hero can grab obstacles.

### `state:get_can_push()`

Returns whether the entity can try to push the obstacles it is facing.

Return value (boolean)
: `true` if pushing is allowed in this state.

### `state:set_can_push(can_push)`

Sets whether the entity can start pushing the obstacles it is facing.

If `true`, the entity will go to state `"pushing"` when it reaches an obstacle and continues to move toward this obstacle for a configurable [delay](#stateget_pushing_delay). This does not mean that the obstacle being pushed will actually move ([blocks](./map-entities/block.md) can move when being pushed, but other entities usually cannot).

`can_push` (boolean, default: `true`)
: `true` to allow to push in this state.

!!! note "Note"

    Ability [push](./game.md#gameget_abilityability_name) is necessary so that the hero can push.

### `state:get_pushing_delay()`

Returns the delay before pushing when moving towards an obstacle during this state.

Return value (number)
: The pushing delay in milliseconds.

### `state:set_pushing_delay(pushing_delay)`

Sets the delay before pushing when moving towards an obstacle during this state.

This only has an effect if [`state:get_can_push()`](#stateget_can_push) is `true`.

`pushing_delay` (number, default: `1000`)
: The pushing delay in milliseconds (`0` for no delay).

### `state:get_can_pick_treasure()`

Returns whether [pickable treasures](./map-entities/pickable.md) can be picked during this state.

Return value (boolean)
: `true` if pickable treasures can be picked during this state.

### `state:set_can_pick_treasure(can_pick_treasure)`

Sets whether [pickable treasures](./map-entities/pickable.md) can be picked during this state.

`can_pick_treasure` (boolean, default: `true`)
: `true` to allow the entity to pick treasures during this state.

### `state:get_can_use_teletransporter()`

Returns whether the entity can take [teletransporters](./map-entities/teletransporter.md) during this state.

Return value (boolean)
: `true` if the entity can take teletransporter during this state.

### `state:set_can_use_teletransporter(can_use_teletransporter)`

Sets whether the entity can take [teletransporters](./map-entities/teletransporter.md) during this state.

`can_use_teletransporter` (boolean, default: `true`)
: `true` to allow the entity to take teletransporters during this state.

### `state:get_can_use_switch()`

Returns whether the entity can activate [switches](./map-entities/switch.md) during this state.

Return value (boolean)
: `true` if the entity can activate switches during this state.

### `state:set_can_use_switch(can_use_switch)`

Sets whether the entity can activate [switches](./map-entities/switch.md) during this state.

`can_use_switch` (boolean, default: `true`)
: `true` to allow the entity to activate switches during this state.

### `state:get_can_use_stream()`

Returns whether the entity can take [streams](./map-entities/stream.md) during this state.

Return value (boolean)
: `true` if the entity can take stream during this state.

### `state:set_can_use_stream(can_use_stream)`

Sets whether the entity can take [streams](./map-entities/stream.md) during this state.

`can_use_stream` (boolean, default: `true`)
: `true` to allow the entity to take streams during this state.

### `state:get_can_use_stairs()`

Returns whether the entity can take [stairs](./map-entities/stairs.md) during this state.

Return value (boolean)
: `true` if the entity can take stairs during this state.

### `state:set_can_use_stairs(can_use_stairs)`

Sets whether the entity can take [stairs](./map-entities/stairs.md) during this state.

`can_use_stairs` (boolean, default: `true`)
: `true` to allow the entity to take stairs during this state.

### `state:get_can_use_jumper()`

Returns whether the entity can activate [jumpers](./map-entities/jumper.md) during this state.

Return value (boolean)
: `true` if the entity can take jumpers during this state.

### `state:set_can_use_jumper(can_use_jumper)`

Sets whether the entity can take [jumpers](./map-entities/jumper.md) during this state.

`can_use_jumper` (boolean, default: `true`)
: `true` to allow the entity to take jumpers during this state.

### `state:get_jumper_delay()`

Returns the delay before jumping when taking a jumper during this state.

Return value (number)
: The jump delay in milliseconds (`0` means no delay).

### `state:set_jumper_delay(jumper_delay)`

Sets the delay before jumping when taking a jumper during this state.

This only has an effect if [`state:get_can_use_jumper()`](#stateget_can_use_jumper) is `true`

`jumper_delay` (number, default: `200`)
: The jump delay in milliseconds (`0` for no delay).

### `state:get_carried_object_action()`

Returns what happens during this state to an object that was [carried](./map-entities/carried-object.md) the previous state.

Return value (string)
: One of:

    - `"throw"` (default): The carried object is automatically thrown.
    - `"remove"`: The carried object is silently destroyed.
    - `"keep"`: The carried object continues to be carried.

### `state:set_carried_object_action(action)`

Sets what happens during this state to an object that was [carried](./map-entities/carried-object.md) the previous state.

`action` (string)
: One of:

    - `"throw"` (default): The carried object is automatically thrown.
    - `"remove"`: The carried object is silently destroyed.
    - `"keep"`: The carried object continues to be carried.

## Events of a `state`

Events are callback methods automatically called by the engine if you define them. In the case of states, they are only called on an active state.

### `state:on_started(previous_state_name, previous_state)`

Called when this state starts.

`previous_state_name` (string or `nil`)
: Name of the state that was active before. See [`hero:get_state()`](./map-entities/hero.md#heroget_state) for the possible values. This value is `nil` if there was no state before (only possible for the first state of an entity).

`previous_state` (state or `nil`)
: Custom state object that was active before, if it was a custom one, `nil` otherwise.

### `state:on_finished(next_state_name, next_state)`

Called when this state finishes.

`next_state_name` (string or `nil`)
: Name of the state that is about to be active after yours. See [`hero:get_state()`](./map-entities/hero.md#heroget_state) for the possible values. This value is `nil` if there is no state after yours (only possible if the entity is being removed).

`next_state` (state or `nil`)
: Custom state object about to start, if it is a custom one, `nil` otherwise.

### `state:on_update()`

Called at each cycle of the main loop while this state is active.

!!! note "Note"

    As this function is called at each cycle, it is recommended to use other solutions when possible, like [timers](./timers.md) and other events.

### `state:on_pre_draw(camera)`

Called just before the entity is drawn on the map during this state.

You may display additional things below the entity. To do so, you can either call [`map:draw_visual()`](./map.md#mapdraw_visualdrawable-x-y) or draw on [`camera:get_surface()`](./map-entities/camera.md#cameraget_surface).

`camera` ([camera](./map-entities/camera.md))
: The camera where this entity is being drawn.

### `state:on_post_draw(camera)`

Called just after the entity is drawn on the map during this state.

You may display additional things above the entity. To do so, you can either call [`map:draw_visual()`](./map.md#mapdraw_visualdrawable-x-y) or draw on [`camera:get_surface()`](./map-entities/camera.md#cameraget_surface).

`camera` ([camera](./map-entities/camera.md))
: The camera where this entity is being drawn.

### `state:on_suspended(suspended)`

Called when the [entity](./map-entities/index.md) has just been suspended or resumed.

The entity is suspended by the engine in a few cases, like when the [game](./game.md) is paused or when a dialog is active. When this happens, all [map entities](./map-entities/index.md) stop moving and most [sprites](./drawable-objects/sprite.md) stop their animation.

`suspended` (boolean)
: `true` if the entity was just suspended, `false` if it was resumed.

### `state:on_map_started(map, destination)`

Called when a map starts (when the player enters it) during this state.

`map` ([map](./map.md))
: The new map.

`destination` ([destination](./map-entities/destination.md) or `nil`)
: The destination entity from where the [hero](./map-entities/hero.md) arrives on the map, or `nil` if he used another way than a destination entity (like the side of the map or direct coordinates).

### `state:on_map_finished()`

Called when the map stops (when the player leaves it) during this state.

### `state:on_map_opening_transition_finished(map, destination)`

When a map begins during this state, called when the opening transition effect finishes.

`map` ([map](./map.md))
: The map.

`destination` ([destination](./map-entities/destination.md) or `nil`)
: The destination entity from where the [hero](./map-entities/hero.md) arrived on the map, or `nil` if he used another way than a destination entity (like the side of the map or direct coordinates).

### `state:on_position_changed(x, y, layer)`

Called when the coordinates of the entity controlled by this state have just changed.

`x` (number)
: The new X coordinate of the entity.

`y` (number)
: The new Y coordinate of the entity.

`layer` (number)
: The new layer of the entity.

### `state:on_ground_below_changed(ground_below)`

Called when the kind of [ground](./map.md#mapget_groundx-y-layer) on the map below the entity controlled by this state has changed. It may change because the entity is moving, or when because another entity changes it.

`ground_below` (string)
: The kind of ground at the [ground point](./map-entities/index.md#entityget_ground_position) of the entity controlled by this state. `nil` means empty, that is, there is no ground at this point on the current layer.

### `state:on_obstacle_reached()`

Called when the [movement](./movements/index.md) of the entity was stopped because of an obstacle during this state.

When an obstacle is reached, this event is called instead of [`state:on_position_changed()`](#stateon_position_changedx-y-layer).

`movement` ([movement](./movements/index.md))
: The movement of the entity.

### `state:on_movement_started(movement)`

Called when a [movement](./movements/index.md) is started on the entity controlled by this state.

`movement` ([movement](./movements/index.md))
: The movement that was just started on the entity.

### `state:on_movement_changed(movement)`

Called when some characteristics of the entity's [movement](./movements/index.md) (like the speed or the angle) have just changed during this state.

`movement` ([movement](./movements/index.md))
: The movement of the entity.

### `state:on_movement_finished()`

Called when the [movement](./movements/index.md) of the entity controlled by this state is finished (if there is an end).

### `state:on_attacked_enemy(enemy, enemy_sprite, attack, consequence)`

Called when the entity has just attacked an enemy during this state, even if the attack was not successful.

`enemy` ([enemy](./map-entities/enemy.md))
: The attacked enemy.

`enemy_sprite` ([sprite](./drawable-objects/sprite.md) or `nil`)
: Sprite of the enemy that received the attack, or `nil` if the attack does not come from a pixel-precise collision test.

`attack` (string)
: How the enemy was attacked. See [`enemy:set_attack_consequence()`](./map-entities/enemy.md#enemyset_attack_consequence_spritesprite-attack-consequence) for the possible values.

`consequence` (number, string or function)
: How the enemy reacted to the attack. See [`enemy:set_attack_consequence()`](./map-entities/enemy.md#enemyset_attack_consequence_spritesprite-attack-consequence) for the possible values.

### `state:on_command_pressed(command)`

Called when the player presses a [game command](./game.md#game-commands) (a keyboard key or a joypad action mapped to a built-in game behavior) during this state. You can use this event to override the normal built-in behavior of the game command.

`command` (string)
: Name of the built-in game command that was pressed. Possible commands are:

    - `"action"`
    - `"attack"`
    - `"pause"`
    - `"item_1"`
    - `"item_2"`
    - `"right"`
    - `"up"`
    - `"left"`
    - `"down"`

Return value (boolean)
: Indicates whether the event was handled. If you return `true`, the event won't be propagated to other objects (you are overriding the built-in behavior of pressing this game command).

!!! note "Note"

    This event is not triggered if you already handled its underlying low-level keyboard or joypad event.

### `state:on_command_released(command)`

Called when the player released a [game command](./game.md#game-commands) (a keyboard key or a joypad action mapped to a built-in game behavior). during this state. You can use this event to override the normal built-in behavior of the game command.

`command` (string)
: Name of the built-in game command that was released. Possible commands are:

    - `"action"`
    - `"attack"`
    - `"pause"`
    - `"item_1"`
    - `"item_2"`
    - `"right"`
    - `"up"`
    - `"left"`
    - `"down"`

Return value (boolean)
: Indicates whether the event was handled. If you return `true`, the event won't be propagated to other objects (you are overriding the built-in behavior of releasing this game command).

!!! note "Note"

    This event is not triggered if you already handled its underlying low-level keyboard or joypad event.

## Events as an Input Handler

In addition to its own events a state is also an input handler and receives all of those events.

The hero's current state is checked after the current map. Only its menus and the conversion to a [game command](./game.md#game-commands) follow.

See [inputs page](./controls/inputs.md#events-of-input-handlers) to see all of these events.
