include(GNUInstallDirs)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set_property(GLOBAL PROPERTY AUTOGEN_TARGETS_FOLDER CMakeAutogenTargets)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Print some information about the system.
include(ProcessorCount)
ProcessorCount(NUM_CORES)
message(STATUS "Architecture: ${CMAKE_HOST_SYSTEM_PROCESSOR}")
message(STATUS "Available cores: ${NUM_CORES}")
message(STATUS "Compiler ID: ${CMAKE_CXX_COMPILER_ID}")

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/")

# Set the deployment target for macOS.
if(APPLE)
  if(NOT CMAKE_OSX_DEPLOYMENT_TARGET)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "14.0")
  endif()
endif()
