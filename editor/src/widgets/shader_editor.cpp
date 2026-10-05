/*
 * Copyright (C) 2014-2018 Christopho, Solarus - http://www.solarus-games.org
 *
 * Solarus Quest Editor is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Solarus Quest Editor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "widgets/gui_tools.h"
#include "widgets/map_view.h"
#include "widgets/shader_editor.h"
#include "widgets/text_editor.h"
#include "editor_exception.h"
#include "editor_settings.h"
#include "editor_style.h"
#include "map_model.h"
#include "quest.h"
#include "shader_model.h"
#include "sprite_model.h"
#include <QFileDialog>
#include <QInputDialog>
#include <QUndoStack>

namespace SolarusEditor {

namespace {

/**
 * @brief Parent class of all undoable commands of the shader editor.
 */
class ShaderEditorCommand : public QUndoCommand {

public:

  ShaderEditorCommand(ShaderEditor& editor, const QString& text) :
    QUndoCommand(text),
    editor(editor) {
  }

  ShaderEditor& get_editor() const {
    return editor;
  }

  ShaderModel& get_shader() const {
    return editor.get_shader();
  }

private:

  ShaderEditor& editor;

};

/**
 * @brief Changing the scaling factor of a shader.
 */
class SetScalingFactorCommand : public ShaderEditorCommand {

public:
  SetScalingFactorCommand(ShaderEditor& editor, double scaling_factor) :
    ShaderEditorCommand(editor, ShaderEditor::tr("Scaling factor")),
    before(get_shader().get_scaling_factor()),
    after(scaling_factor) {
  }

  void undo() override {
    get_shader().set_scaling_factor(before);
  }

  void redo() override {
    get_shader().set_scaling_factor(after);
  }

private:
  double before, after;
};

/**
 * @brief Changing a source file in a shader program.
 */
class SetGlslFileCommand : public ShaderEditorCommand {

public:
  SetGlslFileCommand(ShaderEditor& editor, WhichGlslEditor which, const QString& file_name) :
    ShaderEditorCommand(editor, ShaderEditor::tr("Shader file")),
    which(which),
    before(),
    after(file_name) {

    switch (which) {
    case WhichGlslEditor::VERTEX_EDITOR:
      before = get_shader().get_vertex_file();
      break;
    case WhichGlslEditor::FRAGMENT_EDITOR:
      before = get_shader().get_fragment_file();
      break;
    }
  }

  void undo() override {
    switch (which) {
    case WhichGlslEditor::VERTEX_EDITOR:
      get_shader().set_vertex_file(before);
      break;
    case WhichGlslEditor::FRAGMENT_EDITOR:
      get_shader().set_fragment_file(before);
      break;
    }
  }

  void redo() override {
    switch (which) {
    case WhichGlslEditor::VERTEX_EDITOR:
      get_shader().set_vertex_file(after);
      break;
    case WhichGlslEditor::FRAGMENT_EDITOR:
      get_shader().set_fragment_file(after);
      break;
    }
  }

private:
  WhichGlslEditor which;
  QString before, after;
};

}  // Anonymous namespace.

/**
 * @brief Creates a shader editor.
 * @param quest The quest containing the file.
 * @param path Path of the shader data file to open.
 * @param parent The parent object or nullptr.
 * @throws EditorException If the file could not be opened.
 */
