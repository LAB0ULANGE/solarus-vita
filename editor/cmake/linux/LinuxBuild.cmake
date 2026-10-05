# FreeDesktop compatible icons
foreach(SIZE IN ITEMS 16 20 24 32 40 48)
  # Pixmap icons for sizes under 48x48 pixels
  install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/resources/app_icon/solarus-editor-${SIZE}.png
    DESTINATION ${SOLARUS_INSTALL_DATAROOTDIR}/icons/hicolor/${SIZE}x${SIZE}/apps RENAME ${SOLARUSEDITOR_APP_ID}.png)
endforeach(SIZE)

# Pixmap icon for desktops that don't support multiple sizes
install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/resources/app_icon/solarus-editor-512.png
  DESTINATION ${SOLARUS_INSTALL_DATAROOTDIR}/pixmaps RENAME ${SOLARUSEDITOR_APP_ID}.png)

# Vector icons, automatically chosen for sizes above 48x48 pixels
install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/resources/app_icon/solarus-editor.svg
  DESTINATION ${SOLARUS_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/apps RENAME ${SOLARUSEDITOR_APP_ID}.svg)
install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/resources/app_icon/solarus-editor-symbolic.svg
  DESTINATION ${SOLARUS_INSTALL_DATAROOTDIR}/icons/hicolor/symbolic/apps RENAME ${SOLARUSEDITOR_APP_ID}-symbolic.svg)

# FreeDesktop compatible start menu editor
configure_file (resources/app.desktop.in ${CMAKE_CURRENT_BINARY_DIR}/resources/${SOLARUSEDITOR_APP_ID}.desktop @ONLY)
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/resources/${SOLARUSEDITOR_APP_ID}.desktop
  DESTINATION ${SOLARUS_INSTALL_DATAROOTDIR}/applications)

set_target_properties(solarus-editor PROPERTIES
  OUTPUT_NAME ${SOLARUSEDITOR_EXECUTABLE_NAME}
)
