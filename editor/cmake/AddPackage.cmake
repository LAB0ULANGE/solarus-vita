set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_DESCRIPTION}")
set(CPACK_PACKAGE_VENDOR "${SOLARUSEDITOR_ORGANISATION}")
set(CPACK_PACKAGE_CONTACT "${SOLARUSEDITOR_EMAIL}")
set(CPACK_PACKAGE_LICENSE "${SOLARUSEDITOR_LICENSE}")
set(CPACK_COMPONENTS_ALL ${COMPONENT_NAME})

# Function to get a user-friendly system name.
function(get_pretty_system_name OUTPUT_VAR)
  if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(SYS_NAME "macos")
  elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(SYS_NAME "windows")
  elseif(UNIX AND NOT APPLE OR MINGW)
    set(SYS_NAME "linux")
  else()
    set(SYS_NAME "${CMAKE_SYSTEM_NAME}")
  endif()
  string(TOLOWER "${SYS_NAME}" SYS_NAME)
  string(REPLACE " " "_" SYS_NAME "${SYS_NAME}")
  string(REPLACE "-" "_" SYS_NAME "${SYS_NAME}")
  set(${OUTPUT_VAR} "${SYS_NAME}" PARENT_SCOPE)
endfunction()

# Function to get a user-friendly architecture name.
function(get_pretty_arch_name OUTPUT_VAR)
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
    set(ARCH_NAME "x64")
  elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
    set(ARCH_NAME "arm64")
  else()
    set(ARCH_NAME "${CMAKE_SYSTEM_PROCESSOR}")
  endif()
  string(TOLOWER "${ARCH_NAME}" ARCH_NAME)
  string(REPLACE " " "_" ARCH_NAME "${ARCH_NAME}")
  string(REPLACE "-" "_" ARCH_NAME "${ARCH_NAME}")
  set(${OUTPUT_VAR} "${ARCH_NAME}" PARENT_SCOPE)
endfunction()

# Function to get the package name in the following format:
# name-version-platform-architecture
# Example: solarus-editor-v2.0.0-macos-arm64
function(get_package_name OUTPUT_VAR)
  get_pretty_system_name(SYS_NAME)
  get_pretty_arch_name(ARCH_NAME)
  set(PACKAGE_NAME "${PROJECT_NAME}-v${CPACK_PACKAGE_VERSION}-${SYS_NAME}-${ARCH_NAME}")
  set(${OUTPUT_VAR} "${PACKAGE_NAME}" PARENT_SCOPE)
endfunction()

# Override the default package name
get_package_name(CPACK_PACKAGE_FILE_NAME)
