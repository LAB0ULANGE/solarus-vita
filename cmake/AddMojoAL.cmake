# MojoAL: a pure-C OpenAL implementation built on SDL2.
# Used instead of vitasdk's prebuilt OpenAL-soft on PS Vita, since
# OpenAL-soft's internal C++ code relies on a broken C++ runtime
# mechanism on this toolchain (see Sound.cpp history).
if(DEFINED VITASDK)
  add_library(mojoal STATIC
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party/mojoal/mojoal.c"
  )
  target_include_directories(mojoal PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party/mojoal"
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party/mojoal/AL"
  )
  target_link_libraries(mojoal PUBLIC SDL2::Core)
endif()
