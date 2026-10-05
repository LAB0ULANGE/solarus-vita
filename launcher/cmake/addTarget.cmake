message(STATUS "Using Qt ${Qt6_VERSION}")
if (Qt6_VERSION VERSION_LESS 6.7)
  qt_standard_project_setup()
else()
  qt_standard_project_setup(
    I18N_TRANSLATED_LANGUAGES en_US fr_FR
    I18N_SOURCE_LANGUAGE en_US
  )
endif()

# Get all sources.
file(GLOB_RECURSE SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/sources/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/sources/*.h"
)

# Target.
qt_add_executable(${PROJECT_NAME} MANUAL_FINALIZATION
  ${SOURCES}
  ${CMAKE_CURRENT_BINARY_DIR}/sources/SolarusLauncherConfig.h
)
target_include_directories(${PROJECT_NAME}
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/sources
    ${CMAKE_CURRENT_SOURCE_DIR}/../build/include
    ${CMAKE_CURRENT_BINARY_DIR}/sources
    "${OPENAL_INCLUDE_DIR}"
)

qt_add_resources(${PROJECT_NAME} "resources"
  PREFIX "/solarus/launcher/"
  FILES
    # resources/flags/de.svg
    resources/flags/en.svg
    # resources/flags/es.svg
    resources/flags/fr.svg
    # resources/flags/it.svg
    # resources/flags/pt.svg
    # resources/flags/ru.svg
    # resources/flags/zh.svg
    resources/app_icon/solarus_launcher_icon_16.png
    resources/app_icon/solarus_launcher_icon_20.png
    resources/app_icon/solarus_launcher_icon_32.png
    resources/app_icon/solarus_launcher_icon_40.png
    resources/app_icon/solarus_launcher_icon_48.png
    resources/app_icon/solarus_launcher_icon_64.png
    resources/app_icon/solarus_launcher_icon_96.png
    resources/app_icon/solarus_launcher_icon_128.png
    resources/app_icon/solarus_launcher_icon_256.png
    resources/app_icon/solarus_launcher_icon_512.png
    resources/app_icon/solarus_launcher_icon_1024.png
    resources/app_icon/solarus_launcher_icon.icns
    resources/app_icon/solarus_launcher_icon.ico
    resources/app_icon/solarus_launcher_icon.svg
    resources/themes/dark.json
    resources/themes/light.json
    resources/age_ratings/all.svg
    resources/age_ratings/restricted.svg
    resources/age_ratings/warning.svg
)

# Qt6 automatically uses the project name as the prefix for the translation files.
# It is possibble to use a custom prefix, but to keep things simple,
# we will use the built-in Qt6 translation system.
if (Qt6_VERSION VERSION_LESS 6.7)
  qt_add_translations(${PROJECT_NAME}
    TS_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/i18n/${SOLARUSLAUNCHER_QM_PREFIX}_en_US.ts
      ${CMAKE_CURRENT_SOURCE_DIR}/i18n/${SOLARUSLAUNCHER_QM_PREFIX}_fr_FR.ts
  )
else()
  qt_add_translations(${PROJECT_NAME}
    TS_FILE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/i18n
  )
endif()

target_link_libraries(${PROJECT_NAME}
  PRIVATE
    Qt6::Widgets
    Qt6::Svg
    Qt6::Network
    qlementine
    qlementine-icons
    solarus
    oclero::QtAppInstanceManager
)

set(COMPONENT_NAME "${PROJECT_NAME}")
