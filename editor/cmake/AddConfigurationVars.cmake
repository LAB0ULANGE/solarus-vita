set(SOLARUS_INSTALL_DATAROOTDIR "share" CACHE PATH "dataroot dir")
set(SOLARUS_INSTALL_DATADIR "${SOLARUS_INSTALL_DATAROOTDIR}/solarus-editor" CACHE PATH "data dir")
set(SOLARUS_INSTALL_BINDIR "bin" CACHE PATH "bin dir")

# As Qt generates ui header files, the build directory needs to be an include
# directory too.
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Tell CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Assets directory (autodetect if empty).
set(SOLARUSEDITOR_ASSETS_DIR "" CACHE PATH "Assets directory (autodetect if empty).")

# Disable GUI native dialogs.
set(SOLARUSEDITOR_NO_NATIVE_DIALOGS "OFF" CACHE BOOL "Disable native dialogs in the Solarus Editor GUI.")
