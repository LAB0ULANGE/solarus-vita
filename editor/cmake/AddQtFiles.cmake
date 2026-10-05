# Add the three types of Qt6 files.

# UI files.
set(SOLARUSEDITOR_FORMS
  src/widgets/change_border_set_id_dialog.ui
  src/widgets/change_dialog_id_dialog.ui
  src/widgets/change_file_info_dialog.ui
  src/widgets/change_resource_id_dialog.ui
  src/widgets/change_source_image_dialog.ui
  src/widgets/change_string_key_dialog.ui
  src/widgets/console.ui
  src/widgets/dialogs_editor.ui
  src/widgets/edit_entity_dialog.ui
  src/widgets/external_script_dialog.ui
  src/widgets/find_text_dialog.ui
  src/widgets/import_dialog.ui
  src/widgets/input_dialog_with_check_box.ui
  src/widgets/main_window.ui
  src/widgets/map_editor.ui
  src/widgets/new_entity_user_property_dialog.ui
  src/widgets/new_quest_dialog.ui
  src/widgets/new_quest_dialog_contents_page.ui
  src/widgets/new_quest_dialog_directory_page.ui
  src/widgets/new_quest_dialog_title_page.ui
  src/widgets/new_resource_element_dialog.ui
  src/widgets/new_element_dialog.ui
  src/widgets/new_string_dialog.ui
  src/widgets/package_dialog.ui
  src/widgets/pattern_picker_dialog.ui
  src/widgets/quest_properties_editor.ui
  src/widgets/settings_dialog.ui
  src/widgets/shader_editor.ui
  src/widgets/sprite_editor.ui
  src/widgets/sprite_previewer.ui
  src/widgets/strings_editor.ui
  src/widgets/tileset_editor.ui
)

# Generate .h from .ui.
qt6_wrap_ui(SOLARUSEDITOR_FORMS_HEADERS
  ${SOLARUSEDITOR_FORMS}
)

# Resources.
set(SOLARUSEDITOR_RESOURCES
  resources/images.qrc
  resources/initial_files.qrc
  resources/quest_converter.qrc
  resources/themes.qrc
  resources/app_icon.qrc
)

# Compile resource files.
qt6_add_resources(SOLARUSEDITOR_RESOURCES_RCC
  ${SOLARUSEDITOR_RESOURCES}
)

# Translations.
set(SOLARUSEDITOR_TRANSLATIONS
  translations/solarus_editor_es.ts
  translations/solarus_editor_fr.ts
  translations/solarus_editor_zh_CN.ts
  translations/solarus_editor_zh_TW.ts
)
qt6_add_translation(SOLARUSEDITOR_TRANSLATIONS_QM
  ${SOLARUSEDITOR_TRANSLATIONS}
)
