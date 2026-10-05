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
#include "editor_exception.h"
#include "quest.h"
#include "shader_model.h"
#include <solarus/graphics/DefaultShaders.h>

namespace SolarusEditor {

/**
 * @brief Creates a shader model.
 * @param quest The quest.
 * @param shader_id Id of the shader to represent
 * @param parent Parent object or nullptr.
 */
ShaderModel::ShaderModel(
    const Quest& quest,
    const QString& shader_id,
    QObject* parent) :
  QObject(parent),
  quest(quest),
  shader_id(shader_id) {

  Q_ASSERT(!shader_id.isEmpty());

  // Load the shader data file.
  QString path = quest.get_shader_data_file_path(shader_id);

  if (!shader.import_from_file(path.toLocal8Bit().toStdString())) {
    throw EditorException(tr("Cannot open shader data file '%1'").arg(path));
  }
}

/**
 * @brief Returns the quest this shader belongs to.
 * @return The quest.
 */
const Quest& ShaderModel::get_quest() const {
  return quest;
}

/**
 * @brief Returns the id of this shader.
 * @return The shader id.
 */
QString ShaderModel::get_shader_id() const {
  return shader_id;
}

/**
 * @brief Returns the name of the vertex shader file.
 * @return The vertex shader filename or an empty string.
 */
QString ShaderModel::get_vertex_file() const {

  return QString::fromStdString(shader.get_vertex_file());
}

/**
 * @brief Sets the name of the vertex shader file.
 *
 * Emits vertex_file_changed() if there is a change.
 *
 * @param vertex_file The vertex shader filename or an empty string.
 */
void ShaderModel::set_vertex_file(const QString& vertex_file) {

  const std::string& std_vertex_file = vertex_file.toStdString();

  if (std_vertex_file == shader.get_vertex_file()) {
    return;
  }

  shader.set_vertex_file(std_vertex_file);
  emit vertex_file_changed(vertex_file);
}

/**
 * @brief Returns the name of the fragment shader file.
 * @return The fragment shader filename or an empty string.
 */
QString ShaderModel::get_fragment_file() const {

  return QString::fromStdString(shader.get_fragment_file());
}

/**
 * @brief Sets the name of the fragment shader file.
 *
 * Emits fragment_file_changed() if there is a change.
 *
 * @param fragment_file The fragment shader filename or an empty string.
 */
void ShaderModel::set_fragment_file(const QString& fragment_file) {

  const std::string& std_fragment_file = fragment_file.toStdString();

  if (std_fragment_file == shader.get_fragment_file()) {
    return;
  }

  shader.set_fragment_file(std_fragment_file);
  emit fragment_file_changed(fragment_file);
}

/**
 * @brief Returns the default vertex shader GLSL code.
 * @return The default vertex shader.
 */
QString ShaderModel::get_default_vertex_source() {

  return QString::fromStdString(
        Solarus::DefaultShaders::get_default_vertex_source());
}

/**
 * @brief Returns the default fragment shader GLSL code.
 * @return The default fragment shader.
 */
QString ShaderModel::get_default_fragment_source() {

  return QString::fromStdString(
        Solarus::DefaultShaders::get_default_fragment_source());
}

/**
 * @brief get the scaling factor of this shader
 * @return scaling factor >= 0, other values mean there is no scaling factor
 */
double ShaderModel::get_scaling_factor() const {
  return shader.get_scaling_factor();
}

/**
 * @brief Sets this shader scaling factor
 * @param factor
 */
void ShaderModel::set_scaling_factor(double factor) {
  shader.set_scaling_factor(factor);
  emit scaling_factor_changed(factor);
}

/**
 * @brief Saves the sprite to its data file.
 * @throws EditorException If the file could not be saved.
 */
void ShaderModel::save() const {

  QString path = quest.get_shader_data_file_path(shader_id);

  if (!shader.export_to_file(path.toLocal8Bit().toStdString())) {
    throw EditorException(tr("Cannot save shader '%1'").arg(path));
  }
}

}
