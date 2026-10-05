# Inputs

You can get information about the low-level keyboard and joypad inputs through `sol.input` and input handlers.

`sol.input` functions are used to check the current state of input while the handlers are notified when an input event to change the state of input occurs. Using an input handler is prefered, but you can use `sol.input` for the cases where more information than a single event is needed.

Also note that during the game, there exists the higher-level notion of [game commands](../game.md#game-commands) to ease your life which have their own higher-level events. Commands are only generated from unhandled input, so handling a piece of input will prevent the command from being activated.

## Functions of `sol.input`

### `sol.input.is_joypad_enabled()`

Returns whether joypad support is enabled. This may be true even without any joypad plugged.

Return value (boolean)
: `true` if joypad support is enabled.

### `sol.input.set_joypad_enabled([joypad_enabled])`

Enables or disables legacy joypad support. For new joypad support see [joypad functions](./joypad.md).

Joypad support may be enabled even without any joypad plugged.

`joypad_enabled` (boolean, default: `true`)
: `true` to enable joypad support, `false` to disable it.

### `sol.input.is_key_pressed(key)`

Returns whether a keyboard key is currently down.

`key` (string)
: The name of a keyboard key.

Return value (boolean)
: `true` if this keyboard key is currently down.

### `sol.input.get_key_modifiers()`

Returns the keyboard key modifiers currently active.

Return value (table)
: A table whose keys indicate what modifiers are currently down. Possible table keys are `"shift"`, `"control"`, `"alt"` and `"caps` lock". Table values are `true`.

### `sol.input.is_joypad_button_pressed(button)`

Returns whether a joypad button is currently down.

`button` (number)
: Index of a button of the joypad.

Return value (boolean)
: `true` if this joypad button is currently down.

### `sol.input.get_joypad_axis_state(axis)`

Returns the current state of an axis of the joypad.

`axis` (number)
: Index of a joypad axis. The first one is `0`.

Return value (number)
: The state of that axis. `-1` means left or up, `0` means centered and `1` means right or down.

### `sol.input.get_joypad_hat_direction(hat)`

Returns the current direction of a hat of the joypad.

`hat` (number)
: Index of a joypad hat. The first one is `0`.

Return value (number)
: The direction of that hat. `-1` means that the hat is centered. `0` to `7` indicates that the hat is in one of the eight main directions.

### `sol.input.get_mouse_position()`

Returns the current position of the mouse cursor relative to the quest size.

If the mouse is outside the window, mouse coordinates are captured only if a mouse button is pressed. In this case, the returned values can be out of bounds of the quest size and can be negative. This allows you to keep track of the mouse movement when dragging something. Otherwise, when no mouse button is pressed, the returned coordinates are the last position of the mouse in the window.

Return value 1 (integer)
: The `x` position of the mouse in [quest size](../video.md#solvideoget_quest_size) coordinates.

Return value 2 (integer)
: The `y` position of the mouse in [quest size](../video.md#solvideoget_quest_size) coordinates.

### `sol.input.is_mouse_button_pressed(button)`

Returns whether a mouse button is currently down.

`button` (string)
: The name of a mouse button. Possible values are:

    - `"left"`
    - `"middle"`
    - `"right"`
    - `"x1"`
    - `"x2"`

Return value (boolean)
: `true` if mouse button is down.

### `sol.input.get_finger_position(finger)`

Returns the current position of a finger if it exists.

`finger` (integer)
: The finger id to check.

Return value 1 (integer or `nil`)
: The `x` position of the finger in [quest size](../video.md#solvideoget_quest_size) coordinates. Return `nil` if the finger does not exist or is not pressed.

Return value 2 (integer)
: The `y` position of the finger in [quest size](../video.md#solvideoget_quest_size) coordinates.

### `sol.input.get_finger_pressure(finger)`

Returns the current pressure of a finger if it exists.

`finger` (integer)
: The finger id to check.

Return value (number or `nil`)
: The `pressure` of the finger between `0.0` and `1.0`. Return `nil` if there is no such finger.

### `sol.input.is_finger_pressed(finger)`

Returns whether a finger is currently pressed.

`finger` (integer)
: The finger id to check.

Return value (boolean)
: `true` if the finger is down.

### `sol.input.simulate_key_pressed(key)`

Simulates pressing a keyboard key.

`key` (string)
: The keyboard key to simulate.

### `sol.input.simulate_key_released(key)`

Simulates releasing a keyboard key.

`key` (string)
: The keyboard key to simulate.

### `sol.input.get_joypad_count()`

Return value (number)
: The amount of connected joypads.

### `sol.input.get_joypads()`

Return value (array of [joypad](./joypad.md))
: Array of connected joypad objects.

## Events of `sol.input`

### `sol.input.on_joypad_connected(joypad)`

Called when a joypad has just been plugged.

`joypad` ([joypad](./joypad.md))
: a hot plugged joypad.

## Events of Input Handlers

There are several Lua types that act as input handlers. These are [sol.main](../general-features.md), the running [game](../game.md), the active [map](../map.md), the hero's current [custom state](../custom-states.md) and any [menus](../menus.md) in the context of another event handler.

When input occurs, the first four (main, game, map and state) are checked in that order with menus in between. Menus are checked after their context unless their context is another menu then they are checked before. There is no strict ordering between menus in the same context.

When an input handler is checked if it has the approprate event defined on it the event is called. If the handler returns true the input is fully handled and propogation stops. If there is no handler or the event returns false or nothing then the next handler is checked.

!!! note "Note"

    It is only after all of these handlers are checked and none of them handle the low-level input that the input is convered into a [game command](../game.md#game-commands).

### `handler:on_key_pressed(key, modifiers)`

Called when the user presses a keyboard key while the handler is active.

`key` (string)
: Name of the raw key that was pressed.

`modifiers` (table)
: A table whose keys indicate what modifiers were down during the event. Possible table keys are `"shift"`, `"control"` and `"alt"`. Table values are `true`.

Return value (boolean)
: Indicates whether the event was handled. If you return `true`, the event won't be propagated to other objects. If you return `false` or nothing, the event will continue its propagation.

!!! note "Note"

    This event indicates the raw keyboard key pressed. If you want the corresponding character instead (if any), see [`handler:on_character_pressed()`](#handleron_character_pressedcharacter). If you want the corresponding higher-level game command (if any), check if the handler also supports an `on_command_pressed` event.

### `handler:on_key_released(key)`

Called when the user releases a keyboard key while the handler is active.

`key` (string)
: Name of the raw key that was released.

Return value (boolean)
: Indicates whether the event was handled. If you return `true`, the event won't be propagated to other objects. If you return `false` or nothing, the event will continue its propagation.

!!! note "Note"

    This event indicates the raw keyboard key released. If you want the corresponding higher-level game command (if any), check if the handler also supports an `on_command_released` event.

### `handler:on_character_pressed(character)`

Called when the user enters text while the handler is active.

`character` (string)
: A utf-8 string representing the character that was pressed.

Return value (boolean)
: Indicates whether the event was handled. If you return `true`, the event won't be propagated to other objects. If you return `false` or nothing, the event will continue its propagation.

!!! note "Note"

    When a character key is pressed, two events are called: [`handler:on_key_pressed()`](#handleron_key_pressedkey-modifiers) (indicating the raw key) and [`handler:on_character_pressed()`](#handleron_character_pressedcharacter) (indicating the utf-8 character).

    If your script needs to input text from the user, [`handler:on_character_pressed()`](#handleron_character_pressedcharacter) is what you want because it considers the keyboard's layout and gives you international utf-8 strings.

### `handler:on_joypad_button_pressed(button, joypad)`

Called when the user presses a joypad button while the handler is active.

`button` (string)
: name of the button that was pressed.

`joypad` ([joypad](./joypad.md))
: Joypad that triggered the event

Return value (boolean)
: Indicates whether the event was handled. If you return `true`, the event won't be propagated to other objects. If you return `false` or nothing, the event will continue its propagation.

### `handler:on_joypad_button_released(button, joypad)`

Called when the user releases a joypad button while the handler is active.

`button` (string)
: Name of the button that was released.

`joypad` ([joypad](./joypad.md))
: Joypad that triggered the event

Return value (boolean)
: Indicates whether the event was handled. If you return `true`, the event won't be propagated to other objects. If you return `false` or nothing, the event will continue its propagation.

### `handler:on_joypad_axis_moved(axis, state, joypad)`

Called when the user moves a joypad axis while the handler is active.

`axis` (string)
: Name of the axis that was moved. Usually, `0` is an horizontal axis and `1` is a vertical axis.

`state` (number)
: The new state of the axis that was moved. `-1` means left or up, `0` means centered and `1` means right or down.

`joypad` ([joypad](./joypad.md))
: Joypad that triggered the event

Return value (boolean)
: Indicates whether the event was handled. If you return `true`, the event won't be propagated to other objects. If you return `false` or nothing, the event will continue its propagation.

### `handler:on_joypad_hat_moved(hat, direction8, joypad)`

!!! warning "Deprecated"

    This event is deprecated since Solarus 2.0. Hat directions are reported as buttons. Called when the user moves a joypad hat while the handler is active.

`hat` (number)
: Index of the hat that was moved.

`direction8` (number)
: The new direction of the hat. `-1` means that the hat is centered. `0` to `7` indicates that the hat is in one of the eight main directions.

`joypad` ([joypad](./joypad.md))
: Joypad that triggered the event

Return value (boolean)
: Indicates whether the event was handled. If you return `true`, the event won't be propagated to other objects. If you return `false` or nothing, the event will continue its propagation.

### `handler:on_mouse_pressed(button, x, y)`

Called when the user presses a mouse button while the handler is active.

`button` (string)
: Name of the mouse button that was pressed. Possible values are `"left"`, `"middle"`, `"right"`, `"x1"` and `"x2"`.

`x` (integer)
: The x position of the mouse in [quest size](../video.md#solvideoget_quest_size) coordinates.

`y` (integer)
: The y position of the mouse in [quest size](../video.md#solvideoget_quest_size) coordinates.

Return value (boolean)
: Indicates whether the event was handled. If you return `true`, the event won't be propagated to other objects. If you return `false` or nothing, the event will continue its propagation.

### `handler:on_mouse_released(button, x, y)`

Called when the user releases a mouse button while the handler is active.

`button` (string)
: Name of the mouse button that was released. Possible values are `"left"`, `"middle"`, `"right"`, `"x1"` and `"x2"`

`x` (integer)
: The x position of the mouse in [quest size](../video.md#solvideoget_quest_size) coordinates.

`y` (integer)
: The y position of the mouse in [quest size](../video.md#solvideoget_quest_size) coordinates.

Return value (boolean)
: Indicates whether the event was handled. If you return `true`, the event won't be propagated to other objects. If you return `false` or nothing, the event will continue its propagation.

### `handler:on_finger_pressed(finger, x, y, pressure)`

Called when the user presses a finger while the handler is active.

`finger` (integer)
: ID of the finger that was pressed.

`x` (integer)
: The x position of the finger in [quest size](../video.md#solvideoget_quest_size) coordinates.

`y` (integer)
: The y position of the finger in [quest size](../video.md#solvideoget_quest_size) coordinates.

`pressure` (number)
: The pressure of the finger, normalized between `0` and `1`.

Return value (boolean)
: Indicates whether the event was handled. If you return `true`, the event won't be propagated to other objects. If you return `false` or nothing, the event will continue its propagation.

### `handler:on_finger_released(finger, x, y, pressure)`

Called when the user releases a finger while the handler is active.

`finger` (integer)
: ID of the finger that was pressed.

`x` (integer)
: The x position of the finger in [quest size](../video.md#solvideoget_quest_size) coordinates.

`y` (integer)
: The y position of the finger in [quest size](../video.md#solvideoget_quest_size) coordinates.

`pressure` (number)
: The pressure of the finger, normalized between `0` and `1`.

Return value (boolean)
: Indicates whether the event was handled. If you return `true`, the event won't be propagated to other objects. If you return `false` or nothing, the event will continue its propagation.

### `handler:on_finger_moved(finger, x, y, dx, dy, pressure)`

Called when the user moves a finger while the handler is active.

`finger` (integer)
: ID of the finger that was pressed.

`x` (integer)
: The x position of the finger in [quest size](../video.md#solvideoget_quest_size) coordinates.

`y` (integer)
: The y position of the finger in [quest size](../video.md#solvideoget_quest_size) coordinates.

`dx` (integer)
: The horizontal distance moved by finger in [quest size](../video.md#solvideoget_quest_size) coordinates.

`dy` (integer)
: The vertical distance moved by finger in [quest size](../video.md#solvideoget_quest_size) coordinates.

`pressure` (number)
: The pressure of the finger, normalized between `0` and `1`.

Return value (boolean)
: Indicates whether the event was handled. If you return `true`, the event won't be propagated to other objects. If you return `false` or nothing, the event will continue its propagation.
