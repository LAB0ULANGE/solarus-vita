// SPDX-License-Identifier: GPL-3.0-or-later
#include <widgets/Console.h>

#include <quests/QuestRunner.h>

#include <oclero/qlementine/style/QlementineStyle.hpp>

#include <QDebug>
#include <QFont>
#include <QRegularExpression>
#include <QPlainTextEdit>
#include <QBoxLayout>
#include <QApplication>

namespace solarus::launcher {
namespace i18n {
/**
 * @brief Translates a quest error code to a human-readable string.
 *
 * @param error The quest error code.
 * @return QString The translated error message.
 */
QString questError(QuestRunner::ErrorCode error) {
  switch (error) {
    case QuestRunner::ErrorCode::ProcessFailedToStart:
      return QApplication::translate("SolarusLauncher", "The quest process failed to start.");
    case QuestRunner::ErrorCode::ProcessCrashed:
      return QApplication::translate("SolarusLauncher", "The quest process crashed.");
    case QuestRunner::ErrorCode::ProcessTimedOut:
      return QApplication::translate("SolarusLauncher", "The quest process timed out.");
    case QuestRunner::ErrorCode::ProcessWriteError:
    case QuestRunner::ErrorCode::ProcessReadError:
      return QString();
    default:
    case QuestRunner::ErrorCode::UnknownError:
      return QApplication::translate("SolarusLauncher", "An unknown error occurred to the quest process.");
  }
}
} // namespace i18n

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
    { 30, "#000000" }, // Black.
    { 31, "#ff0000" }, // Red.
    { 32, "#00ff00" }, // Green.
    { 33, "#ffff00" }, // Yellow.
    { 34, "#0000ff" }, // Blue.
    { 35, "#ff00ff" }, // Magenta.
    { 36, "#00ffff" }, // Cyan.
    { 37, "#d3d3d3" }, // Light Gray.
    { 90, "#808080" }, // Dark Gray.
    { 91, "#ff8080" }, // Light Red.
    { 92, "#80ff80" }, // Light Green.
    { 93, "#ffff80" }, // Light Yellow.
    { 94, "#8080ff" }, // Light Blue.
    { 95, "#ff80ff" }, // Light Magenta.
    { 96, "#80ffff" }, // Light Cyan.
    { 97, "#ffffff" } // White.
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
    last_position = match.capturedEnd();
  }

  // Append the remaining text after the last escape code.
  result += text.mid(last_position);
  if (in_span) {
    result += "</span>";
  }

  return result;
}

/**
 * @brief Converts a Qlementine theme to a Console theme.
 * @param theme The Qlementine theme.
 * @return The corresponding Console theme.
 */
Console::Theme themeFromStyleTheme(const oclero::qlementine::Theme& theme) {
  return {
    theme.primaryColor,
    theme.statusColorInfo,
    theme.statusColorWarning,
    theme.statusColorError,
    theme.statusColorError,
  };
}

} // Anonymous namespace.

/**
 * @brief Creates a console view.
 * @param parent Parent object or nullptr.
 */
Console::Console(QWidget* parent)
  : QWidget(parent)
  , quest_runner()
  , raw_content()
  , last_new_message_index(-1)
  , pending_commands()
  , output_command_id(-1) {
  setupUi();

  // Theming.
  const auto update_theme = [this]() {
    if (const auto* qlementine = qobject_cast<oclero::qlementine::QlementineStyle*>(this->style())) {
      // Recompute the theme.
      theme = themeFromStyleTheme(qlementine->theme());

      // Recompute all colorization.
      ui.log_view->clear();
      last_new_message_index = -1;
      update_ui_with_new_messages();
    }
  };
  update_theme();
  if (const auto* qlementine = qobject_cast<oclero::qlementine::QlementineStyle*>(this->style())) {
    QObject::connect(qlementine, &oclero::qlementine::QlementineStyle::themeChanged, this, update_theme);
  }
}

/**
 * @brief Sets up the user interface.
 */
void Console::setupUi() {
  auto* layout = new QVBoxLayout(this);
  this->setLayout(layout);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);

  ui.log_view = new QPlainTextEdit(this);
  ui.log_view->setFrameShape(QFrame::Shape::NoFrame);
  ui.log_view->setReadOnly(true);
  ui.log_view->setMaximumBlockCount(100000);

  layout->addWidget(ui.log_view);

  if (const auto* style = qobject_cast<oclero::qlementine::QlementineStyle*>(this->style())) {
    auto font = style->theme().fontMonospace;
    font.setPointSizeF(11.);
    ui.log_view->setFont(font);
  } else {
    const auto font = QFontDatabase::systemFont(QFontDatabase::SystemFont::FixedFont);
    ui.log_view->setFont(font);
  }
}

