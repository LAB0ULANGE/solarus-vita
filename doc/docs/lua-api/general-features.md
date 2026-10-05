# General Features

## Overview

`sol.main` contains general features and utility functions that are global to the execution of the program, no matter if a game or some menus are running.

## Functions of `sol.main`

### `sol.main.get_solarus_version()`

Returns the current Solarus version, which includes three numbers: `x.y.z`, where `x` is the major version, `y` is the minor version and `z` is the patch version.

Changes of major and minor versions may introduce some incompatibilities in the Lua API. Patch versions are only for bug fixing and never introduce incompatibilities. See the [migration guide](http://wiki.solarus-games.org/doku.php?id=migration_guide) for instructions about how to upgrade your quest to the latest version.

Return value (string)
: The Solarus version.

### `sol.main.get_quest_version()`

Returns the version of the currently running quest as specified in the [Quest Properties file](../files-specs/properties-file.md).

Return value (string or `nil`)
: The quest version, or `nil` if no quest version was set the quest properties.

### `sol.main.get_quest_format()`

Returns the Solarus compatibility format of this quest. This corresponds to a version of Solarus with major and minor version numbers (no patch number), for example `"1.5"`.

Return value (string)
: The format of this quest.

### `sol.main.load_file(script_name)`

Loads a Lua file (but does not run it).

This function is a replacement to the usual Lua function `loadfile()`. The difference is that it looks for a file in the quest tree (which may be a directory or an archive) and also in the [quest write directory](#solmainset_quest_write_dirquest_write_dir). The quest write directory is tried first.

`script_name` (string)
: Name of the Lua file to load (with or without extension), relative to the data directory or the write directory of your quest.

Return value (function or `nil`)
: A function representing the chunk loaded, or `nil` if the file does not exist or could not be loaded as Lua.

!!! note "Note"

    Keep in mind that Lua files, as all data files of your quest, may be located inside an archive instead of being regular files.

    Therefore, to run them, you cannot use usual Lua functions like `loadfile()` or `dofile()` Use [`sol.main.load_file()`](#solmainload_filescript_name) and [`sol.main.do_file()`](#solmaindo_filescript_name) instead to let Solarus determine how to locate and open the file.

    Note however that `require()` can be used normally because it is a higher-level function. Indeed, a specific loader is automatically set by the engine so that `require()` looks in the archive if necessary and in the quest write directory.

    `require()` is the recommended way to load code from another file, because unlike [`sol.main.load_file()`](#solmainload_filescript_name) and [`sol.main.do_file()`](#solmaindo_filescript_name), it does not parses the file again every time you call it.

### `sol.main.do_file(script_name)`

Loads and runs a Lua file into the current context.

This function is a replacement to the usual Lua function `dofile()`. The difference is that it looks for a file in the quest tree (which may be a directory or an archive) and also in the [quest write directory](#solmainset_quest_write_dirquest_write_dir). The quest write directory is tried first. The file must exist.

This function is equivalent to `sol.main.load_file(script_name)`. Use [`sol.main.load_file()`](#solmainload_filescript_name) explicitly if you need to check the existence of the file or to use parameters and return values.

`script_name` (string)
: Name of the Lua file to load (with or without extension), relative to the data directory or the write directory of your quest.

!!! note "Note"

    `require()` is the recommended way to load code from another file, because unlike [`sol.main.load_file()`](#solmainload_filescript_name) and [`sol.main.do_file()`](#solmaindo_filescript_name), it does not parses the file again every time you call it.

### `sol.main.reset()`

Resets the whole program after the current cycle. Lua will be entirely shut down and then initialized again.

### `sol.main.exit()`

Exits the program after the current cycle.

### `sol.main.get_elapsed_time()`

Returns the simulated time elapsed since Solarus was started. This corresponds to real time, unless the system is too slow to play at normal speed.

Return value (number)
: The number of simulated milliseconds elapsed since the beginning of the program.

!!! note "Note"

    This time is not reset when you call [`sol.main.reset()`](#solmainreset).

### `sol.main.get_quest_write_dir()`

Returns the subdirectory where files specific to the quest are saved, like savegames and settings. The quest write directory is specified in your [quest.dat](../files-specs/properties-file.md) file and may be changed dynamically with [`sol.main.set_quest_write_dir()`](#solmainget_quest_write_dir).

Return value (string or `nil`)
: The quest write directory, relative to the Solarus write directory, or `nil` if it was not set.

### `sol.main.set_quest_write_dir(quest_write_dir)`

Changes the subdirectory where files specific to the quest are saved, like savegames and settings.

Note that the quest write directory can already be specified in your [quest.dat](../files-specs/properties-file.md) file. You usually don't have to call this function, unless you need to change it dynamically for some reason.

`quest_write_dir` (string or `nil`)
: The quest write directory, relative to the Solarus write directory, or `nil` to unset it.

### `sol.main.load_settings([file_name])`

Loads and applies the built-in settings from a file previously saved with [`sol.main.save_settings()`](#solmainsave_settingsfile_name). Settings from the file include user preferences such as the language, the video mode and the audio volume.

Note that all these settings can already be modified individually with the Lua API, so you can either use this function or implement something more fitted to your needs.

A valid quest write directory must be set (in your [quest.dat](../files-specs/properties-file.md) file or by calling [`sol.main.set_quest_write_dir()`](#solmainset_quest_write_dirquest_write_dir)), otherwise this function generates a Lua error.

`file_name` (string, optional, default: `"settings.dat"`)
: Settings file to read, relative to the quest write directory.

Return value (boolean)
: `true` if settings were successfully loaded and applied.

!!! note "Note"

    When you launch a quest from Solarus Launcher, user preferences that are currently set in the app are automatically written to `settings.dat` before the quest starts and are automatically applied as soon as the quest starts.

### `sol.main.save_settings([file_name])`

Saves the current built-in settings into a file. This file can be reloaded later with [`sol.main.load_settings()`](#solmainload_settingsfile_name) to restore the saved settings. Settings saved include user preferences such as the current language, the current video mode and the current audio volume.

Note that all these settings can already be modified individually with the Lua API, so you can either use this function or implement something more fitted to your needs.

A valid quest write directory must be set (in your [quest.dat](../files-specs/properties-file.md) file or by calling [`sol.main.set_quest_write_dir()`](#solmainset_quest_write_dirquest_write_dir)), otherwise this function generates a Lua error.

`file_name` (string, optional, default: `"settings.dat"`)
: Settings file to read, relative to the quest write directory.

Return value (boolean)
: `true` if settings were successfully saved.

!!! note "Note"

    When you launch a quest from the Solarus GUI, user preferences that are currently set in the menus of the GUI are automatically written to `settings.dat` before the quest starts and are automatically applied as soon as the quest starts.

### `sol.main.get_distance(x1, y1, x2, y2)`

Utility function that computes the distance in pixels between two points.

`x1` (number)
: X coordinate of the first point.

`y1` (number)
: Y coordinate of the first point.

`x2` (number)
: X coordinate of the second point.

`y2` (number)
: Y coordinate of the second point.

Return value (number)
: The Euclidean distance in pixels, rounded down to the nearest integer.

### `sol.main.get_angle(x1, y1, x2, y2)`

Utility function that computes the angle in radians between the X axis and the specified vector.

`x1` (number)
: X coordinate of the first point.

`y1` (number)
: Y coordinate of the first point.

`x2` (number)
: X coordinate of the second point.

`y2` (number)
: Y coordinate of the second point.

Return value (number)
: The angle in radians between the x axis and this vector.

### `sol.main.get_resource_ids(resource_type)`

Returns an array of all resource elements ids of the given type declared in the [quest database](../files-specs/database-file.md).

`resource_type` (string)
: Name of a resource type. Must be one of:

    - `"enemy"`
    - `"entity"`
    - `"font"`
    - `"item"`
    - `"language"`
    - `"map"`
    - `"music"`
    - `"shader"`
    - `"sound"`
    - `"sprite"`
    - `"tileset"`

Return value (table)
: An array of all elements of this resource type declared in the quest. This is a standard Lua array (the first index is 1), and values are all strings.

### `sol.main.resource_exists(resource_type, id)`

Returns whether a resource element with the specified id is declared in the [quest database](../files-specs/database-file.md). This does not check if the corresponding files can be found and are actually valid: it only tells whether the element is declared in the quest database.

`resource_type` (string)
: Name of a resource type. See [`sol.main.get_resource_ids()`](#solmainget_resource_idsresource_type) for the possible values.

`id` (string)
: Id of the element to check.

Return value (boolean)
: `true` if such an element is declared in the quest database.

### `sol.main.get_resource_description(resource_type, id)`

Returns the description of a resource element as declared in the [quest database](../files-specs/database-file.md).

`resource_type` (string)
: Name of a resource type. See [`sol.main.get_resource_ids()`](#solmainget_resource_idsresource_type) for the possible values.

`id` (string)
: Id of the element to get.

Return value (string)
: The description of this element or `nil` if the element has no description.

### `sol.main.add_resource(resource_type, id, [description])`

Adds the declaration of a new resource element in the [quest database](../files-specs/database-file.md). This does not create any file but just registers an element in the quest database.

`resource_type` (string)
: Name of a resource type. See [`sol.main.get_resource_ids()`](#solmainget_resource_idsresource_type) for the possible values.

`id` (string)
: Id of the element to add.

`description` (string, optional)
: Description to set.

!!! note "Note"

    Usually, resource elements are already declared in the [quest database file](../files-specs/database-file.md) generated by Solarus Editor. You only need this function if you want to modify the quest database dynamically, for example if you generate content dynamically.

### `sol.main.remove_resource(resource_type, id)`

Removes the declaration of a resource element in the [quest database](../files-specs/database-file.md). This does not delete any file but just unregisters an element in the quest database.

`resource_type` (string)
: Name of a resource type. See [`sol.main.get_resource_ids()`](#solmainget_resource_idsresource_type) for the possible values.

`id` (string)
: Id of the element to remove.

!!! note "Note"

    Usually, resource elements are already declared in the [quest database file](../files-specs/database-file.md) generated by Solarus Editor. This function is only useful if you need to modify the quest database dynamically, for advanced uses like dynamically generated content.

### `sol.main.get_type(value)`

Returns the type name of a value.

This function is similar to the standard Lua function type(), except that for userdata known by Solarus, it returns the exact Solarus type name.

`value` (any type)
: Any value or `nil`

Return value (string)
: The name of the type. See below for detailed description of possible return values.

| Category                    | Possibilities                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           |
| --------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Standard Lua type names     | `"nil"` (a string, not the value `nil`), `"number"`, `"string"`, `"boolean"`, `"table"`, `"function"`, `"thread"`.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        |
| Userdata unknown to Solarus | `"userdata"`                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            |
| Solarus userdata            | `"game"`, `"map"`, `"item"`, `"surface"`, `"text_surface"`, `"sprite"`, `"shader"`, `"timer"`, `"movement"`, `"straight_movement"`, `"target_movement"`, `"random_movement"`, `"path_movement"`, `"random_path_movement"`, `"path_finding_movement"`, `"circle_movement"`, `"jump_movement"`, `"pixel_movement"`, `"state"`, `"hero"`, `"dynamic_tile"`, `"teletransporter"`, `"destination"`, `"pickable"`, `"destructible"`, `"carried_object"`, `"chest"`, `"shop_treasure"`, `"enemy"`, `"npc"`, `"block"`, `"jumper"`, `"switch"`, `"sensor"`, `"separator"`, `"wall"`, `"crystal"`, `"crystal_block"`, `"stream"`, `"door"`, `"stairs"`, `"bomb"`, `"explosion"`, `"fire"`, `"arrow"`, `"hookshot"`, `"boomerang"`, `"camera"`, `"custom_entity"` |

### `sol.main.get_metatable(type_name)`

Returns the metatable of a Solarus userdata type.

This function is very powerful and should be used with care.

All userdata objects of a type share the same metatable. So there is a metatable for maps, a metatable for games, a metatable for enemies, etc.

The metatable of a type stores what is common to all instances of this type. For example, the metatable of the `"enemy"`type has a field `"get_life"` that is the Solarus function [`enemy:get_life()`](./map-entities/enemy.md#enemyget_life) shared by all enemies.

Note that you can already get the metatable of any object with the standard Lua function `getmetatable(object)`. This function does the same thing, except that you don't have to provide an existing object: you just provide a type name. This allows you do manipulate the metatable of a type before objects of this type start to exist, typically to set up things before a game is started.

You can use the metatable to add a function to all instances of a type. Thus, you can extend the Solarus API with your own functions. This also works for events (functions that the engine automatically calls when they exist). For example, you can easily provide a function `add_overlay()` to all your maps by defining it only once in the map metatable:

```lua
-- Somewhere in your main script, at initialization time:
local map_metatable = sol.main.get_metatable("map")
function map_metatable:add_overlay(image_file_name)
-- Here, self is the map.
self.overlay = sol.surface.create(image_file_name)
end

function map_metatable:on_draw(dst_surface)
if self.overlay ~= nil then
    self.overlay:draw(dst_surface)
end
end

-- Now, all your maps will have a function map:add_overlay() and an event
-- map:on_draw() that allows to draw an additional image above the map!
```

When you define a field in a metatable, everything acts like if you defined it in all existing and future instances of the type. However, an individual instance is still able to define a field with the same name. In this case, the instance has the priority: the field from the metatable is overridden.

Similarly, you can even remove (by assigning `nil`) or modify (by assigning a new value) any function of the Solarus API. We don't recommend to do this because introducing differences with the official API changes the global behavior. It would be unexpected for other people who contribute to your game, and for people you share your scripts with.

`type_name` (string)
: Name of a Solarus userdata Lua type (see the list of types in [`sol.main.get_type()`](#solmainget_typevalue) above).

Return value (table or `nil`)
: The metatable of this type, or `nil` if there is no such Solarus type.

!!! warning "With great power comes great responsibility"

    If you are a Lua expert, you know that metatables are a very powerful mechanism. They are where the magic happens. Solarus uses metatables internally to do a lot of things, like allowing you to access userdata as tables.

    Therefore, you should never touch any metamethod (fields whose name starts with two underscores) in the metatable of a userdata type, in particular `__index`, `__newindex` and `__gc`

### `sol.main.get_os()`

Returns the name of the running OS. Possible values are: `"Windows"`, `"Mac OS X"`, `"Linux"`, `"iOS"`, `"Android"`. If the correct OS name is not available, returns a string beginning with the text `"Unknown"`

Return value (string)
: The name of the running OS.

### `sol.main.get_game()`

Returns the current [game](./game.md) if a game is running.

Return value ([game](./game.md) or `nil`)
: The game currently running, or `nil` if no game is running.

### `sol.main.rawget(object, key)`

Get a value from a table or a Solarus userdata that acts as a table without using metamethods.

`object` (table or userdata)
: The table or userdata.

`key` (any type)
: The key to look up.

Return value (any type)
: The value associated with the key on the object.

### `sol.main.rawset(object, key, value)`

Set a value on a table or a Solarus userdata that acts as a table without using metamethods.

`object` (table or userdata)
: The table or userdata.

`key` (any type except `nil` or `NaN`)
: The key to set.

`value` (any type)
: The value that should now be associated with the key.

Return value (table or userdata)
: The `object` argument.

## Events of `sol.main`

Events are callback methods automatically called by the engine if you define them.

### `sol.main:on_started()`

Called at the beginning of the program.

This function is called when Lua starts. In other words, the function is called when the program begins or after it was reset. In this function, you will typically start an initial menu like a title screen or a language selection screen. If you prefer, you can also start immediately a game.

### `sol.main:on_finished()`

Called at the end of the program.

This function is called when Lua is about to be shut down, i.e., just before the program stops or is reset.

### `sol.main:on_update()`

Called at each cycle of the program's main loop.

!!! note "Note"

    This event is very powerful (because it is called at global level of the program) but it may be costly if you do heavy operations. Keep in mind that it is called at each cycle.

    You can usually use other callbacks instead to get notified of when an event happens. You can also use [timers](./timers.md) if you want to regularly check something.

### `sol.main:on_draw(dst_surface)`

Called when the program's screen is redrawn.

At this point, the engine has already drawn the game (if a game is running) and has not drawn yet the menus of `sol.main` (if you have created menus on `sol.main`) Use this event if you want to draw some additional content.

`dst_surface` ([surface](./drawable-objects/surface.md))
: The surface where the screen is drawn.

## Events as an Input Handler

In addition to its own events a state is also an input handler and receives all of those events.

`sol.main` is the first input handler checked when an input event occurs.

See [inputs](./controls/inputs.md) to see all of these events.
