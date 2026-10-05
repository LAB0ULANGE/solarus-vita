// SPDX-License-Identifier: GPL-3.0-or-later
#include <widgets/StatusBar.h>

#include <quests/QuestListModel.h>
#include <Utils.h>
#include <BasicUpdater.h>
#include <Controller.h>
#include <SolarusLauncherConfig.h>

#include <QApplication>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>
#include <QDesktopServices>
#include <QStackedWidget>

#include <oclero/qlementine/widgets/IconWidget.hpp>
#include <oclero/qlementine/widgets/LoadingSpinner.hpp>

namespace solarus::launcher {
namespace i18n {
static QString noQuestFound() {
  return QApplication::translate("SolarusLauncher", "No Quest added");
}
static QString questsFound(int count) {
  return QApplication::translate("SolarusLauncher", "%n Quest(s) added", "", count);
}
static QString updateAvailable() {
  return QApplication::translate("SolarusLauncher", "Update Available");
}
static QString noUpdateAvailable() {
  return QApplication::translate("SolarusLauncher", "No Update Available");
}
static QString versionComparison() {
  return QApplication::translate("SolarusLauncher", "You have %1, %2 is available.");
}
static QString checkingForUpdates() {
  return QApplication::translate("SolarusLauncher", "Checking for updates…");
}
static QString failedToCheckMessage(int errorCode) {
  return QApplication::translate("SolarusLauncher", "Failed to check for updates. Error code: %1").arg(errorCode);
}
} // namespace i18n

class MessageWidget : public QStackedWidget {
  oclero::qlementine::LoadingSpinner* loadingSpinner{ nullptr };
  QLabel* loadingLabel{ nullptr };
  QLabel* temporaryMessageLabel{ nullptr };
  QPushButton* button{ nullptr };
  int timerId{ -1 };

public:
  enum class Page {
    Empty,
    Loading,
    TemporaryMessage,
    Button,
  };

  std::function<void()> onButtonClicked;

  MessageWidget(QWidget* parent)
    : QStackedWidget(parent) {
    setupUi();
    setCurrentPage(Page::Empty);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  }

