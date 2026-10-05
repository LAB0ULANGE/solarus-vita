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
#include "widgets/quest_properties_editor.h"
#include "widgets/gui_tools.h"
#include "quest.h"
#include <QRegularExpressionValidator>
#include <QUndoStack>
#include <QButtonGroup>

namespace SolarusEditor {

namespace {

/**
 * @brief Parent class of all undoable commands of the quest properties editor.
 */
class QuestPropertiesEditorCommand : public QUndoCommand {

public:

  QuestPropertiesEditorCommand(
      QuestPropertiesEditor& editor, const QString& text) :
    QUndoCommand(text),
    editor(editor) {
  }

  QuestPropertiesEditor& get_editor() const {
    return editor;
  }

  QuestProperties& get_model() const {
    return editor.get_model();
  }

private:

  QuestPropertiesEditor& editor;

};

/**
 * @brief Change write directory.
 */
class SetWriteDirCommand : public QuestPropertiesEditorCommand {

public:

  SetWriteDirCommand(QuestPropertiesEditor& editor, const QString& write_dir) :
    QuestPropertiesEditorCommand(
      editor, QuestPropertiesEditor::tr("Change write directory")),
    write_dir_before(get_model().get_write_dir()),
    write_dir_after(write_dir) {
  }

  virtual void undo() override {

    get_model().set_write_dir(write_dir_before);
  }

  virtual void redo() override {

    get_model().set_write_dir(write_dir_after);
  }

private:

  QString write_dir_before;
  QString write_dir_after;
};

/**
 * @brief Change title.
 */
class SetTitleCommand : public QuestPropertiesEditorCommand {

public:

  SetTitleCommand(QuestPropertiesEditor& editor, const QString& title) :
    QuestPropertiesEditorCommand(
      editor, QuestPropertiesEditor::tr("Change title")),
    title_before(get_model().get_title()),
    title_after(title) {
  }

  virtual void undo() override {

    get_model().set_title(title_before);
  }

  virtual void redo() override {

    get_model().set_title(title_after);
  }

private:

  QString title_before;
  QString title_after;
};

/**
 * @brief Changing the short description.
 */
class SetShortDescriptionCommand : public QuestPropertiesEditorCommand {

public:
  SetShortDescriptionCommand(QuestPropertiesEditor& editor, const QString& short_description) :
    QuestPropertiesEditorCommand(
      editor, QuestPropertiesEditor::tr("Change summary")),
    short_description_before(get_model().get_short_description()),
    short_description_after(short_description) {
  }

  void undo() override {
    get_model().set_short_description(short_description_before);
  }

  void redo() override {
    get_model().set_short_description(short_description_after);
  }

private:
  QString short_description_before;
  QString short_description_after;
};

/**
 * @brief Changing the long description.
 */
class SetLongDescriptionCommand : public QuestPropertiesEditorCommand {

public:
  SetLongDescriptionCommand(QuestPropertiesEditor& editor, const QString& long_description) :
    QuestPropertiesEditorCommand(
      editor, QuestPropertiesEditor::tr("Change detailed description")),
    long_description_before(get_model().get_long_description()),
    long_description_after(long_description) {
  }

  void undo() override {
    get_model().set_long_description(long_description_before);
  }

  void redo() override {
    get_model().set_long_description(long_description_after);
  }

private:
  QString long_description_before;
  QString long_description_after;
};

/**
 * @brief Changing the author.
 */
class SetAuthorCommand : public QuestPropertiesEditorCommand {

public:
  SetAuthorCommand(QuestPropertiesEditor& editor, const QString& author) :
    QuestPropertiesEditorCommand(
      editor, QuestPropertiesEditor::tr("Change author")),
    author_before(get_model().get_author()),
    author_after(author) {
  }

  void undo() override {
    get_model().set_author(author_before);
  }

  void redo() override {
    get_model().set_author(author_after);
  }

private:
  QString author_before;
  QString author_after;
};

/**
 * @brief Changing the quest version.
 */
class SetQuestVersionCommand : public QuestPropertiesEditorCommand {

public:
  SetQuestVersionCommand(QuestPropertiesEditor& editor, const QString& quest_version) :
    QuestPropertiesEditorCommand(
      editor, QuestPropertiesEditor::tr("Change quest version")),
    quest_version_before(get_model().get_quest_version()),
    quest_version_after(quest_version) {
  }

