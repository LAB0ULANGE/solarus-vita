# AppID to report to the window system on Linux/Wayland.
# From a user PoV this also determines were the corresponding XDG metadata is installed in the system.
# Also used as the macOS bundle identifier.
set(SOLARUSEDITOR_APP_ID "org.solarus-games.solarus-editor")

# Application display name.
set(SOLARUSEDITOR_APP_DISPLAY_NAME "Solarus Editor")

# Executable name.
set(SOLARUSEDITOR_EXECUTABLE_NAME "solarus-editor")

# More metadata use on multiple platforms.
string(TIMESTAMP PROJECT_BUILD_YEAR "%Y")
set(SOLARUSEDITOR_DISPLAY_NAME "Solarus Editor")
set(SOLARUSEDITOR_EXECUTABLE_NAME "${PROJECT_NAME}")
set(SOLARUSEDITOR_ORGANISATION "Solarus Labs")
set(SOLARUSEDITOR_COPYRIGHT "© 2006-${PROJECT_BUILD_YEAR} ${SOLARUSEDITOR_ORGANISATION}.")
set(SOLARUSEDITOR_WEBSITE "https://www.solarus-games.org")
set(SOLARUSEDITOR_LICENSE "GPL-3.0 and CC-BY-SA-4.0") # Should be SPDX identifiers.
set(SOLARUSEDITOR_TRADEMARKS "Licensed under ${SOLARUSEDITOR_LICENSE}")
set(SOLARUSEDITOR_EMAIL "solarus-labs@solarus-games.org")

# Install paths to help find the application assets.
set(SOLARUSEDITOR_SOURCE_PATH "${CMAKE_SOURCE_DIR}")
set(SOLARUSEDITOR_BINDIR_PATH "${CMAKE_INSTALL_PREFIX}/${SOLARUS_INSTALL_BINDIR}")
set(SOLARUSEDITOR_DATADIR_PATH "${CMAKE_INSTALL_PREFIX}/${SOLARUS_INSTALL_DATADIR}")
