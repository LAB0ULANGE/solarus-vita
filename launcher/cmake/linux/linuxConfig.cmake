 # Executable.
 install(TARGETS ${PROJECT_NAME}
 RUNTIME DESTINATION bin
)

# Desktop environment metadata.
set(DESKTOP_FILE ${CMAKE_CURRENT_BINARY_DIR}/${SOLARUSLAUNCHER_APP_IDENTIFIER}.desktop)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/linux/app.desktop.in
 ${DESKTOP_FILE}
 @ONLY
)
install(FILES ${DESKTOP_FILE}
 DESTINATION share/applications
)

# AppStream Metadata metadata (for Gnome Software Center).
set(METAINFO_FILE ${CMAKE_CURRENT_BINARY_DIR}/${SOLARUSLAUNCHER_APP_IDENTIFIER}.metainfo.xml)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/linux/app.metainfo.xml.in
 ${METAINFO_FILE}
 @ONLY
)
install(FILES ${METAINFO_FILE}
 DESTINATION share/metainfo
)

# Raster images for icon sizes under 48x48 pixels.
foreach(SIZE IN ITEMS 16 20 24 32 40 48)
 install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/resources/app_icon/solarus_launcher_icon_${SIZE}.png
   DESTINATION share/icons/hicolor/${SIZE}x${SIZE}/apps RENAME ${SOLARUSLAUNCHER_APP_IDENTIFIER}.png
 )
endforeach(SIZE)

# Raster image for desktops that don't support multiple sizes.
install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/resources/app_icon/solarus_launcher_icon_512.png
 DESTINATION share/pixmaps RENAME ${SOLARUSLAUNCHER_APP_IDENTIFIER}.png
)

# Vector images, automatically chosen for icon sizes above 48x48 pixels
install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/resources/app_icon/solarus_launcher_icon.svg
 DESTINATION share/icons/hicolor/scalable/apps RENAME ${SOLARUSLAUNCHER_APP_IDENTIFIER}.svg
)

# Monochromatic vector image for some desktop environments that need it.
install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/resources/app_icon/solarus_launcher_icon_symbolic.svg
 DESTINATION share/icons/hicolor/symbolic/apps RENAME ${SOLARUSLAUNCHER_APP_IDENTIFIER}.svg
)

# Register a .solarus extension.
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/cmake/linux/solarus.xml DESTINATION share/mime/packages)

# Association to files wth the "".solarus" extension.
install(CODE "execute_process(COMMAND update-mime-database /usr/share/mime)")
install(CODE "execute_process(COMMAND update-desktop-database /usr/share/applications)")
install(CODE "execute_process(COMMAND mkdir -p /root/.config)") # Ensure the .config directory exists.
install(CODE "execute_process(COMMAND touch /root/.config/mimeapps.list)") # Ensure the mimeapps.list file exists.
install(CODE "execute_process(COMMAND xdg-mime default ${SOLARUSLAUNCHER_APP_IDENTIFIER}.desktop application/x-solarus)")
