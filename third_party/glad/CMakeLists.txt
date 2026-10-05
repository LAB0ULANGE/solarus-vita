# Add Glad as a library.
add_library(glad STATIC)

target_sources(glad
  PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include/glad/glad.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/KHR/khrplatform.h"
  PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/glad.c"
)

target_include_directories(glad
  PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_compile_options(glad
  PRIVATE
    "$<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-w>"
)

set_target_properties(glad
  PROPERTIES
    FOLDER "third_party"
    POSITION_INDEPENDENT_CODE ON
)

install(
  DIRECTORY
    "${CMAKE_CURRENT_SOURCE_DIR}/include/glad"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/KHR"
  DESTINATION
    "${SOLARUS_HEADERS_INSTALL_DESTINATION}/solarus/third_party"
)

add_library(Glad::Glad ALIAS glad)
