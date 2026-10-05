/*
 * Copyright (C) 2006-2019 Christopho, Solarus - http://www.solarus-games.org
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
#include "widgets/console.h"
#include "editor_settings.h"
#include "editor_style.h"
#include "quest_runner.h"
#include <QDebug>
#include <QFont>
#include <QMenu>
#include <QRegularExpression>

namespace SolarusEditor {

namespace {

/**
 * @brief Wraps a line of plain text in html color tags.
 * @param line A plain text line.
 * @param color The color to set, with "#rrggbb" syntax.
 * @return The HTML colorized line.
 */
QString colorize(const QString& line, const QString& color) {
  return QString("<span style=\"color: %1\">%2</span>").arg(color, line.toHtmlEscaped());
}

/**
 * @brief Replaces ANSI color codes by HTML color tags.
 * @param line A line of output.
 * @return The line with ANSI color codes replaced.
 */
QString ansi_to_html(const QString& text) {

  static const QMap<int, QString> ansi_to_html_colors = {
      {30, "#000000"},  // Black.
      {31, "#ff0000"},  // Red.
      {32, "#00ff00"},  // Green.
      {33, "#ffff00"},  // Yellow.
      {34, "#0000ff"},  // Blue.
      {35, "#ff00ff"},  // Magenta.
      {36, "#00ffff"},  // Cyan.
      {37, "#d3d3d3"},  // Light Gray.
      {90, "#808080"},  // Dark Gray.
      {91, "#ff8080"},  // Light Red.
      {92, "#80ff80"},  // Light Green.
      {93, "#ffff80"},  // Light Yellow.
      {94, "#8080ff"},  // Light Blue.
      {95, "#ff80ff"},  // Light Magenta.
      {96, "#80ffff"},  // Light Cyan.
      {97, "#ffffff"}   // White.
  };

  static const QRegularExpression regex("\\033\\[(\\d+)m");
  QRegularExpressionMatchIterator i = regex.globalMatch(text);

  QString result;
  int last_position = 0;
  bool in_span = false;

  while (i.hasNext()) {
    QRegularExpressionMatch match = i.next();
    const int color_code = match.captured(1).toInt();

    // Append the text before the ANSI code.
    result += text.mid(last_position, match.capturedStart() - last_position).toHtmlEscaped();

    // Insert the HTML <span> tag with the corresponding color.
    if (ansi_to_html_colors.contains(color_code)) {
      if (in_span) {
        result += "</span>";
      }
      result += QString("<span style=\"color:%1;\">").arg(ansi_to_html_colors[color_code]);
      in_span = true;
    }
    last_position = static_cast<int>(match.capturedEnd());
  }

  // Append the remaining text after the last escape code.
  result += text.mid(last_position);
  if (in_span) {
    result += "</span>";
  }

  return result;
}

}  // Anonymous namespace.

/**
 * @brief Creates a console view.
 * @param parent Parent object or nullptr.
 */
Console::Console(QWidget* parent) :
  QWidget(parent),
  quest_runner(),
  raw_content(),
  last_new_message_index(-1),
  pending_commands(),
  output_command_id(-1),
  command_enabled(true) {

  ui.setupUi(this);

  // Font.
  reload_settings();

  // Theme.
  const EditorStyle* style = qobject_cast<EditorStyle*>(qApp->style());
  if (style != nullptr) {
    connect(style, &EditorStyle::actual_mode_changed, this, [this]() {
      // Recompute all colorization.
      ui.log_view->clear();
      last_new_message_index = -1;
      update_ui_with_new_messages();
    });
  }

  ui.log_view->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
  connect(ui.log_view, &QPlainTextEdit::customContextMenuRequested,
          this, &Console::context_menu_requested);
}

/**
 * @brief Reloads the settings from the editor settings.
 */
void Console::reload_settings() {

  EditorSettings settings;
  const QString font_family = settings.get_value_string(EditorSettings::font_family);
  const int font_size = settings.get_value_int(EditorSettings::font_size);

  QFont font(font_family);
  font.setPointSize(font_size);
  ui.log_view->setFont(font);
  ui.command_field->setFont(font);
}

/**
 * @brief Clears the content of the console.
 */
void Console::clear() {

  ui.log_view->clear();
  raw_content.clear();
  last_new_message_index = -1;
}

/**
 * @brief Adds a message to the console with a log level.
 * @param log_level Log level of the message.
 * @param message The text to add, possibly with multiple lines.
 */
void Console::add_message(const QString& log_level, const QString& message) {

  const QStringList lines = message.split("\n");
  for (QString line : lines) {
    if (!line.isEmpty()) {
      line = log_level + ": " + line;
    }
    raw_content.append(line);
  }
  update_ui_with_new_messages();
}

/**
 * @brief Adds a line of text to the console.
 */
void Console::add_line(const QString& line) {
  raw_content.append(line);
  update_ui_with_new_messages();
}

/**
 * @brief Formats and shows lines that are not yet in the console.
 */
