# Main executable.
add_executable(solarus-editor
  ${SOLARUSEDITOR_SOURCES}
  ${SOLARUSEDITOR_FORMS_HEADERS}
  ${SOLARUSEDITOR_RESOURCES_RCC}
  ${SOLARUSEDITOR_TRANSLATIONS_QM}
)

target_link_libraries(solarus-editor
  Qt6::Widgets
  Qt6::OpenGL
  Qt6::OpenGLWidgets
  "${SOLARUS_LIBRARIES}"
  "${SDL2_LIBRARY}"
  "${SDL2_IMAGE_LIBRARY}"
  "${SDL2_TTF_LIBRARY}"
  "${OPENAL_LIBRARY}"
  "${LUA_LIBRARY}"
  "${DL_LIBRARY}"
  "${PHYSFS_LIBRARY}"
  "${VORBISFILE_LIBRARY}"
  "${OGG_LIBRARY}"
  "${MODPLUG_LIBRARY}"
  qlementine
)
