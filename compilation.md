# Solarus compilation

- [Overview](#overview)
- [External dependencies](#external-dependencies)
  - [Linux](#linux)
  - [macOS](#macos)
- [Compilation instructions](#compilation-instructions)
  - [Notes on some dependencies](#notes-on-some-dependencies)
  - [Configuring](#configuring)
  - [Building](#building)
  - [Installation](#installation)
- [Running a quest](#running-a-quest)
- [Platform-specific information](#platform-specific-information)
  - [Mac OS X](#mac-os-x)

## Overview

Solarus can run quests scripted in Lua.

A quest is a game, represented as a directory called `data` or a zip archive
called `data.solarus` or `data.solarus.zip` and containing all the data
files of the quest (graphics, musics, sounds, scripts, dialogs, etc.).
A short sample quest is provided in the directory `sample_quest`.

Our quests can be found on the website: https://www.solarus-games.org

This project is separated in mutliple parts: the core, its quest launcher and
its quest editor. This repository contains only the core part.

Compiling the core part generates:

1. A dynamic library called `solarus`.
2. An executable file called `solarus-run` that directly runs the quest passed
   as argument.

## External dependencies

The build process of Solarus uses [CMake](https://cmake.org/).
You also will need the
[pkg-config](https://www.freedesktop.org/wiki/Software/pkg-config/) helper tool
installed.

To compile Solarus, you need a C++ compiler with support of C++11.
Solarus compiles fine with GCC 4.8.1 or greater and Clang 3.4 or greater, and
probably with other recent compilers (please let us know).

The following libraries are required to compile and execute Solarus:

- SDL2 (2.0.18 or greater)
- SDL2main
- SDL2_image
- SDL2_ttf
- OpenGL or OpenGL ES
- OpenAL
- Vorbisfile
- Ogg
- Modplug (0.8.8.4 or greater)
- Lua 5.1 or LuaJIT 2.0 (LuaJIT is recommended)
- Physfs

Note that another library is directly embedded in the source code: `snes_spc`,
an SPC (Nintendo SNES) music decoding library.

### Notes on some dependencies

#### About Lua

Solarus uses the Lua scripting language.
LuaJIT, a _just-in-time_ implementation of Lua, is used by default for optimal
performance.
However, if you want to use vanilla Lua rather than LuaJIT, set the CMake
variable `SOLARUS_USE_LUAJIT` to `OFF` at the configuration step.

#### About ModPlug

We maintain a fork of libmodplug 0.8.8.4 here:
https://gitlab.com/solarus-games/libmodplug
Previous versions of libmodplug cause compilation problems as well as issues
like `.it` files that do not correctly loop.

#### About OpenGL ES

If you wish to enable the OpenGL ES implementation, set the CMake
variable `SOLARUS_GL_ES` to `ON` at the configuration step.

### Linux

Just install the corresponding packages.
For example, in Ubuntu, Debian or any Debian derivative:

```bash
apt update
apt install --no-install-recommends \
    build-essential \
    cmake \
    pkgconf \
    libsdl2-dev \
    libsdl2-image-dev \
    libsdl2-ttf-dev \
    libglm-dev \
    libluajit-5.1-dev \
    libphysfs-dev \
    libopenal-dev \
    libvorbis-dev \
    libmodplug-dev
```

### macOS

The directory `libraries/macOS` in the Solarus repository contains some
required static and dynamic libraries that are not provided as binaries by
their authors, but we don't guarantee that they are up-to-date.
The Lua documentation recommends to link statically against Lua, so you
shouldn't embed it.

Also, the OpenAL framework is already part of the macOS system since 10.4, so
you don't have to embed this library either.
You can use both `.dylib` or `.framework` for the other libraries, but just
remember that you have to check the version number, runpath search,
architecture and backward compatibility if you want to deploy your application.

## Compilation instructions

The following are the instructions for configuring and building Solarus.

Instructions for building for Windows using MSYS2 are in
[`compilation.MSYS2.md`](compilation.MSYS2.md).

### Configuring

Use CMake to generate the Makefiles or a project for your favorite IDE from the
CMake GUI or from the command-line.

#### Using the CMake GUI

Run the CMake GUI, then select the source directory of Solarus (for example
`C:\solarus`) and choose a build directory of your choice (for example
`C:\solarus\build`).

Click _Configure_ and choose what kind of project you want CMake to generate
Makefiles, a Code::Blocks project, a Visual Studio project, etc.

If CMake does not automatically find a compiler, indicate its installation path
by creating a variable `CMAKE_PREFIX_PATH` with the appropriate value
(for example `C:\MinGW`).

This prefix path should be a directory that contains include, lib and bin
directories with all the required dependencies in them.
After that, click _Configure_ again. CMake should be now able to find all
includes and libraries.
If it does not, check that you have correctly installed them.

Then click _Generate_ to generate the project files.

#### Using the CMake command-line

Go to the `solarus` directory and type the following:

```bash
mkdir build
cd build
cmake ..
```

This generates appropriate Makefiles for your system in the `build` directory.
If you prefer, you can also generate other kinds of projects with the
appropriate `-G` option (see the documentation of CMake for more information).

### Building

After configuring the build, you can now compile the Solarus project using the
files generated by CMake.

#### Using a GUI

If you chose to generate a project for an IDE (like Code::Blocks or
Qt Creator), open the generated project an compile it from your IDE.

#### Using the command-line

If you chose to generate Makefiles, you can compile from the command-line:

```bash
make
```

Any build error messages will tell you which headers and libraries are missing
(it is hard to get them right in one try!).

Once the compilation is finished, you will obtain a dynamic library named
`solarus` and an executable named `solarus-run`.

### Installation

Installing Solarus is _optional_.

However, installing Solarus can be useful in some cases.
If you want it in your `PATH` (on Unix), or if you want to remove the build
directory, or if you want to use the Solarus libraries in another project.

#### Using a GUI

Copy the `solarus` library and the `solarus-run` executable where you want them
to be installed.

If you intend to use the Solarus libraries from another project, you need to
copy the whole include/solarus directory somewhere as well.

_Important:_ also copy the `build/include/solarus/config.h` file and place it
with the other installed headers, next to `config.h.in`.
The `config.h` file is a header generated at the configuration step, this is
why it is in the build directory and not with the rest of source files.

#### Using the command-line

If you generated Makefiles with CMake, an appropriate install rule is
available:

```bash
make install
```

This will install all compiled Solarus libraries, executables and headers
(including the generated header `config.h`).
The default installation directory probably requires administrative privileges.

To change the installation directory, change the CMake variable
`CMAKE_INSTALL_PREFIX` at the configuration step.
The executables, the libraries and the headers will be installed in
standard subdirectories of this install prefix, i.e. `bin`, `lib` and `include`.

If you need finer-grained customization, the name of these three subdirectories
can also be changed in CMake.
For this, set the variables `SOLARUS_EXECUTABLE_INSTALL_DESTINATION`,
`SOLARUS_LIBRARY_INSTALL_DESTINATION` and `SOLARUS_HEADERS_INSTALL_DESTINATION`.

## Running a quest

To directly execute a quest without a GUI, run the `solarus-run` executable.
The quest path must be specified on the command line.
If you don't specify it, it will be the current directory by default.

The quest path is a directory containing the `data` subdirectory of your quest,
or containing the `data.solarus` or `data.solarus.zip` archive of your quest,
or an archive containing your quest files (content of the `data` directory).

This means that if you run `solarus-run` without parameters, then a quest must
exist in the current directory.

Type from your build directory:

```bash
./solarus-run path/to/your/quest
```

## Platform-specific information

### macOS

#### Pre-requisites

To install the needed packages via the brew package manager, type:

```bash
brew install cmake ninja sdl2 sdl2_image sdl2_ttf luajit lua@5.1 libvorbis libmodplug physfs glm openal-soft
```

You will also need a copy of the OS X SDK, which is included with XCode.
XCode is freely available on the App Store.

#### Compile on macOS

You can follow the standard compilation instructions to build Solarus on macOS.
However here are some tips that may be useful depending on your environment.

You can build the XCode project using CMake:

```bash
cmake -G Xcode ..
```

If you are compiling on an Apple Silicon architecture, Solarus will by default
still compile an Intel x86_64 binary for better compatibility.
But for this to work, you need all your dependencies to be also x86_64.
Alternatively, you can also build an actual Apple Silicen (arm64)
binary by using the SOLARUS_ARCH CMake variable as follows:

```bash
cmake -DSOLARUS_ARCH=arm64 ..
```

#### Deploy on macOS

If you wish to deploy a quest on non-developer environment with a standalone
bundle, the better way is to use the Solarus package provided on
<https://www.solarus-games.org/> and use a post-build script to replace the
`Solarus-run.app/Contents/Resources/solarus-run` and
`Solarus-run.app/Contents/Frameworks/libsolarus.dylib` files by the ones
just compiled.
The bundle is made to run the `data/` or `data.solarus` quest located in the
`Solarus-run.app/Contents/Resources` directory.

Once done, you eventually have to make sure that all new embedded binaries
are undependant from your developer environment.

Open a new command-line window and run the following on all of them:

```bash
otool -L <binary_to_check>
```

Verify that all dependencies path related to an embedded binary are correct
and begin with the `@rpath/` string. If not, run this to update the first line
returned by `otool -L`:

```bash
install_name_tool -id <binary_path>
```

or run this to update one of the other:

```bash
install_name_tool -change <old_path> <new_path>
```
