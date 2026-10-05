# - Find Solarus
# Find the Solarus includes and library.
# Does not look for the Solarus GUI library.
#
#  SOLARUS_INCLUDE_DIRS   - where to find solarus headers.
#  SOLARUS_LIBRARIES      - List of libraries when using libsolarus.
#  SOLARUS_VERSION_STRING -
#    Human-readable string containing the version of Solarus.
#
# A custom installation path to search can be provided using
# the SOLARUS_DIR environment or CMake variables.

# Try to find the Solarus include directory.
find_path(SOLARUS_INCLUDE_DIR
  NAMES
    solarus/core/MainLoop.h
  HINTS
    ${SOLARUS_DIR}
    $ENV{SOLARUS_DIR}
  PATHS
    /sw # Fink
    /opt/local # DarwinPorts
    /opt/csw # Blastwave
    /opt
  PATH_SUFFIXES
    include
)

# Try to find the Solarus library.
find_library(SOLARUS_LIBRARY
  NAMES
    solarus
  HINTS
    ${SOLARUS_DIR}
    $ENV{SOLARUS_DIR}
  PATHS
    /sw # Fink
    /opt/local # DarwinPorts
    /opt/csw # Blastwave
    /opt
  PATH_SUFFIXES
    lib
    lib64
)

# Extract Solarus version data from config header.
if(SOLARUS_INCLUDE_DIR AND EXISTS "${SOLARUS_INCLUDE_DIR}/solarus/core/config.h")
  file(STRINGS "${SOLARUS_INCLUDE_DIR}/solarus/core/config.h" SOLARUS_VERSION_DATA
    REGEX "^#define[ \t]+SOLARUS_[A-Z]+_VERSION[ \t]+[0-9]+")
  string(REGEX REPLACE
    ".*#define[ \t]+SOLARUS_MAJOR_VERSION[ \t]+([0-9]+).*" "\\1"
    SOLARUS_VERSION_MAJOR "${SOLARUS_VERSION_DATA}")
  string(REGEX REPLACE
    ".*#define[ \t]+SOLARUS_MINOR_VERSION[ \t]+([0-9]+).*" "\\1"
    SOLARUS_VERSION_MINOR "${SOLARUS_VERSION_DATA}")
  string(REGEX REPLACE
    ".*#define[ \t]+SOLARUS_PATCH_VERSION[ \t]+([0-9]+).*" "\\1"
    SOLARUS_VERSION_PATCH "${SOLARUS_VERSION_DATA}")
  set(SOLARUS_VERSION_STRING
    "${SOLARUS_VERSION_MAJOR}.${SOLARUS_VERSION_MINOR}.${SOLARUS_VERSION_PATCH}")
  unset(SOLARUS_VERSION_DATA)
  unset(SOLARUS_VERSION_MAJOR)
  unset(SOLARUS_VERSION_MINOR)
  unset(SOLARUS_VERSION_PATCH)
endif()

# Check if include and library directories were found.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Solarus
  REQUIRED_VARS
    SOLARUS_INCLUDE_DIR
    SOLARUS_LIBRARY
  VERSION_VAR
    SOLARUS_VERSION_STRING
)
set(SOLARUS_INCLUDE_DIRS
  "${SOLARUS_INCLUDE_DIR}"
)
set(SOLARUS_LIBRARIES ${SOLARUS_LIBRARY})

# Create an imported target for Solarus, including required dependencies.
if(SOLARUS_FOUND AND NOT TARGET solarus)
  find_package(SDL2 REQUIRED CONFIG)

  # TODO: Handle direct definition of libsolarus path in CMAKE
  # if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  #   list(APPEND SOLARUS_INCLUDE_DIRS
  #     "${SDL2_INCLUDE_DIR}"
  #     "${SOLARUS_INCLUDE_DIR}/../../solarus/build/include"
  #     "${SOLARUS_INCLUDE_DIR}/../../solarus/third_party/glad/include"
  #     "${SOLARUS_INCLUDE_DIR}/../../solarus/third_party/hqx"
  #     "${SOLARUS_INCLUDE_DIR}/../../solarus/third_party/snes_spc"
  #   )
  # else()
  list(APPEND SOLARUS_INCLUDE_DIRS
    "${SDL2_INCLUDE_DIR}"
    "${SOLARUS_INCLUDE_DIR}/solarus/third_party"
  )
  # endif()

  add_library(solarus UNKNOWN IMPORTED)
  set_target_properties(solarus PROPERTIES
    IMPORTED_LOCATION "${SOLARUS_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${SOLARUS_INCLUDE_DIRS}"
  )
endif()

# Mark cache variables as advanced.
mark_as_advanced(
  SOLARUS_LIBRARY
  SOLARUS_INCLUDE_DIR
)
