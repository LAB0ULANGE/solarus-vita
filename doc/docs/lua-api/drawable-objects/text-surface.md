# Text Surfaces

## Overview

A text surface is a single line of text that you can display. A text surface can be seen as a special [surface](./surface.md) able to contain text.

## Functions of `sol.text_surface`

### `sol.text_surface.create([properties])`

Creates a text surface with the specified properties.

`properties` (table, optional)
: A table that describes all properties of the text surface to create. Its key-value pairs are all optional, they can be:

    `horizontal_alignment` (string, optional, default: `"left"`)
    : When you draw the text surface at some coordinates on a destination surface, it is anchored at this position. Possible values are:
        
        - `"left"`
        - `"center"`
        - `"right"`

    `vertical_alignment` (string, optional, default: `"middle"`)
    : When you draw the text surface at some coordinates on a destination surface, it is anchored at this position. Possible values are:

        - `"top"`
        - `"middle"`
        - `"bottom"`

    `font` (string, optional)
    : Name of the font file to use, relative to the `fonts` directory and without extension. It must be a declared in the resource list of the [quest database](../../files-specs/database-file.md). The following extensions are auto-detected in this order: `.png`, `.ttf`, `.otf`, `.ttc` and `.fon`. The default is the first one in alphabetical order.

    `rendering_mode` (string, optional, default: `"solid"`)
    : Valid values are:

        - `"solid"` (default): faster.
        - `"antialiasing"`: smooth effect on letters.

    `color` (table, optional, default: `{255, 255, 255}`)
    : Color of the text to draw (array of 3 RGB values between 0 and 255). No effect on bitmap fonts.

    `font_size` (number, optional, default: `11`)
    : Font size to use. No effect on bitmap fonts.

    `font_hinting` (string, optional, default: `"normal"`)
    : Sets the font hinting mode for the surface, which controls how characters are rendered, there are four options: 

        - `"normal"` (default)
        - `"none"`
        - `"light"`
        - `"mono"`: No effect on bitmap fonts.

    `font_kerning` (boolean, optional, default: `true`)
    : Whether kerning is used for the surface, which controls spacing between characters. Font dependent. No effect on bitmap fonts.

    `text` (string, optional, default: `""`)
    : The text to show (must be valid UTF-8).

    `text_key` (string or `nil`, optional, default: `nil`)
    : Key of the localized text to show. The string must exist in the file [text/strings.dat](../../files-specs/languages/strings.md) of the current [language](../language-functions.md).

Return value (text surface)
: The text surface created.

### `sol.text_surface.get_predicted_size(font_id, font_size, text)`

Predicts the size of a text surface while avoid its costly creation. This can be useful for example to split text into lines or to insert an image in your dialog box.

`font_id` (string)
: Name of the font file to use, relative to the `fonts` directory and without extension. It must be declared in the resource list of the [quest database](../../files-specs/database-file.md). The following extensions are auto-detected in this order: `.png`, `.ttf`, `.otf`, `.ttc` and `.fon`

`font_size` (number)
: Font size to use. No effect on bitmap fonts.

`text` (string)
: Text to predict the surface size for. Must be valid UTF-8.

Return value 1 (number)
: Predicted width in pixels.

Return value 2 (number)
: Predicted height in pixels.

## Methods Inherited from `drawable`

Text surfaces are particular [drawable](./index.md) objects. Therefore, they inherit all methods from the type drawable.

See [drawable](./index.md) to know these methods.

## Methods of the type `text_surface`

The following methods are specific to text surfaces.

### `text_surface:get_horizontal_alignment()`

Returns the horizontal alignment of the text.

When you draw the text surface at some coordinates on a destination surface, it is anchored at this position.

Return value (string)
: Possible values are:

    - `"left"`
    - `"center"`
    - `"right"`

### `text_surface:set_horizontal_alignment(horizontal_alignment)`

Sets the horizontal alignment of the text.

When you draw the text surface at some coordinates on a destination surface, it is anchored at this position.

`horizontal_alignment` (string)
: Possible values are:

    - `"left"`
    - `"center"`
    - `"right"`

### `text_surface:get_vertical_alignment()`

Returns the vertical alignment of the text.

When you draw the text surface at some coordinates on a destination surface, it is anchored at this position.

Return value (string)
: Possible values are:

    - `"top"`
    - `"middle"`
    - `"bottom"`

### `text_surface:set_vertical_alignment(vertical_alignment)`

Sets the vertical alignment of the text.

When you draw the text surface at some coordinates on a destination surface, it is anchored at this position.

`vertical_alignment` (string)
: Possible values are:

    - `"top"`
    - `"middle"`
    - `"bottom"`

### `text_surface:get_font()`

Returns the font used to draw this text surface.

Return value (string)
: Id of the font of this text surface.

### `text_surface:set_font(font_id)`

Sets the font used to draw this text surface.

`font_id` (string)
: Name of the font file to use, relative to the `fonts` directory and without extension. It must be a declared in the resource list of the [quest database](../../files-specs/database-file.md). The following extensions are auto-detected in this order: `.png`, `.ttf`, `.ttc` and `.fon`

### `text_surface:get_rendering_mode()`

Returns the rendering mode of the text.

Return value (string)
: Possible values are:

    - `"solid"`: faster.
    - `"antialiasing"`: smooth effect on letters.

### `text_surface:set_rendering_mode(rendering_mode)`

Sets the rendering mode of the text.

`rendering_mode` (string)
: Possible values are:

    - `"solid"`: faster.
    - `"antialiasing"`: smooth effect on letters.

### `text_surface:get_color()`

Returns the color used to draw the text. This only has an effect for outline fonts.

Return value (table)
: The text color as an array of 3 RGB values (`0` to `255`).

### `text_surface:set_color(color)`

Sets the color used to draw the text. This only has an effect for outline fonts.

`color` (table)
: The text color as an array of 3 RGB values (`0` to `255`).

### `text_surface:get_font_size()`

Returns the font size used to draw the text. This only has an effect for outline fonts.

Return value (number)
: The font size.

### `text_surface:set_font_size(font_size)`

Sets the size used to draw the text. This only has an effect for outline fonts.

`font_size` (number)
: The font size.

### `text_surface:get_text()`

Returns the string displayed in this object.

Return value (string)
: The current text (possibly an empty string).

### `text_surface:set_text([text])`

Sets the string displayed in this object. The string must be encoded in UTF-8.

`text` (string, optional)
: The text to set. No value or an empty string mean no text.

### `text_surface:set_text_key(key)`

Sets the text as a localized string in the current language.

`key` (string)
: Key of the text to set.

!!! note "Note"

    This function is equivalent to `text_surface:set_text(sol.language.get_string(key))`.

### `text_surface:get_size()`

Returns the size of this text surface.

Return value 1 (number)
: Width of the text surface in pixels.

Return value 2 (number)
: Height of the text surface in pixels.

!!! note "Note"

    Note that you cannot set the size of a text surface. The size is determined by the text and the font.
