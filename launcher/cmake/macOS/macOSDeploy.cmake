set(SOLARUSLAUNCHER_CODESIGN_IDENTITY "4464B2CGYB" CACHE STRING "Codesigning identity (`security find-identity -v -p codesigning`)")
set(SOLARUSLAUNCHER_CODESIGN_ENABLED OFF CACHE BOOL "Enable codesigning")
set(SOLARUSLAUNCHER_NOTARIZE_ENABLED OFF CACHE BOOL "Enable notarization")
set(SOLARUSLAUNCHER_NOTARIZE_PROFILE "notarytool-password" CACHE STRING "Notarization profile (`xcrun notarytool store-credentials --apple-id <YOUR_APPLE_ID_EMAIL> --team-id 4464B2CGYB --password <APP_SPECIFIC_PASSWORD> --name <PROFILE_NAME>`)")
# set(DEPLOY_TOOL_OPTIONS_ARG "-libpath=/opt/homebrew/lib")

install(TARGETS ${PROJECT_NAME}
  BUNDLE DESTINATION . COMPONENT ${PROJECT_NAME}
  LIBRARY DESTINATION $<TARGET_FILE_NAME:${PROJECT_NAME}>.app/Contents/Frameworks COMPONENT ${PROJECT_NAME}
)

find_program(MAC_DEPLOY_QT NAMES macdeployqt)

message(STATUS "Deployment: enabled with ${MAC_DEPLOY_QT}")
if(NOT MAC_DEPLOY_QT)
  message(FATAL_ERROR "macdeployqt not found. Please install Qt and add it to your PATH.")
endif()

get_target_property(SOLARUSLAUNCHER_BUNDLE_BASENAME ${PROJECT_NAME} OUTPUT_NAME)
set(SOLARUSLAUNCHER_BUNDLE_NAME "${SOLARUSLAUNCHER_BUNDLE_BASENAME}.app")

file(CONFIGURE
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/fixup_bundle.cmake
  @ONLY
  CONTENT
  [=[
include(BundleUtilities)

# Used to generate Qt.conf & copy Qt plugins.
execute_process(COMMAND @MAC_DEPLOY_QT@ "${CMAKE_INSTALL_PREFIX}/@SOLARUSLAUNCHER_BUNDLE_NAME@")

# Get installed Qt Plugins.
file(GLOB_RECURSE QT_PLUGINS "${CMAKE_INSTALL_PREFIX}/@SOLARUSLAUNCHER_BUNDLE_NAME@/Contents/Plugins/*.dylib")

# Fixup the bundle.
fixup_bundle("${CMAKE_INSTALL_PREFIX}/@SOLARUSLAUNCHER_BUNDLE_NAME@"
  "${QT_PLUGINS}"
  "@QT6_INSTALL_PREFIX@/lib;/opt/homebrew/lib;@CMAKE_BINARY_DIR@/_deps/solarus-build"
)
]=]
)

install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/fixup_bundle.cmake COMPONENT ${PROJECT_NAME})

# Enable the following if you want to sign the bundle.
if(SOLARUSLAUNCHER_CODESIGN_ENABLED)
  if(SOLARUSLAUNCHER_CODESIGN_IDENTITY STREQUAL "")
    message(FATAL_ERROR "Please set SOLARUSLAUNCHER_CODESIGN_IDENTITY to a valid identity.")
  endif()
  message(STATUS "Codesigning: enabled with identity: ${SOLARUSLAUNCHER_CODESIGN_IDENTITY}")
  file(CONFIGURE
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/codesign_bundle.cmake
    @ONLY
    CONTENT
    [=[
message(STATUS "Executing codesign with @SOLARUSLAUNCHER_CODESIGN_IDENTITY@")
# FIXME: --deep is deprecated....
execute_process(COMMAND xcrun codesign
  --force
  --deep
  --options runtime
  --timestamp
  --sign "@SOLARUSLAUNCHER_CODESIGN_IDENTITY@"
  "${CMAKE_INSTALL_PREFIX}/@SOLARUSLAUNCHER_BUNDLE_NAME@"
  COMMAND_ERROR_IS_FATAL ANY
)

# check if the codesigning was successful
execute_process(COMMAND codesign --verify --deep --strict --verbose=4 "${CMAKE_INSTALL_PREFIX}/@SOLARUSLAUNCHER_BUNDLE_NAME@"
  COMMAND_ERROR_IS_FATAL ANY
)
]=]
  )
  install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/codesign_bundle.cmake COMPONENT ${PROJECT_NAME})

  # Enable the following if you want to notarize the bundle.
  if(SOLARUSLAUNCHER_NOTARIZE_ENABLED)
    file(CONFIGURE
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/notarize_bundle.cmake
      @ONLY
      CONTENT
      [=[
message(STATUS "Notarizing the bundle")
set(TMP_ZIP_PATH "/tmp/@SOLARUSLAUNCHER_BUNDLE_NAME@.zip")
execute_process(COMMAND ditto -c -k --keepParent
  "${CMAKE_INSTALL_PREFIX}/@SOLARUSLAUNCHER_BUNDLE_NAME@"
  "${TMP_ZIP_PATH}"
  COMMAND_ERROR_IS_FATAL ANY
)

execute_process(COMMAND xcrun notarytool submit --wait
  --keychain-profile "@SOLARUSLAUNCHER_NOTARIZE_PROFILE@"
  "${TMP_ZIP_PATH}"
  RESULT_VARIABLE NOTARIZE_RESULT
  COMMAND_ERROR_IS_FATAL ANY
)

if(NOT NOTARIZE_RESULT EQUAL 0)
  message(STATUS "Notarization failed with error code ${NOTARIZE_RESULT}")
  message(FATAL_ERROR "run : xcrun notarytool log --keychain-profile \"@SOLARUSLAUNCHER_NOTARIZE_PROFILE@\" <id>"
    COMMAND_ERROR_IS_FATAL ANY
  )
endif()

execute_process(COMMAND xcrun stapler staple "${CMAKE_INSTALL_PREFIX}/@SOLARUSLAUNCHER_BUNDLE_NAME@"
  COMMAND_ERROR_IS_FATAL ANY
)
]=]
    )
    install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/notarize_bundle.cmake COMPONENT ${PROJECT_NAME})
  endif()
endif()
