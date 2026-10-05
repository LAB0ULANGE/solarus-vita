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
#ifndef SOLARUSEDITOR_SHADER_PREVIEWER_H
#define SOLARUSEDITOR_SHADER_PREVIEWER_H

#include "shader_preview_mode.h"

// WORKAROUND Avoid QOpenGL headers to redefine glad symbols when including GL/glext.h on macOS
#include "solarus/core/Common.h"
#ifdef SOLARUS_OSX
#  define GL_GLEXT_FUNCTION_POINTERS
#endif

#include <QFileSystemWatcher>
#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLDebugLogger>
#include <QOpenGLFramebufferObject>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>
#include <QPointer>
#include <QTimer>
#include <QWidget>

// #define SOLARUSEDITOR_DEBUG_GL

namespace SolarusEditor {

class ShaderModel;
class ViewSettings;

/**
 * @brief A widget to preview shaders.
 */
class ShaderPreviewer : public QOpenGLWidget {
  Q_OBJECT

public:

  ShaderPreviewer(QWidget* parent = nullptr);

  void set_model(ShaderModel* model);
  void set_view_settings(ViewSettings& view_settings);

  ShaderPreviewMode get_preview_mode() const;
  void set_preview_mode(ShaderPreviewMode preview_mode);
  void setup_framebuffers(const QSize& output_size);

  void zoom_in();
  void zoom_out();

  /// Mouse events
  void mouseMoveEvent(QMouseEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  void wheelEvent(QWheelEvent* event) override;

  void set_preview_image(QImage image);

  /// Opengl events
  virtual void paintGL() override;
  virtual void initializeGL() override;
  virtual void resizeGL(int w, int h) override;

signals:
  void shader_compilation_started(const QString& shader_id);
  void shader_compilation_finished(const QString& shader_id);
  void shader_error(const QString& message);
  void shader_warning(const QString& message);

public slots:
 void on_source_changed();
 void on_scaling_factor_changed(double factor);

private:
  using Textures = std::vector<std::pair<const char*,GLuint>>;

  /// Render utils
  void render_quad(QOpenGLShaderProgram& shader, const Textures& textures);
  void render_fbs();
  void render_swipe(float factor);
  void render_sbs();
  void compile_program();
  void build_preview_texture();

  QVector2D to_frame_center(const QPoint& mouse_position) const;
  float pixelFactor() const;

  void update_zoom();
  QSize get_letter_box(const QSize& qsize, const QSize& basesize) const;

  QString sanitizeShaderCode(const QString& code) const;

  bool should_recompile = true;
  QImage preview_image;
  QPointer<ViewSettings>
      view_settings;                            /**< How the view is displayed. */

  /// Move
  bool grabbing = false;                        /**< grab state */
  float zoom = 1.f;                             /**< zoom factor */
  QPointF last_mouse_pos;                       /**< last registered mouse position */
  QVector2D translation;                        /**< Translation vector */
  QCursor grab_cursor;                          /**< Cursor displayedd while grabbing */
  QCursor hover_cursor;                         /**< Cursor displayed while hovering */

  /// Opengl
  QOpenGLFramebufferObject* input_fb = nullptr; /**< Framebuffer to chich the input is drawn */
  QOpenGLFramebufferObject* output_fb = nullptr;/**< Framebuffer to which the output is drawn */
  QOpenGLBuffer* vertex_buffer = nullptr;       /**< quad buffer */
  QOpenGLVertexArrayObject* vao = nullptr;      /**< Empty vertex array for core profiles*/
  QOpenGLShaderProgram program;                 /**< shader program*/
  QOpenGLTexture* input_texture = nullptr;      /**< Texture of the input*/
  QPointer<ShaderModel> model;                  /**< The shader model. */
  ShaderPreviewMode preview_mode;               /**< Display mode of the preview. */
  QOpenGLShaderProgram simple_program;          /**< simple default shader for bliting*/
  QOpenGLShaderProgram swipe_program;           /**< swipe shader to draw two textures */

  QTimer fps_timer;                             /**< timer to update output periodically */

  QString glsl_version;
#ifdef SOLARUSEDITOR_DEBUG_GL
  QOpenGLDebugLogger gl_logger;                 /**< Logger to track opengl error in debug mode*/
private slots:
  void on_gl_log(const QOpenGLDebugMessage& message);
#endif
};

}

#endif
