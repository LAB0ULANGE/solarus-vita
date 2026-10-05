# Translated Dialogs

## Overview

Dialogs are all messages shown to the player in the dialog box during the game. Some dialogs are triggered by the engine and others by your quest. They are all defined in the file `languages/xx/text/dialogs.dat,` where `xx` is a language code defined in [project_db.dat](../database-file.md) (e.g. `"en"` or `"fr"`)

When translating the game, you have to translate this `dialogs.dat`file as well as the [strings file](./strings.md) (`strings.dat`) Have a look at the [translation page](../../tutorials/manual/translate-a-quest.md) if you are interested in translating a Solarus game.

Solarus Editor fully supports the edition of dialog files. You can either use it to edit a dialog file graphically, or you can edit `dialogs.dat` by hand in a text editor if you prefer.

`dialogs.dat` is a text file encoded in UTF-8. We specify here its full syntax, with instructions for quest makers and translators.

## Definition

### Properties

The definition of a dialog starts with `dialog{` and ends with `}` Inside the braces, the properties of the dialog are specified. Properties are declared with the syntax `key = value` and separated with commas. It is allowed to have an extra comma after the last property. String values should be enclosed within double quotes, except when specified otherwise. Each dialog must have at least the following two properties:

`id` (string)
: A name identifying the dialog. The id allows the engine and your Lua scripts to refer to the dialog whenever they want to display it. If you are translating dialogs, you must never translate the id.

`text` (multi-line string, optional)
: Contents of this dialog. If you are translating dialogs, `text` is the property to translate. The text usually has several lines, but can be empty. The text should be enclosed between `[[` and `]],` which is the notation for multiline strings. The closing marker `]]` should be on a new line.

Only `id` and `text` are recognized by the engine (because the default minimal built-in dialog box use them), but you can add other properties depending on your dialog box implementation. Properties of a dialog are always of type string. Numbers and booleans are also accepted for historical reasons, but they will be converted to strings at runtime. For booleans, `true` is replaced by the string "1" and `false` is replaced by the string "0".

### Custom Dialogs

The dialog box system is entirely customizable, so you it is up to you to put the properties that fit your needs. For example, you can make properties that define:

- the speed of the dialog (assuming that your dialog box displays the letters gradually),
- whether the dialog stops automatically or manually,
- whether it can be skipped by the player,
- whether it contains special features like a question or an icon,
- etc.

When a dialog starts at runtime, your dialog box script receives these properties through the [`game:on_dialog_started()`](../../lua-api/game.md#gameon_dialog_starteddialog-info) event and is responsible to handle them.

## Example

Here is a small example of dialog file. Again, remember that only `id` and `text` are mandatory: other properties are specific to a particular implementation of dialog box.

```lua
dialog {
  id = "wise_man",
  skip = "current",
  question = true,
  next = "wise_man.thanks",
  next2 = "wise_man.insisting",
  text = [[I can feel courage in
you.
Do you think you can
protect the Princess?
Definitely!
No way
]],
}

dialog {
  id = "wise_man.insisting",
  question = true,
  next = "wise_man.thanks",
  next2 = "wise_man.insisting",
  text = [[
I beg you.
You are the only one
one able to protect
our kingdom…
Sure
No way
]],
}

dialog {
  id = "wise_man.thanks",
  text = [[
I knew I could count on
your immense bravery.
]],
}
```