  void undo() override {
    get_model().set_quest_version(quest_version_before);
  }

  void redo() override {
    get_model().set_quest_version(quest_version_after);
  }

private:
  QString quest_version_before;
  QString quest_version_after;
};

/**
 * @brief Changing the release date.
 */
class SetReleaseDateCommand : public QuestPropertiesEditorCommand {

public:
  SetReleaseDateCommand(QuestPropertiesEditor& editor, const QDate& release_date) :
    QuestPropertiesEditorCommand(
      editor, QuestPropertiesEditor::tr("Change release date")),
    release_date_before(get_model().get_release_date()),
    release_date_after(release_date) {
  }

  void undo() override {
    get_model().set_release_date(release_date_before);
  }

  void redo() override {
    get_model().set_release_date(release_date_after);
  }

private:
  QDate release_date_before;
  QDate release_date_after;
};

/**
 * @brief Changing the website.
 */
class SetWebsiteCommand : public QuestPropertiesEditorCommand {

public:
  SetWebsiteCommand(QuestPropertiesEditor& editor, const QString& website) :
    QuestPropertiesEditorCommand(
      editor, QuestPropertiesEditor::tr("Change website")),
    website_before(get_model().get_website()),
    website_after(website) {
  }

  void undo() override {
    get_model().set_website(website_before);
  }

  void redo() override {
    get_model().set_website(website_after);
  }

private:
  QString website_before;
  QString website_after;
};

/**
 * @brief Change normal size.
 */
class SetNormalSizeCommand : public QuestPropertiesEditorCommand {

public:

  SetNormalSizeCommand(QuestPropertiesEditor& editor, const QSize& size) :
    QuestPropertiesEditorCommand(
      editor, QuestPropertiesEditor::tr("Change normal size")),
    size_before(get_model().get_normal_quest_size()),
    size_after(size) {
  }

  virtual void undo() override {

    get_model().set_normal_quest_size(size_before);
  }

  virtual void redo() override {

    get_model().set_normal_quest_size(size_after);
  }

private:

  QSize size_before;
  QSize size_after;
};

/**
 * @brief Change minimum size.
 */
class SetMinSizeCommand : public QuestPropertiesEditorCommand {

public:

  SetMinSizeCommand(QuestPropertiesEditor& editor, const QSize& size) :
    QuestPropertiesEditorCommand(
      editor, QuestPropertiesEditor::tr("Change minimum size")),
    normal_size_before(get_model().get_normal_quest_size()),
    min_size_before(get_model().get_min_quest_size()),
    min_size_after(size) {

    // Normal size after.
    normal_size_after = normal_size_before;
    if (normal_size_after.width() < min_size_after.width()) {
      normal_size_after.setWidth(min_size_after.width());
    }
    if (normal_size_after.height() < min_size_after.height()) {
      normal_size_after.setHeight(min_size_after.height());
    }
  }

  virtual void undo() override {

    get_model().set_min_quest_size(min_size_before);
    get_model().set_normal_quest_size(normal_size_before);
  }

  virtual void redo() override {

    get_model().set_min_quest_size(min_size_after);
    get_model().set_normal_quest_size(normal_size_after);
  }

private:

  QSize normal_size_before;
  QSize normal_size_after;
  QSize min_size_before;
  QSize min_size_after;
};

/**
 * @brief Change maximum size.
 */
class SetMaxSizeCommand : public QuestPropertiesEditorCommand {

public:

  SetMaxSizeCommand(QuestPropertiesEditor& editor, const QSize& size) :
    QuestPropertiesEditorCommand(
      editor, QuestPropertiesEditor::tr("Change maximum size")),
    normal_size_before(get_model().get_normal_quest_size()),
    max_size_before(get_model().get_max_quest_size()),
    max_size_after(size) {

    // Normal size after.
    normal_size_after = normal_size_before;
    if (normal_size_after.width() > max_size_after.width()) {
      normal_size_after.setWidth(max_size_after.width());
    }
    if (normal_size_after.height() > max_size_after.height()) {
      normal_size_after.setHeight(max_size_after.height());
    }
  }

  virtual void undo() override {

    get_model().set_max_quest_size(max_size_before);
    get_model().set_normal_quest_size(normal_size_before);
  }

