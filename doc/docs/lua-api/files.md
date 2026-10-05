# Files

## Overview

This module provides functions to manually read and write files from the quest data directory and from quest write directory.

Data files of the quest are located either in a `data/` directory or in a `data.solarus` or `data.solarus.zip` archive. Files that you write are in the [write directory](./general-features.md#solmainset_quest_write_dirquest_write_dir) of the quest.

This module encapsulates the transparent search of these files in both locations. You can access them independently of their actual place.

However, note that all built-in file accesses in Solarus (loading maps, loading sprites, reading and writing savegames, etc.) already implement this transparent search. You will only need this module if you implement your custom data files or if you have some custom files to save.

In all functions of `sol.file`, the notation for the directory separator is always `"/"` (slash), no matter the underlying platform.

You can read and write files in the quest write directory, and you can only read files in the data directory or archive. If a file to read exists in both locations, the quest write directory is given priority.

Examples of use:

- You can implement **your own savegame system** if [the built-in one](./game.md#gamesave) does not fit your needs.
- You can save a file that contain **some user settings**, and have a default version of that file in the data directory. Since the quest write directory has the priority, the saved version will be used if any.
- You can save **dynamic content** like maps automatically generated. Since data files are also read from the quest write directory, map files can be loaded from there.
- You can make an **in-game dialog editor** for translators.

## Functions of `sol.file`

### `sol.file.open(file_name, [mode])`

Same as [`io.open()`](http://www.lua.org/manual/5.1/manual.html#pdf-io.open), but relative to the quest write directory or to the data directory.

If a valid quest write directory is set (in your [quest.dat file](../files-specs/properties-file.md) or by calling [`sol.main.set_quest_write_dir()`](./general-features.md#solmainset_quest_write_dirquest_write_dir)), that directory is tried first. Then, the `data` directory of your quest is tried if the mode is `"r"`(read mode).

This function just calls `io.open()` with the actual path and the mode as parameters, and returns its results.

`file_name` (string)
: Name of the file to open, relative to the quest write directory or to the data directory.

`mode` (string, optional)
: Opening mode (see the Lua documentation of [`io.open()`](http://www.lua.org/manual/5.1/manual.html#pdf-io.open) ).

Return value (file or `nil`+string)
: The file object created, or `nil` plus an error message in case of failure. The file value is the standard Lua file object as returned by `io.open()`, and you can then use all Lua file functions (file:read(), file:write(), file:lines(), file:close(), etc.).

### `sol.file.exists(file_name)`

Returns whether the specified file or directory exists in the quest write directory or in the data directory.

If a valid quest write directory is set (in your [quest.dat file](../files-specs/properties-file.md) or by calling [`sol.main.set_quest_write_dir()`](./general-features.md#solmainset_quest_write_dirquest_write_dir)), that directory is tried first. Then, the `data` directory of your quest is tried.

`file_name` (string)
: Name of the file to test, relative to the [quest write directory](./general-features.md#solmainget_quest_write_dir) or to the data directory.

Return value (boolean)
: `true` if there exists a file or directory with this name.

### `sol.file.is_dir(file_name)`

Returns whether the specified file exists and is a directory.

If a valid quest write directory is set (in your [quest.dat file](../files-specs/properties-file.md) or by calling [`sol.main.set_quest_write_dir()`](./general-features.md#solmainset_quest_write_dirquest_write_dir)), that directory is tried first. Then, the `data` directory of your quest is tried.

`file_name` (string)
: Name of the file to test, relative to the [quest write directory](./general-features.md#solmainget_quest_write_dir) or to the data directory.

Return value (boolean)
: `true` if there exists a directory with this name.

### `sol.file.remove(file_name)`

Deletes a file or a directory from the quest write directory.

A valid quest write directory must be set (in your [quest.dat](../files-specs/properties-file.md) file or by calling [`sol.main.set_quest_write_dir()`](./general-features.md#solmainset_quest_write_dirquest_write_dir)), otherwise this function generates a Lua error.

`file_name` (string)
: Name of the file to delete, relative to the [quest write directory](./general-features.md#solmainget_quest_write_dir). If it is a directory, it must be empty before you delete it.

Return value (boolean or `nil`+string)
: `true` in case of success, `nil` plus an error message in case of failure.

### `sol.file.rename(old_file_name, new_file_name)`

Renames a file or a directory in the quest write directory.

A valid quest write directory must be set (in your [quest.dat](../files-specs/properties-file.md) file or by calling [`sol.main.set_quest_write_dir()`](./general-features.md#solmainset_quest_write_dirquest_write_dir)), otherwise this function generates a Lua error.

`old_file_name` (string)
: Name of the file to rename, relative to the [quest write directory](./general-features.md#solmainget_quest_write_dir).

`new_file_name` (string)
: New name to set, relative to the [quest write directory](./general-features.md#solmainget_quest_write_dir).

Return value (boolean or `nil`+string)
: `true` in case of success, `nil` plus an error message in case of failure.

### `sol.file.mkdir(dir_name)`

Creates a directory in the quest write directory.

A valid quest write directory must be set (in your [quest.dat](../files-specs/properties-file.md) file or by calling [`sol.main.set_quest_write_dir()`](./general-features.md#solmainset_quest_write_dirquest_write_dir)), otherwise this function generates a Lua error.

`dir_name` (string)
: Name of the directory to create, relative to the [quest write directory](./general-features.md#solmainget_quest_write_dir).

Return value (boolean or `nil`+string)
: `true` in case of success, `nil` plus an error message in case of failure.

### `sol.file.list_dir(dir_name)`

Returns the list of files in a directory.

`dir_name` (string)
: Name of the directory to explore, relative to the [quest write directory](./general-features.md#solmainget_quest_write_dir) or to the data directory.

Return value (table or `nil`)
: An array with the name of files and directories inside the directory. Names are ordered alphabetically. Returns `nil` if the given name is not a directory or does not exist.
