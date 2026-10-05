set_target_properties(${PROJECT_NAME} PROPERTIES
  WIN32_EXECUTABLE TRUE
)

# Generate the manifest file.
set(APPLICATION_MANIFEST_ABSOLUTE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${SOLARUSLAUNCHER_EXECUTABLE_NAME}.manifest")
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/windows/App.exe.manifest.in"
  "${APPLICATION_MANIFEST_ABSOLUTE_PATH}"
)
file(TO_CMAKE_PATH
  "${APPLICATION_MANIFEST_ABSOLUTE_PATH}"
  WINDOWS_APPLICATION_MANIFEST_ABSOLUTE_PATH
)

# We embed both the app icon and its document icon in the exe.
file(TO_CMAKE_PATH
  "${CMAKE_CURRENT_SOURCE_DIR}/resources/app_icon/solarus_launcher_icon.ico"
  WINDOWS_APPLICATION_ICON_ABSOLUTE_PATH
)
file(TO_CMAKE_PATH
  "${CMAKE_CURRENT_SOURCE_DIR}/resources/doc_icon/windows/win_doc_icon.ico"
  WINDOWS_FILE_ICON_ABSOLUTE_PATH
)

# Generate the resources.rc file.
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/windows/Resources.rc.in"
  "${CMAKE_CURRENT_BINARY_DIR}/Resources.rc"
)
target_sources(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/Resources.rc")