ShaderEditor::ShaderEditor(Quest& quest, const QString& path, QWidget* parent) :
  Editor(quest, path, parent),
  shader(nullptr),
  quest(quest),
  vertex_editor(nullptr),
  fragment_editor(nullptr) {

  ui.setupUi(this);

  // Get the shader.
  ResourceType resource_type;
  QString shader_id;
  quest.check_exists(path);
  if (!quest.is_resource_element(path, resource_type, shader_id) ||
      resource_type != ResourceType::SHADER) {
    throw EditorException(tr("File '%1' is not a shader").arg(path));
  }
  this->shader_id = shader_id;

  // Editor properties.
  set_title(tr("Shader %1").arg(get_file_name_without_extension()));
  set_icon(QIcon(":/images/icon_resource_shader.svg"));
  set_close_confirm_message(
        tr("Shader '%1' has been modified. Save changes?").arg(shader_id));
  set_zoom_supported(true);
  ui.shader_previewer->set_view_settings(get_view_settings());

  // Open the file.
  shader = std::unique_ptr<ShaderModel>(new ShaderModel(quest, shader_id, this));
  get_undo_stack().setClean();

  // Prepare the GUI.
  ui.scaling_factor_check_box->setAttribute(Qt::WA_LayoutUsesWidgetRect);
  ui.preview_mode_selector->setAttribute(Qt::WA_LayoutUsesWidgetRect);

  ui.preview_picture_page->layout()->setAlignment(ui.preview_picture_field_layout, Qt::AlignTop);
  ui.preview_map_page->layout()->setAlignment(ui.preview_map_field, Qt::AlignTop);
  ui.preview_map_field->set_resource_type(ResourceType::MAP);
  ui.preview_map_field->set_quest(quest);
  ui.preview_sprite_field->set_resource_type(ResourceType::SPRITE);
  ui.preview_sprite_field->set_quest(quest);

  ui.vertex_file_check_box->setAttribute(Qt::WA_LayoutUsesWidgetRect);
  ui.vertex_file_field->setAttribute(Qt::WA_LayoutUsesWidgetRect);
  ui.vertex_shader_page->layout()->setAlignment(ui.vertex_file_check_box, Qt::AlignTop);
  ui.fragment_file_check_box->setAttribute(Qt::WA_LayoutUsesWidgetRect);
  ui.fragment_file_field->setAttribute(Qt::WA_LayoutUsesWidgetRect);
  ui.fragment_shader_page->layout()->setAlignment(ui.fragment_file_check_box, Qt::AlignTop);

  EditorSettings settings;

  const int side_width = settings.get_value_int(EditorSettings::shader_side_width);
  ui.main_splitter->setSizes({ side_width, width() - side_width });
  ui.main_splitter->setStretchFactor(0, 0);
  ui.main_splitter->setStretchFactor(1, 1);
  
  const int preview_height = settings.get_value_int(EditorSettings::shader_files_height);
  ui.right_splitter->setSizes({ preview_height, height() - preview_height });
  ui.right_splitter->setStretchFactor(0, 1);
  ui.right_splitter->setStretchFactor(1, 0);

  ui.shader_previewer->set_model(shader.get());

  QString vertex_file = shader->get_vertex_file();
  QString vertex_file_path = quest.get_shader_code_file_path(vertex_file);
  if (!quest.exists(vertex_file_path) || !quest.is_shader_code_file(vertex_file_path)) {
    vertex_file_path = QString();
  }
  vertex_editor = new TextEditor(
        quest, vertex_file_path, this);
  ui.vertex_editor_layout->addWidget(vertex_editor);
  ui.vertex_editor_layout->removeItem(ui.vertex_editor_placeholder);

  QString fragment_file = shader->get_fragment_file();
  QString fragment_file_path = quest.get_shader_code_file_path(fragment_file);
  if (!quest.exists(fragment_file_path) || !quest.is_shader_code_file(fragment_file_path)) {
    fragment_file_path = QString();
  }
  fragment_editor = new TextEditor(
        quest, fragment_file_path, this);
  ui.fragment_editor_layout->addWidget(fragment_editor);
  ui.fragment_editor_layout->removeItem(ui.fragment_editor_placeholder);

  update();
  reload_settings();

  // Make connections.
  connect(&get_database(), &QuestDatabase::element_description_changed,
          this, &ShaderEditor::update_description_to_gui);
  connect(ui.description_field, &QLineEdit::editingFinished,
          this, &ShaderEditor::set_description_from_gui);

  connect(shader.get(), &ShaderModel::scaling_factor_changed,
          this, &ShaderEditor::update_scaling_factor_field);
  connect(ui.scaling_factor_check_box, &QCheckBox::clicked,
          this, &ShaderEditor::scaling_factor_check_box_changed);
  connect(ui.scaling_factor_field, qOverload<double>(&QDoubleSpinBox::valueChanged),
          this, &ShaderEditor::scaling_factor_field_changed);

  connect(ui.preview_mode_selector, qOverload<int>(&QComboBox::currentIndexChanged),
          this, [this]() {
    ui.shader_previewer->set_preview_mode(ui.preview_mode_selector->get_selected_value());
  });

  connect(ui.preview_picture_radio, &QRadioButton::clicked,
          this, &ShaderEditor::preview_radio_changed);
  connect(ui.preview_picture_browse_button, &QToolButton::clicked,
          this, &ShaderEditor::browse_preview_picture);
  connect(ui.preview_map_radio, &QRadioButton::clicked,
          this, &ShaderEditor::preview_radio_changed);
  connect(ui.preview_map_field, qOverload<int>(&ResourceSelector::currentIndexChanged),
          this, &ShaderEditor::update_preview_image);
  connect(ui.preview_sprite_radio, &QRadioButton::clicked,
          this, &ShaderEditor::preview_radio_changed);
  connect(ui.preview_sprite_field, qOverload<int>(&QComboBox::currentIndexChanged),
          this, &ShaderEditor::preview_selected_sprite_changed);
  connect(ui.preview_sprite_animation_field, qOverload<int>(&QComboBox::currentIndexChanged),
          this, &ShaderEditor::preview_sprite_animation_changed);
  connect(ui.preview_sprite_direction_field, qOverload<int>(&QSpinBox::valueChanged),
          this, &ShaderEditor::update_preview_image);

  connect(ui.vertex_file_check_box, &QCheckBox::clicked, this, [this]() {
    source_file_check_box_changed(WhichGlslEditor::VERTEX_EDITOR);
  });
  connect(ui.vertex_file_new_button, &QToolButton::clicked, this, [this]() {
    new_source_file(WhichGlslEditor::VERTEX_EDITOR);
  });
  connect(ui.vertex_file_browse_button, &QToolButton::clicked, this, [this]() {
    browse_source_file(WhichGlslEditor::VERTEX_EDITOR);
  });
  connect(ui.vertex_file_save_button, &QToolButton::clicked, this, [this]() {
    save_source_file(WhichGlslEditor::VERTEX_EDITOR);
  });
  connect(shader.get(), &ShaderModel::vertex_file_changed, this, [this]() {
    update_source_editor_tab(WhichGlslEditor::VERTEX_EDITOR);
  });
  connect(&vertex_editor->get_undo_stack(), &QUndoStack::cleanChanged, this, [this](bool clean) {
    source_editor_modification_state_changed(WhichGlslEditor::VERTEX_EDITOR, clean);
  });

  connect(ui.fragment_file_check_box, &QCheckBox::clicked, this, [this]() {
    source_file_check_box_changed(WhichGlslEditor::FRAGMENT_EDITOR);
  });
  connect(ui.fragment_file_new_button, &QToolButton::clicked, this, [this]() {
    new_source_file(WhichGlslEditor::FRAGMENT_EDITOR);
  });
  connect(ui.fragment_file_browse_button, &QToolButton::clicked, this, [this]() {
    browse_source_file(WhichGlslEditor::FRAGMENT_EDITOR);
  });
  connect(ui.fragment_file_save_button, &QToolButton::clicked, this, [this]() {
    save_source_file(WhichGlslEditor::FRAGMENT_EDITOR);
  });
  connect(shader.get(), &ShaderModel::fragment_file_changed, this, [this]() {
    update_source_editor_tab(WhichGlslEditor::FRAGMENT_EDITOR);
  });
  connect(&fragment_editor->get_undo_stack(), &QUndoStack::cleanChanged, this, [this](bool clean) {
    source_editor_modification_state_changed(WhichGlslEditor::FRAGMENT_EDITOR, clean);
  });

  connect(ui.shader_previewer, &ShaderPreviewer::shader_compilation_started,
          this, &ShaderEditor::clear_console);
  connect(ui.shader_previewer, &ShaderPreviewer::shader_error,
        this, [this](const QString& message) {
    emit log_message_to_console("Error", message);
  });
  connect(ui.shader_previewer, &ShaderPreviewer::shader_warning,
          this, [this](const QString& message) {
    emit log_message_to_console("Warning", message);
  });

  connect(ui.main_splitter, &QSplitter::splitterMoved,
          this, &ShaderEditor::side_panel_resized);
  connect(ui.right_splitter, &QSplitter::splitterMoved,
          this, &ShaderEditor::preview_panel_resized);

  // Qlementine-related stuff.
  for (auto* widget : std::vector<QWidget*>{
           ui.fragment_file_browse_button,
           // ui.fragment_file_new_button,
           ui.fragment_file_save_button,
           ui.vertex_file_browse_button,
           ui.vertex_file_new_button,
           ui.vertex_file_save_button,
           ui.preview_picture_browse_button,
       }) {
    EditorStyle::setAutoIconColor(widget, EditorStyle::AutoIconColor::ForegroundColor);
  }
}

