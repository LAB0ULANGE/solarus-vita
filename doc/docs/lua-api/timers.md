# Timers

## Overview

Timers allow you to call a function in the future with a specified delay.

Here is a first example of use:

```lua
-- Play sound "secret" in one second.
local function play_secret_sound()
  sol.audio.play_sound("secret")
end

sol.timer.start(1000, play_secret_sound)
```

Shorter version to do the same thing:

```lua
-- Equivalent code using an anonymous function.
sol.timer.start(1000, function()
  sol.audio.play_sound("secret")
end)
```

You can repeat a timer by returning `true` from your function:

```lua
-- Call a function every second.
sol.timer.start(1000, function()
  sol.audio.play_sound("danger")
  return true  -- To call the timer again (with the same duration).
end)
```

To make a timer that repeats itself 10 times, just return `false` after 10 calls:

```lua
-- Call a function ten times, with one second between each call.
local num_calls = 0
sol.timer.start(1000, function()
  sol.audio.play_sound("danger")
  num_calls = num_calls + 1
  return num_calls < 10
end)
```

It is possible to restrict the lifetime of a timer to a context, like the game, the map or an enemy:

```lua
-- Enemy that shoots a fireball every 5 seconds until it is killed.
sol.timer.start(your_enemy, 5000, function()
  sol.audio.play_sound("attack_fireball")
  map:create_enemy(…)  -- Code that creates the fireball.
  return true  -- Repeat the timer.
end)
```

Setting the context to an enemy ensures that when the enemy is killed, the timer is canceled. Otherwise, the callback function would still be called: in this example, you would hear the `"attack_fireball"` sound and the fireball would be created even if the enemy is killed in the meantime.

## Functions of `sol.timer`

### `sol.timer.start([context], duration, callback)`

Sets a function to be called after a delay.

If the duration is set to zero, the function is called immediately.

| With context | Without context |
|------------------|------------------------------|
| <dl><dt>`context` ([map](./map.md) or [game](./game.md) or [item](./equipment-items.md) or [map entity](./map-entities/index.md) or [state](./custom-states.md) or [menu](./menus.md) or [sol.main](./general-features.md) or `nil`, default: `sol.main`)</dt><dd><p>Determines the lifetime of the timer. The context is where the timer belongs. If the context gets closed before the timer is finished, then the timer is automatically canceled. More precisely, the following rules are applied.</p><ul><li>If you set the context to a [map](./map.md), the timer is canceled when the player goes to another map. Example: a button that opens a door for a limited time.</li><li>If you set the context to a [game](./game.md) or an [item](./equipment-items.md), the timer is canceled when the game is closed. (Items have the same lifetime as the game they belong to.) This is only possible when the game is running. Example: hot water that becomes cold after a few minutes, and that the player should bring to an NPC on another map while it's still hot.</li><li>If you set the context to a [map entity](./map-entities/index.md), the timer is canceled when the entity is removed from the map. In the case of an enemy, the timer is also canceled when the enemy is hurt, immobilized or restarts. Also note that while the entity is suspended, the timer is also suspended. An entity may be suspended when the [game is suspended](./game.md#gameis_suspended), or when the entity is [disabled](./map-entities/index.md#entityset_enabledenabled). Example: a boss who shoots fireballs every 10 seconds. Most enemy scripts usually create timers.</li><li>If you set the context to a [state](./custom-states.md), the timer is canceled when the custom hero state finishes or when the hero is removed. Like entity timers, state timers get suspended when the hero is suspended. Example: charging an attack during 3 seconds in a custom state.</li><li>If you set the context to a [menu](./menus.md), the timer is canceled when the menu is closed. Example: in the title screen, show some animations after a few seconds without action from the user.</li><li>If you set the context to the [sol.main](./general-features.md) table, the timer is canceled when Lua is closed. Thus, it will be a global timer. This kind of timer is not often needed. Example: dumping some global information periodically while the program is running.</li><li>If you don't specify a context, then a default context is set for you: the current [map](./map.md) during a [game](./game.md), and [sol.main](./general-features.md) if no game is running.</li></ul></dd><dt>`duration` (number)</dt><dd>Delay before calling the function in milliseconds.</dd></dl> | <dl><dt>`duration` (number)</dt><dd>Delay before calling the function in milliseconds.</dd></dl>|

`callback` (function)
: The function to be called when the timer finishes.

    - If this callback function returns `true`, then the timer automatically repeats itself with the same duration.
    - If the callback function returns a positive integer value, then the timer automatically repeats itself after this specified duration in milliseconds.
    - Otherwise, the timer does not repeats. If the repeating duration is shorter than the time of a cycle of the main loop, then the callback may be executed several times in the same cycle in order to catch up.

