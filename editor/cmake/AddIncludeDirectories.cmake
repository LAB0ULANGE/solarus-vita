# Include directories.
include_directories(
  "${CMAKE_BINARY_DIR}"
  "${CMAKE_BINARY_DIR}/include"  # For generated headers.
  "${CMAKE_SOURCE_DIR}/include"
)

# External include paths, marked as system ones to disable their warnings.
include_directories(SYSTEM
  "${SOLARUS_INCLUDE_DIRS}"
  "${MODPLUG_INCLUDE_DIRS}"  # Before SDL2 because we want the sndfile.h of ModPlug.
  "${SDL2_INCLUDE_DIRS}"
  "${SDL2_TTF_INCLUDE_DIR}"
  "${SDL2_IMAGE_INCLUDE_DIR}"
  "${OPENAL_INCLUDE_DIR}"
  "${VORBISFILE_INCLUDE_DIR}"
  "${OGG_INCLUDE_DIR}"
  "${LUA_INCLUDE_DIR}"
  "${PHYSFS_INCLUDE_DIR}"
)
