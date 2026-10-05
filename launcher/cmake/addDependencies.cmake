# Find Qt.
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Svg Network LinguistTools)
if (NOT Qt6LinguistTools_FOUND AND QT_HOST_PATH)
  find_package(Qt6LinguistTools "6.4"
               PATHS "${QT_HOST_PATH}" "${QT_HOST_PATH}/lib/cmake"
               NO_CMAKE_FIND_ROOT_PATH NO_DEFAULT_PATH QUIET)
endif()
if (NOT Qt6LinguistTools_FOUND)
  message(FATAL_ERROR "CMake module Qt6LinguistTools could not be found.")
endif()
if (Qt6_VERSION VERSION_LESS 6.4)
  message(FATAL_ERROR "At least Qt 6.4 is needed for this project.")
endif()

# ------------------------------------------------------------------------------

include(FetchContent)

# ------------------------------------------------------------------------------

# Qlementine-Icons: an SVG icon library made for Qt.
FetchContent_Declare(qlementine-icons
  GIT_REPOSITORY "https://github.com/oclero/qlementine-icons.git"
  GIT_TAG v1.13.0
  EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(qlementine-icons)
set_target_properties(qlementine-icons
  PROPERTIES
    FOLDER dependencies
)

# ------------------------------------------------------------------------------

# Qlementine: the QStyle library to have a modern look n' feel.
FetchContent_Declare(qlementine
  GIT_REPOSITORY "https://github.com/oclero/qlementine.git"
  GIT_TAG v1.4.1
  EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(qlementine)
set_target_properties(qlementine
  PROPERTIES
    FOLDER dependencies
)

# ------------------------------------------------------------------------------

# QtAppInstanceManager: a library to manage multiple instances of a Qt application.
FetchContent_Declare(QtAppInstanceManager
 GIT_REPOSITORY "https://github.com/oclero/qtappinstancemanager.git"
 GIT_TAG v1.3.1
 EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(QtAppInstanceManager)
set_target_properties(QtAppInstanceManager
  PROPERTIES
    FOLDER dependencies
)

# ------------------------------------------------------------------------------

# Solarus: the engine.
# If this flag is set, CMake will download Solarus from its git repository
# instead of using a locally pre-built version.
set(SOLARUS_FETCHCONTENT OFF CACHE BOOL "Enable downloading of Solarus from its git repository")
set(SOLARUS_CLI OFF)
if(SOLARUS_FETCHCONTENT)
  message(STATUS "Solarus: Fetching from git repository")
  set(SOLARUS_DEPLOYMENT ${CMAKE_OSX_DEPLOYMENT_TARGET})
  set(SOLARUS_FILE_LOGGING OFF)
  FetchContent_Declare(solarus
    GIT_REPOSITORY "https://gitlab.com/solarus-games/solarus.git"
    GIT_TAG v2.0.0
  )
  FetchContent_MakeAvailable(solarus)
  set_target_properties(solarus
    PROPERTIES
      FOLDER dependencies
  )
  # Disable warnings for Solarus.
  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    target_compile_options(solarus PRIVATE -w)
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(solarus PRIVATE /w)
  endif()
else()
  # Ensure that Solarus is found, one way or another.
  message(STATUS "Solarus: Using from system")
  find_package(OpenAL REQUIRED)
  find_package(Solarus REQUIRED)
endif()
