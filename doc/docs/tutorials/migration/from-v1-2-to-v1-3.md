# Upgrade Your Quest from Solarus 1.2 to Solarus 1.3

The main improvements of Solarus 1.3 are in Solarus Editor: there is now a sprite editor, the quest tree gives more control over various files, and tile patterns are now identified by strings. There are also a few changes, like more customizable switches and bugfixes. Good news: there is no incompatible change in the Lua API. The only changes in the Lua API are bugfixes and new features, in particular with sprites and switches.

As always, you should make a backup before any upgrade.

In this page, we only focus on the incompatibilities. See the [changelog](https://gitlab.com/solarus-games/solarus/blob/v1.3/ChangeLog) to know more about the new features.

## Upgrading Data Files

Data files other than scripts can be upgraded automatically with the editor.
Open your quest with Solarus Editor 1.3 and a dialog will let you to perform the upgrade.

Note that the operation can also be done from the command line, by running the script `update_quest.lua` in the `tools` directory of the git repository. Internally, the editor actually calls this script.

The only changes in the data files are minor:

- The id of tile patterns is now a string. This affects tilesets and maps. However, since integers are particular strings, this is actually not an incompatible change.
- In maps, the subtype of switches is now a string.
- In maps, switches have new properties `sprite` and `sound`.

## Upgrading Lua Scripts

The Lua scripting API of Solarus 1.3 introduces new functions, in particular on sprites and switches, but there is no incompatibility. Scripts that worked with Solarus 1.2 also work with Solarus 1.3. So you have nothing to do.