/**
 * @brief Destructor.
 */
ShaderEditor::~ShaderEditor() {
}

/**
 * @brief Returns the shader model being edited.
 * @return The shader model.
 */
ShaderModel& ShaderEditor::get_shader() {
  return *shader;
}

/**
 * @copydoc Editor::save
 */
void ShaderEditor::save() {

  shader->save();
  save_source_file(WhichGlslEditor::VERTEX_EDITOR);
  save_source_file(WhichGlslEditor::FRAGMENT_EDITOR);
}

/**
 * @copydoc Editor::has_unsaved_changes
 */
bool ShaderEditor::has_unsaved_changes() const {

  return Editor::has_unsaved_changes() ||
      vertex_editor->has_unsaved_changes() ||
      fragment_editor->has_unsaved_changes();
}

/**
 * @copydoc Editor::reload_settings
 */
void ShaderEditor::reload_settings() {

  EditorSettings settings;

  QString file = settings.get_value_string(EditorSettings::shader_preview_picture_file);
  ui.preview_picture_field->setText(file);
  QString map_id = settings.get_value_string(EditorSettings::shader_preview_map_id);
  ui.preview_map_field->set_selected_id(map_id);
  QString sprite_id = settings.get_value_string(EditorSettings::shader_preview_sprite_id);
  QString animation = settings.get_value_string(EditorSettings::shader_preview_sprite_animation);
  int direction = settings.get_value_int(EditorSettings::shader_preview_sprite_direction);
  ui.preview_sprite_field->set_selected_id(sprite_id);
  preview_selected_sprite_changed();
  ui.preview_sprite_animation_field->setCurrentText(animation);
  preview_sprite_animation_changed();
  ui.preview_sprite_direction_field->setValue(direction);

  QString preview_type = settings.get_value_string(EditorSettings::shader_preview_type);
  if (preview_type.isEmpty() || preview_type == "picture") {
    ui.preview_picture_radio->setChecked(true);
    ui.preview_file_widget->setCurrentWidget(ui.preview_picture_page);
  }
  else if (preview_type == "map") {
    ui.preview_map_radio->setChecked(true);
    ui.preview_file_widget->setCurrentWidget(ui.preview_map_page);
  }
  else if (preview_type == "sprite") {
    ui.preview_sprite_radio->setChecked(true);
    ui.preview_file_widget->setCurrentWidget(ui.preview_sprite_page);
  }
  update_preview_image();
}

