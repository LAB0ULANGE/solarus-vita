# Contributing

## Build Instructions

### macOS

1. Install dependencies.

   ```sh
   brew install
     cmake \
     ninja \
     sdl2 \
     sdl2_image \
     sdl2_ttf \
     luajit \
     lua@5.1 \
     libvorbis \
     libmodplug \
     physfs \
     glm \
     openal-soft \
     qt@6
   ```

2. Configure and build the project. CMake presets are provided for ease of use.

   ```sh
   cmake --preset macos
   cmake --build --preset macos
   cpack --preset macos
   ```

   Alternatively, there is simply this one-liner to configure, build and package:

   ```sh
   cmake --workflow macos
   ```

   Available options when configuring with cmake:
   - `-DSOLARUS_FETCHCONTENT=<ON|OFF>`: Will clone Solarus' repo instead of trying to finding it in the system's installed libraries. Default is `ON`.
   - `-DSOLARUSLAUNCHER_DEPLOY_ENABLED=<ON|OFF>`: Enables deploying all the dependencies to the macOS bundle with `macdeployqt`. Default is `ON`.
   - `-DSOLARUSLAUNCHER_CODESIGN_ENABLED=<ON|OFF>`: Enables codesigning the macOS bundle with `codesign`. Default is `OFF`.
   - `-DSOLARUSLAUNCHER_CODESIGN_IDENTITY=<YOUR_ID>`: Organization's identifier to use with macOS's `codesign`. Default is Solarus Lab's id.
   - `-DSOLARUSLAUNCHER_NOTARIZE_ENABLED=<ON|OFF>`: Enables notarizing after codesigning the macOS bundle. Must be done on an approved machine by Solarus Labs and Apple. Default is `ON`.
   - `-DSOLARUSLAUNCHER_CPACK_PRETTY_DMG_ENABLED=<ON|OFF>`: Activates or skips the long AppleScript call when building the `.dmg`. Useful when debugging. Default is `ON`.

   Typically, to build a signed and notarized package for a release, these variables should be enabled. This is handlded by CMake presets:

   ```sh
   rm -rf build
   cmake --preset macos-signed \
   cmake --build --preset macos-signed \
   cpack --preset macos-signed
   ```

   Alternatively, you may use this equivalent one-liner:

   ```sh
   cmake --workflow macos-signed
   ```

   Note that notarizing may take some time (from 5 minutes to a few hours) as it sends the package to Apple for validation.

   See the [Code-Signing on macOS](#code-signing-on-macos) section for more details.

### Windows (MSYS2)

On Windows, we use MSYS2, a Unix-like environment that makes installing dependencies and compiling much simpler.

1. Install dependencies.

   ```sh
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
       mingw-w64-ucrt-x86_64-SDL2_ttf \
       mingw-w64-ucrt-x86_64-qt6-base \
       mingw-w64-ucrt-x86_64-qt6-tools
   ```

2. Configure and build the project. CMake presets are provided for ease of use.

   ```sh
   cmake --preset msys2
   cmake --build --preset msys2
   cpack --preset msys2
   ```

   Alternatively, there is simply this one-liner to configure, build and package:

   ```sh
   cmake --workflow msys2
   ```

### Debian (Ubuntu, etc.)

1. Install dependencies.

   ```sh
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
       libmodplug-dev \
       qt6-base-dev \
       qt6-base-dev-tools \
       qt6-tools-dev
   ```

2. Configure and build the project. CMake presets are provided for ease of use.

   ```sh
   cmake --preset linux
   cmake --build --preset linux
   ```

## Internationalization

1. If needed, add the language to `CMakeLists.txt`. Please use its standard code.

   ```cmake
   qt_standard_project_setup(
     I18N_TRANSLATED_LANGUAGES en_US fr_FR # <-- Here.
     I18N_SOURCE_LANGUAGE en_US
   )
   ```

2. Configure the project with CMake.

   ```sh
   cmake --preset macos
   ```

3. Update the `.ts` files by building the target that will internally call `lupdate`:

   ```sh
   cmake --build --preset macos-i18n
   ```

4. Open the `.ts` file(s) with **Qt Linguist** and fill the blanks for all the strings. Don't forget to save the file.

## Code-Signing

### Code-Signing on macOS

On macOS, the app needs to be code-signed and notarized in order to be properly distributed outside the App Store. This process ensures that the app is trusted by macOS and avoids security warnings when users try to run it.

1. Make sure you have access to Solarus Labs' Apple Developer account, with your own Apple ID registered in the Solarus Labs team.

2. Download the following files from Solarus Labs' Apple Developer account and install them in your Keychain Access on your machine:
   - **For code-signing:** A certificate named `"Developer ID Application: Solarus Labs (4464B2CGYB)"`.
   - **For notarization:** An App-Specific Password generated from the Apple ID account page.

3. Create a profile for notarization on your machine with the following command:

   ```sh
   xcrun notarytool store-credentials "notarytool-password" --apple-id "<YOUR_APPLE_ID_EMAIL>" --team-id "4464B2CGYB" --password "<APP_SPECIFIC_PASSWORD>"
   ```

   - `"notarytool-password"`: is the default name of the profile (see the macOSDeploy.cmake file). It could be "solarus-launcher-notarytool" or any name you want. You can set it with the CMake variable `-DSOLARUSLAUNCHER_NOTARIZE_PROFILE` when configuring the project.
   - `<YOUR_APPLE_ID_EMAIL>`: is your Apple ID email, registered in Solarus Labs team.
   - `4464B2CGYB`: is Solarus Labs' Team ID.
   - `<APP_SPECIFIC_PASSWORD>`: is the app-specific password you generated in step 2.

   It will link your Apple ID and the team to the profile name you chose, and allow your machine to notarize apps on behalf of Solarus Labs.

4. When building the macOS signed and notarized package, make sure to set the following CMake variables, or just use the provided CMake preset `macos-signed` that already includes them:
   - `-DSOLARUSLAUNCHER_CODESIGN_ENABLED=ON`
   - `-DSOLARUSLAUNCHER_CODESIGN_IDENTITY="Developer ID Application: Solarus Labs (4464B2CGYB)"` (already set in the CMake presets)
   - `-DSOLARUSLAUNCHER_NOTARIZE_ENABLED=ON`
   - `-DSOLARUSLAUNCHER_NOTARIZE_PROFILE="notarytool-password"` (or the name you chose in step 3)

   See the [macOS Build Instructions](#macos) section.