void Console::update_ui_with_new_messages() {

  while (raw_content.size() > last_new_message_index + 1) {
    ++last_new_message_index;
    QString raw_line = raw_content.at(last_new_message_index);
    const QString &wrapped = QString("<pre>%1</pre>").arg(colorize_line(raw_line));
    ui.log_view->appendHtml(wrapped);
  }
}

/**
 * @brief Returns whether the Lua command input field is available.
 * @return @c true if the user can type Lua commands.
 */
bool Console::is_command_enabled() const {
  return command_enabled;
}

/**
 * @brief Sets whether the Lua command input field is available.
 * @param enable @c true to allow user to type Lua commands.
 */
void Console::set_command_enabled(bool enable) {

  this->command_enabled = enable;
  ui.command_field->setVisible(enable);
}

/**
 * @brief Connects this console to a quest runner.
 * @param quest_runner The quest runner.
 */
void Console::set_quest_runner(QuestRunner& quest_runner) {

  this->quest_runner = &quest_runner;

  connect(ui.command_field, &QLineEdit::returnPressed,
          this, &Console::command_field_activated);

  connect(&quest_runner, &QuestRunner::running,
          this, &Console::quest_running);
  connect(&quest_runner, &QuestRunner::finished,
          this, &Console::quest_finished);
  connect(&quest_runner, &QuestRunner::error,
          this, &Console::quest_error);
  connect(&quest_runner, &QuestRunner::output_produced,
          this, &Console::quest_output_produced);
}

/**
 * @brief Slot called when the quest execution begins.
 */
void Console::quest_running() {

  clear();
}

/**
 * @brief Slot called when the quest execution is finished.
 */
void Console::quest_finished() {

  output_command_id = -1;
  pending_commands.clear();
  output_command_result.clear();
}

/**
 * @brief Slot called when the quest execution produced some output lines.
 * @param lines The lines read from the standard output of the quest.
 */
void Console::quest_output_produced(const QStringList& lines) {

  for (const QString& line : lines) {
    parse_output(line);
  }
}


/**
 * @brief Slot called when the quest encounters a process execution error.
 * @param error The process error that happened.
 */
void Console::quest_error(QProcess::ProcessError error) {

  switch (error) {
  case QProcess::FailedToStart:
    add_message("Fatal", tr("The quest process failed to start."));
    break;
  case QProcess::Crashed:
    add_message("Fatal", tr("The quest process crashed."));
    break;
  case QProcess::Timedout:
    add_message("Error", tr("The quest process timed out."));
    break;
  case QProcess::WriteError:
  case QProcess::ReadError:
    // Nothing useful to log. Usually happens when the process is already
    // finished but then the user was already notified.
    break;
  case QProcess::UnknownError:
    add_message("Error", tr("An unknown error occurred to the quest process."));
    break;
  }
}

/**
 * @brief Slot called when the user wants to execute a Lua instruction from the console.
 */
void Console::command_field_activated() {

  if (quest_runner == nullptr) {
    return;
  }

  if (!quest_runner->is_running()) {
    return;
  }

  const QString& command = ui.command_field->text();

  execute_command(command);
  ui.command_field->command_executed(command);
}

/**
 * @brief Executes some Lua code in the quest process and logs it into the console.
 *
 * Use the command_result_received() signal to be notified of the result if you
 * need to.
 *
 * @param command The Lua code.
 * @return The id of the command executed, or -1 if it could not be sent
 * to the process.
 */
int Console::execute_command(const QString& command) {

  if (quest_runner == nullptr) {
    return -1;
  }

  if (!quest_runner->is_started()) {
    return -1;
  }

  if (command.isEmpty()) {
    return -1;
  }

  int command_id = quest_runner->execute_command(command);
  pending_commands[command_id] = command;

  return command_id;
}

/**
 * @brief Parses a Solarus output line and handles it.
 * @param line The output line.
 */
void Console::parse_output(const QString& line) {

  static const QRegularExpression output_regexp("^\\[Solarus\\] \\[(\\d+)\\] (\\w*): (.+)$");
  static const QRegularExpression output_simplify_console_error_regexp("In Lua command: \\[string \".*\"\\]:\\d+: ");

  if (line.isEmpty()) {
    add_line("");
    return;
  }

  QString log_level;
  QString message = line;
  QRegularExpressionMatch match_result = output_regexp.match(line);

  if (match_result.hasMatch()) {
    // 4 captures expected: full line, time, log level, message.
    QStringList captures = match_result.capturedTexts();
    if (captures.size() != 4) {
      add_line(line.toHtmlEscaped());
      return;
    }

    log_level = captures[2];
    message = captures[3];
  }

  if (!log_level.isEmpty() && message.isEmpty()) {
    // Solarus produced an empty message just to flush its stdout.
    return;
  }

  // Detect technical delimiters of commands output but don't show them.
  if (detect_command_result(log_level, message)) {
    return;
  }

  if (log_level.isEmpty()) {
    // Not a line from Solarus, probably one from the quest.
    add_line(line.toHtmlEscaped());
    return;
  }

  // Report system setting changes.
  detect_setting_change(log_level, message);

  if (log_level == "Error") {
    // Clean specific error messages.
    message.remove(output_simplify_console_error_regexp);
  }

  // Add color.
  if (message.isEmpty()) {
    return;
  }
  add_line(log_level + ": " + message);
}

