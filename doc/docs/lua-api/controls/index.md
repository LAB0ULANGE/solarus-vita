# Controls

This module provides a datatype controls that represent key or joypad buttons / axis bindings. Controls can be seen as an abstraction of the [inputs](./inputs.md) and the [joypad](./joypad.md).

Controls are like a virtual game device that provides buttons and axes abstracted away from the actual input devices that the user is interacting with. Of course, these virtual commands and axes are mapped to real, low-level inputs from the keyboard and/or the joypad.

You script can control which keyboard and joypad inputs are associated to each controls command or axes.

## Overview

### Commands

Commands are the "buttons" of Solarus' virtual controls.

| Command    | Role                                                                                                               |
| ---------- | ------------------------------------------------------------------------------------------------------------------ |
| `"action"` | Contextual action such as talking, swimming, throwing, etc                                                         |
| `"attack"` | Main attack (using the sword)                                                                                      |
| `"pause"`  | Pausing or unpausing the game                                                                                      |
| `"item_1"` | Using the equipment item placed in slot 1 (see [`game:get_item_assigned()`](../game.md#gameget_item_assignedslot)) |
| `"item_2"` | Using the equipment item placed in slot 2 (see [`game:get_item_assigned()`](../game.md#gameget_item_assignedslot)) |
| `"right"`  | Moving to the right.                                                                                               |
| `"left"`   | Moving to the left.                                                                                                |
| `"up"`     | Moving to the top.                                                                                                 |
| `"down"`   | Moving to the bottom.                                                                                              |

When a game command is pressed, no matter if the command came from the keyboard or the joypad, the engine performs some built-in behavior by default (like pausing the game or moving the hero to the right).

But you can also extend or override this behavior, because the engine notifies you first (see [`game:on_command_pressed()`]). Therefore, you usually don't have to worry about the underlying keyboard or joypad input (but if you want to you can as the game object is an [input](./inputs.md#events-of-input-handlers).)

### Axes

Axes are used by the engine only for movements of the hero.

| Name  | Role            |
| ----- | --------------- |
| `"X"` | Horizontal axis |
| `"Y"` | Vertical axis   |

### Extending

Every method or function accepting commands or axes names actually accept any string and will happily register / propagate the custom name you provided. This permits you extend over the default commands and axes that the engine uses.

For example, you could bind additional axes of the [joypad](./joypad.md) instances to custom functionalities while keeping the controls abstraction which would allow users with a keyboard to also profit from your game.

## Functions of `sol.controls`

### `sol.controls.create_from_keyboard()`

Creates the controls using the default mapping for the keyboard. Default mappings are:

| Key                                                 | Role       |
| --------------------------------------------------- | ---------- |
| <kbd>←</kbd> <kbd>↑</kbd> <kbd>→</kbd> <kbd>↓</kbd> | Directions |
| <kbd>Space</kbd>                                    | Action     |
| <kbd>C</kbd>                                        | Attack     |
| <kbd>X</kbd>                                        | Item1      |
| <kbd>V</kbd>                                        | Item2      |
| <kbd>D</kbd>                                        | Pause      |

Axes are mapped to their corresponding directions.

Return value (controls)
: `controls` The controls object created.

### `sol.controls.create_from_joypad(joypad)`

Creates the controls using the default mapping for a joypad instance. Those contols respond only to the given joypad.

Default mappings are:

| Button       | Role       |
| ------------ | ---------- |
| Left Stick   | Directions |
| A            | Attack     |
| B            | Action     |
| X            | Item1      |
| Y            | Item2      |
| Start        | Pause      |
| Left Stick X | X          |
| Left Stick Y | Y          |

`joypad` ([joypad](./joypad.md))
: a joypad instance to create the controls for.

Return value (controls)
: `controls` The controls object created.

### `sol.controls.set_analog_commands_enabled(enabled)`

Enable the use of axes for built-in movements of the hero when controlled by those controls.

`enabled` (boolean)
: enable the analog behavior of those controls.

### `sol.controls.are_analog_commands_enabled()`

Tells whether those controls are analog.

Return value (boolean)
: `true` if those controls are analog.

## Methods of the type `controls`

### `controls:is_pressed(command)`

Tells if a command is pressed.

`command` (string)
: Any valid command name.

### `controls:get_axis_state(axis)`

Get the last known state of an axis, returns 0 if the axis hasn't emitted since these controls where created.

`axis` (string)
: an axis name, can be one of the built in names or any name.

Return value (number)
: axis state [-1 -> 1].

### `controls:get_direction()`

Gets the direction8 of the commands.

Return value (number)
: direction8.

### `controls:set_keyboard_binding(command, key)`

Sets the key that will trigger the given command.

`command` (string)
: Any valid command name.

`key` (string)
: A valid key code (see [inputs](./inputs.md)).

### `controls:get_keyboard_binding(command)`

Gets the key that is bound to the given command.

`command` (string)
: Any valid command name.

Return value (string)
: A key code (see [inputs](./inputs.md)).

### `controls:set_joypad_binding(command, button)`

`command` (string)
: Any valid command name

`button` (string)
: A valid joypad binding code. Binding codes must be one of the following:

    - A valid joypad button string (see [joypad](./joypad.md)).

    - An axis binding formed like this: `"<axis_name> {+/-}"` where `axis_name` is a valid joypad axis string (see [joypad](./joypad.md)).

### `controls:get_joypad_binding(command)`

Gets the joypad binding code bound to the given command

`command` (string)
: Any valid command name

Return value (string)
: A joypad binding code.

### `controls:set_keyboard_axis_binding(axis, negative_key, positive_key)`

Sets the keyboard keys that will move the given axis.

`axis` (string)
: Any valid axis name.

`negative_key` (string)
: A valid keyboard key code, triggers `-1` state.

`positive_key` (string)
: A valid keyboard key code, triggers `1` state.

### `controls:get_keyboard_axis_binding(axis)`

Returns the keys bound to move the given axis.

Return value 1 (string)
: The positive key code.

Return value 2 (string)
: The negative key code.

### `controls:set_joypad_axis_binding(axis, joypad_axis_binding)`

Sets the joypad axis that will drive the given virtual axis.

`axis` (string)
: Any valid control axis name.

`joypad_axis` (string)
: A valid joypad axis binding (see [`controls:set_keyboard_axis_binding`](#controlsset_keyboard_axis_bindingsbindings)).

### `controls:get_joypad_axis_binding(axis)`

Gets the joypad axis that is bound to the given axis.

Return value (string)
: Joypad axis binding.

### `controls:set_keyboard_bindings(bindings)`

Sets the commands bindings for the keyboard.

`bindings` (table)
: A table of key codes -> array of commands.

### `controls:get_keyboard_bindings()`

Gets the command bindings for the keyboard.

Return value (table)
: A table of key codes -> array of commands (see [inputs](./inputs.md)).

### `controls:set_joypad_bindings(bindings)`

Sets the command bindings for the joypad.

`commands` (table)
: A table of joypad command bindings -> array of commands.

### `controls:get_joypad_bindings()`

Gets the joypad command bindings.

Return value (table)
: A table of joypad command bindings -> array of commands.

### `controls:set_keyboard_axis_bindings(bindings)`

Sets keyboard's axes bindings

`bindings` (table)
: A table of key codes -> array of axes bindings.

### `controls:get_keyboard_axis_bindings()`

Gets the keyboard's axes bindings.

Return value (table)
: A table of key codes -> array of axes bindings.

### `controls:set_joypad_axis_bindings(bindings)`

Sets the joypad axes bindings.

`bindings` (table)
: A table of joypad axis names -> array of axes bindings.

### `controls:get_joypad_axis_bindings()`

Gets the joypad axes bindings.

Return value (table)
: A table of joypad axis names -> array of axes bindings.

### `controls:get_effect(command)`

Gets the corresponding effect to the given command.

### `controls:simulate_pressed(command)`

Simulate a command pressed event. You can also simulate custom command names.

`command` (string)
: Any valid command name.

### `controls:simulate_released(command)`

Simulate a command released event. You can also simulate custom command names.

`command` (string)
: Any valid command name.

### `controls:simulate_axis_moved(axis, state)`

Simulate an axis moved event. You can also simulate custom axis names.

`axis` (string)
: Any valid axis name.

`state` (number)
: State to signal.

### `controls:set_joypad(joypad)`

Sets the joypad that is bound to those controls.

`joypad` ([joypad](./joypad.md) or `nil`)
: joypad to set, `nil` to disable joypad input.

### `controls:get_joypad()`

Gets the joypad bound to those controls.

Return value ([joypad](./joypad.md) or `nil`)
: bound joypad or `nil` if none.

### `controls:remove()`

Remove those controls from the event handler, effectively disabling them forever. The Lua object is still usable but will never trigger any output. If the reference is forgotten by Lua the object is freed.

## Events of Control Handlers

Controls raise events that are propagated to all event handlers defined in your code. Objects that can handle controls event are [game](../game.md), [menu](../menus.md), [map](../map.md) and [state](../custom-states.md).

### `handler:on_command_pressed(command, controls)`

Called when a command is pressed on a given controls instance.

`command` (string)
: Any valid command name.

`controls` (controls)
: The controls instance that raised this event.

### `handler:on_command_released(command, controls)`

Called when a command is released on a given controls instance.

`command` (string)
: Any valid command name.

`controls` (controls)
: The controls instance that raised this event.

### `handler:on_axis_moved(axis, state, controls)`

Called when an axis is moved on a given controls instance.

`axis` (string)
: Any valid command name.

`state` (number)
: New axis state.

`controls` (controls)
: The controls instance that raised this event.
