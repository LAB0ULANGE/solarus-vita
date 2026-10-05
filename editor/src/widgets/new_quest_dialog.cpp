/*
 * Copyright (C) 2020-2021 Christopho, Solarus - http://www.solarus-games.org
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
#include "widgets/new_quest_dialog.h"
#include "file_tools.h"
#include "editor_style.h"

#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QButtonGroup>
#include <QPainter>
#include <QApplication>
#include <QDir>
#include <QStandardPaths>

namespace SolarusEditor {

namespace {
/**
 * @brief Event filter used to draw the background of the bottom widget.
 */
class BottomWidgetPainter : public QObject {
  using QObject::QObject;

  bool eventFilter(QObject* obj, QEvent* event) override {
    if (event->type() == QEvent::Paint) {
      QWidget* widget = qobject_cast<QWidget*>(obj);
      const EditorStyle* style = qobject_cast<EditorStyle*>(widget->style());
      const QColor& bgColor = style ? style->theme().backgroundColorMain3 : widget->palette().base().color();
      QPainter p(widget);
      p.fillRect(widget->rect(), bgColor);
    }
    return false;
  }
};
}

/**
 * @brief Gets the error message that corresponds to the error.
 * @param error The quest path error.
 * @return The error message to display.
 */
static QString get_error_message(const FileTools::NewQuestPathError error) {

  switch (error) {
  case FileTools::NewQuestPathError::ParentDirDoesNotExist:
    return QApplication::translate("NewQuestDialog", "The parent directory does not exist.");
  case FileTools::NewQuestPathError::PathIsAfile:
    return QApplication::translate("NewQuestDialog", "This path is not a directory.");
  case FileTools::NewQuestPathError::AlreadyAQuest:
    return QApplication::translate("NewQuestDialog", "A quest already exists in this directory.");
  case FileTools::NewQuestPathError::PathNotAbsolute:
    return QApplication::translate("NewQuestDialog", "The path must be absolute.");
  case FileTools::NewQuestPathError::EmptyPath:
    return QApplication::translate("NewQuestDialog", "The path is empty.");
  case FileTools::NewQuestPathError::InvalidCharacters:
    return QApplication::translate("NewQuestDialog", "The path contains invalid chars.");
  default:
    return QString{};
  }
}

/**
 * @brief Constructor for the NewQuestDialog.
 * @param directory The default parent directory for the new quest.
 */
