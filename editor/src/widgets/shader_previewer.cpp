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
#include <glad/glad.h>
#include <solarus/graphics/VertexArray.h>

// Workaround for conflicting declaration of GLsizeiptrARB on Windows.
#ifndef GL_ARB_vertex_buffer_object
#define GL_ARB_vertex_buffer_object 1
#endif

#include "widgets/shader_previewer.h"
#include "quest.h"
#include "shader_model.h"
#include "view_settings.h"

#define GLM_FORCE_INLINE
#include <solarus/graphics/DefaultShaders.h>
#include <solarus/graphics/Shader.h>

#include <QFile>
#include <QMatrix3x3>
#include <QMouseEvent>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QWheelEvent>

namespace SolarusEditor {

constexpr const char* SWIPE_VERTEX_SHADER =
    R"(
      #if __VERSION__ >= 130
      #define COMPAT_VARYING out
      #define COMPAT_ATTRIBUTE in
      #else
      #define COMPAT_VARYING varying
      #define COMPAT_ATTRIBUTE attribute
      #endif

      #ifdef GL_ES
      precision mediump float;
      #define COMPAT_PRECISION mediump
      #else
      #define COMPAT_PRECISION
      #endif

      uniform mat4 sol_mvp_matrix;
      uniform mat3 sol_uv_matrix;
      COMPAT_ATTRIBUTE vec2 sol_vertex;
      COMPAT_ATTRIBUTE vec2 sol_tex_coord;
      COMPAT_ATTRIBUTE vec4 sol_color;

      COMPAT_VARYING vec2 sol_vtex_coord;
      COMPAT_VARYING vec4 sol_vcolor;
      COMPAT_VARYING float vfactor;

      void main() {
         gl_Position = sol_mvp_matrix * vec4(sol_vertex,0,1);
         vfactor = (gl_Position.x + 1.0)*0.5;
         sol_vcolor = sol_color;
         sol_vtex_coord = (sol_uv_matrix * vec3(sol_tex_coord,1)).xy;
      }
    )";

constexpr const char* SWIPE_FRAGMENT_SHADER =
    R"(
      #if __VERSION__ >= 130
      #define COMPAT_VARYING in
      #define COMPAT_TEXTURE texture
      out vec4 FragColor;
      #else
      #define COMPAT_VARYING varying
      #define FragColor gl_FragColor
      #define COMPAT_TEXTURE texture2D
      #endif

      #ifdef GL_ES
      precision mediump float;
      #define COMPAT_PRECISION mediump
      #else
      #define COMPAT_PRECISION
      #endif

      uniform float factor;
      uniform sampler2D input_tex;
      uniform sampler2D output_tex;
      uniform vec2 sol_output_size;
      COMPAT_VARYING vec2 sol_vtex_coord;
      COMPAT_VARYING vec4 sol_vcolor;
      COMPAT_VARYING float vfactor;

      void main() {
        vec4 tex_i = COMPAT_TEXTURE(input_tex, sol_vtex_coord);
        vec4 tex_o = COMPAT_TEXTURE(output_tex, vec2(sol_vtex_coord.x,1.0-sol_vtex_coord.y));
        if(vfactor<factor) {
          FragColor = tex_i;
        } else {
          FragColor = tex_o;
        }
      }
    )";

/**
 * @brief Creates a shader previewer.
 * @param parent The parent object or nullptr.
 */
ShaderPreviewer::ShaderPreviewer(QWidget *parent) :
  QOpenGLWidget(parent),
  program(this),
  model(nullptr),
  preview_mode(ShaderPreviewMode::SIDE_BY_SIDE)
#ifdef SOLARUSEDITOR_DEBUG_GL
  ,gl_logger(this)
#endif
{

  QSurfaceFormat format;
  format.setProfile(QSurfaceFormat::CoreProfile);
#ifdef SOLARUSEDITOR_DEBUG_GL
  format.setMajorVersion(3);
  format.setMinorVersion(2);
  format.setOption(QSurfaceFormat::DebugContext);
#endif
  setFormat(format);

  // Setup cursors
  grab_cursor.setShape(Qt::ClosedHandCursor);
  hover_cursor.setShape(Qt::OpenHandCursor);

  setCursor(hover_cursor);
  setMouseTracking(true);

  connect(&fps_timer, &QTimer::timeout, this, [this]{
    update();
  });
  fps_timer.setInterval(10);
  fps_timer.start();
}