/**
 * @brief Updates everything in the GUI.
 */
void ShaderEditor::update() {

  update_shader_id_field();
  update_description_to_gui();
  update_scaling_factor_field();
  update_source_editor_tab(WhichGlslEditor::VERTEX_EDITOR);
  update_source_editor_tab(WhichGlslEditor::FRAGMENT_EDITOR);
}

/**
 * @brief Updates the shader id displaying.
 */
void ShaderEditor::update_shader_id_field() {

  ui.shader_id_field->setText(shader_id);
}

/**
 * @brief Updates the content of the shader description text edit.
 */
void ShaderEditor::update_description_to_gui() {

  QString description = get_database().get_description(ResourceType::SHADER, shader_id);
  if (ui.description_field->text() != description) {
    ui.description_field->setText(description);
  }
}

/**
 * @brief Modifies the shader description in the quest database with
 * the new text entered by the user.
 *
 * If the new description is invalid, an error dialog is shown.
 */
void ShaderEditor::set_description_from_gui() {

  QString description = ui.description_field->text();
  if (description == get_database().get_description(ResourceType::SHADER, shader_id)) {
    return;
  }

  if (description.isEmpty()) {
    GuiTools::error_dialog(tr("Invalid description"));
    update_description_to_gui();
    return;
  }

  const bool was_blocked = blockSignals(true);
  try {
    get_database().set_description(ResourceType::SHADER, shader_id, description);
    get_database().save();
  }
  catch (const EditorException& ex) {
    ex.print_message();
  }
  update_description_to_gui();
  blockSignals(was_blocked);
}

