// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <quests/QuestRunner.h>

#include <QWidget>
#include <QMap>
#include <QPointer>
#include <QProcess>

class QPlainTextEdit;

namespace solarus::launcher {

class Console : public QWidget {
  Q_OBJECT

public:
  struct Theme {
    QColor debugColor{ 255, 255, 255, 127 };
    QColor infoColor{ 126, 160, 255 };
    QColor warningColor{ 255, 188, 0 };
    QColor errorColor{ 252, 72, 80 };
    QColor fatalColor{ 252, 72, 80 };
  };

public:
  explicit Console(QWidget* parent = nullptr);

  void clear();
  bool is_empty() const;
  Q_SIGNAL void empty_changed();

  void add_message(const QString& log_level, const QString& message);
  void add_line(const QString& line);

  void set_quest_runner(QuestRunner* quest_runner);

  QPlainTextEdit* log_view();

signals:

  void setting_changed_in_quest(const QString& key, const QVariant& value);
  void command_result_received(int id, const QString& command, bool success, const QString& result);

private slots:

  void quest_running();
  void quest_finished();
  void quest_output_produced(const QStringList& lines);
  void quest_error(QuestRunner::ErrorCode error);

private:
  void setupUi();
  void parse_output(const QString& line);
  bool detect_command_result(const QString& log_level, const QString& message);
  void detect_setting_change(const QString& log_level, const QString& message);
  QString colorize_line(const QString& line);
  void update_ui_with_new_messages();

  struct {
    QPlainTextEdit* log_view{ nullptr };
  } ui; /**< The widgets. */
  QPointer<QuestRunner> quest_runner; /**< The quest execution. */
  QStringList raw_content; /**< Raw text before colorization. */
  int last_new_message_index; /**< Index of the last string from raw content that
                                        * was formatted and added to the console. */
  QMap<int, QString> pending_commands; /**< Commands for which we are waiting a result. */
  int output_command_id; /**< Id of the command we are reading the result of
                                        * (-1 if none). */
  QString output_command_result; /**< Partial result of the command. */
  Theme theme;
};
} // namespace solarus::launcher
