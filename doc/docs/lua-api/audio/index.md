# Audio

You can play musics and sound effects through `sol.audio`.

## Functions of `sol.audio`

### `sol.audio.play_sound(sound_id)`

Plays a sound effect. Generates a Lua error if the sound does not exist.

Multiple sounds can be played in parallel. But to get more control of your sound effect being played, especially if you want to pause or stop it, you can use the more powerful [sol.sound module](./sound.md) instead of this function.

Unlike music files, sound files are entirely read before being played.

This is equivalent to [`sol.sound.create(sound_id):play()`](./sound.md#solsoundcreatesound_id).

`sound_id` (string)
: Name of the sound file to play, relative to the `sounds` directory and without extension. Currently, only OGG Vorbis (`.ogg`) sound files are supported.

### `sol.audio.preload_sounds()`

Loads all sounds effects into memory for faster future access.

!!! warning "Deprecated"

    This method is deprecated since Solarus 2.0. Sound effects are always automatically preloaded now.

### `sol.audio.play_music(music_id, [action])`

Plays a music. If the music does not exist, a Lua error is generated.

Only one music can be played at a time. If the same music was already playing, it continues normally and does not
restart. If a different music was already playing, it is stopped and replaced by the new one.

When the music reaches the end, the `action` parameter indicates what to do next. The default behavior is to loop from the beginning.

However, some music files already have their own loop internal loop information. Such musics are able to loop to a specific point rather than
to the beginning. Since they already loop forever internally, they don't have an end and the `action` parameter has no effect on them.

See [Music Loop Settings](../../files-specs/audio/musics.md#loop-settings) to know how Solarus supports internal loop information for each format.

`music_id` (string or `nil`)
: Name of the music file to play, relative to the `musics` directory and without extension. The following extensions will be tried in this order: `.ogg`, `.it` and `.spc`. Passing `nil` stops playing any music (the second parameter has no effect in this case).

    If you set the music name to the same music that is already playing, or to the special value `"same"`, then this function does nothing: the music keeps playing (it does not restart) and the second parameter is ignored.

`action` (function or boolean, optional, default: `true`)
: What to do when the music finishes (reaches its end).

    - A **boolean** value indicates whether or not the music should loop.
    - A **function** value indicates a custom action (and implies no loop). It will be called when the music finishes. This allows you to perform an action of your choice, like playing another music.

        Note however that there might be some small delay between the exact end of the music and the time when your function is called, because the music can end between simulation frames but your function will be called the frame after that.

### `sol.audio.get_music()`

Returns the name of the music currently playing.

Return value (string or `nil`)
: Name of the music file currently playing, relative to the `musics` directory and without extension. Returns `nil` if no music is playing.

### `sol.audio.stop_music()`

Stops playing music. This function has no effect if no music was playing.

This is equivalent to `sol.audio.play_music("none")`.

### `sol.audio.get_sound_volume()`

Returns the global volume of sound effects.

Return value (number)
: The global volume of sound effects, as an integer between `0` (mute) and `100` (full volume).

### `sol.audio.set_sound_volume(volume)`

Sets the default volume of sound effects. Note that individual sound effects have their own volume relative to this global sound volume.

`volume` (number)
: The new default volume of sound effects, as an integer between `0` (mute) and `100` (full volume).

### `sol.audio.get_music_volume()`

Returns the current volume of musics. This volume applies to all musics played by [`sol.audio.play_music()`](#solaudioplay_musicmusic_id-action).

Return value (number)
: The current volume of musics, as an integer between `0` (no sound effects) and `100` (full volume).

### `sol.audio.set_music_volume(volume)`

Sets the volume of musics. This volume applies to all musics played by [`sol.audio.play_music()`](#solaudioplay_soundsound_id).

`volume` (number)
: The new volume of musics, as an integer between `0` (no music) and `100` (full volume).

### `sol.audio.get_music_format()`

Returns the format of the music currently playing.

Return value (string or `nil`)
: Returns `nil` if no music is playing. Format of the music:

    - `"ogg"`
    - `"it"`
    - `"spc"`

### `sol.audio.get_music_num_channels()`

Returns the number of channels of the current `.it` music. This function is only supported for `.it` musics.

Return value (number or `nil`)
: Number of channels of the music. Returns `nil` if the current music format is not `.it`.

### `sol.audio.get_music_channel_volume(channel)`

Returns the volume of notes of a channel for the current `.it` music. This function is only supported for `.it` musics.

`channel` (number)
: Index of a channel (the first one is zero).

Return value (number or `nil`)
: Volume of the channel. Returns `nil` if the current music format is not `.it`.

!!! note "Note"

    The channel should have the same volume for all its notes. Otherwise, calling this function does not make much sense.

### `sol.audio.set_music_channel_volume(channel, volume)`

Sets the volume of all notes of a channel for the current `.it` music. This function has no effect for musics other than `.it`.

`channel` (number)
: Index of a channel (the first one is zero).

`volume` (number)
: The volume to set.

### `sol.audio.get_tempo()`

Returns the tempo of the current `.it` music. This function is only supported for `.it` musics.

Return value (number or `nil`)
: Tempo of the music. Returns `nil` if the current music format is not `.it`.

### `sol.audio.set_tempo(tempo)`

Sets the tempo of the current `.it` music. This function is only supported for `.it` musics.

`tempo` (number)
: Tempo to set.
