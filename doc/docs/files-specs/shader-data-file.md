# Shader Description File

## Overview

Shaders allow you to modify the video rendering of the window or of drawable objects like sprites, surfaces and text surfaces.

For example, shaders can be used to apply a smoothing filter or to make some advanced visual effects to a sprite, to a surface or to the whole window.

Shaders are programs executed by the GPU during a rendering pass. They are written in a shading language (GLSL or GLSL ES in Solarus). See the documentation of OpenGL and OpenGL ES for more information about shaders and shading languages.

A shader program is usually divided in two parts: a vertex shader and a fragment shader (also called pixel shader). As Solarus is a 2D engine, the vertex shader transforms the 4 vertices of the 2D image to be rendered, and the fragment shader determines all final pixels to be displayed for this image.

!!! note "Note"

    We often say a shader for short to actually mean shader program, that is, a program composed of a vertex shader plus a fragment shader.

We explain here the organization of shader files in Solarus and their syntax.
See the [shader API](../lua-api/shaders.md) to know how to use shaders at runtime from your Lua scripts.

The `shaders` directory contains all shaders. For a shader program with id `xx,`there can be three files:

- `xx.dat:` A data file describing the properties of the shader program, in particular the name of the vertex shader file and of the fragment shader file. The syntax of this description file is detailed below.

`*.glsl` (optional)
: The vertex shader code. It can have any name (with the extension `.glsl`). If no vertex shader file is specified, a default vertex shader that does nothing special will be used.

`*.glsl` (optional)
: The fragment shader code. It can have any name (with the extension `.glsl`). If no fragment shader file is specified, a default fragment shader that does nothing special will be used (pixels will be unchanged).

See the documentation of OpenGL and OpenGL ES for the specification of GLSL and GLSL ES shading languages.

Note that this file structure allows different shader programs to use the same vertex shader code file or the same fragment shader code file.

## Syntax of the Shader Description File

Solarus Editor fully supports the edition of shader description files (not to be confused with vertex shaders and fragment shaders themselves, that is, the GLSL / GLSL ES code files). You should not have to edit shader description files by hand unless you know what you are doing.

A shader description file defines its properties, including the name of the vertex shader file and the name of the fragment shader file.

We now specify the syntax of a shader description file.

The sequence of characters `--` (two dashes) marks the beginning of a comment. After them, the rest of the line is ignored by the engine.

Empty lines are also ignored. The definition of a shader program starts with `shader{` and ends with `}` Inside the braces, the properties of the shader program are specified. Properties are declared with the syntax `key = value` and separated with commas. It is allowed to have an extra comma after the last property. String values should be enclosed within double quotes and cannot have line breaks.

The following properties are supported:

`vertex_file` (string, optional)
: Name of the shader code file for the vertex shader, relative to the `shaders` directory. It must end with `.glsl`. No value means to use a default vertex shader that does nothing special.

`fragment_file` (string, optional)
: Name of the shader code file for the fragment shader, relative to the `shaders` directory. It must end with `.glsl`. No value means to use a default fragment shader that does nothing special.

`scaling_factor` (number, optional)
: When the shader is applied to the whole window, indicates how much the input texture will be scaled by the shader. Only useful for scaling shaders. If a value is set, the output texture of the shader will have the size of the input texture multiplied by this scaling factor (however, the output texture will then still be fitted to the window). If no value is set, the output texture of the shader will have the size of the window. This value only has an effect when the shader is applied to the window, and does nothing when the shader is applied to a drawable object.

## Example

Example of a shader description file:

```lua
shader {
  vertex_file = "scale2x.vertex.glsl",
  fragment_file = "scale_2x.fragment.glsl",
  scaling_factor = 2,
}
```