/**
 * @brief Updates the scaling factor from data to the GUI.
 */
void ShaderEditor::update_scaling_factor_field() {

  double scaling_factor = get_shader().get_scaling_factor();
  if (scaling_factor == 0.0) {
    ui.scaling_factor_check_box->setChecked(false);
    ui.scaling_factor_field->setEnabled(false);
  }
  else {
    ui.scaling_factor_check_box->setChecked(true);
    ui.scaling_factor_field->setEnabled(true);
    ui.scaling_factor_field->setValue(scaling_factor);
  }
}

/**
 * @brief Called when the user changes the scaling factor check box.
 */
void ShaderEditor::scaling_factor_check_box_changed() {

  double old_scaling_factor = get_shader().get_scaling_factor();
  double new_scaling_factor = 0.0;
  if (ui.scaling_factor_check_box->isChecked()) {
    new_scaling_factor = ui.scaling_factor_field->value();;
  }

  if (new_scaling_factor != old_scaling_factor) {
    try_command(new SetScalingFactorCommand(*this, new_scaling_factor));
  }
}

/**
 * @brief Called when the user changes the scaling factor value.
 */
void ShaderEditor::scaling_factor_field_changed() {

  double scaling_factor = ui.scaling_factor_field->value();
  if (scaling_factor == get_shader().get_scaling_factor()) {
    return;
  }
  try_command(new SetScalingFactorCommand(*this, scaling_factor));
}

/**
 * @brief Returns the specified GLSL code editor widget.
 * @param which Which GLSL editor to return.
 * @return The corresponding code editor.
 */
TextEditor* ShaderEditor::get_glsl_editor(WhichGlslEditor which) {

  TextEditor* glsl_editor = nullptr;
  switch (which) {
  case WhichGlslEditor::VERTEX_EDITOR:
    glsl_editor = vertex_editor;
    break;
  case WhichGlslEditor::FRAGMENT_EDITOR:
    glsl_editor = fragment_editor;
    break;
  }

  return glsl_editor;
}

/**
 * @brief Updates a GLSL editor tab from the data.
 * @param which The GLSL editor tab to update.
 */
void ShaderEditor::update_source_editor_tab(WhichGlslEditor which) {

  if (shader == nullptr) {
    return;
  }

  QString file_name;
  QLineEdit* file_name_field = nullptr;
  QCheckBox* check_box = nullptr;
  QStackedWidget* stacked_widget = nullptr;
  QToolButton* save_button = nullptr;
  TextEditor* glsl_editor = get_glsl_editor(which);

  switch (which) {

  case WhichGlslEditor::VERTEX_EDITOR:
    file_name = shader->get_vertex_file();
    file_name_field = ui.vertex_file_field;
    check_box = ui.vertex_file_check_box;
    stacked_widget = ui.vertex_editor_stacked_widget;
    save_button = ui.vertex_file_save_button;
    break;

  case WhichGlslEditor::FRAGMENT_EDITOR:
    file_name = shader->get_fragment_file();
    file_name_field = ui.fragment_file_field;
    check_box = ui.fragment_file_check_box;
    stacked_widget = ui.fragment_editor_stacked_widget;
    save_button = ui.fragment_file_save_button;
    break;
  }

  QString path = get_quest().get_shader_code_file_path(file_name);
  if (!file_name.isEmpty() &&
      quest.exists(path) &&
      quest.is_shader_code_file(path)) {
    file_name_field->setText(file_name);
    stacked_widget->setCurrentIndex(1);  // Normal page.
    get_glsl_editor(which)->set_file_path(path);
    glsl_editor->setEnabled(true);
    save_button->setEnabled(true);
    check_box->setChecked(true);
  }
  else {
    file_name_field->setText(QString());
    stacked_widget->setCurrentIndex(0);  // Empty page.
    glsl_editor->setEnabled(false);
    save_button->setEnabled(false);
    check_box->setChecked(false);
  }
}

