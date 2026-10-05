# Declare the "solarus" library target and its public/private sources
# Note: will be STATIC or SHARED depending on BUILD_SHARED_LIBS=<ON|OFF>
add_library(solarus "")
include(cmake/SolarusLibrarySources.cmake)

# Add additional public/private sources for Apple systems
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  target_sources(solarus
    PUBLIC
      "${CMAKE_CURRENT_SOURCE_DIR}/include/solarus/core/AppleInterface.h"
    PRIVATE
      "${CMAKE_CURRENT_SOURCE_DIR}/src/core/AppleInterface.mm"
  )
endif()

# Declare the public/private libraries that "solarus" depends on
target_link_libraries(solarus
  PUBLIC
    # Third-party libs managed within the project.
    Glad::Glad
    SNES_SPC::SNES_SPC
    HQX::HQX
    # External dependencies.
    SDL2::Core
    SDL2::Image
    SDL2::TTF
    GLM::GLM
    OpenAL::OpenAL
    PhysFS::PhysFS
    Vorbis::File
    Vorbis::Vorbis
    Ogg::Ogg
    ModPlug::ModPlug
)

# Add dynamic loader library if required
if(CMAKE_DL_LIBS)
  target_link_libraries(solarus PUBLIC ${CMAKE_DL_LIBS})
endif()

# Add profiler library if enabled
if(SOLARUS_PROFILING)
  target_link_libraries(solarus PUBLIC easy_profiler)
  target_compile_definitions(solarus PUBLIC -DBUILD_WITH_EASY_PROFILER=1)
endif()

# Add OpenGL imported target to "solarus" declared dependencies
if(OPENGL_FOUND)
  target_link_libraries(solarus PUBLIC OpenGL::GL)
endif()

# Add Android-specific libraries
if(ANDROID)
  target_link_libraries(solarus PUBLIC android log EGL GLESv1_CM GLESv2)
endif()

# Add Lua or LuaJIT imported target to "solarus" declared dependencies
if(LUA_FOUND)
  target_link_libraries(solarus PUBLIC Lua::Lua)
elseif(LUAJIT_FOUND)
  target_link_libraries(solarus PUBLIC Lua::LuaJIT)
endif()

# Set the build properties for "solarus"
set_target_properties(solarus PROPERTIES
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}
)

# Include additional configuration for macOS build and deployment
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  include(cmake/macOS/macOSBuild.cmake)
endif()

# Include directories acessible for "solarus".
target_include_directories(solarus
  PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
)
