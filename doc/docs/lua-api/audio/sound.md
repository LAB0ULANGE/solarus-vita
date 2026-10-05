# Sound

You can create, play, or pause sound effects through `sol.sound`.

An object of type `sound` represents a sound effect with its current playing state. Multiple sounds effects can be played, paused, resumed or stopped independently, even from the same sound file.

## Functions of `sol.sound`

### `sol.sound.create(sound_id)`

Creates a new sound effect but does not play it. Generates a Lua error if the sound file does not exist.

Unlike music files, sound files are entirely read before being played.

`sound_id` (string)
: Name of the sound file to read, relative to the `sounds` directory and without extension. Currently, only OGG Vorbis (`.ogg`) sound files are supported.

### `sol.sound.stop_all()`

Stops all currently playing sounds. This is useful if you started multiple looped sounds and want them to stop all at once (for example when a map finishes).

## Methods of `sol.sound`

### `sound:play()`

Starts playing the sound effect.

### `sound:stop()`

Stops playing the sound effect.

### `sound:is_playing()`

Returns whether the sound effect is currently playing.

Return value (boolean)
: `true` if the sound is playing, `false` otherwise.

### `sound:is_paused()`

Returns whether the sound effect is currently paused.

Return value (boolean)
: `true` if the sound is paused, `false` otherwise.

### `sound:set_paused([paused])`

Pauses or resumes the sound effect.

`paused` (boolean, optional, default: `true`)
: `true` to pause the sound, `false` to unpause it.

### `sound:is_looped()`

Returns whether the sound effect should be played in a loop.

Return value (boolean)
: `true` if the sound is played in a loop, `false` otherwise.

### `sound:set_looped([looped])`

Sets that the sound effect should be played in a loop.

`looped` (boolean, optional, default: `true`)
: `true` to set the loop, `false` to clear the loop.

!!! note "Note"

    Sounds (especially when they are looped) are played accross maps and games. You will certainly want to stop all playing sound when the hero is leaving a map or exiting a game. For this, use the function [`sol.sound.stop_all()`](#solsoundstop_all).

### `sound:get_volume()`

Returns the volume of this sound effect. This is relative to the global [sound volume](./index.md#solaudioget_sound_volume).

Return value (number)
: The current volume of this sound effect, as an integer between `0` (mute) and `100` (full volume).

### `sound:set_volume(volume)`

Sets the volume of this sound effect. This is relative to the global [sound volume](./index.md#solaudioset_sound_volumevolume).

`volume` (number)
: The new volume of this sound effect, as an integer between `0` (mute) and `100` (full volume).

### `sound:get_pan()`

Returns the pan value of this sound effect.

Return value (number)
: The current pan of this sound effect, as a number between `-1.0` (left) and `1.0` (right).

### `sound:set_pan(pan)`

Sets the pan of this sound effect. This has no effect on stereo sounds.

`pan` (number)
: The new pan of this sound effect, as a number between `-1.0` (left) and `1.0` (right).

### `sound:get_pitch()`

Returns the pitch value of this sound effect.

Return value (number)
: The current pitch of this sound effect, as a number between `0.5` and `2.0`.

### `sound:set_pitch(pitch)`

Sets the pitch of this sound effect. The value acts as a sample rate multiplier.

`pitch` (number)
: The new pitch of this sound effect, as a number between `0.5` and `2.0`.