/**
 * @brief Detects output messages that are the result of a command that was
 * send from the console.
 * @param log_level The Solarus log level of the line.
 * @param message The rest of the message.
 * @return @c true if the message is a command result delimiter and was consumed.
 */
bool Console::detect_command_result(
    const QString& log_level,
    const QString& message) {

  static const QRegularExpression output_command_result_begin_regexp("^====== Begin Lua command #(\\d+) ======$");
  static const QRegularExpression output_command_result_end_regexp("^====== End Lua command #(\\d+): (\\w+) ======$");

  QRegularExpressionMatch match_result;

  // Detect the beginning of a console command result.
  match_result = output_command_result_begin_regexp.match(message);
  if (log_level == "Info" && match_result.lastCapturedIndex() == 1) {
    // Start of a command result.
    if (output_command_id != -1) {
      qWarning() << "Beginning of a command result inside another command result";
    }

    output_command_id = match_result.captured(1).toInt();
    output_command_result = QString();

    // Show the command in the log view.
    // We show the command only when receiving its results,
    // to make sure it is displayed just before its results.
    QString command = pending_commands.take(output_command_id);
    add_line(QString("&gt; %1").arg(command.toHtmlEscaped()));

    return true;
  }

  // Detect the end of a console command result.
  match_result = output_command_result_end_regexp.match(message);
  if (log_level == "Info" && match_result.lastCapturedIndex() == 2) {
    // End of a command result.
    if (output_command_id == -1) {
      qWarning() << "End of a command result without beginning";
      return false;
    }
    int id = match_result.captured(1).toInt();
    bool success = (match_result.captured(2) == "success");
    QString command = pending_commands.take(output_command_id);
    QString result = output_command_result;

    if (id != output_command_id) {
      qWarning() << "Unmatched command delimiters";
    }

    pending_commands.remove(output_command_id);
    output_command_id = -1;
    output_command_result.clear();

    emit command_result_received(id, command, success, result);

    return true;
  }

  // Process the current result between delimiters.
  if (output_command_id != -1) {
    // We are inside the delimiters.
    output_command_result += message;
    return false;  // Let the console colorize and show the text normally.
  }

  return false;
}

/**
 * @brief Emits the signal system_setting_changed if an output message
 * indicates that a setting has just changed.
 * @param log_level The Solarus log level of the line.
 * @param message The rest of the message.
 */
void Console::detect_setting_change(
    const QString& log_level,
    const QString& message) {

  static const QRegularExpression output_setting_fullscreen_regexp("^Fullscreen: (\\w+)$");

  if (log_level != "Info") {
    return;
  }

  QRegularExpressionMatch match_result;

  match_result = output_setting_fullscreen_regexp.match(message);
  if (match_result.lastCapturedIndex() == 1) {
    QVariant value = (match_result.captured(1) == "yes");
    emit setting_changed_in_quest("quest_fullscreen", value);
    return;
  }
}

/**
 * @brief Returns a colorized version of a Solarus output line.
 *
 * Colors may be added if a log level or ANSI sequences are detected.
 *
 * @param line An output line.
 * @return The HTML decorated line.
 */
QString Console::colorize_line(const QString& message) {

  if (message.isEmpty()) {
    return message;
  }

  // Colorize warnings and errors.
  QString decorated_line = message;
  const EditorStyle* style = qobject_cast<EditorStyle*>(qApp->style());
  if (style != nullptr) {
    if (message.startsWith("Debug: ")) {
      decorated_line = colorize(decorated_line, style->get_mode_info().log_debug_color.name());
    }
    else if (message.startsWith("Info: ")) {
      decorated_line = colorize(decorated_line, style->get_mode_info().log_info_color.name());
    }
    else if (message.startsWith("Warning: ")) {
      decorated_line = colorize(decorated_line, style->get_mode_info().log_warning_color.name());
    }
    else if (message.startsWith("Error: ")) {
      decorated_line = colorize(decorated_line, style->get_mode_info().log_error_color.name());
    }
    else if (message.startsWith("Fatal: ")) {
      decorated_line = colorize(decorated_line, style->get_mode_info().log_error_color.name());
    }
  }

  // Also replace ANSI color codes if any.
  decorated_line = ansi_to_html(decorated_line);

  return decorated_line;
}

/**
 * @brief Shows a context menu in the console.
 * @param position Where to create the context menu.
 */
void Console::context_menu_requested(const QPoint& position) {

  QMenu* menu = ui.log_view->createStandardContextMenu();

  QAction* clear_action = new QAction("Clear", menu);
  clear_action->setEnabled(!ui.log_view->toPlainText().isEmpty());
  menu->addAction(clear_action);
  QObject::connect(clear_action, &QAction::triggered, this, [this]() {
    ui.log_view->clear();
  });

  menu->exec(ui.log_view->mapToGlobal(position));
}

}
