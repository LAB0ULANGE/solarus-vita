# Lua API Reference

## Overview

This is the **API specification of Lua functions, methods, callbacks and types defined by Solarus**. This documentation page is intented to quest makers who want to write scripts for their [maps](./map.md), [items](./equipment-items.md), [enemies](./map-entities/enemy.md) and [menus](./menus.md).

For the point of view of the C++ code of the engine, see the documentation of class `LuaContext`.

Most of the data types defined in the C++ engine (like [sprites](./drawable-objects/sprite.md), [map entities](./map-entities/index.md), [movements](./movements/index.md), [savegames](./game.md), etc.) are exported as Lua types in the scripting API of Solarus. We give here the full reference of these types and the functions available for each type.

The API exports C++ functions and C++ datatypes that may be used by your Lua scripts. Examples of such features are [creating a sprite](./drawable-objects/sprite.md#solspritecreateanimation_set_id), [drawing an image](./drawable-objects/index.md#drawabledrawdst_surface-x-y) or [moving an enemy](./movements/index.md#movementstartobject_to_move-callback).

In the opposite way, Solarus will also call your own Lua functions (if you define them), for example to notify your script that [an enemy has reached an obstacle](./map-entities/index.md#entityon_obstacle_reachedmovement), that [a pressure plate has just been activated](./map-entities/switch.md#switchon_activatedentity) or that the [hero](./map-entities/hero.md) is [talking to a particular non-playing character](./map-entities/npc.md#npcon_interaction).

The following script files are loaded by the engine when they exist:

| Script             | Role                                                                                                                                                     |
| ------------------ | -------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `main.lua`         | Global script that controls the [menus](./menus.md) (if any) before starting a [game](./game.md), and then decides to start a game.                      |
| `maps/XXXX.lua`    | The script of a [map](./map.md) controls the map `XXXX`.</br>Called when the player enters the map.                                                      |
| `enemies/XXXX.lua` | The script of an [enemy](./map-entities/enemy.md) controls an enemy whose breed is `XXXX`.</br>Called when an enemy of this breed is created on the map. |
| `items/XXXX.lua`   | The script of an [item](./equipment-items.md) : controls the equipment item named `XXXX`.</br>Called when a [savegame](./game.md) is loaded or created.  |

All these various scripts run in the same Lua state. In other words, they share global values.

Interactions between your Lua world and the engine are managed through a predefined global table called `sol`. The whole Solarus Lua API is available in the `sol` table. It contains functions, types and values that allow you to interact with the C++ engine.

Most types of the Lua API (like [game](./game.md), [item](./equipment-items.md), [map](./map.md), [entity](./map-entities/index.md), [movement](./movements/index.md) and [sprite](./drawable-objects/sprite.md)) are Lua userdata that have something special: they can also be indexed like tables. This mechanism is used by the engine when it needs to invoke callback methods that you defined on your objects.

But you can also extend these objects with your own functions and data. This is very useful in the [game](./game.md) and [map](./map.md) objects to implement and store everything that is not built-in in the Solarus API: your pause menu, your HUD, a puzzle, or some properties and utility functions specific to your quest.

## Features

The following features are defined in the global `sol` table. See the specification page of each feature for more details.

| API                                                  | Role                                                                                                                   |
| ---------------------------------------------------- | ---------------------------------------------------------------------------------------------------------------------- |
| [`sol.main`](./general-features.md)                  | General-purpose features.                                                                                              |
| [`sol.audio`](./audio/index.md)                   | Playing musics and managing sounds.                                                                                    |
| [`sol.music`](./audio/music.md)                      | Playing musics.                                                                                                        |
| [`sol.sound`](./audio/sound.md)                      | Playing sound effects.                                                                                                 |
| [`sol.video`](./video.md)                            | Changing video settings.                                                                                               |
| [`sol.shader`](./shaders.md)                         | Modifying the rendering.                                                                                               |
| [`sol.controls`](./controls/index.md)             | Binding low-level input to game commands.                                                                              |
| [`sol.input`](./controls/inputs.md)                  | Checking keyboard and joypad state.                                                                                    |
| [`sol.joypad`](./controls/joypad.md)                 | Handling joypads.                                                                                                      |
| [`sol.file`](./files.md)                             | Directly accessing data files.                                                                                         |
| [`sol.menu`](./menus.md)                             | Showing various information on the screen.                                                                             |
| [`sol.language`](./language-functions.md)            | Handling translations.                                                                                                 |
| [`sol.timer`](./timers.md)                           | Making an action later with a delay.                                                                                   |
| [`sol.sprite`](./drawable-objects/index.md)       | Displaying animated images.                                                                                            |
| [`sol.surface`](./drawable-objects/index.md)      | Displaying fixed images.                                                                                               |
| [`sol.text_surface`](./drawable-objects/index.md) | Displaying text.                                                                                                       |
| [`sol.movement`](./movements/index.md)            | Moving objects.                                                                                                        |
| [`sol.game`](./game.md)                              | Handling data saved (life, equipment, etc.) and running a game.                                                        |
| [`sol.item`](./equipment-items.md)                   | Controlling a specific type of equipment item and its behavior.                                                        |
| [`sol.map`](./map.md)                                | Handling the current map and its properties (only during a game).                                                      |
| [`sol.entity`](./map-entities/index.md)           | Managing entities placed on the map (only during a game), like the hero, enemies, chests, non-playing characters, etc. |
| [`sol.state`](./custom-states.md)                    | Customizing the behavior of the hero entity.                                                                           |