NewQuestDialog::NewQuestDialog(
    const QString& directory,
    QWidget* parent) :
    QDialog(parent), start_directory(directory) {

  ui.setupUi(this);
  ui.stacked_widget->setCurrentIndex(0);

  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  setWindowModality(Qt::WindowModality::ApplicationModal);
  setWindowFlag(Qt::WindowType::MSWindowsFixedSizeDialogHint, true);
  setWindowFlag(Qt::WindowType::WindowContextHelpButtonHint, false);
  setWindowFlag(Qt::WindowType::WindowMaximizeButtonHint, false);
  setWindowFlag(Qt::WindowType::WindowMinimizeButtonHint, false);
  setWindowFlag(Qt::WindowType::WindowFullscreenButtonHint, false);

  // Enable Qlementine's auto coloring.
  for (QWidget* widget : std::vector<QWidget*>{
        ui.cancel_button,
        ui.next_button,
        ui.previous_button,
        ui.quest_title_edit,
        ui.quest_path_directory_lineedit,
        ui.quest_contents_community_resources_radiobutton,
        ui.quest_contents_empty_quest_radiobutton,
        ui.quest_path_browse_button,
       }) {
    EditorStyle::setAutoIconColor(widget, EditorStyle::AutoIconColor::ForegroundColor);
  }

  // Draw a background for the bottom widget.
  ui.bottom_widget->installEventFilter(new BottomWidgetPainter(ui.bottom_widget));

  // Tweak the font sizes.
  if (EditorStyle* style = qobject_cast<EditorStyle*>(this->style())) {
    const QFont title_font = style->theme().fontH4;
    const std::vector<QWidget*> title_labels = std::vector<QWidget*>{
        ui.quest_title_title,
        ui.quest_path_title,
        ui.quest_contents_title,
    };
    for (QWidget* label : title_labels) {
      label->setFont(title_font);
    }

    const QFont bold_font = style->theme().fontH5;
    const std::vector<QWidget*> radiobuttons = std::vector<QWidget*>{
        ui.quest_contents_community_resources_radiobutton,
        ui.quest_contents_empty_quest_radiobutton,
    };
    for (QWidget* radiobutton : radiobuttons) {
      radiobutton->setFont(bold_font);
    }
  }

  // Shift the labels to align them with the QRadioButtons' contents.
  ui.quest_contents_community_resources_description->setContentsMargins(24, 0, 0, 0);
  ui.quest_contents_empty_quest_description->setContentsMargins(24, 0, 0, 0);

  // Make buttons having the same width: more esthetically pleasing and more convenient.
  int max_width = 0;
  const std::vector<QWidget*> bottom_buttons = std::vector<QWidget*>{
      ui.previous_button,
      ui.next_button,
      ui.cancel_button,
  };
  for (QWidget* button : bottom_buttons) {
    button->ensurePolished();
    max_width = std::max(max_width, button->sizeHint().width());
  }
  for (QWidget* button : bottom_buttons) {
    button->setMinimumWidth(max_width);
  }
  ui.bottom_widget->setFixedHeight(ui.bottom_widget->sizeHint().height());

  // Initialize the bottom buttons.
  update_page_buttons();

  // Prevent resizing the window.
  setFixedSize(sizeHint());

  // Connections for the widgets that modify the config..
  connect(ui.quest_title_edit, &QLineEdit::textChanged,
          this, &NewQuestDialog::on_quest_title_changed);

  connect(ui.quest_path_directory_lineedit, &QLineEdit::textChanged,
          this, &NewQuestDialog::on_quest_path_changed);
  connect(ui.quest_path_browse_button, &QPushButton::clicked,
          this, &NewQuestDialog::on_browse_button_clicked);

  connect(ui.quest_contents_community_resources_radiobutton, &QRadioButton::toggled,
          this, &NewQuestDialog::on_contents_mode_changed);
  connect(ui.quest_contents_empty_quest_radiobutton, &QRadioButton::toggled,
          this, &NewQuestDialog::on_contents_mode_changed);

  // Connect page buttons.
  QObject::connect(ui.next_button, &QPushButton::clicked, this, [this]() {
    const int current_index = ui.stacked_widget->currentIndex();

    // Warn the user if the chosen dir is not empty.
    bool user_confirm = true;
    if (current_index == 1) {
      user_confirm = confirm_non_empty_dir();
    }
    if (!user_confirm)
      return;

    update_config(current_index);
    if (current_index < ui.stacked_widget->count() -1) {
      const int next_index = current_index + 1;
      ui.stacked_widget->setCurrentIndex(next_index);
    } else {
      accept();
    }
  });

  QObject::connect(ui.previous_button, &QPushButton::clicked, this, [this]() {
    const int current_index = ui.stacked_widget->currentIndex();
    update_config(current_index);

    const int prev_index = std::max(0, ui.stacked_widget->currentIndex() - 1);
    ui.stacked_widget->setCurrentIndex(prev_index);
  });

  QObject::connect(ui.cancel_button, &QPushButton::clicked, this, [this]() {
    reject();
  });

  // Connect to page changes.
  QObject::connect(ui.stacked_widget, &QStackedWidget::currentChanged,
                   this, [this](int page_index) {
    initialize_from_config(page_index);
    update_page_buttons();
    update_next_button();
  });
}

/**
 * @brief Get the NewQuestConfig set by the user.
 */
const NewQuestBuilder::NewQuestConfig& NewQuestDialog::get_new_quest_config() const {

  return config;
}

/**
 * @brief Called when the page changes.
 */
void NewQuestDialog::update_page_buttons() {

  const int page_count = ui.stacked_widget->count();
  const int current_index = ui.stacked_widget->currentIndex();

  const bool is_first_page = current_index == 0;
  ui.previous_button->setEnabled(!is_first_page);
  ui.previous_button->setVisible(!is_first_page);

  // ui.next_button->setEnabled(page_count > 0);
  const bool is_last_page = current_index == page_count - 1;
  const QString next_text = is_last_page ? tr("OK") : tr("Next");
  const QIcon next_icon = is_last_page ? QIcon(":/images/icon_valid.svg") : QIcon(":/images/icon_next.svg");
  ui.next_button->setIcon(next_icon);
  ui.next_button->setText(next_text);
  ui.next_button->setEnabled(false);
}

/**
 * @brief Called when the quest title did change.
 */
void NewQuestDialog::on_quest_title_changed() {

  update_next_button();
}

/**
 * @brief Called when the quest path did change.
 */
void NewQuestDialog::on_quest_path_changed() {

  const QString path = ui.quest_path_directory_lineedit->text();
  const FileTools::NewQuestPathError path_error = FileTools::check_new_quest_path(path);
  const QString error_message = get_error_message(path_error);
  ui.quest_path_error_label->setText(error_message);
  ui.quest_path_error_widget->setVisible(path_error != FileTools::NewQuestPathError::NoError);

  update_next_button();
}

/**
 * @brief Called when the contents mode did change.
 */
void NewQuestDialog::on_contents_mode_changed() {

  update_next_button();
}

/**
 * @brief NewQuestDialog::update_next_button
 */
void NewQuestDialog::update_next_button() {

  const bool enabled = next_button_enabled(ui.stacked_widget->currentIndex());
  ui.next_button->setEnabled(enabled);
}