/**
 * @brief Called when the user clicks a source file check box.
 * @param which The GLSL editor whose check box has changed.
 */
void ShaderEditor::source_file_check_box_changed(WhichGlslEditor which) {

  QCheckBox* check_box = nullptr;
  QString* last_file_name = nullptr;
  QString current_file_name;
  QStackedWidget* stacked_widget = nullptr;

  switch (which) {

  case WhichGlslEditor::VERTEX_EDITOR:
    last_file_name = &last_vertex_file;
    current_file_name = shader->get_vertex_file();
    check_box = ui.vertex_file_check_box;
    stacked_widget = ui.vertex_editor_stacked_widget;
    break;

  case WhichGlslEditor::FRAGMENT_EDITOR:
    last_file_name = &last_fragment_file;
    current_file_name = shader->get_fragment_file();
    check_box = ui.fragment_file_check_box;
    stacked_widget = ui.fragment_editor_stacked_widget;
    break;
  }

  const bool checked = check_box->isChecked();
  if (checked) {
    if (current_file_name.isEmpty() &&
       !last_file_name->isEmpty()) {
      // Use the previous file name.
      try_command(new SetGlslFileCommand(*this, which, *last_file_name));
    }
    else {
      stacked_widget->setCurrentIndex(1);  // Normal page.
    }
  }
  else {
    if (!current_file_name.isEmpty()) {
      // Remove the value but remember it.
      *last_file_name = current_file_name;
      try_command(new SetGlslFileCommand(*this, which, ""));
    }
    else {
      stacked_widget->setCurrentIndex(0);  // Empty page.
    }
  }
}

/**
 * @brief Lets the user choose a GLSL source file to create.
 * @param which The GLSL editor where to create a file.
 */
void ShaderEditor::new_source_file(WhichGlslEditor which) {

  if (shader == nullptr) {
    return;
  }

  try {
    TextEditor* glsl_editor = get_glsl_editor(which);
    if (!glsl_editor->confirm_before_closing()) {
      return;
    }

    QString suffix;
    QString content;
    switch (which) {
    case WhichGlslEditor::VERTEX_EDITOR:
      suffix = ".vert.glsl";
      content = ShaderModel::get_default_vertex_source();
      break;
    case WhichGlslEditor::FRAGMENT_EDITOR:
      suffix = ".frag.glsl";
      content = ShaderModel::get_default_fragment_source();
      break;
    }

    bool ok = false;
    QString file_name = QInputDialog::getText(
          this,
          tr("New GLSL file"),
          tr("File name:"),
          QLineEdit::Normal,
          shader_id + suffix,
          &ok);

    if (ok) {
      // Automatically add .glsl extension if not present.
      if (!file_name.contains(".")) {
        file_name = file_name + ".glsl";
      }
      Quest::check_valid_file_name(file_name);
      const QString& shaders_path = get_quest().get_resource_path(ResourceType::SHADER);
      QString file_path = shaders_path + '/' + file_name;

      get_quest().create_file_from_string(file_path, content);

      file_name = file_name.right(shaders_path.size() + 1);
      try_command(new SetGlslFileCommand(*this, which, file_name));
    }
  }
  catch (const EditorException& ex) {
    ex.show_dialog();
  }
}

/**
 * @brief Lets the user choose a GLSL code file to open.
 * @param which The GLSL editor where to browse a file.
 */