/**
 * @brief handle mouse movement, moving scene if grabbed
 * @param event
 */
void ShaderPreviewer::mouseMoveEvent(QMouseEvent* event) {
  Q_UNUSED(event);
  if (grabbing) {
    QPointF d = event->position().toPoint() - last_mouse_pos;
    d.setY(-d.y());
    translation += QVector2D(d) / (zoom*pixelFactor());
    last_mouse_pos = event->position().toPoint();
    event->accept();
  }
  update();
}

/**
 * @brief handle mouse press
 * @param event
 */
void ShaderPreviewer::mousePressEvent(QMouseEvent* event) {
  Q_UNUSED(event);
  if (event->button() == Qt::LeftButton ||
      event->button() == Qt::MiddleButton) {
    grabbing = true;
    last_mouse_pos = event->position().toPoint();
    setCursor(grab_cursor);
    event->accept();
  }
}

/**
 * @brief handle mouse release
 * @param event
 */
void ShaderPreviewer::mouseReleaseEvent(QMouseEvent* event) {
  Q_UNUSED(event);
  if (event->button() == Qt::LeftButton ||
      event->button() == Qt::MiddleButton) {
    grabbing = false;
    setCursor(hover_cursor);
    event->accept();
  }
}


/**
 * @brief get displacement to the frame center
 * @param mouse_position
 * @return
 */
QVector2D ShaderPreviewer::to_frame_center(const QPoint& mouse_position) const {
  auto center = [&]()->QVector2D{
    switch(preview_mode) {
    case ShaderPreviewMode::SIDE_BY_SIDE:
    {
      float y = frameSize().height() / 2.f;
      QSize qsize = model->get_quest().get_properties().get_normal_quest_size();
      qsize.setWidth(qsize.width()*2);
      QSize letterb = get_letter_box(qsize,frameSize());
      int rx = (frameSize().width() - letterb.width()) / 2;
      if(mouse_position.x() < frameSize().width() / 2) {
        return {rx+letterb.width()/4.f,y};
      } else {
        return {rx+(letterb.width()*3.f)/4.f,y};
      }
    }
    default:
    {
      QSize fsize = frameSize();
      return {fsize.width()/2.f,fsize.height()/2.f};
    }
    };
  };
  return (center() - QVector2D(mouse_position))*QVector2D(1,-1);
}

/**
 * @brief Receives a mouse wheel event.
 * @param event The event to handle.
 */
void ShaderPreviewer::wheelEvent(QWheelEvent* event) {

  float old_zoom = zoom;
  if (event->angleDelta().y() > 0) {
    zoom_in();
    if (zoom != old_zoom) {
        translation += to_frame_center(event->position().toPoint() / zoom) / pixelFactor();
    }
  }
  else {
    zoom_out();
    if (zoom != old_zoom) {
      translation -= 0.5 * to_frame_center(event->position().toPoint() / zoom) / pixelFactor();
    }
  }

}

/**
 * @brief Scales the view by a factor of 2.
 *
 * Zooming will be anchored at the mouse position.
 * The maximum zoom value is 4.0: this function does nothing if you try to
 * zoom more.
 */
void ShaderPreviewer::zoom_in() {

  if (view_settings == nullptr) {
    return;
  }

  view_settings->set_zoom(view_settings->get_zoom() * 2.0);
}

/**
 * @brief Scales the view by a factor of 0.5.
 *
 * Zooming will be anchored at the mouse position.
 * The maximum zoom value is 0.25: this function does nothing if you try to
 * zoom less.
 */
void ShaderPreviewer::zoom_out() {

  if (view_settings == nullptr) {
    return;
  }

  view_settings->set_zoom(view_settings->get_zoom() / 2.0);
}

