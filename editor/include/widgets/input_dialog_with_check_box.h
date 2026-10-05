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
#ifndef SOLARUSEDITOR_INPUT_DIALOG_WITH_CHECK_BOX_H
#define SOLARUSEDITOR_INPUT_DIALOG_WITH_CHECK_BOX_H

#include <QDialog>
#include <QScopedPointer>

namespace SolarusEditor {

namespace Ui {
class InputDialogWithCheckBox;
}

/**
 * @brief A dialog to choose a text value with an additional checkbox.
 *
 * This dialog is similar to a standard QInputDialog with a line edit,
 * but has an additional checkbox to let the user choose for example
 * if she wants to update references in existing files.
 */
class InputDialogWithCheckBox : public QDialog {
  Q_OBJECT

public:

  InputDialogWithCheckBox(
      const QString& title,
      const QString& message,
      const QString& check_box_text,
      const QString& initial_value,
      QWidget* parent = nullptr
  );
  ~InputDialogWithCheckBox();

  QString get_value() const;
  void set_value(const QString& value);

  bool is_checked() const;
  void set_checked(bool checked);

  QString get_check_box_setting() const;
  void set_check_box_setting(const QString& check_box_setting);

public slots:

  void done(int result) override;

private:

  QScopedPointer<Ui::InputDialogWithCheckBox> ui;   /**< The widgets. */
  QString check_box_setting;                        /**< Setting name that saves the last
                                                     * check box state, or an empty string. */
};

}

#endif
