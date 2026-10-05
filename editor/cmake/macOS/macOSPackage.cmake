set(SOLARUSEDITOR_CPACK_PRETTY_DMG_ENABLED ON CACHE BOOL "Enable DMG customization with AppleScript")

get_target_property(SOLARUSEDITOR_BUNDLE_BASENAME ${PROJECT_NAME} OUTPUT_NAME)

if(SOLARUSEDITOR_CPACK_PRETTY_DMG_ENABLED)
  # TODO it should be possible to have a different name for the bundle and the
  # executable, but I can't make it work...
  set(DMG_APPLESCRIPT "${CMAKE_CURRENT_BINARY_DIR}/DMGSetup.applescript")
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cpack/DragNDrop/DMGSetup.applescript.in
    ${DMG_APPLESCRIPT}
    @ONLY
  )
endif()

set(CPACK_PACKAGE_NAME "${SOLARUSEDITOR_BUNDLE_BASENAME}")
set(CPACK_PACKAGE_FILE_NAME "${SOLARUSEDITOR_BUNDLE_BASENAME}") # CPack can't find if not named like this.
set(CPACK_GENERATOR "DragNDrop")
set(CPACK_PACKAGE_VENDOR "${SOLARUSEDITOR_ORGANISATION}")
set(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cpack/DragNDrop/DMGVolumeIcon.icns")
set(CPACK_BUNDLE_NAME "${SOLARUSEDITOR_BUNDLE_BASENAME}")
set(CPACK_DMG_VOLUME_NAME "${SOLARUSEDITOR_DISPLAY_NAME} Installer v${PROJECT_VERSION}")
set(CPACK_DMG_BACKGROUND_IMAGE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cpack/DragNDrop/DMGBackground@2x.png")
if(DMG_APPLESCRIPT)
  set(CPACK_DMG_DS_STORE_SETUP_SCRIPT "${DMG_APPLESCRIPT}")
endif()
set(CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_CURRENT_BINARY_DIR};${PROJECT_NAME};${PROJECT_NAME};/")
# Components.
set(CPACK_COMPONENTS_ALL ${PROJECT_NAME})

# Generate a CMake script that will rename the package after it has been
# created. This is needed because CPack does not allow to set the package name
# and the file name separately.
get_package_name(CPACK_RENAMED_MACOS_PACKAGE_FILE_NAME)
message(STATUS "CPack renamed package name: ${CPACK_RENAMED_MACOS_PACKAGE_FILE_NAME}")
file(CONFIGURE
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/rename_package.cmake
  @ONLY
  CONTENT
  [=[
# Extract the first file from CPACK_PACKAGE_FILES
string(FIND "${CPACK_PACKAGE_FILES}" ";" FIRST_SEMICOLON_INDEX)
if(FIRST_SEMICOLON_INDEX EQUAL -1)
  set(FIRST_FILE "${CPACK_PACKAGE_FILES}")
else()
  string(SUBSTRING "${CPACK_PACKAGE_FILES}" 0 ${FIRST_SEMICOLON_INDEX} FIRST_FILE)
endif()

# Copy the first file to the renamed file
execute_process(COMMAND ${CMAKE_COMMAND} -E copy
  "${FIRST_FILE}"
  "@CMAKE_CURRENT_BINARY_DIR@/@CPACK_RENAMED_MACOS_PACKAGE_FILE_NAME@.dmg"
)
]=]
)

set(CPACK_POST_BUILD_SCRIPTS "${CMAKE_CURRENT_BINARY_DIR}/rename_package.cmake")
