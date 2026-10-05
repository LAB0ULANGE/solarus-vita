# Glad

Glad is a Vulkan/GL/GLES/EGL/GLX/WGL Loader-Generator based on the official specifications for
multiple programming languages. Solarus uses Glad for GL/GLES programming.

## Glad Generator

Glad source files can be easily generated online using an [official generator](https://glad.dav1d.de/).

This document explains how to generate Glad source files for Solarus.

1. Open the current [`include/glad/glad.h`](include/glad/glad.h) file to obtain the URL used
   to generate the Glad source files in a comment at the top. For example:
   ```
   https://glad.dav1d.de/#profile=core&language=c&specification=gl&loader=on&api=gl%3D3.0&api=gles2%3D2.0&extensions=GL_ARB_framebuffer_object&extensions=GL_ARB_vertex_array_object
   ```

2. Navigate to the generator URL obtained above and click the **Generate** button at the bottom of
   the page. All settings for generating the Glad source files should be pre-configured.

3. The generator application will present the generated files for downloading. They are provided in
   both, ZIP compressed (recommended) and uncompressed forms.

4. Download the compressed or uncompressed files and replace the files in the repository with these.
