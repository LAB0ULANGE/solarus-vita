/*
 * Copyright (C) 2014-2019 Christopho, Solarus - http://www.solarus-games.org
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
#include "widgets/package_dialog.h"
#include "editor_settings.h"
#include "quest.h"
#include "ui_package_dialog.h"

#include <QFileDialog>

/* In terms of implementation, this class wraps four states and handles the
 * transitions
 *
 * 0. selection: Choose the package location and name.
 *    You can cancel the operation (close) or go ahead to 1.
 * 1. ongoing: Tells you the operation is in progress.
 *    You can cancel the operation, it goes to 2 on success
 * 2. complete: Just a little message and a close button.
 *    Optionally there is an auto-close feature.
 * 3. failed: The packaging failed, error message should be displayed.
 *    After looking at the message close the dialog.
 *
 * For simplicity we use the widget pointers instead of the index to refer to
 * them. (Except that we start at index 0: selection.)
 */

namespace SolarusEditor {

namespace {

QString get_default_save_path(const Quest &quest) {
  return quest.get_root_path() + "/" + quest.get_name() + ".solarus";
}

} // Anonymous namespace.

PackageDialog::PackageDialog(Quest const& quest, QWidget *parent) :
  QDialog(parent),
  ui(new Ui::PackageDialog),
  process(),
  quest(quest),
  save_path() {

  ui->setupUi(this);
  ui->stacked_widget->setCurrentWidget(ui->selection);
  ui->button_box->button(QDialogButtonBox::Apply)->setText(tr("Build"));

  process.setReadChannel(QProcess::StandardOutput);

  connect(ui->button_box->button(QDialogButtonBox::Apply), &QPushButton::clicked,
          this, &PackageDialog::process_start);
  connect(ui->selection_browse, &QPushButton::clicked,
          this, &PackageDialog::start_file_selection);
  connect(&process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
          this, &PackageDialog::process_finished);
  connect(&process, &QProcess::readyReadStandardOutput,
          this, &PackageDialog::handle_process_standard_output);

  const QString &saved_path = EditorSettings()
      .get_value_string(EditorSettings::package_save_path);
  set_save_path(saved_path.isEmpty() ? get_default_save_path(quest) : saved_path);
}

PackageDialog::~PackageDialog() = default;

void PackageDialog::set_save_path(const QString &new_save_path) {
  save_path = new_save_path;
  ui->selection_file->setText(save_path);
  EditorSettings().set_value(
        EditorSettings::package_save_path, QVariant(save_path));
}

void PackageDialog::process_start() {
  ui->button_box->button(QDialogButtonBox::Apply)->setEnabled(false);
  ui->button_box->button(QDialogButtonBox::Close)->setEnabled(false);
  ui->stacked_widget->setCurrentWidget(ui->ongoing);
  ui->ongoing_output->setPlainText(tr("Starting...\n"));

  process.setWorkingDirectory(quest.get_data_path());
  process.start("zip", QStringList() << "-r" << save_path << ".");
}

void PackageDialog::process_finished(int code, QProcess::ExitStatus status) {

  if (status == QProcess::CrashExit || code != 0) {
    ui->failed_output->setPlainText(
          QString(process.readAllStandardError()));
    ui->failed_code->setText(
          status == QProcess::CrashExit ?
            tr("Crashed") : QString::number(code));
    ui->stacked_widget->setCurrentWidget(ui->failed);
  } else {
    ui->stacked_widget->setCurrentWidget(ui->completed);
  }
  ui->button_box->button(QDialogButtonBox::Close)->setEnabled(true);
}

void PackageDialog::start_file_selection() {

  const QString& path = QFileDialog::getSaveFileName(
        this,
        tr("Quest package location:"),
        save_path,
        tr("Solarus Packages (*.solarus)"),
        nullptr,
#ifdef SOLARUSEDITOR_NO_NATIVE_DIALOGS
        QFileDialog::DontUseNativeDialog
#else
        QFileDialog::Options()
#endif
  );
  if (!path.isEmpty()) {
    set_save_path(path);
  }
}

void PackageDialog::handle_process_standard_output() {
  while (process.canReadLine()) {
    const QByteArray &line = process.readLine();
    ui->ongoing_output->moveCursor(QTextCursor::End);
    ui->ongoing_output->insertPlainText(QString(line));
  }
}

}
