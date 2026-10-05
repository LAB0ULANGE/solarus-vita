# Add hqx as a library.
add_library(hqx STATIC)

# Note: 'hqx.c' implements a command-line tool which is not required for Solarus.
# It is not included in the sources below because it requires another third-party library.
target_sources(hqx
  PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/hqx.h"
  PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/common.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/hq2x.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/hq3x.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/hq4x.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/init.c"
)

target_include_directories(hqx
  PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_compile_options(hqx
  PRIVATE
    "$<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-w>"
)

set_target_properties(hqx
  PROPERTIES
    FOLDER "third_party"
    POSITION_INDEPENDENT_CODE ON
)

install(
  FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/hqx.h"
  DESTINATION
    "${SOLARUS_HEADERS_INSTALL_DESTINATION}/solarus/third_party"
)

add_library(HQX::HQX ALIAS hqx)
