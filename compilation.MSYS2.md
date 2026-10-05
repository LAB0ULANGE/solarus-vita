# Solarus Compilation Using MSYS2

This document details how to build Solarus for Windows using the MSYS2 toolchain.

From <https://www.msys2.org>:

> MSYS2 is a collection of tools and libraries providing you with an easy-to-use
> environment for building, installing and running native Windows software.
>
> It consists of a command line terminal called mintty, bash, version control systems
> like git and subversion, tools like tar and awk and even build systems like autotools,
> all based on a modified version of Cygwin.
>
> Despite some of these central parts being
> based on Cygwin, the main focus of MSYS2 is to provide a build environment for native
> Windows software and the Cygwin-using parts are kept at a minimum.
>
> MSYS2 provides
> up-to-date native builds for GCC, mingw-w64, CPython, CMake, Meson, OpenSSL,
> FFmpeg, Rust, Ruby, just to name a few.

## Installing MSYS2

Follow the official [Installation](https://www.msys2.org/#installation) documentation to
setup and prepare MSYS2. The MSYS2 installer will create a shortcut for each available
[environment](https://www.msys2.org/docs/environments/) in your Start Menu.

The recommended environment to use for building Solarus is **UCRT64**.

## Updating MSYS2

MSYS2 is a rolling release distribution and it is strongly advised to keep it up to date.
Follow the official [Updating](https://www.msys2.org/docs/updating/) documentation to
update your MSYS2 installation from time to time.

## Dependency Packages

To build Solarus in MSYS2, the following dependency packages are required to be installed:

```bash
pacman --noconfirm --needed -S \
    git \
    mingw-w64-ucrt-x86_64-cmake \
    mingw-w64-ucrt-x86_64-gcc \
    mingw-w64-ucrt-x86_64-glm \
    mingw-w64-ucrt-x86_64-libmodplug \
    mingw-w64-ucrt-x86_64-libvorbis \
    mingw-w64-ucrt-x86_64-luajit \
    mingw-w64-ucrt-x86_64-ninja \
    mingw-w64-ucrt-x86_64-openal \
    mingw-w64-ucrt-x86_64-physfs \
    mingw-w64-ucrt-x86_64-pkgconf \
    mingw-w64-ucrt-x86_64-SDL2 \
    mingw-w64-ucrt-x86_64-SDL2_image \
    mingw-w64-ucrt-x86_64-SDL2_ttf
```

The following additional dependency packages are required for building GUI components:

```bash
pacman --noconfirm --needed -S \
    mingw-w64-ucrt-x86_64-qt6-base \
    mingw-w64-ucrt-x86_64-qt6-svg \
    mingw-w64-ucrt-x86_64-qt6-tools
```

> **Note:** The GUI components include the Solarus Launcher and the Solarus Editor.

The following additional dependency packages are required for debugging Solarus builds:

```bash
pacman --noconfirm --needed -S \
    mingw-w64-ucrt-x86_64-gdb
```

## Building Solarus

Ensure that the [dependency packages](#dependency-packages) for Solarus are installed in
your MSYS2 installation.

Launch the **UCRT64** shell and clone the repository:

```bash
git clone https://gitlab.com/solarus-games/solarus.git
```

Go into the cloned repository and configure the build using CMake:

```bash
cd solarus
cmake -B build -G "Ninja"
```

The following common configuration options can be added to the CMake command above:

- `-DCMAKE_BUILD_TYPE=Debug`: Configure the build for debugging.

Start the configured build using CMake:

```bash
cmake --build build
```

> **Note:** The built binaries will be located in the `build` directory.

After the build is completed, optionally run the Solarus test suite:

```bash
ctest --test-dir build
```

## Running the Build

After [building Solarus](#building-solarus), working binaries should be in the `build` directory.

You can invoke the CLI binary directly from the **UCRT64** shell to start a quest:

```bash
cd build
cli/solarus-run.exe /path/to/quest
```

If you configured the build for debugging, you can use the standard
[GDB debugger](https://sourceware.org/gdb/download/onlinedocs/gdb.html/index.html):

```bash
cd build
gdb --args solarus-run.exe /path/to/quest
(gdb) run
```

### Tips

- To stop the program execution at any time, use <kbd>CTRL</kbd>+<kbd>C</kbd> in the `gdb` console.
- To resume execution after a stop, type `continue` or `c`.
- To get a stack trace for the currently running thread, type `bt`, or for all running threads, `thread apply all bt` (long output!).
- To exit `gdb` and stop the program, type `quit` or `q`.

A cheat sheet for `gdb` can be found [here](https://github.com/nicolasventer/GDB-cheat-sheet).
