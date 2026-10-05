set(CMAKE_MACOSX_RPATH ON)
set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)

####
# By default, you will build a binary with the maximum of compatibility that your current version can provide.
# You may want to produce an optimized (or exotic) binary instead of a standard one.
# To do so, overload the following options :
#
# SOLARUS_ARCH represent the build (multi-)architecture.
# SOLARUS_DEPLOYMENT represent the oldest macOS version supported.
#
# Some options set to the cache also need the FORCE parameter for obscure reasons.
#
# Exportable to XCode.
####

# Build architectures.
if(NOT SOLARUS_ARCH)
  set(SOLARUS_ARCH ${CMAKE_HOST_SYSTEM_PROCESSOR})
endif()
set(CMAKE_OSX_ARCHITECTURES "${SOLARUS_ARCH}" CACHE STRING "Build architecture" FORCE)

# Deployment version.
if(NOT SOLARUS_DEPLOYMENT)
  set(SOLARUS_DEPLOYMENT "14.0")
endif()
set(CMAKE_OSX_DEPLOYMENT_TARGET "${SOLARUS_DEPLOYMENT}" CACHE STRING "Oldest OS version supported" FORCE)

# LuaJIT workaround.
# According to LuaJIT doc, OSX needs to link with additional flags if 64bit build is requested
if(SOLARUS_USE_LUAJIT AND SOLARUS_ARCH MATCHES "x86_64")
  if(XCODE)
    set_property(TARGET solarus-editor PROPERTY
      "XCODE_ATTRIBUTE_LINKER_FLAGS[arch=x86_64]" "-pagezero_size 10000 -image_base 100000000"
    )
  elseif(NOT CMAKE_EXE_LINKER_FLAGS MATCHES "-pagezero_size 10000 -image_base 100000000")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pagezero_size 10000 -image_base 100000000" CACHE STRING "LuaJIT woraround" FORCE)
  endif()
endif()

# Bundle icon.
set(SOLARUSEDITOR_ICON_FILENAME "solarus-editor.icns")
set(SOLARUSEDITOR_ICON_PATH "${CMAKE_CURRENT_SOURCE_DIR}/resources/app_icon/${SOLARUSEDITOR_ICON_FILENAME}")
set_source_files_properties(${SOLARUSEDITOR_ICON_PATH} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
target_sources(solarus-editor PRIVATE
  ${SOLARUSEDITOR_ICON_PATH}
)

# Plist.
set(SOLARUSEDITOR_PLIST_PATH "${CMAKE_CURRENT_BINARY_DIR}/Info.plist")
set(SOLARUSEDITOR_EXECUTABLE_NAME ${SOLARUSEDITOR_DISPLAY_NAME})
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/macOS/Info.plist.in"
  "${SOLARUSEDITOR_PLIST_PATH}"
  @ONLY
)
target_sources(solarus-editor PRIVATE
  ${SOLARUSEDITOR_PLIST_PATH}
)
set_target_properties(${PROJECT_NAME} PROPERTIES
  OUTPUT_NAME ${SOLARUSEDITOR_DISPLAY_NAME} # To get a pretty name in  macOS Launchpad.OUTPUT_NAME ${SOLARUSLAUNCHER_DISPLAY_NAME} # To get a pretty name in  macOS Launchpad."
  MACOSX_BUNDLE_INFO_PLIST "${SOLARUSEDITOR_PLIST_PATH}"
  MACOSX_BUNDLE_BUNDLE_NAME "${SOLARUSEDITOR_APP_DISPLAY_NAME}"
  MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
  MACOSX_BUNDLE_EXECUTABLE_NAME ${SOLARUSEDITOR_EXECUTABLE_NAME}
  MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
  MACOSX_BUNDLE_GUI_IDENTIFIER ${SOLARUSEDITOR_APP_ID}
  MACOSX_BUNDLE_COPYRIGHT ${SOLARUSEDITOR_COPYRIGHT}
  MACOSX_BUNDLE_ICON_FILE ${SOLARUSEDITOR_ICON_FILENAME}
  MACOSX_BUNDLE TRUE
  XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED OFF
  XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ""
  XCODE_ATTRIBUTE_CODE_SIGN_STYLE "Manual"
  XCODE_ATTRIBUTE_CODE_SIGN_INJECT_BASE_ENTITLEMENTS OFF
  XCODE_ATTRIBUTE_OTHER_CODE_SIGN_FLAGS ""
)