void ShaderEditor::browse_source_file(WhichGlslEditor which) {

  if (shader == nullptr) {
    return;
  }

  try {
    if (get_glsl_editor(which)->confirm_before_closing()) {
      const QString& directory = QFileInfo(get_file_path()).dir().path();
      QString file_name = QFileDialog::getOpenFileName(
          this,
          tr("Open a GLSL file"),
          directory,
          tr("GLSL shader file (*.glsl)"),
          nullptr,
#ifdef SOLARUSEDITOR_NO_NATIVE_DIALOGS
          QFileDialog::DontUseNativeDialog
#else
          QFileDialog::Options()
#endif
      );

      if (!file_name.isEmpty()) {
        const QString& shaders_path = get_quest().get_resource_path(ResourceType::SHADER);
        if (!file_name.startsWith(shaders_path)) {
          throw EditorException(tr("Shader GLSL files must be in the shaders directory"));
        }

        file_name = file_name.right(file_name.size() - shaders_path.size() - 1);
        if (!file_name.isEmpty()) {
          try_command(new SetGlslFileCommand(*this, which, file_name));
        }
      }
    }
  }
  catch (const EditorException& ex) {
    ex.show_dialog();
  }
}

/**
 * @brief Saves the fragment shader GLSL file.
 * @param which The GLSL editor to save.
 */
void ShaderEditor::save_source_file(WhichGlslEditor which) {

  if (shader == nullptr) {
    return;
  }

  TextEditor* glsl_editor = get_glsl_editor(which);
  glsl_editor->save();
  glsl_editor->get_undo_stack().setClean();
}

/**
 * @brief Called when the is-modified state of a GLSL editor has changed.
 * @param which The GLSL editor whose file has changed.
 * @param clean @c true if the file is now clean, @c false if it is now
 * modified.
 */
void ShaderEditor::source_editor_modification_state_changed(WhichGlslEditor which, bool clean) {

  QString title;
  int tab_index = 0;
  switch (which) {
  case WhichGlslEditor::VERTEX_EDITOR:
    title = "Vertex shader";
    tab_index = 0;
    break;
  case WhichGlslEditor::FRAGMENT_EDITOR:
    title = "Fragment shader";
    tab_index = 1;
    break;
  }

  if (!clean) {
    title += '*';
  }
  ui.shader_files_tab_widget->setTabText(tab_index, title);

  if (clean) {
    // A GLSL file was just saved: tell the preview to update.
    ui.shader_previewer->on_source_changed();
  }
}

/**
 * @brief Shows the appropriate preview settings page depending on the radio
 * button checked.
 */
void ShaderEditor::preview_radio_changed() {

  if (ui.preview_picture_radio->isChecked()) {
    ui.preview_file_widget->setCurrentWidget(ui.preview_picture_page);
  }
  else if (ui.preview_map_radio->isChecked()) {
    ui.preview_file_widget->setCurrentWidget(ui.preview_map_page);
  }
  else if (ui.preview_sprite_radio->isChecked()) {
    ui.preview_file_widget->setCurrentWidget(ui.preview_sprite_page);
    preview_selected_sprite_changed();
  }

  update_preview_image();
}

/**
 * @brief Sets up the sprite animation and direction fields according
 * to the selected sprite.
 */
void ShaderEditor::preview_selected_sprite_changed() {

  ui.preview_sprite_animation_field->clear();

  const QString& sprite_id = ui.preview_sprite_field->get_selected_id();
  if (sprite_id.isEmpty() ||
      !quest.get_database().exists(ResourceType::SPRITE, sprite_id) ||
      !quest.exists(quest.get_sprite_path(sprite_id))) {
    update_preview_image();
    return;
  }

  SpriteModel sprite(get_quest(), sprite_id);
  ui.preview_sprite_animation_field->addItems(sprite.get_animation_names());
  ui.preview_sprite_animation_field->setCurrentText(sprite.get_default_animation_name());
}

/**
 * @brief Called when the user selects a sprite animation.
 */
