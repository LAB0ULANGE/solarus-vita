# Musics

## Supported Formats

The `musics` directory contains all musics files used by the quest. For now, the following formats are recognized:

| Format | Description                   |
| ------ | ----------------------------- |
| OGG    | Ogg Vorbis audio file         |
| IT     | Impulse Tracker Module        |
| SPC    | Super NES original music file |

!!! warning "SPC Format not recommended"

    The SPC format is supported but not recommended because it is slow to decode. Therefore, it is recommended to convert SPC musics to IT musics.

Future releases may support more music formats.

## Loop Settings

When starting a map, its music is automatically played and will loop to the beginning if it finishes. When you play a music from a script (using [`sol.audio.play_music()`](../../lua-api/audio/index.md#solaudioplay_musicmusic_id-action)), you can specify the action to do when the music finishes. By default, this action is also to loop the music to the beginning.

However, some musics already have their own loop internal loop. It means that they don't finish (they loop by themselves) so the rules above have no effect on them. Instead, such musics are able to loop to a specific point rather than to the beginning.

### SPC Looping

This is the case of SPC musics because of their special emulation format. Some of them simply play forever, other play an empty sound forever after some point, but SPC musics have no end.

### IT Looping

This can also be the case of IT musics if they contain internal loop information to play forever.

### OGG Looping

For OGG musics, Solarus detects the metadata tags `LOOPSTART,` `LOOPEND` and `LOOPLENGTH` These tags are all optional. They should be set to a number of PCM samples.

There will be an internal loop if `LOOPSTART` is specified. You can then use either `LOOPEND` or `LOOPLENGTH` to indicate from where you want the music to loop (by default: from the end of the file).

If `LOOPEND` is specified, the music will loop from position `LOOPEND` to position `LOOPSTART`.

Otherwise, if `LOOPLENGTH` is specified, the music will loop from position `LOOPSTART + LOOPLENGTH` to position `LOOPSTART`
