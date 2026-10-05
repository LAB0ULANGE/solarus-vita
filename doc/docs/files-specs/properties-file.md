# Quest Properties File

## Overview

Some general information regarding your quest are stored in the quest properties file `quest.dat` This include information that the Solarus GUI can use to show a description of each quest before the user plays, as well as technical settings indicating how your quest should run.

`quest.dat` is mandatory: if this file does not exist, the engine considers that there is no quest in the directory.

## Syntax of the Quest Properties File

Solarus Editor fully supports the edition of the quest properties file. You should not have to edit `quest.dat` by hand unless you know what you are doing.

`quest.dat` is a text file encoded in UTF-8.

The sequence of characters `--` (two dashes) marks the beginning of a comment. After them, the rest of the line is ignored by the engine. Empty lines are also ignored.

The definition of your quest properties starts with `quest{` and ends with `}` Each property is declared with the syntax `key = value` and separated with commas. It is allowed to have an extra comma after the last property. String values should be enclosed within double quotes, except when specified otherwise.

The recognized properties are listed below. Only `solarus_version` and `title` are mandatory, but `write_dir` is also required if you want to be able to use savegames.

`solarus_version` (string)
: Format of the engine your data files are compatible with. This string should be a major and minor version, for example "2.0". You can also indicate a full version string, including the patch number (like "2.0.0") but the patch number will be ignored. Patch versions are ignored because they don't break compatibility.

`title` (string)
: Name of your quest.

`short_description` (string, optional)
: A one-line description of the quest.

`long_description` (multi-line string, optional)
: A longer description of the quest. The text should be enclosed between `[[` and `]],` which is the notation for multiline strings.

`author` (string, optional)
: Person or team who created the quest.

`quest_version` (string, optional)
: Current version of your quest.

`initial_release_date` (string, optional)
: Date for your quest first release (YYYY-MM-DD). Example: `"2025-03-31"`.

`release_date` (string, optional)
: Date of your quest latest release (YYYY-MM-DD).

`website` (string, optional)
: URL of your website.

`write_dir` (string, optional)
: Directory where Solarus will write savegames and setting files for your quest. It will be a subdirectory of `'$HOME/.solarus/'`, automatically created by the engine. Its name should identify your quest, to avoid confusion with other Solarus quests that might also be installed on the user's machine. You must define it before you can use savegames or setting files. It is like a unique identifier for your quest.

`subpixel_camera` (boolean, optional)
: `true` to enable subpixel smoothing of camera movement. The default value is `false`

`dynamic_timestep` (boolean, optional)
: `true` to synchronize the main loop update rate with the video refresh rate. The default value is `false`

`normal_quest_size` (string, optional)
: Usual size of the logical screen in pixels, as two integer values separated by `"x"` Example: `"320x240"` The default value is `"320x240"`

`min_quest_size` (string, optional)
: Minimum size of the logical screen in pixels. Example: `"320x200"` No value means the same value as `normal_quest_size`

`max_quest_size` (string, optional)
: Maximum size of the logical screen in pixels. Example: `"400x240"` No value means the same value as `normal_quest_size` Note: For the vast majority of quests, `min_quest_size` and `max_quest_size` are the same as `normal_quest_size` Changing them is only useful for quests that want to support a range of logical screen sizes.

`license` (string, optional)
: License of your quest.

`languages` (table, optional)
: Languages your quest is translated in. Use ISO 639 standard nomenclature strings. Examples: `"en"`, `"fr"`, `"en"`.

`min_players` (number, optional)
: Minimum player count (positive integer greater than zero) necessary to play your quest. The default value is `1`.

`max_players` (number, optional)
: Maximum player count necessary to play your quest. Should be equal or greater than `min_players`. The default value is `1`.

`genres` (table, optional)
: Which genre(s) your quest can be classified in. Use one string per genre.

`age` (string, optional)
: The target audience of your quest. Possible values: `"all"` (no age restriction), `"warning"` (potentially violent content) or `"restricted"` (for mature audience only).

`controls` (number, optional)
: Types of controls supported by your quest. Add the number value to show that your quest supports the corresponding control type. Example: a quest that supports keyboard and joypad would have `1`+`4`=`5`.

| Control  | Value |
| -------- | ----- |
| Keyboard | `1`   |
| Mouse    | `2`   |
| Joypad   | `4`   |
| Other    | `8`   |

## About the Quest Size

`normal_quest_size,` `min_quest_size` and `max_quest_size` define the range of possible sizes of the game screen. This is the logical size of the game area. It represents how much content the user can see on the map when playing your quest. Allowing a range of sizes instead of only one size improves the portability: each system and each user can choose a size that occupies the full ratio of the screen instead of having black bars. However, be aware that if you allow a range, some users will be able to see more game content than others!

At compilation time, systems can specify their preferred quest size. It will be used if it is in the range of sizes allowed by your quest. And the quest size can also be overriden at runtime with a command-line option.

At runtime, you are guaranteed that the actual quest size will be in the range allowed in this quest properties file.

!!! note "Note"

    The quest size is the logical size of your quest and it never changes at runtime. Don't confuse it with the size of the window.

    The actual window shown to the user may be bigger or smaller than that. It can resized and it can also be in fullscreen mode.

    When the window size is different from the quest size, the quest image is scaled to fit the window. Black bars are added if necessary to keep the correct ratio.

## Example

Example of `quest.dat` file:

```lua
quest {
  solarus_version = "2.0",
  title = "My incredible quest",
  write_dir = "my_incredible_quest",
  short_description = "This is the best game ever",
  long_description = [[Travel the world and save the princess.
You will encounter great characters, epic battles and awesome puzzles.]],
  author = "John Doe",
  quest_version = "1.0",
  initial_release_date = "20250401",
  release_date = "",
  website = "http://www.example.com",
  normal_quest_size = "320x240",
  min_quest_size = "320x240",
  max_quest_size = "320x240",
  license = "GPL v3",
  languages = { "fr", "en", "es" },
  min_players = 1,
  max_players = 4,
  genres = { "Action", "Aventure" },
  age = "all",
  controls = 7,
  dynamic_timestep = false,
  subpixel_camera = false,
}
```