  void setCurrentPage(Page page, const QString& text = "", const QString& tooltip = "") {
    const auto page_int = static_cast<int>(page);
    if (page_int != currentIndex()) {
      if (timerId != -1) {
        killTimer(timerId);
      }
      timerId = -1;
      switch (page) {
        case Page::Loading:
          setVisible(true);
          temporaryMessageLabel->setText("");
          button->setText("");
          loadingLabel->setText(text);
          loadingSpinner->setSpinning(true);
          break;
        case Page::TemporaryMessage:
          setVisible(true);
          temporaryMessageLabel->setText(text);
          button->setText("");
          loadingLabel->setText("");
          timerId = startTimer(10000);
          loadingSpinner->setSpinning(false);
          break;
        case Page::Button:
          setVisible(true);
          temporaryMessageLabel->setText("");
          button->setText(text);
          button->setToolTip(tooltip);
          loadingLabel->setText("");
          loadingSpinner->setSpinning(false);
          break;
        case Page::Empty:
        default:
          setVisible(false);
          temporaryMessageLabel->setText("");
          button->setText("");
          loadingLabel->setText("");
          loadingSpinner->setSpinning(false);
          break;
      }
      setCurrentIndex(page_int);
    }
  }

protected:
  void timerEvent(QTimerEvent* evt) override {
    QStackedWidget::timerEvent(evt);
    if (evt->timerId() == timerId) {
      if (timerId != -1) {
        killTimer(timerId);
      }
      timerId = -1;
      setCurrentPage(Page::Empty);
    }
  }

private:
  void setupUi() {
    auto* emptyWidget = new QWidget(this);
    emptyWidget->setEnabled(false);
    emptyWidget->setAttribute(Qt::WidgetAttribute::WA_TransparentForMouseEvents);

    auto* loadingContainer = new QWidget(this);
    auto* loadingLayout = new QHBoxLayout(loadingContainer);
    loadingLayout->setContentsMargins(0, 0, 0, 0);
    loadingLayout->setSpacing(8);
    loadingSpinner = new oclero::qlementine::LoadingSpinner(loadingContainer);
    loadingSpinner->setSpinning(false);
    loadingLabel = new QLabel(this);
    loadingLayout->addWidget(loadingSpinner);
    loadingLayout->addWidget(loadingLabel);

    temporaryMessageLabel = new QLabel(this);

    button = new QPushButton(this);
    button->setAutoDefault(false);
    button->setDefault(true);
    button->setFixedHeight(20);
    button->setIcon(makeIcon(Icons16::Action_Update));
    QObject::connect(button, &QPushButton::clicked, this, [this]() {
      if (onButtonClicked)
        onButtonClicked();
    });
    addWidget(emptyWidget);
    addWidget(loadingContainer);
    addWidget(temporaryMessageLabel);
    addWidget(button);
  }
};

StatusBar::StatusBar(Controller* controller, QWidget* parent)
  : QStatusBar(parent)
  , _controller(controller) {
  setupUi();
}

void StatusBar::setupUi() {
  setFixedHeight(28);
  setSizeGripEnabled(false);
  setContentsMargins(16, 0, 16, 0);

  _ui.messageWidget = new MessageWidget(this);
  {
    QObject::connect(_controller->updater(), &BasicUpdater::checkStarted, this, [this]() {
      _ui.messageWidget->onButtonClicked = nullptr;
      _ui.messageWidget->setCurrentPage(MessageWidget::Page::Loading, i18n::checkingForUpdates());
    });

    QObject::connect(
      _controller->updater(), &BasicUpdater::checkFinished, this, [this](const BasicUpdater::Result& result) {
        switch (result.status) {
          case BasicUpdater::Status::UpdateAvailable:
            _ui.messageWidget->setCurrentPage(MessageWidget::Page::Button, i18n::updateAvailable(),
              i18n::versionComparison().arg(result.currentVersion, result.newVersion));
            _ui.messageWidget->onButtonClicked = [this, result]() {
              // Temporary, before getting the better update system:
              // we just redirect the user to the website, and he/she will download
              // the new version manually.
              QDesktopServices::openUrl({ SOLARUSLAUNCHER_LINKS_RELEASES });
            };
            break;
          case BasicUpdater::Status::NoUpdate:
            _ui.messageWidget->setCurrentPage(MessageWidget::Page::TemporaryMessage, i18n::noUpdateAvailable());
            break;
          case BasicUpdater::Status::Error:
            _ui.messageWidget->setCurrentPage(
              MessageWidget::Page::TemporaryMessage, i18n::failedToCheckMessage(result.errorCode));
            break;
          default:
            break;
        }
      });
  }

  auto* questCountContainer = new QWidget(this);
  {
    questCountContainer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    auto* questCountLayout = new QHBoxLayout(questCountContainer);
    questCountLayout->setContentsMargins(0, 0, 0, 0);
    questCountLayout->setSpacing(8);

    auto* iconWidget = new oclero::qlementine::IconWidget(questCountContainer);
    {
      const auto updateIcon = [this, iconWidget]() {
        const auto count = _controller->model()->rowCount({});
        iconWidget->setIcon(makeIcon(count > 0 ? Icons16::Misc_Library : Icons16::Misc_Warning));
      };
      updateIcon();
      QObject::connect(_controller->model(), &QuestListModel::rowCountChanged, this, updateIcon);
    }
    questCountLayout->addWidget(iconWidget);

    auto* questCountLabel = new QLabel(this);
    {
      questCountLabel->setText(i18n::noQuestFound());
      questCountLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

      const auto updateText = [this, questCountLabel]() {
        const auto count = _controller->model()->rowCount({});
        questCountLabel->setText(count > 0 ? i18n::questsFound(count) : i18n::noQuestFound());
      };
      updateText();
      QObject::connect(_controller->model(), &QuestListModel::rowCountChanged, this, updateText);
    }
    questCountLayout->addWidget(questCountLabel);

    if (const auto* qlementine = qobject_cast<oclero::qlementine::QlementineStyle*>(style())) {
      const auto updatePalette = [qlementine, iconWidget, questCountLabel]() {
        const auto palette = qlementine->paletteForTextRole(oclero::qlementine::TextRole::Caption);
        iconWidget->setPalette(palette);
        questCountLabel->setPalette(palette);
      };
      updatePalette();
      QObject::connect(qlementine, &oclero::qlementine::QlementineStyle::themeChanged, this, [updatePalette]() {
        updatePalette();
      });
    }
  }

  addPermanentWidget(_ui.messageWidget);
  addWidget(questCountContainer);
}
} // namespace solarus::launcher
