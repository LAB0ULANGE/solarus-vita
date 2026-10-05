# Template for Lua API Reference

If you need help, you can ask to [S2009](https://s2009.fr) on Discord.

## Overview

Here is a formatting guide to follow when creating a new Markdown page for the Lua API Reference.  
You just need to replace the placeholders with the actual content.

```md
# <feature_title>

## Overview

<feature_description>

## Functions of `sol.<feature>`

<some_content>

### `sol.<feature>.<function_name>(<function_arguments>)`

<function_description>

`<argument_name>` (<argument_type><argument_optionality><argument_requirement><argument_default_value><argument_deprecation>)
: <argument_description>

Return value (<return_type>)
: <return_description>

<inherited_methods>

## Methods of [`<feature>` / the type `<feature>` / all `<feature>` types] 

<some_content>

### `<feature>:<method_name>(<method_arguments>)`

<method_description>

`<argument_name>` (<argument_type><argument_optionality><argument_requirement><argument_default_value><argument_deprecation>)
: <argument_description>

Return value (<return_type>)
: <return_description>

<inherited_events>

## Events of [`<feature>` / the type `<feature>` / all `<feature>` types] 

<some_content>

### `<feature>:on_<event_name>(<event_arguments>)`

<event_description>

`<argument_name>` (<argument_type><argument_optionality><argument_requirement><argument_default_value><argument_deprecation>)
: <argument_description>

Return value (<return_type>)
: <return_description>

<additional_sections>
```

## Specifics

### Inherited methods / events

A `<feature>` can inherit methods and events from its parent type. If it's the case, you must replace the `<inherited_methods>` and `<inherited_events>` placeholders with sections following this template:

```md
## [Methods / Events] Inherited from `<parent_feature>`

<some_content>

See [<parent_feature>](<parent_feature_link>) to know these [methods / events].
```

Note that the `<parent_feature_link>` placeholder must be replaced by the relative path to the parent feature's documentation file.

### Arguments

The placeholders `<function_arguments>`, `<method_arguments>` and `<event_arguments>` are lists of arguments separated by commas. Each argument is written as `<argument_name>`. There are special cases for optional and nillable arguments, see below.

**Examples:**

- `<argument_name>, <argument_name>, <argument_name>` to describe a function taking 3 required arguments.
- `<argument_name>, [<argument_name>]` to describe a function taking 1 required argument and 1 optional argument.
- `[<argument_name>], [<argument_name>]` to describe a function taking 1 nillable argument and 1 optional argument.
- `[<argument_name>, <argument_name>]` to describe a function taking 2 nillable arguments.
- `[<argument_name>, [<argument_name>]]` to describe a function taking 1 nillable and 1 optional arguments, where the second argument requires the first one to be specified.
- `[<argument_name>, [<argument_name>, <argument_name>]]` to describe a function taking 2 nillable arguments and 1 optional argument, where the second and the last arguments require the first one to be specified.

The placeholder `<argument_requirement>` helps to understand the special cases above. You can replace it with `, requires: <argument_name>`. If the argument has multiple requirements, you must replace `<argument_name>` with `<argument_name> and <argument_name>` as much as you need.
When an argument is optional or nillable, you need to replace `<argument_optionality>` with `, optional` (otherwise, you just replace it with nothing).  
In the same way, when an argument has a default value, you need to replace `<argument_default_value>` with `, default: <default_value>` (otherwise, you just replace it with nothing).  
As well as if an argument is deprecated, you need to replace `<argument_deprecation>` with `, deprecated: <since_version>` (otherwise, you just replace it with nothing).
Note that the `<default_value>` and `<since_version>` must be surrounded by backticks.

When an argument takes multiple types, you need to replace `<argument_type>` with `<argument_type> or <argument_type>` as much as you need.  
If the `<argument_type>` is an external `<feature>`, please link it to the corresponding documentation page or section with `[<return_type>](<feature_page_link>)` using the **relative path**.  
If you set an `<argument_type>` to `table`, you need to specify the key-value pairs of this table under the `<argument_description>`. These key-value pairs must be written in the same way as the arguments.  
Note that is the same if you set an `<argument_type>` to `function` and want to specify the arguments of this function.

**Example:**

```md
`<argument_name>` (table<argument_optionality>)
: <argument_description>

    `<key_name>` (<key_type><key_optionality>)
    : <key_description>
```

If an argument can accept only specific values, you need to specify them under the `<argument_description>`.

**Example:**

```md
`<argument_name>` (string<argument_optionality>)
: <argument_description>

    - `"<value>"` (default): <value_description>
    - `"<value>"`: <value_description>
```

### Returns

If the `<return_type>` is an external `<feature>`, please link it to the corresponding documentation page or section with `[<return_type>](<feature_page_link>)` using the **relative path**.
If the function returns multiple values at once, you need to specify them with an index.

**Example:**

```md
Return value <index> (<return_type>)
: <return_description>

Return value <index> (<return_type>)
: <return_description>
```

When a function can return different types of values, you need to specify them like arguments with `<return_type> or <return_type>` as much as you need.
Specific case: if a function return `nil` **and a value like a string** (for an error message for example), you need to replace `<return_type>` with `nil+<return_type>`.

### Admonitions

If you want to add a note or a warning, you can use the following syntax:

```md
!!! note "Note"

    <note_content>
```

```md
!!! warning "<warning_title>"

    <warning_content>
```

If you want to add a deprecation warning, you must use the following syntax:

```md
!!! warning "Deprecated"

    This [function/method/event] is deprecated since Solarus <deprecated_version>.<additional_content>
```

The admonitions must be added **only at the very end** of the function, the method or the event.
We use the admonitions syntax of Material for MkDocs, you can also check their [official documentation](https://squidfunk.github.io/mkdocs-material/reference/admonitions/#inline-blocks-inline-end
) for more information.

### Additional sections

You can add some `<additional_sections>` to the `<feature>` page. These sections must be added **only at the very end** of the page.  
Here are the possible section titles, in the order they must follow if they are all present:

```md
## Event of <feature> Handlers
## Event as [a / an] `<other_feature>` Handler
## Deprecated functions of `sol.<feature>`
## Deprecated methods of [`<feature>` / the type `<feature>` / all `<feature>` types] 
## Deprecated events of [`<feature>` / the type `<feature>` / all `<feature>` types]
```

The second section must contains a link to the page of the `<other_feature>`. The other sections works like the normal functions, methods and events, but they must have a deprecation warning.
