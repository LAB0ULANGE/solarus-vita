# Quest Data Files Specification

## Solarus _Quests_

A _quest_ is how we call a game made with Solarus. We explain here how a quest is built.

Concretely, a _quest_ is a data package that may be run by Solarus C++ engine. The engine is used either by the `solarus-run` executable or by Solarus Launcher.

## How to Run a Quest

### With `solarus-run`

When you run the `solarus-run` executable file, it needs the path of the quest to launch. It can be a folder or a file. This can be done at runtime by specifying the path as a command-line argument.

```sh
solarus-run path/to/your/quest
```

If the quest path is not specified, the current directory is considered by default, unless you compiled Solarus with another default quest path.

!!! info "On Windows"

    While on Unix systems (macOS, Linux), the executables don't have file extensions, on Windows, the executable is `solarus-run.exe`.

### With Solarus Launcher

When you run Solarus Launcher, you can select graphically the quest to run and you can change some settings.

## Possible Quest Formats

In both cases, the quest may have one of the following forms:

- A `.zip` archive containing all data of your quest. This archive form is useful when your quest is finished and you distribute it to people.

- A `.solarus` archive containing. This is just a `.zip` archive, with a different extension.

- A directory called `data` and containing all data of your quest.

The latter form is handy when you are developing your quest. It is also the only possible form to edit your quest with Solarus Editor. In fact, when you modify a map in the editor, you don't even need to restart the game.

## Content of a Quest

The data files represent all resources used by both the engine and the quest, such as sounds, musics, images, sprites, dialogs, maps and Lua scripts. We specify here the details for all those files.

Here is the full structure of the `data` directory / `zip` archive of a quest and the syntax of each file.

- [`quest.dat`](./properties-file.md): global properties of your quest.

- [`project_db.dat`](./database-file.md): list of all resources (maps, sprites, enemies…) and additional information about quest files.

- [`main.lua`](./main-script.md): main Lua script of your quest.
- [`**/*.lua:`](../lua-api/index.md) other Lua scripts, possibly organized in subdirectories.
- [`logos/`](./logos.md): images used by Solarus Launcher to represent your game.
- [`sounds/`](./audio/sounds.md): contains all sound effects.
- [`musics/`](./audio/musics.md): contains all musics.
- [`fonts/`](./fonts.md): contains the fonts used to draw text in your quest.
- [`languages/`](./languages/index.md): contains the language-specific files.
- [`sprites/`](./sprite-data-file.md): contains all animated sprites (`xx.dat` and `*.png`), possibly organized in subdirectories.
- [`maps/`](./map-data-file.md): contains all the maps of your quest (map definitions and their [scripts](../lua-api/map.md)).
- [`tilesets/`](./tileset-data-file.md): contains all tilesets available to maps (`xx.dat`, `xx.tiles.png` and `xx.entities.png`).
- [`items/`](../lua-api/equipment-items.md): Lua scripts that define the properties and the behavior of equipment items.
- [`enemies/`](../lua-api/map-entities/enemy.md): Lua scripts that define the properties and the behavior of the enemies.
- [`entities/`](../lua-api/map-entities/custom-entity.md): contains all custom entity Lua scripts.
- [`shaders/`](./shader-data-file.md): contain all shaders data files and their source code files (`*.glsl`).
