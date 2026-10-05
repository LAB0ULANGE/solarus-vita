# How to translate a quest

We describe here how to translate a Solarus quest.
If you wish to contribute translate one of our games, feel free to contact us.

Several files have to be translated so that a quest works in a new language:

- some images containing text,
- some strings displayed in the HUD and the menu,
- all dialogs of the game (most of the work to do is here).

All these language-specific data files are in the directory `languages/xx` of the quest data (where `xx`is the code of the language, for instance `en` or `fr`)

## Images

Some images used by the game engine contain text.
They all are in the directory `languages/xx/images`If you are making a translation of one of our games, we can do the pixel work for you, just give us the textual translations.
We can give you explanations about all these images so that you can know when they are used.

## Strings

The engine displays some text in the menus, before and during the game. All these strings are loaded from the file `languages/xx/text/strings.dat`.

The file must be encoded in UTF-8 and respect the syntax specified in the [Translated strings](../../files-specs/languages/strings.md) page.

## Dialogs

Dialogs are all messages displayed to the player in the dialog box during the game. They represent the biggest part of the translation work.

All dialogs are defined in the file `languages/xx/text/dialogs.dat`. The syntax of this file is specified in the [Translated dialogs](../../files-specs/languages/dialogs.md) page.

However, this is a very extensible syntax because the dialog box system is entirely customizable.
Each game extends the syntax of `dialogs.dat` in its own way by adding custom properties.

If you want to translate one of our games, we give translation instructions and describe precisely these custom properties in the comments of the `dialogs.dat` file itself.
