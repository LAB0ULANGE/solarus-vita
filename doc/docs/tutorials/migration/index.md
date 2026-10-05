# Migration Guide

If your game is still using an old Solarus version, you may want to migrate to a new one. This page explains how to upgrade a Solarus quest to the latest Solarus version.

## Solarus Version Numbers

Since Solarus `1.0.0`, versions of Solarus are numbered as follows: `x.y.z`, where

- `x` is the **major version**,
- `y` is the **minor version**,
- `z` is the **patch version**.

Patch versions contain only bug fixes. They never introduce incompatibilities, so when only the patch version changes, your quest continues to work.

Therefore, when we talk about compatibility, only the major and minor numbers are considered. In your quest properties file `quest.dat`, the value `solarus_version` indicates the format of your quest, with only the major and minor numbers.

For example, if `solarus_version` is `2.0`, your quest can run with Solarus `2.0.*`, where `*` is any patch version number.

To make your quest compatible with the latest version of Solarus, there are two steps:

1. **Upgrading Data Files**: when your quest is obsolete, the editor shows a dialog that lets you automatically convert it to the latest version. You have nothing to do, the editor converts data files for you.
2. **Upgrading scripts**: Lua scripts are programs, so the editor does not convert them automatically when something changes in the [Solarus Lua API](../../lua-api/index.md). Our goal with this migration guide is to help you doing the upgrade.