  virtual void redo() override {

    get_model().set_max_quest_size(max_size_after);
    get_model().set_normal_quest_size(normal_size_after);
  }

private:

  QSize normal_size_before;
  QSize normal_size_after;
  QSize max_size_before;
  QSize max_size_after;
};

/**
 * @brief Change subpixel camera
 */
class SetSubpixelCameraCommand : public QuestPropertiesEditorCommand {

  public:

  SetSubpixelCameraCommand(QuestPropertiesEditor& editor, bool enabled) :
      QuestPropertiesEditorCommand(
          editor, QuestPropertiesEditor::tr("Set subpixel camera")),
      enabled_before(get_model().is_subpixel_camera()),
      enabled_after(enabled) {
  }

  virtual void undo() override {
    get_model().set_subpixel_camera(enabled_before);
  }

  virtual void redo() override {
    get_model().set_subpixel_camera(enabled_after);
  }

  private:
  bool enabled_before;
  bool enabled_after;
};

/**
 * @brief Change dynamic timestep
 */
class SetDynamicTimestepCommand : public QuestPropertiesEditorCommand {

  public:

  SetDynamicTimestepCommand(QuestPropertiesEditor& editor, bool enabled) :
      QuestPropertiesEditorCommand(
          editor, QuestPropertiesEditor::tr("Set dynamic timestep")),
      enabled_before(get_model().is_dynamic_timestep()),
      enabled_after(enabled) {
  }

  virtual void undo() override {
    get_model().set_dynamic_timestep(enabled_before);
  }

  virtual void redo() override {
    get_model().set_dynamic_timestep(enabled_after);
  }

  private:
  bool enabled_before;
  bool enabled_after;
};

/**
 * @brief Changing the initial release date.
 */
class SetInitialReleaseDateCommand : public QuestPropertiesEditorCommand {

public:

  SetInitialReleaseDateCommand(QuestPropertiesEditor& editor, const QDate& initial_release_date) :
      QuestPropertiesEditorCommand(
          editor, QuestPropertiesEditor::tr("Change initial release date")),
      initial_release_date_before(get_model().get_initial_release_date()),
      initial_release_date_after(initial_release_date) {
  }

  void undo() override {
    get_model().set_initial_release_date(initial_release_date_before);
  }

  void redo() override {
    get_model().set_initial_release_date(initial_release_date_after);
  }

private:

  QDate initial_release_date_before;
  QDate initial_release_date_after;
};

/**
 * @brief Changing the license.
 */
class SetLicenseCommand : public QuestPropertiesEditorCommand {

public:

  SetLicenseCommand(QuestPropertiesEditor& editor, const QString& license) :
      QuestPropertiesEditorCommand(
          editor, QuestPropertiesEditor::tr("Change license")),
      license_before(get_model().get_license()),
      license_after(license) {
  }

  virtual void undo() override {

    get_model().set_license(license_before);
  }

  virtual void redo() override {

    get_model().set_license(license_after);
  }

private:

  QString license_before;
  QString license_after;
};

/**
 * @brief Changing the min players.
 */
class SetMinPlayersCommand : public QuestPropertiesEditorCommand {

public:

  SetMinPlayersCommand(QuestPropertiesEditor& editor, int min_players) :
      QuestPropertiesEditorCommand(
          editor, QuestPropertiesEditor::tr("Change maximum players")),
      min_players_before(get_model().get_min_players()),
      min_players_after(min_players) {
  }

  virtual void undo() override {

    get_model().set_min_players(min_players_before);
  }

  virtual void redo() override {

    get_model().set_min_players(min_players_after);
  }

private:

  int min_players_before;
  int min_players_after;
};

/**
 * @brief Changing the max players.
 */
class SetMaxPlayersCommand : public QuestPropertiesEditorCommand {

  public:

  SetMaxPlayersCommand(QuestPropertiesEditor& editor, int max_players) :
      QuestPropertiesEditorCommand(
          editor, QuestPropertiesEditor::tr("Change maximum players")),
      max_players_before(get_model().get_max_players()),
      max_players_after(max_players) {
  }

  virtual void undo() override {

    get_model().set_max_players(max_players_before);
  }

  virtual void redo() override {

    get_model().set_max_players(max_players_after);
  }

  private:

  int max_players_before;
  int max_players_after;
};

/**
 * @brief Changing the langauges.
 */
class SetLanguagesCommand : public QuestPropertiesEditorCommand {

  public:

  SetLanguagesCommand(QuestPropertiesEditor& editor, const QStringList& languages) :
      QuestPropertiesEditorCommand(
          editor, QuestPropertiesEditor::tr("Change languages")),
      languages_before(get_model().get_license()),
      languages_after(languages) {
  }

  virtual void undo() override {

    get_model().set_languages(languages_before);
  }

  virtual void redo() override {

    get_model().set_languages(languages_after);
  }

  private:

  QStringList languages_before;
  QStringList languages_after;
};

/**
 * @brief Changing the genres.
 */
class SetGenresCommand : public QuestPropertiesEditorCommand {

  public:

  SetGenresCommand(QuestPropertiesEditor& editor, const QStringList& genres) :
      QuestPropertiesEditorCommand(
          editor, QuestPropertiesEditor::tr("Change genres")),
      genres_before(get_model().get_genres()),
      genres_after(genres) {
  }

  virtual void undo() override {

    get_model().set_genres(genres_before);
  }

  virtual void redo() override {

    get_model().set_genres(genres_after);
  }

  private:

  QStringList genres_before;
  QStringList genres_after;
};

/**
 * @brief Splits the QString into elements separated by a commo. Spaces are ignored.
 * @param str The string to split.
 * @return The QStringList with the elements.
 */
QStringList split_string_with_comma(const QString& str) {
  static const QRegularExpression re(R"(\s*,\s*)");
  const QString text = str.simplified();
  const QStringList list = text.split(re, Qt::SkipEmptyParts);
  return list;
}

} // anonymous namespace

/**
 * @brief Creates a quest properties editor.
 * @param quest The quest.
 * @param parent Parent object or nullptr.
 */
QuestPropertiesEditor::QuestPropertiesEditor(Quest& quest, QWidget* parent) :
  Editor(quest, quest.get_properties_path(), parent),
  model(quest) {

  ui.setupUi(this);
  ui.initial_release_date_field->setDate(QDate::currentDate());
  ui.release_date_field->setDate(QDate::currentDate());

  QButtonGroup* release_date_button_group = new QButtonGroup(this);
  release_date_button_group->setExclusive(true);
  release_date_button_group->addButton(ui.release_status_progress_radio);
  release_date_button_group->addButton(ui.release_status_released_radio);

  QButtonGroup* initial_release_date_button_group = new QButtonGroup(this);
  initial_release_date_button_group->setExclusive(true);
  initial_release_date_button_group->addButton(ui.initial_release_status_progress_radio);
  initial_release_date_button_group->addButton(ui.initial_release_status_released_radio);

  // Don't allow slashes or backslashes in the write dir field.
  ui.write_dir_field->setValidator(
        new QRegularExpressionValidator(QRegularExpression(R"(^[^/\\]*$)"), this));

  update();

  // Editor properties.
  set_title(tr("Quest properties"));
  set_icon(QIcon(":/images/icon_solarus.svg"));
  set_close_confirm_message(
        tr("Quest properties have been modified. Save changes?"));

  get_undo_stack().setClean();

  connect(&model, SIGNAL(write_dir_changed(QString)),
          this, SLOT(update_write_dir_field()));
  connect(ui.write_dir_field, SIGNAL(editingFinished()),
          this, SLOT(change_write_dir_requested()));

  connect(&model, SIGNAL(title_changed(QString)),
          this, SLOT(update_title_field()));
  connect(ui.title_field, SIGNAL(editingFinished()),
          this, SLOT(change_title_requested()));

  connect(&model, SIGNAL(short_description_changed(QString)),
          this, SLOT(update_short_description_field()));
  connect(ui.short_description_field, SIGNAL(editingFinished()),
          this, SLOT(change_short_description_requested()));

  connect(&model, SIGNAL(long_description_changed(QString)),
          this, SLOT(update_long_description_field()));
  connect(ui.long_description_field, SIGNAL(editing_finished()),
          this, SLOT(change_long_description_requested()));

  connect(&model, SIGNAL(author_changed(QString)),
          this, SLOT(update_author_field()));
  connect(ui.author_field, SIGNAL(editingFinished()),
          this, SLOT(change_author_requested()));

  connect(&model, SIGNAL(quest_version_changed(QString)),
          this, SLOT(update_quest_version_field()));
  connect(ui.quest_version_field, SIGNAL(editingFinished()),
          this, SLOT(change_quest_version_requested()));

  connect(&model, SIGNAL(release_date_changed(QDate)),
          this, SLOT(update_release_date_field()));
  connect(ui.release_status_progress_radio, SIGNAL(clicked()),
          this, SLOT(change_release_date_requested()));
  connect(ui.release_status_released_radio, SIGNAL(clicked()),
          this, SLOT(change_release_date_requested()));
  connect(ui.release_date_field, SIGNAL(dateChanged(QDate)),
          this, SLOT(change_release_date_requested()));

  connect(&model, SIGNAL(website_changed(QString)),
          this, SLOT(update_website_field()));
  connect(ui.website_field, SIGNAL(editingFinished()),
          this, SLOT(change_website_requested()));

  connect(&model, SIGNAL(normal_size_changed(QSize)),
          this, SLOT(update_normal_size_field()));
  connect(ui.normal_size_width_field, SIGNAL(editingFinished()),
          this, SLOT(change_normal_size_requested()));
  connect(ui.normal_size_height_field, SIGNAL(editingFinished()),
          this, SLOT(change_normal_size_requested()));

  connect(&model, SIGNAL(min_size_changed(QSize)),
          this, SLOT(update_min_size_field()));
  connect(ui.min_size_width_field, SIGNAL(editingFinished()),
          this, SLOT(change_min_size_requested()));
  connect(ui.min_size_height_field, SIGNAL(editingFinished()),
          this, SLOT(change_min_size_requested()));

  connect(&model, SIGNAL(max_size_changed(QSize)),
          this, SLOT(update_max_size_field()));
  connect(ui.max_size_width_field, SIGNAL(editingFinished()),
          this, SLOT(change_max_size_requested()));
  connect(ui.max_size_height_field, SIGNAL(editingFinished()),
          this, SLOT(change_max_size_requested()));

  connect(&model, SIGNAL(subpixel_camera_changed(bool)),
          this, SLOT(update_subpixel_camera_field()));
  connect(ui.subpixel_camera, SIGNAL(clicked(bool)),
          this, SLOT(change_subpixel_camera_requested()));

  connect(&model, SIGNAL(dynamic_timestep_changed(bool)),
          this, SLOT(update_dynamic_timestep_field()));
  connect(ui.dynamic_timestep, SIGNAL(clicked(bool)),
          this, SLOT(change_dynamic_timestep_requested()));

  connect(&model, &QuestProperties::initial_release_date_changed,
          this, &QuestPropertiesEditor::update_initial_release_date_field);
  connect(ui.initial_release_status_progress_radio, &QRadioButton::clicked,
          this, &QuestPropertiesEditor::change_initial_release_date_requested);
  connect(ui.initial_release_status_released_radio, &QRadioButton::clicked,
          this, &QuestPropertiesEditor::change_initial_release_date_requested);
  connect(ui.initial_release_date_field, &QDateEdit::dateChanged,
          this, &QuestPropertiesEditor::change_initial_release_date_requested);

  connect(&model, &QuestProperties::license_changed,
          this, &QuestPropertiesEditor::update_license_field);
  connect(ui.license_field, &QLineEdit::editingFinished,
          this, &QuestPropertiesEditor::change_license_requested);

  connect(&model, &QuestProperties::languages_changed,
          this, &QuestPropertiesEditor::update_language_field);
  connect(ui.languages_field, &QLineEdit::editingFinished,
          this, &QuestPropertiesEditor::change_language_requested);

  connect(&model, &QuestProperties::min_players_changed,
          this, &QuestPropertiesEditor::update_min_players_field);
  connect(ui.min_players_field, &QSpinBox::editingFinished,
          this, &QuestPropertiesEditor::change_min_players_requested);

  connect(&model, &QuestProperties::max_players_changed,
          this, &QuestPropertiesEditor::update_max_players_field);
  connect(ui.max_players_field, &QSpinBox::editingFinished,
          this, &QuestPropertiesEditor::change_max_players_requested);

  connect(&model, &QuestProperties::genres_changed,
          this, &QuestPropertiesEditor::update_genres_field);
  connect(ui.genres_field, &QLineEdit::editingFinished,
          this, &QuestPropertiesEditor::change_genres_requested);
}

/**
 * @brief Returns the model.
 * @return The model.
 */
QuestProperties& QuestPropertiesEditor::get_model() {

  return model;
}

/**
 * @brief Saves the properties file.
 */
void QuestPropertiesEditor::save() {

  model.save();
}

/**
 * @brief Update field values.
 */
void QuestPropertiesEditor::update() {

  ui.solarus_version_value->setText(model.get_solarus_version());

  update_write_dir_field();
  update_title_field();
  update_short_description_field();
  update_long_description_field();
  update_author_field();
  update_quest_version_field();
  update_release_date_field();
  update_website_field();
  update_normal_size_field();
  update_min_size_field();
  update_max_size_field();
  update_subpixel_camera_field();
  update_dynamic_timestep_field();
  update_initial_release_date_field();
  update_license_field();
  update_language_field();
  update_min_players_field();
  update_max_players_field();
  update_genres_field();
}

/**
 * @brief Update the write directory field.
 */
void QuestPropertiesEditor::update_write_dir_field() {

  ui.write_dir_field->setText(model.get_write_dir());
}

/**
 * @brief Slot called when the user change the write directory.
 */
void QuestPropertiesEditor::change_write_dir_requested() {

  QString write_dir = ui.write_dir_field->text();
  QString old_write_dir = model.get_write_dir();
  if (write_dir == old_write_dir) {
    // No change.
    return;
  }

  try_command(new SetWriteDirCommand(*this, write_dir));
}

/**
 * @brief Updates the title field.
 */
void QuestPropertiesEditor::update_title_field() {

  ui.title_field->setText(model.get_title());
}

/**
 * @brief Slot called when the user changes the title.
 */
void QuestPropertiesEditor::change_title_requested() {

  QString title = ui.title_field->text();
  QString old_title = model.get_title();
  if (title == old_title) {
    // No change.
    return;
  }

  try_command(new SetTitleCommand(*this, title));
}

/**
 * @brief Updates the short description field.
 */
void QuestPropertiesEditor::update_short_description_field() {

  ui.short_description_field->setText(model.get_short_description());
}

/**
 * @brief Slot called when the user changes the short description.
 */
void QuestPropertiesEditor::change_short_description_requested() {

  QString short_description = ui.short_description_field->text();
  QString old_short_description = model.get_short_description();
  if (short_description == old_short_description) {
    // No change.
    return;
  }

  try_command(new SetShortDescriptionCommand(*this, short_description));
}

/**
 * @brief Updates the long description field.
 */
void QuestPropertiesEditor::update_long_description_field() {

  ui.long_description_field->setPlainText(model.get_long_description());
}

/**
 * @brief Slot called when the user changes the long description.
 */
void QuestPropertiesEditor::change_long_description_requested() {

  QString long_description = ui.long_description_field->toPlainText();
  QString old_long_description = model.get_long_description();
  if (long_description == old_long_description) {
    // No change.
    return;
  }

  try_command(new SetLongDescriptionCommand(*this, long_description));
}

/**
 * @brief Updates the author field.
 */
void QuestPropertiesEditor::update_author_field() {

  ui.author_field->setText(model.get_author());
}

/**
 * @brief Slot called when the user changes the author.
 */
void QuestPropertiesEditor::change_author_requested() {

  QString author = ui.author_field->text();
  QString old_author = model.get_author();
  if (author == old_author) {
    // No change.
    return;
  }

  try_command(new SetAuthorCommand(*this, author));
}

/**
 * @brief Updates the quest version field.
 */
void QuestPropertiesEditor::update_quest_version_field() {

  ui.quest_version_field->setText(model.get_quest_version());
}

/**
 * @brief Slot called when the user changes the quest version.
 */
void QuestPropertiesEditor::change_quest_version_requested() {

  QString quest_version = ui.quest_version_field->text();
  QString old_quest_version = model.get_quest_version();
  if (quest_version == old_quest_version) {
    // No change.
    return;
  }

  try_command(new SetQuestVersionCommand(*this, quest_version));
}

/**
 * @brief Updates the short release date field.
 */
void QuestPropertiesEditor::update_release_date_field() {

  QDate release_date = model.get_release_date();
  if (!release_date.isValid()) {
    ui.release_status_progress_radio->setChecked(true);
    ui.release_date_field->setEnabled(false);
  }
  else {
    ui.release_status_released_radio->setChecked(true);
    ui.release_date_field->setEnabled(true);
    ui.release_date_field->setDate(release_date);
  }
}

/**
 * @brief Slot called when the user changes the release date.
 */
void QuestPropertiesEditor::change_release_date_requested() {

  QDate release_date;
  if (ui.release_status_released_radio->isChecked()) {
    release_date = ui.release_date_field->date();
  }
  QDate old_release_date = model.get_release_date();
  if (release_date == old_release_date) {
    return;
  }

  try_command(new SetReleaseDateCommand(*this, release_date));
}

/**
 * @brief Updates the website field.
 */
void QuestPropertiesEditor::update_website_field() {

  ui.website_field->setText(model.get_website());
}

/**
 * @brief Slot called when the user changes the website.
 */
void QuestPropertiesEditor::change_website_requested() {

  QString website = ui.website_field->text();
  QString old_website = model.get_website();
  if (website == old_website) {
    // No change.
    return;
  }

  try_command(new SetWebsiteCommand(*this, website));
}

/**
 * @brief Update the normal size field.
 */
void QuestPropertiesEditor::update_normal_size_field() {

  QSize size = model.get_normal_quest_size();
  ui.normal_size_width_field->setValue(size.width());
  ui.normal_size_height_field->setValue(size.height());
}

/**
 * @brief Slot called when the user change the normal size.
 */
void QuestPropertiesEditor::change_normal_size_requested() {

  QSize size(ui.normal_size_width_field->value(),
             ui.normal_size_height_field->value());
  QSize old_size = model.get_normal_quest_size();
  if (size == old_size) {
    // No change.
    return;
  }

  try_command(new SetNormalSizeCommand(*this, size));
}

/**
 * @brief Update the minimum size field.
 */
void QuestPropertiesEditor::update_min_size_field() {

  QSize size = model.get_min_quest_size();
  ui.min_size_width_field->setValue(size.width());
  ui.min_size_height_field->setValue(size.height());

  set_min_size_values();
}

/**
 * @brief Slot called when the user change the minimum size.
 */
void QuestPropertiesEditor::change_min_size_requested() {

  QSize size(ui.min_size_width_field->value(),
             ui.min_size_height_field->value());
  QSize old_size = model.get_min_quest_size();
  if (size == old_size) {
    // No change.
    return;
  }

  try_command(new SetMinSizeCommand(*this, size));
}

/**
 * @brief Update the maximum size field.
 */
void QuestPropertiesEditor::update_max_size_field() {

  QSize size = model.get_max_quest_size();
  ui.max_size_width_field->setValue(size.width());
  ui.max_size_height_field->setValue(size.height());

  set_max_size_values();
}

/**
 * @brief Slot called when the user change the maximum size.
 */
void QuestPropertiesEditor::change_max_size_requested() {

  QSize size(ui.max_size_width_field->value(),
             ui.max_size_height_field->value());
  QSize old_size = model.get_max_quest_size();
  if (size == old_size) {
    // No change.
    return;
  }

  try_command(new SetMaxSizeCommand(*this, size));
}

/**
 * @brief Update the maximum size field.
 */
void QuestPropertiesEditor::update_subpixel_camera_field() {
  auto enabled = model.is_subpixel_camera();
  ui.subpixel_camera->setChecked(enabled);
}

/**
 * @brief Slot called when the user change the maximum size.
 */
void QuestPropertiesEditor::change_subpixel_camera_requested() {
  auto enabled = ui.subpixel_camera->isChecked();
  try_command(new SetSubpixelCameraCommand(*this, enabled));
}

/**
 * @brief Update the maximum size field.
 */
void QuestPropertiesEditor::update_dynamic_timestep_field() {
  auto enabled = model.is_dynamic_timestep();
  ui.dynamic_timestep->setChecked(enabled);
}

/**
 * @brief Slot called when the user change the maximum size.
 */
void QuestPropertiesEditor::change_dynamic_timestep_requested() {
  auto enabled = ui.dynamic_timestep->isChecked();
  try_command(new SetDynamicTimestepCommand(*this, enabled));
}

/**
 * @brief Changes the minimum values of normal and maximum size fields.
 */
void QuestPropertiesEditor::set_min_size_values() {

  QSize size(ui.min_size_width_field->value(),
              ui.min_size_height_field->value());

  ui.normal_size_width_field->setMinimum(size.width());
  ui.normal_size_height_field->setMinimum(size.height());

  ui.max_size_width_field->setMinimum(size.width());
  ui.max_size_height_field->setMinimum(size.height());
}

/**
 * @brief Changes the maximum values of normal and minimum size fields.
 */
void QuestPropertiesEditor::set_max_size_values() {

  QSize size(ui.max_size_width_field->value(),
             ui.max_size_height_field->value());

  ui.normal_size_width_field->setMaximum(size.width());
  ui.normal_size_height_field->setMaximum(size.height());

  ui.min_size_width_field->setMaximum(size.width());
  ui.min_size_height_field->setMaximum(size.height());
}

/**
 * @brief Updates the initial relase date.
 */
void QuestPropertiesEditor::update_initial_release_date_field() {

  QDate date = model.get_initial_release_date();
  if (!date.isValid()) {
    ui.initial_release_status_progress_radio->setChecked(true);
    ui.initial_release_date_field->setEnabled(false);
  }
  else {
    ui.initial_release_status_released_radio->setChecked(true);
    ui.initial_release_date_field->setEnabled(true);
    ui.initial_release_date_field->setDate(date);
  }
}

/**
 * @brief Slot called when the user changes the initial relase date.
 */
void QuestPropertiesEditor::change_initial_release_date_requested() {

  QDate initial_release_date;
  if (ui.initial_release_status_released_radio->isChecked()) {
    initial_release_date = ui.initial_release_date_field->date();
  }
  QDate old_initial_release_date = model.get_initial_release_date();
  if (initial_release_date == old_initial_release_date) {
    return;
  }

  try_command(new SetInitialReleaseDateCommand(*this, initial_release_date));
} 

/**
 * @brief Updates the license.
 */
void QuestPropertiesEditor::update_license_field() {

  ui.license_field->setText(model.get_license());
}

/**
 * @brief Slot called when the user changes the license.
 */
void QuestPropertiesEditor::change_license_requested() {

  const QString value = ui.license_field->text();
  try_command(new SetLicenseCommand(*this, value));
}

/**
 * @brief Updates the languages.
 */
void QuestPropertiesEditor::update_language_field() {

  ui.languages_field->setText(model.get_languages().join(','));
} 

/**
 * @brief Slot called when the user changes the languages.
 */
void QuestPropertiesEditor::change_language_requested() {

  const QStringList value = split_string_with_comma(ui.languages_field->text());
  try_command(new SetLanguagesCommand(*this, value));
}

/**
 * @brief Updates the min players.
 */
void QuestPropertiesEditor::update_min_players_field() {

  ui.min_players_field->setValue(model.get_min_players());
} 

/**
 * @brief Slot called when the user changes the min players.
 */
void QuestPropertiesEditor::change_min_players_requested() {

  const int min_value = ui.min_players_field->value();
  try_command(new SetMinPlayersCommand(*this, min_value));

  const int max_value = ui.max_players_field->value();
  if (min_value > max_value) {
    try_command(new SetMaxPlayersCommand(*this, min_value));
  }
}

/**
 * @brief Updates the max players.
 */
void QuestPropertiesEditor::update_max_players_field() {

  ui.max_players_field->setValue(model.get_max_players());
} 

/**
 * @brief Slot called when the user changes the max players.
 */
void QuestPropertiesEditor::change_max_players_requested() {

  const int max_value = ui.max_players_field->value();
  try_command(new SetMaxPlayersCommand(*this, max_value));

  const int min_value = ui.min_players_field->value();
  if (min_value > max_value) {
    try_command(new SetMinPlayersCommand(*this, max_value));
  }
}

/**
 * @brief Updates the genre date.
 */
void QuestPropertiesEditor::update_genres_field() {

  ui.genres_field->setText(model.get_genres().join(','));
}

/**
 * @brief Slot called when the user changes the genres.
 */
void QuestPropertiesEditor::change_genres_requested() {

  const QStringList value = split_string_with_comma(ui.genres_field->text());
  try_command(new SetGenresCommand(*this, value));
}

}