/**
 * @brief Sets the zoom level of the view from the settings.
 *
 * Zooming will be anchored at the mouse position.
 * The zoom value will be clamped between 1 and 4.0.
 */

void ShaderPreviewer::update_zoom() {

  if (view_settings == nullptr) {
    return;
  }

  float zoom = static_cast<float>(view_settings->get_zoom());
  zoom = qMin(4.0f, qMax(1.f, zoom));
  view_settings->set_zoom(zoom);

  if (zoom == this->zoom) {
    return;
  }


  this->zoom = zoom;
  update();
}

/**
 * @brief compute letter boxed size
 * @param qsize size of the box to fit
 * @param basesize size of the enclosing box
 * @return qsize scaled to fit in basesize
 */
QSize ShaderPreviewer::get_letter_box(const QSize& qsize, const QSize& basesize) const {
  float qratio = qsize.width() / (float) qsize.height();
  float wratio = basesize.width() / (float) basesize.height();
  if (qratio > wratio) {
    return QSize(basesize.width(), basesize.width() / qratio);
  } else {
    return QSize(basesize.height() * qratio, basesize.height());
  }
}

/**
 * @brief ShaderPreviewer::sanitizeShaderCode
 * @param code
 * @return
 */
QString ShaderPreviewer::sanitizeShaderCode(const QString& code) const {
  if(code.contains("#version")) {
    return code;
  } else {
    return glsl_version + code;
  }
}

/**
 * @brief return the factor between inputframebuffer pixels and screen pixels
 * @return
 */
float ShaderPreviewer::pixelFactor() const  {
  QSize qsize = model->get_quest().get_properties().get_normal_quest_size();
  switch(preview_mode) {
    case ShaderPreviewMode::SIDE_BY_SIDE: {
      qsize.setWidth(qsize.width()*2);
      QSize lb = get_letter_box(qsize,frameSize());
      return 0.5f*lb.width() / (float) input_fb->width();
    }
    default: {
      QSize lb = get_letter_box(qsize,frameSize());
      return lb.width() / (float) input_fb->width();
    }
  }
}

/**
 * @brief Sets the shader model.
 * @param model The shader model, or nullptr to remove any model.
 * This class does not take ownership on the model.
 * The model can be deleted safely.
 */
void ShaderPreviewer::set_model(ShaderModel* model) {

  if (this->model != nullptr) {
    this->model->disconnect(this);
    this->model = nullptr;
  }

  this->model = model;

  if (model != nullptr) {
    connect(model, &ShaderModel::fragment_file_changed,
            this, &ShaderPreviewer::on_source_changed);
    connect(model, &ShaderModel::vertex_file_changed,
            this, &ShaderPreviewer::on_source_changed);
    connect(model, &ShaderModel::scaling_factor_changed,
            this, &ShaderPreviewer::on_scaling_factor_changed);
    on_source_changed();
  }
}

/**
 * @brief Sets the view settings for this view.
 *
 * When they change, the view is updated accordingly.
 *
 * @param view_settings The settings to watch.
 */
void ShaderPreviewer::set_view_settings(ViewSettings& view_settings) {

  this->view_settings = &view_settings;

  connect(&view_settings, &ViewSettings::zoom_changed,
          this, &ShaderPreviewer::update_zoom);
  update_zoom();
}

/**
 * @brief Returns the current preview mode.
 * @return The preview mode.
 */
ShaderPreviewMode ShaderPreviewer::get_preview_mode() const {
  return preview_mode;
}

/**
 * @brief Changes the preview mode.
 * @param preview_mode The new mode to set.
 */
void ShaderPreviewer::set_preview_mode(ShaderPreviewMode preview_mode) {
  this->preview_mode = preview_mode;
  update();
}

/**
 * @brief re-create framebuffer with appropriate size
 * @param output_size size of the output buffer
 */
