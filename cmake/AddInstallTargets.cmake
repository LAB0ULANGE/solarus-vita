# Files to install with make install.

# Install the shared library.
install(TARGETS solarus
  ARCHIVE DESTINATION ${SOLARUS_LIBRARY_INSTALL_DESTINATION}
  LIBRARY DESTINATION ${SOLARUS_LIBRARY_INSTALL_DESTINATION}
  RUNTIME DESTINATION ${SOLARUS_EXECUTABLE_INSTALL_DESTINATION}
)

# Install headers: useful for projects that use Solarus as a library.
install(DIRECTORY
  "${CMAKE_CURRENT_BINARY_DIR}/include/solarus"  # For config.h.
  "${CMAKE_CURRENT_SOURCE_DIR}/include/solarus"
  DESTINATION ${SOLARUS_HEADERS_INSTALL_DESTINATION}
)

# Install the data directory for the library.
install(DIRECTORY
  DESTINATION ${SOLARUS_INSTALL_DATADIR}
)

# Install the SDL game controller database into the data directory.
if(SOLARUS_GAMECONTROLLERDB_INSTALL)
  install(FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/assets/gamecontrollerdb.txt"
    DESTINATION ${SOLARUS_INSTALL_DATADIR}
  )
endif()
