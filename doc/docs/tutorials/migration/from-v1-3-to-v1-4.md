# Upgrade Your Quest from Solarus 1.3 to Solarus 1.4

Solarus 1.4 provides a brand new quest editor that was rewritten from scratch. There are almost no new features in the engine itself. The only change is that fonts are now a resource. This means they now appear in the tree of Solarus Editor, so they are now much easier to manage. But right now, this may require manual adjustement to your scripts.

As always, you should make a backup before any upgrade.

In this page, we only focus on the incompatibilities. See the [changelog](https://gitlab.com/solarus-games/solarus/blob/v1.4/ChangeLog) to know more about the new features.

## Upgrading Data Files

Data files other than scripts can be upgraded automatically with the editor. Open your quest with Solarus Editor 1.4 and a dialog will let you to perform the upgrade.

Note that the operation can also be done from the command line, by running the script `update_quest.lua` in the `resources/quest_converter` directory of the git repository of Solarus Editor. Internally, Solarus Editor actually calls this script.

For information, the changes in data files are:

- The font list file `text/fonts.dat` no longer exists. Fonts are now listed as a resource (like maps, tilesets, musics, etc.) and there is no explicit default font anymore.
- Fonts files are now in a `fonts` folder instead of `text`.
- Shop treasures now have a `font` property.

## Upgrading Lua scripts

The Lua scripting API of Solarus 1.3 introduces an incompatibility due to the new way fonts are handled. We explain below how to adjust your scripts to this change.

### Fonts

When you create a text surface, with [`sol.text_surface.create()`](http://www.solarus-games.org/doc/1.4/lua_api_text_surface.html#lua_api_text_surface_create), the font file and the font size are now set separately. Before, there was a font id that referred to both a font file and a size. Now, the font id is the name of a font file with extension. This is more consistent with all other resources: sounds, musics, maps, etc.

If you don't set the font, it will be by default the first one in alphabetical order. If you don't set the font size, the default value is `11`.

- If your existing code calls [`sol.text_surface.create()`](http://www.solarus-games.org/doc/1.4/lua_api_text_surface.html#lua_api_text_surface_create) or [`text_surface:set_font()`](http://www.solarus-games.org/doc/1.4/lua_api_text_surface.html#lua_api_text_surface_set_font) with a font id that does not correspond to a font file without extension, then you need to fix this font id. For example, if you had like me a font called `fixed` referring to the font file `minecraftia.ttf`, the new font id to use is `minecraftia`.

- If your existing code calls [`sol.text_surface.create()`](http://www.solarus-games.org/doc/1.4/lua_api_text_surface.html#lua_api_text_surface_create) without setting an explicit font (therefore relying on the default font of `fonts.dat`), then you should set the font to use, because otherwise the default font would now be the first one in alphabetical order. You can do this in the table parameter of [`sol.text_surface.create()`](http://www.solarus-games.org/doc/1.4/lua_api_text_surface.html#lua_api_text_surface_create) (the `font` field) or with [`text_surface:set_font()`](http://www.solarus-games.org/doc/1.4/lua_api_text_surface.html#lua_api_text_surface_set_font).

- If your existing code used any font with a size different from `11`, you have to set that size whenever you use this font. You can do this in the table parameter of [`sol.text_surface.create()`](http://www.solarus-games.org/doc/1.4/lua_api_text_surface.html#lua_api_text_surface_create) (the `size` field) or with [`text_surface:set_font_size()`](http://www.solarus-games.org/doc/1.4/lua_api_text_surface.html#lua_api_text_surface_set_font_size).
