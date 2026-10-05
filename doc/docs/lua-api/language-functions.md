# Language functions

## Overview

`sol.language` lets you get and set the current language and manage language-specific data.

Like all resources (maps, sounds, etc.), each language is identified by an id (like `"en"`, `"fr"`) and has a human-readable description (like `"English"`, `"Français"`) The language id corresponds to the name of a directory with files translated in this language: dialogs, strings and images. That directory is located in the `languages` directory of your quest.

The list of languages available in your quest is specified in the [quest database file](../files-specs/database-file.md).

If there is only one language in your quest, then it is automatically set as the current language. Otherwise, if you have several languages, no current language is automatically set for you, and you need to call [`sol.language.set_language()`](#sollanguageset_languagelanguage_id) to be able to use dialogs.

## Functions of `sol.language`

### `sol.language.get_language()`

Returns the id of the current language.

Return value (string or `nil`)
: The code that identifies the current language, or `nil` if no language was set.

### `sol.language.set_language(language_id)`

Changes the current language.

`language_id` (string)
: The code that identifies the new language to set. It must be a valid id as defined in your [quest database file](../files-specs/database-file.md).

### `sol.language.get_language_name([language_id])`

Returns the human-readable description of a language.

Equivalent to [`sol.main.get_resource_description("language", language_id)`](./general-features.md#solmainget_resource_descriptionresource_type-id)

`language_id` (string, optional)
: A language id (no value means the current language).

Return value (string)
: The human-readable name of this language.

### `sol.language.get_languages()`

Returns the list of available languages.

Equivalent to [`sol.main.get_resource_ids("language")`](./general-features.md#solmainget_resource_idsresource_type)

Return value (table)
: An array of all language ids declared in the quest.

### `sol.language.get_string(key)`

Returns a string translated in the current language. There must be a current language when you call this function.

Translated strings are defined in the file [text/strings.dat](../files-specs/languages/strings.md) of the language-specific directory (e.g. `languages/en/text/strings.dat`)

`key` (string)
: Key of the string to get. The corresponding key-value pair must be defined in [text/strings.dat](../files-specs/languages/strings.md).

Return value (string or `nil`)
: The value associated to this key in [text/strings.dat](../files-specs/languages/strings.md), or `nil` if it does not exist.

### `sol.language.get_dialog(dialog_id)`

Returns a dialog translated in the current language. There must be a current language when you call this function.

Translated dialogs are defined in the file [text/dialogs.dat](../files-specs/languages/dialogs.md) of the language-specific directory (e.g. `languages/en/text/dialogs.dat`)

`dialog_id` (string)
: Id of the dialog to get.

Return value (table or `nil`)
: The corresponding dialog in the current language, or `nil` if it does not exist. The dialog is a table with at least the following two entries:

    `dialog_id` (string)
    : Id of the dialog.

    `text` (string)
    : Text of the dialog. The table also contains all custom entries defined in [text/dialogs.dat](../files-specs/languages/dialogs.md) for this dialog. These custom entries always have string keys and string values.
