add_executable(solarus-run "")
target_sources(solarus-run
  PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
    "$<$<BOOL:${WIN32}>:${CMAKE_CURRENT_SOURCE_DIR}/src/resources$<$<BOOL:${MINGW}>:-mingw>.rc>"
)

target_link_libraries(solarus-run
  PUBLIC
    solarus
    SDL2::Main
)

install(TARGETS solarus-run
  RUNTIME DESTINATION "${SOLARUS_EXECUTABLE_INSTALL_DESTINATION}"
)

if(UNIX AND NOT APPLE)
  install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/man/solarus-run.6"
    DESTINATION "${SOLARUS_MANUAL_INSTALL_DESTINATION}/man6"
  )
endif()
