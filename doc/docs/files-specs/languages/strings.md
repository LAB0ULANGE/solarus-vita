# Translated Strings

## Overview

Strings displayed during the game, for example in your menus, need to be localized in the current language. To this end, a `strings.dat` file is placed inside each `languages/xx/text` directory, where `xx` is a language code defined in [project_db.dat](../database-file.md) (e.g. `"en"` or `"fr"`)

When translating the game, you have to translate this `strings.dat`file as well as the [dialogs file](./dialogs.md) (`dialogs.dat`) Have a look at the [translation page](../../tutorials/manual/translate-a-quest.md) if you are interested in translating the game (and you should also contact us).

Solarus Editor fully supports the edition of string files. You can either use it to edit a string file graphically, or you can edit `strings.dat` by hand in a text editor if you prefer.

`string.dat` is a text file encoded in UTF-8. We specify here its full syntax, with instructions for quest makers and translators.

## Definition

The definition of a string starts with `text{` and ends with `}` Inside the braces, the properties of the string are specified. Properties are declared with the syntax `property_name = property_content` and separated with commas. It is allowed to have an extra comma after the last property. String contents should be enclosed within double quotes, except when specified otherwise. Each string must have exactly the following two properties:

`key` (string)
: A name identifying this string for all languages. Don't change the key when you are translating a quest, since the key identifies the string.

`value` (string)
: The content of this string in the current language.

For each language, the `strings.dat` file must define strings with the same keys.

It is recommended to add comments (lines beginning by `--`) to explain where each string is used and how many characters it should not exceed. Future translators will thank you.

## Example

An an example, here is a part of a possible `languages/fr/text/strings.dat`file:

```lua
-- Savegame selection menu.
-- Strings beginning with "selection_menu.phase" should not exceed 45 characters.
text{ key = "selection_menu.phase.select_file", value = "Veuillez choisir un fichier" }
text{ key = "selection_menu.phase.choose_mode", value = "Choisissez un mode de jeu" }
text{ key = "selection_menu.phase.choose_name", value = "Quel est votre nom ?" }
text{ key = "selection_menu.phase.confirm_erase", value = "Etes-vous sûr ?" }
text{ key = "selection_menu.phase.erase_file", value = "Quel fichier voulez-vous effacer ?" }
text{ key = "selection_menu.phase.options", value = "Appuyez sur Espace pour modifier" }
text{ key = "selection_menu.phase.options.changing", value = "<  > : choisir, Espace : valider" }
text{ key = "selection_menu.erase", value = "Effacer" }
text{ key = "selection_menu.cancel", value = "Annuler" }
text{ key = "selection_menu.big_yes", value = "OUI" }
text{ key = "selection_menu.big_no", value = "NON" }
text{ key = "selection_menu.empty", value = "Vide" }
text{ key = "selection_menu.options", value = "Options" }
text{ key = "selection_menu.back", value = "Retour" }
text{ key = "selection_menu.options.language", value = "Langue" }
text{ key = "selection_menu.options.video_mode", value = "Mode" }
text{ key = "selection_menu.options.music_volume", value = "Volume de la musique" }
text{ key = "selection_menu.options.sound_volume", value = "Volume des sons" }
```
