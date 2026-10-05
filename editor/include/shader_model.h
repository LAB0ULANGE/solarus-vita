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
#ifndef SOLARUSEDITOR_SHADER_MODEL_H
#define SOLARUSEDITOR_SHADER_MODEL_H

#include <solarus/graphics/ShaderData.h>
#include <QObject>

namespace SolarusEditor {

class Quest;

/**
 * @brief Model that wraps a shader definition file.
 */
class ShaderModel : public QObject {
  Q_OBJECT

public:
  ShaderModel(const Quest& quest, const QString& shader_id, QObject* parent = nullptr);

  const Quest& get_quest() const;
  QString get_shader_id() const;

  QString get_vertex_file() const;
  void set_vertex_file(const QString& vertex_file);

  QString get_fragment_file() const;
  void set_fragment_file(const QString& fragment_file);

  static QString get_default_vertex_source();
  static QString get_default_fragment_source();

  double get_scaling_factor() const;
  void set_scaling_factor(double factor);

  void save() const;

signals:
  void vertex_file_changed(const QString& vertex_file);
  void fragment_file_changed(const QString& fragment_file);
  void scaling_factor_changed(float);

private:
  const Quest& quest;             /**< The quest the shader belongs to. */
  const QString shader_id;        /**< Id of the shader. */
  Solarus::ShaderData shader;     /**< Shader data wrapped by this model. */

};

}

#endif