Return value (timer)
: The timer created. Most of the time, you don't need to store the returned timer: the timer persists until its completion or the end of its context. Usually, you will store the return value only if you need to stop the timer explicitly later or to call another method on it.

!!! note "Note"

    When they are created, [map](./map.md) timers, [map entity](./map-entities/index.md) timers and [item](./equipment-items.md) timers are initially suspended if a dialog is active. After that, they get automatically suspended and unsuspended when the map is suspended or unsuspended.

    This default behavior is suited for most use cases, but if you want to change it, you can use [`timer:set_suspended()`](#timerset_suspendedsuspended) and [`timer:set_suspended_with_map()`](#timerset_suspended_with_mapsuspended_with_map).

### `sol.timer.stop_all(context)`

Cancels all timers that are currently running in a context.

This function is equivalent to calling [`timer:stop()`](./timers.md) on each timer of the context. It may allow you to avoid to store explicitly all your timers.

`context` ([map](./map.md) or [game](./game.md) or [item](./equipment-items.md) or [map entity](./map-entities/index.md) or [menu](./menus.md) or [sol.main](./general-features.md))
: The context where you want to stop timers.

## Methods of the type `timer`

### `timer:stop()`

Cancels this timer. If the timer was already finished or canceled, nothing happens.

!!! note "Note"

    Canceling timers by hand may be tedious and error-prone. In lots of cases, you can simply pass a context parameter to [`sol.timer.start()`](#soltimerstartcontext-duration-callback) in order to restrict the lifetime of your timer to some other object.

### `timer:is_with_sound()`

Returns whether a clock sound is played repeatedly during this timer.

Return value (boolean)
: `true` if a clock sound is played with this timer.

### `timer:set_with_sound(with_sound)`

Sets whether a clock sound is played repeatedly during this timer.

`with_sound` (boolean, optional, default: `true`)
: `true` to play a clock sound repeatedly.

### `timer:is_suspended()`

Returns whether this timer is currently suspended.

Return value (boolean)
: `true` if this timer is currently suspended.

### `timer:set_suspended([suspended])`

Returns whether this timer is currently suspended.

`suspended` (boolean, optional, default: `true`)
: `true` to suspend the timer, `false` to unsuspend it.

### `timer:is_suspended_with_map()`

Returns whether this timer gets automatically suspended when the [map](./map.md) is suspended.

Return value (boolean)
: `true` if this timer gets suspended when the map is suspended.

### `timer:set_suspended_with_map([suspended_with_map])`

Sets whether this timer should automatically be suspended when the [map](./map.md) gets suspended.

The map is suspended by the engine in a few cases, like when the game is paused, when there is a dialog or when the camera is being moved by a script. When this happens, all [map entities](./map-entities/index.md) stop moving and most [sprites](./drawable-objects/sprite.md) stop their animation. With this setting, you can choose whether your timer gets suspended automatically as well.

By default, [map](./map.md) timers, [entity](./map-entities/index.md) timers, [state](./custom-states.md) timers and [item](./equipment-items.md) timers are suspended with the map.

`suspended_with_map` (boolean, optional, default: `true`)
: `true` to suspend the timer when the map is suspended, `false` to continue.

!!! note "Note"

    When this setting is `true`, entity timers also get automatically suspended when the entity is [disabled](./map-entities/index.md#entityset_enabledenabled).

### `timer:get_duration()`

Returns the duration of this timer.

Return value (number)
: The duration of this timer in milliseconds.

!!! note "Note"

    This returns the full duration of the timer, independently of its current progress. If you want to know its current progress, use [`timer:get_remaining_time()`](#timerget_remaining_time) instead.

### `timer:set_duration(duration)`

Sets the duration of this timer.

This is equivalent to returning a new duration from the timer's callback.

`duration` (number)
: The new duration is milliseconds.

!!! note "Note"

    This only acts on subsequent iterations of the timer and only has an effect if the timer is repeated. To modify the remaining time of the current iteration, use [`timer:set_remaining_time()`](#timerset_remaining_timeremaining_time) instead.

### `timer:get_remaining_time()`

Returns the remaining time of the current iteration of this timer.

Return value (number)
: The time remaining in milliseconds. `0` means that the timer is finished (or will finish in the current cycle) or was canceled.

### `timer:set_remaining_time(remaining_time)`

Changes the remaining time of the current iteration of this timer. This function has no effect if the timer is already finished.

`remaining_time` (number)
: The time remaining in milliseconds. `0` makes the timer finish now and immediately executes its callback.

!!! note "Note"

    This function only acts on the current iteration of the timer. When the timer is repeated, the timer gets rescheduled with its full duration again.

    To change the duration of subsequent iterations of a repeated timer, you can either return the new duration in the timer's callback or call [`timer:set_duration()`](#timerset_durationduration).