void ShaderPreviewer::setup_framebuffers(const QSize& output_size) {
  makeCurrent();
  delete input_fb;
  delete output_fb;

  QSize size = model->get_quest().get_properties().get_normal_quest_size();
  input_fb = new QOpenGLFramebufferObject(size);
  if (!input_fb->isValid()) {
    qWarning() << "Failed to create input frame buffer with size" << size;
  }
  output_fb = new QOpenGLFramebufferObject(output_size);
  if (!output_fb->isValid()) {
    qWarning() << "Failed to create output frame buffer with size" << output_size;
  }
}

/**
 * @brief Render input to input buffer and rerender it with shader to output buffer
 */
void ShaderPreviewer::render_fbs() {
  if (input_texture == nullptr || input_fb == nullptr) {
    return;
  }
  QOpenGLFunctions* gl = context()->functions();
  { //Render input to simulate quest surface
    input_fb->bind();
    QMatrix4x4 mvp;
    //TODO mouse control
    mvp.ortho(0, input_fb->width(), input_fb->height(), 0, -1, 1);
    //mvp.scale(zoom);
    mvp.translate(floor(translation.x()), floor(translation.y()), 0);
    mvp.scale(input_texture->width(), input_texture->height(), 1);
    QMatrix3x3 uvm;
    gl->glClearColor(0, 0, 0, 1.f);
    gl->glViewport(0, 0, input_fb->width(), input_fb->height());
    gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    simple_program.bind();
    simple_program.setUniformValue(Solarus::Shader::UV_MATRIX_NAME, uvm);
    simple_program.setUniformValue(Solarus::Shader::MVP_MATRIX_NAME, mvp);
    const char* texture_name = Solarus::Shader::TEXTURE_NAME;
    render_quad(simple_program, {{texture_name, input_texture->textureId()}});
    input_fb->release();
  }
  { // Render output to simulate screen
    output_fb->bind();
    QMatrix4x4 mvp;
    mvp.translate(-1, -1, 0);
    mvp.scale(2);
    QMatrix3x3 uvm;
    gl->glClearColor(0, 0, 0, 0);
    gl->glViewport(0, 0, output_fb->width(), output_fb->height());
    gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    program.bind();
    program.setUniformValue(Solarus::Shader::UV_MATRIX_NAME,uvm);
    program.setUniformValue(Solarus::Shader::MVP_MATRIX_NAME,mvp);
    QSize isize = model->get_quest().get_properties().get_normal_quest_size();
    program.setUniformValue(
          Solarus::Shader::INPUT_SIZE_NAME,
          QVector2D(isize.width(), isize.height()));
    program.setUniformValue(
          Solarus::Shader::OUTPUT_SIZE_NAME,
          QVector2D(output_fb->width(), output_fb->height()));
    int time_loc = program.uniformLocation(Solarus::Shader::TIME_NAME);
    int ms = QTime::currentTime().msecsSinceStartOfDay();
    gl->glUniform1i(time_loc, ms);
    const char* texture_name = Solarus::Shader::TEXTURE_NAME;
    render_quad(program, {{texture_name, input_fb->texture()}});
    output_fb->release();
  }

}

/**
 * @brief render a quad with the given shader and textures
 * @param shader a valid Solarus-Style shader
 * @param textures a vector of name-handle pairs
 */
void ShaderPreviewer::render_quad(QOpenGLShaderProgram& shader,  const Textures& textures) {
  shader.bind();
  vao->bind();
  vertex_buffer->bind();
  QOpenGLFunctions* gl = context()->functions();
  int pos_loc = shader.attributeLocation(Solarus::Shader::POSITION_NAME);
  if (pos_loc > -1) {
    gl->glEnableVertexAttribArray(pos_loc);
    gl->glVertexAttribPointer(pos_loc, 2, GL_FLOAT, GL_FALSE, sizeof(Solarus::Vertex), (void*) offsetof(Solarus::Vertex, position));
  }
  int uv_loc = shader.attributeLocation(Solarus::Shader::TEXCOORD_NAME);
  if (uv_loc > -1) {
    gl->glEnableVertexAttribArray(uv_loc);
    gl->glVertexAttribPointer(uv_loc, 2, GL_FLOAT, GL_FALSE, sizeof(Solarus::Vertex), (void*) offsetof(Solarus::Vertex, texcoords));
  }
  int color_loc = shader.attributeLocation(Solarus::Shader::COLOR_NAME);
  if (color_loc > -1) {
    gl->glEnableVertexAttribArray(color_loc);
    gl->glVertexAttribPointer(color_loc, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Solarus::Vertex), (void*) offsetof(Solarus::Vertex, color));
  }

  for (size_t i = 0; i < textures.size(); ++i) {
    int tex_loc = shader.uniformLocation(textures[i].first);
    gl->glUniform1i(tex_loc, static_cast<GLint>(i));
    gl->glActiveTexture(GL_TEXTURE0 + static_cast<GLint>(i));
    gl->glBindTexture(GL_TEXTURE_2D, textures[i].second);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  }
  gl->glDrawArrays(GL_TRIANGLES, 0, 6);
  vertex_buffer->release();
  vao->release();
  shader.release();
}

