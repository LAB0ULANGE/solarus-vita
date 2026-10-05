# Add an icon for the executable in Windows.
if(MINGW)
  set(SOLARUSEDITOR_SOURCES
    ${SOLARUSEDITOR_SOURCES}
    cmake/win32/resources-mingw.rc
  )
elseif(WIN32)
  set(SOLARUSEDITOR_SOURCES
    ${SOLARUSEDITOR_SOURCES}
    cmake/win32/resources.rc
  )
endif()
target_sources(solarus-editor PRIVATE
  ${SOLARUSEDITOR_SOURCES}
)

# Windows: disable the console.
if(WIN32)
  if(MSVC)
    set_target_properties(solarus-editor PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
  elseif(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-mwindows ${CMAKE_CXX_FLAGS}")
  endif()

  set_target_properties(solarus-editor PROPERTIES
    OUTPUT_NAME ${SOLARUSEDITOR_EXECUTABLE_NAME}
  )
endif()
