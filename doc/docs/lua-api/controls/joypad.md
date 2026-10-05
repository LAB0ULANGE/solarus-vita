# Joypad

## Overview

The `sol.joypad` type provides an abstraction over the plugged joypads, it uses [SDL's `GameController` API](https://wiki.libsdl.org/SDL2/CategoryGameController) to map any compatible joypad to standard buttons resembling those of the Xbox.

Joypad instances can be fetched using the `sol.input` module.

## Axes and Buttons Names

| Kind    | Names                                                                                                                                                                            |
| ------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| buttons | `a`, `b`, `x`, `y`,</br>`back`, `guide`, `start`,</br>`left_stick`, `right_stick`,</br>`left_shoulder`, `right_shoulder`, </br>`dpad_up`, `dpad_down`, `dpad_left`, `dpad_right` |
| axes    | `left_x`, `left_y`, `right_x`, `right_y`,</br>`trigger_left`, `trigger_right`                                                                                                    |

## Methods of the type `joypad`

### `joypad:get_name()`

Returns the name of this joypad.

Return value (string)
: The name if the joypad.

### `joypad:has_rumble()`

Tells whether this joypad has rumble support.

Return value (boolean)
: `true` if the joypad has rumble support.

### `joypad:rumble(low_frequency_intensity, high_frequency_intensity, duration)`

Makes the joypad rumble. Calling this method with intensity of `0` stops the rumbling. Each call to this method cancels any previous rumbling.

`low_frequency_intensity` (number)
: Intensity for the low frequency rumble (left) (0->1).

`high_frequency_intensity` (number)
: Intensity for the high frequency rumble (right) (0->1).

`duration` (number)
: Duration of the rumble in milliseconds.

### `joypad:get_axis(axis)`

Returns the value of a joypad axis (-1 -> 1).

`axis` (string)
: Axis name (see [axes](#axes-and-buttons-names)).

Return value (number)
: The value of a joypad axis (-1 -> 1).

### `joypad:is_button_pressed(button)`

Returns `true` if the button is pressed, `false` otherwise.

`button` (string)
: Button name (see [button](#axes-and-buttons-names)).

Return value (boolean)
: `true` if the button is pressed, `false` otherwise.

### `joypad:is_attached()`

Returns `true` if the joypad is still attached.

Return value (boolean)
: `true` if the joypad is still attached, `false` otherwise.

## Events of the type `joypad`

### `joypad:on_button_pressed(button)`

Called when a button of this joypad is pressed.

`button` (string)
: Name of the pressed button.

### `joypad:on_button_released(button)`

Called when a button of this joypad is released.

`button` (string)
: Name of the released button.

### `joypad:on_axis_moved(axis, value)`

Called when a axis of this joypad moves.

`axis` (string)
: Name of the axis.

`value` (number)
: New value of the axis.

### `joypad:on_removed()`

Called when the joypad is removed.
