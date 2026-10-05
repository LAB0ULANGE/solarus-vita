# This file adds the necessary dependencies of the Solarus library.

# Whether LuaJIT should be used instead of vanilla Lua.
option(SOLARUS_USE_LUAJIT "Use LuaJIT instead of default Lua (recommended)" ON)

# Tell FindOpengl to use modern GL lib system
set(OpenGL_GL_PREFERENCE GLVND)

# Find dependencies.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/")
find_package(SDL2 "2.0.18" REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
if(NOT ANDROID)
  find_package(OpenGL)
endif()
find_package(GLM REQUIRED)
find_package(OpenAL REQUIRED)
find_package(Vorbis REQUIRED)
find_package(VorbisFile REQUIRED)
find_package(Ogg REQUIRED)
find_package(ModPlug REQUIRED)
find_package(PhysFS REQUIRED)
if(SOLARUS_USE_LUAJIT)
  find_package(LuaJIT REQUIRED)
else()
  find_package(Lua "5.1" EXACT REQUIRED)
  add_library(Lua::Lua UNKNOWN IMPORTED)
  set_target_properties(Lua::Lua PROPERTIES
    IMPORTED_LOCATION "${LUA_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${LUA_INCLUDE_DIR}"
  )
endif()
if(SOLARUS_PROFILING)
  find_package(easy_profiler REQUIRED)
endif()

# These dependencies are managed internally.
add_subdirectory(third_party)
