include(GNUInstallDirs)

set(SOLARUS_LIBRARY_INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}" CACHE PATH "Library install destination")
# Install location for Debian-based systems
if(EXISTS '/etc/debian_version')
  set(SOLARUS_EXECUTABLE_INSTALL_DESTINATION "games" CACHE PATH "Binary install destination")
else()
  set(SOLARUS_EXECUTABLE_INSTALL_DESTINATION "${CMAKE_INSTALL_BINDIR}" CACHE PATH "Binary install destination")
endif()
set(SOLARUS_SHARE_INSTALL_DESTINATION "${CMAKE_INSTALL_DATADIR}" CACHE PATH "Shared files install destination")
set(SOLARUS_MANUAL_INSTALL_DESTINATION "${CMAKE_INSTALL_MANDIR}" CACHE PATH "Manual install destination")
set(SOLARUS_HEADERS_INSTALL_DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}" CACHE PATH "Headers install destination")

set(SOLARUS_INSTALL_DATADIR "${SOLARUS_SHARE_INSTALL_DESTINATION}/solarus" CACHE PATH "Solarus data directory")