/**
 * @brief render the framebuffers in a swipe fashion
 * @param factor position of the vertical split [0,1]
 */
void ShaderPreviewer::render_swipe(float factor) {
  swipe_program.bind();
  QMatrix4x4 mvp;
  mvp.scale(zoom);
  mvp.translate(-1, -1, 0);
  mvp.scale(2);
  QMatrix3x3 uvm;
  swipe_program.setUniformValue(Solarus::Shader::UV_MATRIX_NAME, uvm);
  swipe_program.setUniformValue(Solarus::Shader::MVP_MATRIX_NAME, mvp);
  swipe_program.setUniformValue("factor", factor);
  QSize vp = get_letter_box(model->get_quest().get_properties().get_normal_quest_size(), frameSize());
  swipe_program.setUniformValue(
        Solarus::Shader::OUTPUT_SIZE_NAME,
        QVector2D(vp.width(),vp.height()));
  render_quad(swipe_program, {{"input_tex", input_fb->texture()}, {"output_tex", output_fb->texture()}});
}

/**
 * @brief render the framebuffers in a Side by Side fashion
 */
void ShaderPreviewer::render_sbs() {
  QSize qsize = model->get_quest().get_properties().get_normal_quest_size();
  qsize.setWidth(qsize.width()*2);
  QSize letterb = get_letter_box(qsize,frameSize());
  auto render_side = [&](GLuint tex, bool invert) {
    QMatrix4x4 mvp;
    mvp.scale(zoom);
    mvp.translate(-1, -1, 0);
    mvp.scale(2);
    QMatrix3x3 uvm;
    if (invert) {
      uvm.data()[4] = -1;
      uvm.data()[7] = 1;
    }
    simple_program.bind();
    simple_program.setUniformValue(Solarus::Shader::UV_MATRIX_NAME, uvm);
    simple_program.setUniformValue(Solarus::Shader::MVP_MATRIX_NAME, mvp);
    const char* name = Solarus::Shader::TEXTURE_NAME;
    render_quad(simple_program, {{name, tex}});
  };
  QOpenGLFunctions* gl = context()->functions();
  int x = (frameSize().width() - letterb.width()) / 2;
  int y = (frameSize().height() - letterb.height()) / 2;
  gl->glViewport(x, y, letterb.width() / 2, letterb.height());
  render_side(input_fb->texture(), false);
  gl->glViewport(x + letterb.width() / 2, y, letterb.width()/2, letterb.height());
  render_side(output_fb->texture(), true);
}

/**
 * @brief QOpengl pain event all drawing happens here
 */
void ShaderPreviewer::paintGL() {
  QOpenGLFunctions* gl = context()->functions();
  gl->glClearColor(0.3f, 0.3f, 0.3f, 1);
  gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  if (model == nullptr || input_fb == nullptr) {
    return;
  }
  if (should_recompile) {
    compile_program();
  }
  render_fbs();
  QSize letterb = get_letter_box(model->get_quest().get_properties().get_normal_quest_size(), frameSize());
  int x = (frameSize().width() - letterb.width()) / 2;
  int y = (frameSize().height() - letterb.height()) / 2;
  gl->glViewport(x, y, letterb.width(), letterb.height());
  switch (preview_mode) {
  case ShaderPreviewMode::INPUT:
    render_swipe(1.f);
    break;
  case ShaderPreviewMode::OUTPUT:
    render_swipe(0.f);
    break;
  case ShaderPreviewMode::SIDE_BY_SIDE:
    render_sbs();
    break;
  case ShaderPreviewMode::SWIPE:
  {
    QPoint p = mapFromGlobal(QCursor::pos());
    float factor = (p.x() - x) / (float) letterb.width();
    render_swipe(factor); // TODO pass actual factor
    break;
  }
  default:
    break;
  }
}