void ShaderEditor::preview_sprite_animation_changed() {

  const QString& sprite_id = ui.preview_sprite_field->get_selected_id();
  if (!sprite_id.isEmpty() &&
      quest.get_database().exists(ResourceType::SPRITE, sprite_id) &&
      quest.exists(quest.get_sprite_path(sprite_id))) {
    SpriteModel sprite(get_quest(), sprite_id);
    const QString& animation = ui.preview_sprite_animation_field->currentText();
    int num_directions = sprite.get_animation_num_directions({animation, 0});
    if (num_directions > 0) {
      ui.preview_sprite_direction_field->setMaximum(num_directions - 1);
    }
  }
}

/**
 * @brief Lets the user choose a PNG file for the preview.
 */
void ShaderEditor::browse_preview_picture() {

  if (shader == nullptr) {
    return;
  }

  try {
    const QString& directory = get_quest().get_resource_path(ResourceType::SPRITE);
    QString file_name = QFileDialog::getOpenFileName(
        this,
        tr("Open a PNG picture"),
        directory,
        tr("PNG file (*.png)")
    );

    if (file_name.isEmpty()) {
      return;
    }

    ui.preview_picture_field->setText(file_name);
    update_preview_image();
  }
  catch (const EditorException& ex) {
    ex.show_dialog();
  }
}

/**
 * @brief Updates the image to be displayed in the preview widget.
 */
void ShaderEditor::update_preview_image() {

  QImage image;
  EditorSettings settings;

  const QString& picture_file_name = ui.preview_picture_field->text();
  if (!picture_file_name.isEmpty()) {
    settings.set_value(EditorSettings::shader_preview_picture_file, picture_file_name);
  }
  const QString& map_id = ui.preview_map_field->get_selected_id();
  if (!map_id.isEmpty()) {
    settings.set_value(EditorSettings::shader_preview_map_id, map_id);
  }
  const QString& sprite_id = ui.preview_sprite_field->get_selected_id();
  if (!sprite_id.isEmpty()) {
    settings.set_value(EditorSettings::shader_preview_sprite_id, sprite_id);
  }

  if (ui.preview_picture_radio->isChecked()) {
    if (!picture_file_name.isEmpty()) {
      image = QImage(picture_file_name);
    }
    settings.set_value(EditorSettings::shader_preview_type, "picture");
  }
  else if (ui.preview_map_radio->isChecked()) {
    if (!map_id.isEmpty()) {
      MapModel map(get_quest(), map_id);
      MapView view;
      view.set_map(&map);
      image = view.export_to_image();
    }
    settings.set_value(EditorSettings::shader_preview_type, "map");
  }
  else if (ui.preview_sprite_radio->isChecked()) {
    if (!sprite_id.isEmpty() &&
        quest.get_database().exists(ResourceType::SPRITE, sprite_id) &&
        quest.exists(quest.get_sprite_path(sprite_id))) {
      SpriteModel sprite(get_quest(), sprite_id);
      QString animation = ui.preview_sprite_animation_field->currentText();
      int direction = ui.preview_sprite_direction_field->value();
      if (!sprite.animation_exists(animation)) {
        animation = sprite.get_default_animation_name();
      }
      if (direction < 0 || direction >= sprite.get_animation_num_directions({animation, 0})) {
        direction = 0;
      }
      QPixmap pixmap = sprite.get_direction_first_frame({ animation, direction });
      image = pixmap.toImage();
      settings.set_value(EditorSettings::shader_preview_sprite_animation, animation);
      settings.set_value(EditorSettings::shader_preview_sprite_direction, direction);
    }
    settings.set_value(EditorSettings::shader_preview_type, "sprite");
  }

  ui.shader_previewer->set_preview_image(image);
}

/**
 * @brief Saves the side panel width in settings
 * @param pos the position of the splitter
 * @param index of the element in the splitter
 */
void ShaderEditor::side_panel_resized(int pos, int index) {
  if (index == 1) {
    EditorSettings settings;
    settings.set_value(EditorSettings::shader_side_width, pos);
  }
}

/**
 * @brief Saves the preview panel height in settings
 * @param pos the position of the splitter
 * @param index of the element in the splitter
 */
void ShaderEditor::preview_panel_resized(int pos, int index) {
  if (index == 1) {
    EditorSettings settings;
    settings.set_value(EditorSettings::shader_files_height, pos);
  }
}

}