/**
 * @brief Clears the content of the console.
 */
void Console::clear() {
  ui.log_view->clear();
  raw_content.clear();
  last_new_message_index = -1;
  emit empty_changed();
}

/**
 * @brief Returns true if the console does not contain any text, false otherwise.
 */
bool Console::is_empty() const {
  return ui.log_view->document()->isEmpty();
}

/**
 * @brief Adds a message to the console with a log level.
 * @param log_level Log level of the message.
 * @param message The text to add, possibly with multiple lines.
 */
void Console::add_message(const QString& log_level, const QString& message) {
  QStringList lines = message.split("\n");
  for (QString line : std::as_const(lines)) {
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
  const auto is_empty = this->is_empty();
  while (raw_content.size() > last_new_message_index + 1) {
    ++last_new_message_index;
    QString raw_line = raw_content.at(last_new_message_index);
    const QString& wrapped = QString("<pre>%1</pre>").arg(colorize_line(raw_line));
    ui.log_view->appendHtml(wrapped);
  }
  if (is_empty != this->is_empty()) {
    emit empty_changed();
  }
}

/**
 * @brief Connects this console to a quest runner.
 * @param quest_runner The quest runner.
 */
void Console::set_quest_runner(QuestRunner* runner) {
  quest_runner = runner;
  if (quest_runner) {
    QObject::connect(quest_runner.data(), &QuestRunner::stateChanged, this, [this]() {
      const auto state = quest_runner->state();
      if (state == QuestRunner::State::Running) {
        quest_running();
      } else if (state == QuestRunner::State::Stopped) {
        quest_finished();
      }
    });
    QObject::connect(quest_runner.data(), &QuestRunner::errorRaised, this, &Console::quest_error);
    QObject::connect(quest_runner.data(), &QuestRunner::outputProduced, this, &Console::quest_output_produced);
  }
}

/**
 * @brief Returns the QPlainTextEdit where the logs are shown.
 * @return The log view.
 */
QPlainTextEdit* Console::log_view() {
  return ui.log_view;
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
void Console::quest_error(QuestRunner::ErrorCode error) {
  const auto text = i18n::questError(error);
  switch (error) {
    case QuestRunner::ErrorCode::ProcessFailedToStart:
    case QuestRunner::ErrorCode::ProcessCrashed:
      add_message("Fatal", text);
      break;
    case QuestRunner::ErrorCode::ProcessTimedOut:
      add_message("Error", text);
      break;
    case QuestRunner::ErrorCode::ProcessWriteError:
    case QuestRunner::ErrorCode::ProcessReadError:
      // Nothing useful to log. Usually happens when the process is already
      // finished but then the user was already notified.
      break;
    default:
    case QuestRunner::ErrorCode::UnknownError:
      add_message("Error", text);
      break;
  }
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
bool Console::detect_command_result(const QString& log_level, const QString& message) {
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
    return false; // Let the console colorize and show the text normally.
  }

  return false;
}

/**
 * @brief Emits the signal system_setting_changed if an output message
 * indicates that a setting has just changed.
 * @param log_level The Solarus log level of the line.
 * @param message The rest of the message.
 */
void Console::detect_setting_change(const QString& log_level, const QString& message) {
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
  auto decorated_line = QString(message);

  if (message.startsWith("Debug: ")) {
    decorated_line = colorize(decorated_line, theme.debugColor.name());
  } else if (message.startsWith("Info: ")) {
    decorated_line = colorize(decorated_line, theme.infoColor.name());
  } else if (message.startsWith("Warning: ")) {
    decorated_line = colorize(decorated_line, theme.warningColor.name());
  } else if (message.startsWith("Error: ")) {
    decorated_line = colorize(decorated_line, theme.errorColor.name());
  } else if (message.startsWith("Fatal: ")) {
    decorated_line = colorize(decorated_line, theme.fatalColor.name());
  }

  // Also replace ANSI color codes if any.
  decorated_line = ansi_to_html(decorated_line);

  return decorated_line;
}

} // namespace solarus::launcher