/**
 * @brief QOpengl initialisation event, initialize as much as possible
 */
void ShaderPreviewer::initializeGL() {

#ifdef SOLARUSEDITOR_DEBUG_GL
  gl_logger.initialize();
  gl_logger.startLogging(QOpenGLDebugLogger::SynchronousLogging);
  connect(&gl_logger,&QOpenGLDebugLogger::messageLogged, this, &ShaderPreviewer::on_gl_log);
#endif
  QSurfaceFormat format = context()->format();

  auto make_number = [&](int major,int minor) -> QString {
    switch(major*10+minor){
      case 20:
        return "110";
      case 21:
        return "120";
      case 30:
        return "130";
      case 31:
        return "140";
      case 32:
        return "150";
      default:
      if(major*10+minor >= 33) {
        return QString::number(major*100+minor*10);
      } else {
        return "110";
      }
    }
  };

  glsl_version = QString("#version %1\n").arg(make_number(format.majorVersion(),format.minorVersion()));

  // Setup quad
  QOpenGLFunctions* gl = context()->functions();
  gl->initializeOpenGLFunctions();
  gl->glClearColor(0.3f,0.3f,0.3f,1);
  gl->glDisable(GL_DEPTH_TEST);
  gl->glDisable(GL_CULL_FACE);
  vertex_buffer = new QOpenGLBuffer();
  if (!vertex_buffer->create()) {
    qWarning() << "Failed to create glbuffer!"; // TODO fail gracefully
  }
  Solarus::VertexArray array;
  array.add_quad(Solarus::Rectangle(0, 0, 1, 1),
                 Solarus::Rectangle(0, 1, 1, -1),
                 Solarus::Color::white);
  vertex_buffer->bind();
  vertex_buffer->allocate(array.data(), static_cast<int>(array.vertex_count() * sizeof(Solarus::Vertex)));
  vertex_buffer->release();
  // Create empty vao for core profiles
  vao = new QOpenGLVertexArrayObject();
  vao->create();

  // Create simple shader
  simple_program.addShaderFromSourceCode(
        QOpenGLShader::Vertex,
        Solarus::DefaultShaders::get_default_vertex_source().c_str());
  simple_program.addShaderFromSourceCode(
        QOpenGLShader::Fragment,
        Solarus::DefaultShaders::get_default_fragment_source().c_str());
  if (!simple_program.link()) {
    emit shader_error("Failed to link basic shader program:\n" + simple_program.log());
  }

  // Create swipe shader
  swipe_program.addShaderFromSourceCode(
        QOpenGLShader::Vertex,
        sanitizeShaderCode(SWIPE_VERTEX_SHADER));

  swipe_program.addShaderFromSourceCode(
        QOpenGLShader::Fragment,
        sanitizeShaderCode(SWIPE_FRAGMENT_SHADER));
  if (!swipe_program.link()) {
    emit shader_error("Failed to link swipe shader program:\n" + swipe_program.log());
  }

  // OpenGL is now ready, build the texture if it was already set.
  build_preview_texture();
}

/**
 * @brief QOpenGL resize event, framebuffer are recreated at the right size
 * @param w width
 * @param h height
 */
void ShaderPreviewer::resizeGL(int w, int h) {
  Q_UNUSED(w);
  Q_UNUSED(h);
  if (model != nullptr) {
    if (model->get_scaling_factor() > 0) {
      // Intermediate surface with fixed size
      setup_framebuffers(
          model->get_scaling_factor() * model->get_quest().get_properties().get_normal_quest_size());
    } else {
      QSize letterb = get_letter_box(model->get_quest().get_properties().get_normal_quest_size(), frameSize());
      setup_framebuffers(letterb);
    }
  }
}

