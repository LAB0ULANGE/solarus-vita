# Add snes_spc as a library.
add_library(snes_spc STATIC)

target_sources(snes_spc
  PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/spc.h"
  PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/SNES_SPC.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/SPC_CPU.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/SPC_DSP.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/SPC_Filter.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dsp.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/blargg_common.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/blargg_config.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/blargg_endian.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/blargg_source.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/SNES_SPC.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/SNES_SPC_misc.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/SNES_SPC_state.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/SPC_DSP.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/SPC_Filter.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dsp.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/spc.cpp"
)

target_include_directories(snes_spc
  PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_compile_options(snes_spc
  PRIVATE
    "$<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-w>"
)

set_target_properties(snes_spc
  PROPERTIES
    FOLDER "third_party"
    POSITION_INDEPENDENT_CODE ON
)

install(
  FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/spc.h"
  DESTINATION
    "${SOLARUS_HEADERS_INSTALL_DESTINATION}/solarus/third_party"
)

add_library(SNES_SPC::SNES_SPC ALIAS snes_spc)
