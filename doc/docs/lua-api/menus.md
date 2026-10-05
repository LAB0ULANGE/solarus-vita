# Menus

## Overview

To display various information such as a title screen, a dialog box, a HUD (head-up display) or a pause screen, you can use one or several menus.

A menu is an arbitrary Lua table. A menu belongs to a context that may be the current [map](./map.md), the current [game](./game.md), the [sol.main](./general-features.md) table or even another menu. This context is the lifetime of your menu. As long as your menu is active, the engine will call events that are defined in your table, i.e. callback methods like [`menu:on_started()`](#menuon_started), [`menu:on_draw()`](#menuon_drawdst_surface), [`menu:on_key_pressed()`](./controls/inputs.md#handleron_key_pressedkey-modifiers), etc., to notify your menu of something (the player pressed a key, your menu needs to be redrawn, etc.).

This menu API does not provide anything fundamental: indeed, the [map](./map.md), [game](./game.md) and [sol.main](./general-features.md) APIs already provide the necessary features, so you could do what you want from there manually. But the API described on this page makes your life easier because menus automatically receive events whenever they need to be notified, and automatically stop being active when their context no longer exists.

## Functions of `sol.menu`

### `sol.menu.start(context, menu, [on_top])`

Starts a menu in a context. The Solarus engine will then call the appropriate events on your menu until it is stopped.

`context` ([map](./map.md) or [game](./game.md) or table)
: The context your menu will belong to. Similarly to the case of [timers](./timers.md), the context determines the lifetime of your menu. The context must be one of the following four objects:

    - If you make a [map](./map.md) menu, your menu will be drawn above the map surface. It will be stopped when the player goes to another map. This may be useful to show head-up information local to a precise map. Example: a counter or a mini-game that only exists on a specific map.
    - If you make a [game](./game.md) menu, your menu will be global to all maps. As long as the game is running, it will persist accross map changes. Example: the player's life counter.
    - If you make a [main](./general-features.md) menu, your menu will be global to the whole program. It can exist outside a game (and it even persists during the game if you don't stop it). Example: the title screen.
    - If you set the context to another menu, then its lifetime will be limited to this other menu. This allows to make nested menus. Example: a popup that shows some information above another menu.

`menu` (table)
: The menu to activate. It can be any table. The only thing that makes it special is the presence of callback functions (events) as described in section [events](#events-of-the-type-menu).

`on_top` (boolean, optional, default: `true`)
: Whether this menu should be drawn on top of other existing menus of the same context or behind them. If `true`, the [`on_draw()`](#menuon_drawdst_surface) event of your menu will be the last to be called if there are several menus in the context. If `false`, it will be the first one.

### `sol.menu.stop(menu)`

Stops a menu previously activated with [`sol.menu.start()`](#solmenustartcontext-menu-on_top).

After this call, the Solarus engine will no longer call events on your menu. But you can restart it later if you want. Nothing happens is the menu was already stopped or never started.

`menu` (table)
: The menu to stop.

### `sol.menu.stop_all(context)`

Stops all menus that are currently running in a context.

This function is not often needed since menus are already automatically stopped when their context is closed.

`context` ([map](./map.md) or [game](./game.md) or [sol.main](./general-features.md) or table)
: The context where you want to stop menus.

### `sol.menu.is_started(menu)`

Returns whether a table is a currently active menu.

`menu` (table)
: The menu to test.

Return value (boolean)
: `true` if this is an active menu.

### `sol.menu.bring_to_front(menu)`

Places this menu in front of other menus of the same context.

Your menu will then be the first one to receive input events (keyboard, joypad and mouse events) and the last one to be drawn.

`menu` (table)
: The menu to bring to the front.

### `sol.menu.bring_to_back(menu)`

Places this menu behind other menus of the same context.

Your menu will then be the last one to receive input events (keyboard, joypad and mouse events) and the first one to be drawn.

`menu` (table)
: The menu to bring to the back.

## Events of the type `menu`

Events are callback methods automatically called by the engine if you define them.

### `menu:on_started()`

Called when your menu is started.

This event is triggered when you call [`sol.menu.start()`](#solmenustartcontext-menu-on_top).

### `menu:on_finished()`

Called when your menu is stopped.

This event is triggered when you call [`sol.menu.stop()`](#solmenustopmenu) or when the context of your menu finishes.

### `menu:on_update()`

Called at each cycle of the main loop while your menu is active.

Menus of are updated in the following order:

1. [Map](./map.md) menus (only during a game).
2. [Game](./game.md) menus (only during a game).
3. [Main](./general-features.md) menus (the more general ones).

When several menus exist in the same context, they are updated from the back one to the front one. You can control this order thanks to the `on_top` parameter of [`menu:start()`](#solmenustartcontext-menu-on_top) when you start a menu.

!!! note "Note"

    As this function is called at each cycle, it is recommended to use other solutions when possible, like [timers](./timers.md) and other events.

### `menu:on_draw(dst_surface)`

Called when your menu has to be redrawn.

Use this event to draw your menu.

`dst_surface` ([surface](./drawable-objects/surface.md))
: The surface where you should draw your menu.

Menus of are drawn in the following order:

1. [Map](./map.md) menus (only during a game).
2. [Game](./game.md) menus (only during a game).
3. [Main](./general-features.md) menus (the more general ones).

When several menus exist in the same context, they are drawn from the back one to the front one. You can control this order thanks to the `on_top` parameter of [`sol.menu.start()`](#solmenustartcontext-menu-on_top) when you start a menu, or with [`sol.menu.bring_to_front()`](#solmenubring_to_frontmenu) and [`sol.menu.bring_to_back()`](#solmenubring_to_backmenu)

### `menu:on_command_pressed(command)`

Called during a game when the player presses a [game command](./game.md#game-commands) (a keyboard key or a joypad action mapped to a built-in game behavior). You can use this event to override the normal built-in behavior of the game command. Menus on top are notified first.

`command` (string)
: Name of the built-in game command that was pressed (see the [game API](./game.md#game-commands) for the list of existing game commands).

Return value (boolean)
: Indicates whether the event was handled. If you return `true`, the event won't be propagated to other objects (you are overriding the built-in behavior of pressing this game command).

!!! note "Note"

    As the notion of game commands only exist during a game, this event is only called for game menus and map menus.

!!! note "Note"

    This event is not triggered if you already handled the underlying low-level keyboard or joypad event.

### `menu:on_command_released(command)`

Called during a game when the player released a game command (a keyboard key or a joypad action mapped to a built-in game behavior). You can use this event to override the normal built-in behavior of the game command. Menus on top are notified first.

`command` (string)
: Name of the built-in game command that was released (see the [game API](./game.md#game-commands) for the list of existing game commands).

Return value (boolean)
: Indicates whether the event was handled. If you return `true`, the event won't be propagated to other objects (you are overriding the built-in behavior of releasing this game command).

!!! note "Note"

    As the notion of game commands only exist during a game, this event is only called for game menus and map menus.

!!! note "Note"

    This event is not triggered if you already handled the underlying low-level keyboard or joypad event.

## Events as an Input Handler

In addition to its own events a menu is also an input handler and receives all of those events.

Menus are checked to see if they handle an input event after their context is checked. Unless their context is another menu, then they are checked first.

See [inputs](./controls/inputs.md) to see all of these events.
