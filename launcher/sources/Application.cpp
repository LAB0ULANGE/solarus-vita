// SPDX-License-Identifier: GPL-3.0-or-later
#include <Application.h>

#include <oclero/QtAppInstanceManager.hpp>

#include <oclero/qlementine.hpp>
#include <oclero/qlementine/icons/QlementineIcons.hpp>

#include <widgets/MainWindow.h>
#include <Controller.h>
#include <SolarusLauncherConfig.h>

#include <QFileOpenEvent>
#include <QFileInfo>

namespace solarus::launcher {
void configureQApplication() {
  QGuiApplication::setApplicationName(SOLARUSLAUNCHER_DISPLAY_NAME);
  QGuiApplication::setApplicationDisplayName(SOLARUSLAUNCHER_DISPLAY_NAME);
  QGuiApplication::setOrganizationName(SOLARUSLAUNCHER_ORGANISATION);
  QGuiApplication::setOrganizationDomain(SOLARUSLAUNCHER_ORGANISATION_DOMAIN);
  QGuiApplication::setApplicationVersion(SOLARUSLAUNCHER_VERSION);
  QGuiApplication::setDesktopFileName(SOLARUSLAUNCHER_APP_IDENTIFIER);
}

Application::Application(int& argc, char** argv)
  : QApplication(argc, argv) {
  setQuitOnLastWindowClosed(true);
  MainWindow::setAppIcon();

  // Initialize instance manager to force only one instance running.
  _instanceManager.setMode(oclero::QtAppInstanceManager::Mode::SingleInstance);

  // Custom style that supports theming.
  _style = new oclero::qlementine::QlementineStyle(this);
  _style->setAnimationsEnabled(true);
  _style->setAutoIconColor(oclero::qlementine::AutoIconColor::TextColor);
  _style->setIconPathGetter(oclero::qlementine::icons::fromFreeDesktop);
  QApplication::setStyle(_style);

  // Custom icon theme.
  oclero::qlementine::icons::initializeIconTheme();
  QIcon::setThemeName("qlementine");

  // Controller. Must be created after setting the QStyle on the app.
  _controller = new Controller(this);

  // Main window.
  _mainWindow = std::make_unique<MainWindow>(_controller);
  _mainWindow->show();

  // Raise main window if another instance is started but automatically closed.
  // If that other instance was started with a quest as arg, start it in this instance instead.
  QObject::connect(&_instanceManager, &oclero::QtAppInstanceManager::secondaryInstanceMessageReceived, this,
    [this](const unsigned int id, QByteArray const& data) {
      Q_UNUSED(id)
      if (_mainWindow) {
        _mainWindow->raise();
      }
      const auto secondaryInstanceArgs = QString::fromUtf8(data).split(' ');
      if (secondaryInstanceArgs.size() > 0) {
        const auto questFilePath = secondaryInstanceArgs[0];
        _controller->playQuest(questFilePath);
      }
    });
}

bool Application::event(QEvent* event) {
  if (event->type() == QEvent::FileOpen) {
    auto* openEvent = static_cast<QFileOpenEvent*>(event);
    const auto filePath = openEvent->file();
    // Check if the file exists and is a quest file.
    if (QFileInfo::exists(filePath) && filePath.endsWith(".solarus")) {
      _controller->playQuest(filePath);
      openEvent->accept();
    }
  }

  return QApplication::event(event);
}
} // namespace solarus::launcher
