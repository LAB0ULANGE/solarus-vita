# Shaders

This module provides a datatype `shader` that represents an OpenGL or OpenGL ES shader program.

## Overview

Shaders allow you to modify the video rendering of the window or of drawable objects like sprites, surfaces and text surfaces. For example, shaders can be used to apply a smoothing filter or to make some advanced visual effects to a sprite, to a surface or to the whole window.

Shaders are programs executed by the GPU during a rendering pass. They are written in a shading language: **GLSL** or **GLSL ES** in the case of Solarus.

A shader program, represented by the `shader` Solarus datatype, is composed of a **_vertex shader_** and a **_fragment shader_** (also called _pixel shader_).

1. At every rendering frame, the **_vertex shader_** is called for each of the four corners of the input texture. If no vertex shader is defined in your shader program, then a default one is used.

2. The **_fragment shader_** is then called for each pixel of the texture. If no fragment shader is defined in your shader program, then a default one that keeps pixels unchanged is used.

The rest of this documentation page explains how to use shaders with the Solarus Lua API. We assume that you have some basic knowledge of shaders and GLSL or GLSL ES. See [the documentation of OpenGL and OpenGL ES](https://registry.khronos.org/OpenGL-Refpages) for more information about shaders and shading languages.

Solarus compiles them at runtime when you create them with [`sol.shader.create()`](#solshadercreateshader_id-solshadercreateproperties). You can either apply a shader to the whole window with [`sol.video.set_shader()`](./video.md#solvideoset_shadershader), or to an individual surface, text surface or sprite with [`drawable:set_shader()`](./drawable-objects/index.md#drawableset_shadershader).

### Current Window Shader

During the execution, a shader may be applied to the rendering of the whole window. Use [`sol.video.get_shader()`](./video.md#solvideoget_shader) and [`sol.video.set_shader()`](./video.md#solvideoset_shadershader) to control this.

This defines how the quest surface (whose size is [`sol.video.get_quest_size()`](./video.md#solvideoget_quest_size)) is drawn and scaled to fit to the window, which may be bigger. Thus, by default, the fragment shader works on an output texture of the size of the window. This can be changed using [`shader:set_scaling_factor()`](#shaderset_scaling_factorscaling_factor).

### Shaders on Drawable Objects

Shaders can also alter how [drawable objects](./drawable-objects/index.md) are rendered on the screen, using [`drawable:set_shader()`](./drawable-objects/index.md#drawableset_shadershader). In other words, you can apply a shader to any [surface](./drawable-objects/surface.md), [text surface](./drawable-objects/text-surface.md) or [sprite](./drawable-objects/sprite.md).

For example, if you want to apply a shader to the map surface without affecting what is displayed above, like the HUD elements or the dialog box, you can apply the shader to the [camera](./map-entities/camera.md)'s surface, with `map:get_camera():get_surface():set_shader(my_shader)`.

### Built-In Attributes and Uniforms

Additionally to the modern OpenGL or OpenGL ES built-in variables, Solarus automatically passes some additional useful variables to your shaders. Solarus built-in variables are always prefixed with `"sol_"`. Note that using the Lua API described below, you can also pass your own variables.

In a vertex shader, the following three attributes are automatically passed as input:

`attribute vec2 sol_vertex`
: Coordinates of the current vertex in the 3D space (same as `gl_Vertex` in old GLSL versions).

`attribute vec2 sol_tex_coord`
: Texture coordinates of the current vertex (same as `gl_MultiTexCoord0` in old GLSL versions).

`attribute vec4 sol_color`
: Color of the vertex

The following uniforms are automatically passed as input if they are declared in a vertex shader or in a fragment shader:

`uniform mat4 sol_mvp_matrix`
: The model-view projection matrix (same as `gl_ModelViewProjectionMatrix` in old GLSL versions).

`uniform mat3 sol_uv_matrix`
: The texture transform matrix.

`uniform sampler2D sol_texture`
: The input texture to render.

`uniform vec2 sol_input_size`
: Size of the input texture `sol_texture` before any scaling. Equal to [`sol.video.get_quest_size()`](./video.md#solvideoget_quest_size).

`uniform vec2 sol_output_size`
: Size of the output area, including the black bars if any. In windowed mode, equal to [`sol.video.get_window_size()`](./video.md#solvideoget_window_size). In fullscreen mode, equal to the screen resolution.

`uniform int sol_time`
: Simulated time elapsed since Solarus was started. Equal to [`sol.main.get_elapsed_time()`](./general-features.md#solmainget_elapsed_time).

`uniform float opacity`
: Opacity of the object the shader is applied to, between `0.0` and `1.0` Aways `1.0` if the shader is applied to the window.

### Shader examples

Here is a shader program that does nothing special: it just scales the input texture to fit the window, just like when no shader is set. Note that `\#version 100` tells the GLSL compiler to treat this shader as GLSL ES.

```glsl
// Vertex shader:
#version 100

precision mediump float;

uniform mat4 sol_mvp_matrix;
uniform mat3 sol_uv_matrix;
attribute vec2 sol_vertex;
attribute vec2 sol_tex_coord;
attribute vec4 sol_color;

varying vec2 sol_vtex_coord;
varying vec4 sol_vcolor;

void main() {
  gl_Position = sol_mvp_matrix * vec4(sol_vertex, 0.0, 1.0);
  sol_vcolor = sol_color;
  sol_vtex_coord = (sol_uv_matrix * vec3(sol_tex_coord, 1.0)).xy;
}

// Fragment shader:
#version 100

precision mediump float;
uniform sampler2D sol_texture;
in vec2 sol_vtex_coord;
in vec4 sol_vcolor;

void main() {
  vec4 tex_color = texture(sol_texture, sol_vtex_coord);
  gl_FragColor = tex_color * sol_vcolor;
}
```

Now, another example of a shader program that removes the red component of all pixels:

```glsl
// No vertex shader.

// Fragment shader:
#version 100

precision mediump float;
uniform sampler2D sol_texture;
in vec2 sol_vtex_coord;
in vec4 sol_vcolor;
out vec4 FragColor;

void main() {
  vec4 tex_color = texture(sol_texture, sol_vtex_coord);
  FragColor = tex_color * sol_vcolor * vec4(0.0, 1.0, 1.0, 1.0);
}
```

### Portability

The exact supported shader language depends on the OpenGL or OpenGL ES version Solarus is compiled with on every platform. Typically, on desktop platforms, Solarus is compiled with OpenGL support and the shader language used is some version of GLSL. On mobile platforms, OpenGL ES and GLSL ES are normally used instead.

For maximum portability, there are two possible approaches:

- Shaders can be written carefully to make sure that they work with both GLSL and GLSL ES. This requires using portability macros because there are some incompatibilities. This is the approach that we use in all our shaders. If you use the `\#version` macro, be careful because GLSL and GLSL ES have their own history of version numbers.

- Another solution is to load different shaders for GLSL and GLSL ES (but there will be some duplicated code). To do this, you can try to load a first shader, and in case of failure, catch the Lua error and try to load an alternative one. Or you can try to guess the correct one using [`sol.shader.get_opengl_version()`](#solshaderget_opengl_version) and/or [`sol.shader.get_shader_language_version()`](#solshaderget_shading_language_version).

In any case, the legacy OpenGL variables and attributes such as `gl_ModelViewProjectionMatrix` and `gl_Vertex`are not supported on modern plaftorms. You should therefore only use [Solarus built-in variables](#built-in-attributes-and-uniforms) plus your own ones.

## Functions of `sol.shader`

### `sol.shader.create(shader_id), sol.shader.create(properties)`

Loads and returns a shader program from a shader description file or from the given parameters. If the loading or the compilation of the shader program fails, a Lua error is raised.

| With a shader description file | With custom properties |
|------------------|------------------------------|
| <dl><dt>`shader_id` (string)</dt><dd>Id of the shader program to load (filename without extension, relative to the `shaders` directory).</dd></dl> | <dl><dt>`properties` (table)</dt><dd><p>Source and parameters of the shader program. Can have the following entries:</p><dl><dt>`vertex_source` (string, optional)</dt><dd>GLSL code of the vertex shader. No values means to use a default shader that does nothing special</dd><dt>`fragment_source` (string, optional)</dt><dd>GLSL code of the fragment shader. No values means to use a default shader that does nothing special.</dd><dt>`scaling_factor` (number, optional)</dt><dd>When the shader is applied to the whole window, indicates how much the input texture will be scaled by the shader. See [`shader:set_scaling_factor()`](#shaderset_scaling_factorscaling_factor) for more details.</dd></dl></dd></dl>|

Return value (shader)
: The shader program created.

!!! note "Note"
  
    Custom properties can be used for advanced purposes, but the normal way is to use the `shader_id` parameter, that is, to load a shader already described in a data file.

### `sol.shader.get_opengl_version()`

Returns the OpenGL or OpenGL ES version Solarus is being run with.

Return value (string)
: A string describing the OpenGL or OpenGL ES version.

### `sol.shader.get_shading_language_version()`

Returns the GLSL or GLSL ES format supported by the OpenGL or OpenGL ES version currently used.

Return value (string)
: A string describing the shading language version.

## Methods of the type shader

### `shader:get_id()`

Returns the id of this shader program.

Return value (string)
: Id of this shader program (filename without extension).

### `shader:get_vertex_file()`

Returns the name of the vertex shader file of this shader program.

Return value (string or `nil`)
: The vertex shader file name, relative to the `"shaders"` directory, or `nil` if no vertex shader file was set.

### `shader:get_vertex_source()`

Returns the vertex shader code of this shader program.

Return value (string)
: The vertex shader code. If no vertex shader was set, returns the default vertex shader code.

### `shader:get_fragment_file()`

Returns the name of the fragment shader file of this shader program.

Return value (string or `nil`)
: The fragment shader file name, relative to the `"shaders"` directory, or `nil` if no fragment shader file was set.

### `shader:get_fragment_source()`

Returns the fragment shader code of this shader program.

Return value (string)
: The fragment shader code. If no fragment shader was set, returns the default fragment shader code.

### `shader:get_scaling_factor()`

Returns the scaling factor of this shader. The scaling factor indicates how much the input texture will be scaled by the shader. See [`shader:set_scaling_factor()`](#shaderset_scaling_factorscaling_factor) for more details.

Return value (number or `nil`)
: The scaling factor. Returns `nil` if no scaling factor is set.

### `shader:set_scaling_factor(scaling_factor)`

Sets the scaling factor of this shader. The scaling factor indicates how much the input texture will be scaled by the shader when the shader is applied to the window.

This value only has an effect when the shader is applied to the window (with [`sol.video.set_shader()`](./video.md#solvideoset_shadershader)), and does nothing when the shader is applied to a drawable object (with [`drawable:set_shader()`](./drawable-objects/index.md#drawableset_shadershader)).

If no value is set (the default), then the output texture of the shader will directly have the size of the window. If a value is set, then the output texture of the shader will have the size of the input texture multiplied by this scaling factor (however, that output texture will then be scaled a second time to fit to the actual window). This is only useful for scaling shaders.

`scaling_factor` (number or `nil`)
: The scaling factor. A `nil` value means to set no scaling factor.

### `shader:set_uniform(uniform_name, value)`

Sets a uniform value to this shader.

Uniforms are input values to shader programs that are constant for all vertices and pixels during one rendering frame. Solarus automatically sets some uniform variables for you if you define them in the shader (see the [built-in variables](#built-in-attributes-and-uniforms) section above).

Use this function if you want to pass additional uniform values.

The type of the uniform in your shader source code will depend on the Lua type you pass here:

| Type                                     | In the shader, should be declared as |
| ---------------------------------------- | ------------------------------------ |
| `boolean`                                | `bool`                               |
| `number`                                 | `float`                              |
| Array of two `number`s                   | `vec2`                               |
| Array of three `number`s                 | `vec3`                               |
| Array of four `number`s                  | `vec4`                               |
| [surface](./drawable-objects/surface.md) | `sampler2D`                          |

If the shader has no uniform with the given name, then this method does nothing.

`uniform_name` (string)
: Name of the uniform in your shader program.

`value` (boolean or number or table or surface)
: The value to set. If it is a table, it should be an array of 2, 3 or 4 numbers.

!!! note "Note"

    If the value is a [surface](./drawable-objects/surface.md), it is passed by reference. This means that if you modify the surface later, the shader with automatically have its updated content.