/**
 * @brief notify change of the model scaling factor
 * @param factor
 */
void ShaderPreviewer::on_scaling_factor_changed(double factor) {
  Q_UNUSED(factor);
  /*setup_framebuffers(
        factor * model->get_quest().get_properties().get_normal_quest_size());*/
  resizeGL(0,0);
  update();
}

/**
 * @brief notify change of the model sources
 */
void ShaderPreviewer::on_source_changed() {
  should_recompile = true;
  update();
}

/**
 * @brief compile the model shader
 */
void ShaderPreviewer::compile_program() {
  program.removeAllShaders();
  emit shader_compilation_started(model->get_shader_id());
  auto check_warnings = [this]{
    if(program.log().size()) {
      emit shader_warning(program.log());
    }
  };
  auto fail = [this](const QString& s){
    emit shader_error(s);
    should_recompile = false;
  };

  auto readFile = [](const QString& path) -> QString {
        QFile file(path);
        file.open(QIODevice::ReadOnly);

        QTextStream s1(&file);
        return s1.readAll();
  };

  QString vertex_source;
  QString fragment_source;

  QString vertex_file_path = model->get_quest().get_shader_code_file_path(model->get_vertex_file());
  if (!model->get_quest().exists(vertex_file_path) ||
      !model->get_quest().is_shader_code_file(vertex_file_path)) {
    vertex_file_path = QString();
  }
  if (!vertex_file_path.isEmpty()) {
    vertex_source = readFile(vertex_file_path);
  } else {
    vertex_source = QString::fromStdString(Solarus::DefaultShaders::get_default_vertex_source());
  }

  QString fragment_file_path = model->get_quest().get_shader_code_file_path(model->get_fragment_file());
  if (!model->get_quest().exists(fragment_file_path) ||
      !model->get_quest().is_shader_code_file(fragment_file_path)) {
    fragment_file_path = QString();
  }
  if (!fragment_file_path.isEmpty()) {
    fragment_source = readFile(fragment_file_path);
  } else {
    fragment_source = QString::fromStdString(Solarus::DefaultShaders::get_default_fragment_source());
  }

  if (!program.addShaderFromSourceCode(
        QOpenGLShader::Vertex,
        sanitizeShaderCode(vertex_source))) {
    return fail("Failed to compile vertex shader:\n" + program.log());
  }
  if (!program.addShaderFromSourceCode(
        QOpenGLShader::Fragment,
        sanitizeShaderCode(fragment_source))) {
    return fail("Failed to compile fragment shader:\n" + program.log());
  }

  if (!program.link()) {
    return fail("Failed to link shader program:\n" + program.log());
  } else {
    check_warnings();
  }

  should_recompile = false;
  emit shader_compilation_finished(model->get_shader_id());
}

#ifdef SOLARUSEDITOR_DEBUG_GL
/**
 * @brief OpenGL error log slot
 * @param message
 */
void ShaderPreviewer::on_gl_log(const QOpenGLDebugMessage& message) {
  qDebug() << message;
}
#endif

/**
 * @brief Changes the image to be displayed in the preview widget.
 * @param image The new image to show.
 */
void ShaderPreviewer::set_preview_image(QImage image) {

  if (image == this->preview_image) {
    return;
  }
  this->preview_image = image;
  build_preview_texture();
}

/**
 * @brief Creates the OpenGL texture to be displayed.
 */
void ShaderPreviewer::build_preview_texture() {

  if (vertex_buffer == nullptr) {
    // OpenGL is not initialized yet.
    return;
  }

  // TODO manage memory
  makeCurrent();
  delete input_texture;
  input_texture = nullptr;

  if (!preview_image.isNull()) {
    input_texture = new QOpenGLTexture(preview_image, QOpenGLTexture::MipMapGeneration::DontGenerateMipMaps);
  }
  zoom = 1;
  translation = QVector2D();

  update();
}

}
