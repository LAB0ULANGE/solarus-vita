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
#ifndef SOLARUSEDITOR_SHADER_EDITOR_H
#define SOLARUSEDITOR_SHADER_EDITOR_H

#include "widgets/editor.h"
#include "widgets/enum_selector.h"
#include "ui_shader_editor.h"

namespace SolarusEditor {

class ShaderModel;
class TextEditor;

/**
 * @brief A type of shader source editor.
 */
enum class WhichGlslEditor {
  VERTEX_EDITOR,
  FRAGMENT_EDITOR
};

/**
 * @brief A widget to edit graphically a shader description file.
 */
class ShaderEditor : public Editor {
  Q_OBJECT

public:

  ShaderEditor(Quest& quest, const QString& path, QWidget* parent = nullptr);
  ~ShaderEditor();

  ShaderModel& get_shader();

  void save() override;
  bool has_unsaved_changes() const override;
  void reload_settings() override;

private slots:

  void side_panel_resized(int pos, int index);
  void preview_panel_resized(int pos, int index);

private:

  void update();
  void update_shader_id_field();
  void update_description_to_gui();
  void set_description_from_gui();
  void update_scaling_factor_field();
  void scaling_factor_check_box_changed();
  void scaling_factor_field_changed();

  TextEditor* get_glsl_editor(WhichGlslEditor which);
  void update_source_editor_tab(WhichGlslEditor which);
  void source_file_check_box_changed(WhichGlslEditor which);
  void new_source_file(WhichGlslEditor which);
  void browse_source_file(WhichGlslEditor which);
  void save_source_file(WhichGlslEditor which);
  void source_editor_modification_state_changed(WhichGlslEditor which, bool clean);

  void preview_radio_changed();
  void preview_selected_sprite_changed();
  void preview_sprite_animation_changed();
  void browse_preview_picture();
  void update_preview_image();

  Ui::ShaderEditor ui;                /**< The shader editor widgets. */
  QString shader_id;                  /**< Id of the shader being edited. */
  std::unique_ptr<ShaderModel>
      shader;                         /**< Shader model being edited. */
  Quest& quest;                       /**< The quest. */

  TextEditor* vertex_editor;          /**< Code editor of the vertex file. */
  TextEditor* fragment_editor;        /**< Code editor of the fragment file. */
  QString last_vertex_file;           /**< Name of the last vertex GLSL file set. */
  QString last_fragment_file;         /**< Name of the last fragment GLSL file set. */
};

}

#endif
