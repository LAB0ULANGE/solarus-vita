# Logos and Icons

## Overview

The `"logos/"` directory of your quest can contain images that are used to display your quest in GUIs. Solarus Launcher uses them in the quest list to represent each quest before the user plays.

These images are optional, but it is recommended to make some for your quest to be better identified in the quest list.

| File                  | Role                                         |
| --------------------- | -------------------------------------------- |
| `logos/thumbnail.png` | Thumbnail of your quest in Solarus Launcher. |
| `logos/logo.png`      | Logo of your quest in Solarus Launcher v1.6. |
| `logos/icon_xx.png`   | Icon of your quest window.                   |

## Quest Thumbnail

The thumbnail is an image of `700`x`360` pixels (or more, still with the same aspect ratio), called `"logos/thumbnail.png"`. It is used to visually represent your quest in Solarus Launcher or on the Solarus website.

The image should not include border radiuses on corners: they are added automatically by the app.

## Quest Logo

The logo of your quest should be a PNG image of size 200x140 called `"logos/logo.png"` The logo is optional.

!!! info "Starting from Solarus 2.0: deprecated"

    Solarus Launcher now prioritizes the quest thumbnail if found. It maintains backwards compatibility by looking for a quest logo if the thumbnail is not found.

## Quest Icons

The icon of your quest will be used by the operating system's windowing system to decorate the window your quest is running in. The icon is optional.

Multiple icon sizes are allowed, and each size should be in a separate PNG file. The following icon file names are allowed, with the corresponding sizes from 16×16 pixels to 1024×1024 pixels:

- `"logos/icon_16.png",`
- `"logos/icon_24.png",`
- `"logos/icon_32.png",`
- `"logos/icon_48.png",`
- `"logos/icon_64.png",`
- `"logos/icon_128.png",`
- `"logos/icon_256.png",`
- `"logos/icon_512.png",`
- `"logos/icon_1024.png"`

It is possible (and recommended) to provide an icon with multiple sizes. The Solarus GUI will then automatically pick the most suitable one(s) to fit its needs.