/**
 * @brief Stores the user choices in the config struct.
 * @param page_index The page to get information from.
 */
void NewQuestDialog::update_config(int page_index) {

  if (page_index == 0) {
    config.quest_name = ui.quest_title_edit->text();
    // Reset so the automatic naming will be re-triggered.
    config.quest_path.clear();
  }
  else if (page_index == 1) {
    config.quest_path = ui.quest_path_directory_lineedit->text();
  }
  else if (page_index == 2) {
    const NewQuestBuilder::NewQuestMode mode =
        ui.quest_contents_community_resources_radiobutton->isChecked()
            ? NewQuestBuilder::NewQuestMode::COPY_INITIAL_QUEST
            : NewQuestBuilder::NewQuestMode::BLANK_QUEST;
    config.mode = mode;
  }
}

/**
 * @brief Initalizes the page from the config struct.
 * @param page_index The page to initialize.
 */
void NewQuestDialog::initialize_from_config(int page_index) {

  if (page_index == 0) {
    ui.quest_title_edit->setText(config.quest_name);
  }
  else if (page_index == 1) {
    if (config.quest_path.isEmpty()) {
      const QDir parent_dir = QDir(start_directory);
      const QString quest_file = FileTools::to_file_name(config.quest_name);
      const QString quest_path = parent_dir.absoluteFilePath(quest_file);
      ui.quest_path_directory_lineedit->setText(quest_path);
    } else {
      ui.quest_path_directory_lineedit->setText(config.quest_path);
    }
  }
  else if (page_index == 2) {
    ui.quest_contents_community_resources_radiobutton->setFocusPolicy(Qt::StrongFocus);
    ui.quest_contents_empty_quest_radiobutton->setFocusPolicy(Qt::StrongFocus);

    ui.quest_contents_community_resources_radiobutton->setChecked(config.mode == NewQuestBuilder::NewQuestMode::COPY_INITIAL_QUEST);
    ui.quest_contents_empty_quest_radiobutton->setChecked(config.mode == NewQuestBuilder::NewQuestMode::BLANK_QUEST);

    QWidget* focus_widget = ui.quest_contents_community_resources_radiobutton->isChecked()
        ? ui.quest_contents_community_resources_radiobutton
        : ui.quest_contents_empty_quest_radiobutton;
    focus_widget->setFocus(Qt::FocusReason::OtherFocusReason);
  }
}

/**
 * @brief Returns if the page data is valid.
 * @param page_index The page to check validity.
 * @return true if the page data is valid, false otherwise.
 */
bool NewQuestDialog::next_button_enabled(int page_index) const {

  if (page_index == 0) {
    return !ui.quest_title_edit->text().isEmpty();
  }
  else if (page_index == 1) {
    return !ui.quest_path_error_widget->isVisible();
  }
  else if (page_index == 2) {
    return ui.quest_contents_community_resources_radiobutton->isChecked()
    || ui.quest_contents_empty_quest_radiobutton->isChecked();
  }

  return false;
}

/**
 * @brief Opens a dialog to ask the user to choose a directory.
 */
void NewQuestDialog::on_browse_button_clicked() {

  const QFileDialog::Options mode = QFileDialog::Options(
#ifdef SOLARUSEDITOR_NO_NATIVE_DIALOGS
      QFileDialog::Option::ShowDirsOnly | QFileDialog::Option::DontUseNativeDialog
#else
      QFileDialog::Option::ShowDirsOnly
#endif
  );

  QString current_path = ui.quest_path_directory_lineedit->text();
  current_path = current_path.isEmpty() ? start_directory : current_path;
  current_path = FileTools::cd_up_until_exists(current_path);

  const QString& dialog_start_path = current_path.isEmpty()
    ? QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
    : current_path;

  const QString path = QFileDialog::getExistingDirectory(
      this,
      tr("Select quest directory"),
      dialog_start_path,
      mode);

  if (path.isEmpty()) {
    return;
  }

  ui.quest_path_directory_lineedit->setText(path);
}

/**
 * @brief Asks the user if it is OK to use a non-empty dir.
 * @return true if the user accepts.
 */
bool NewQuestDialog::confirm_non_empty_dir() const {

  const QDir quest_directory = QDir(ui.quest_path_directory_lineedit->text());
  if (quest_directory.exists() && !quest_directory.isEmpty()) {
    QMessageBox confirm(
      QMessageBox::Warning,
      tr("Directory not empty"),
      tr("The quest directory is not empty, are you sure you wish to continue?"),
      QMessageBox::Ok | QMessageBox::Cancel,
      nullptr
    );
    int standard_button = confirm.exec();
    return (standard_button == QMessageBox::Ok);
  }
  return true;
}

} // namespace SolarusEditor
