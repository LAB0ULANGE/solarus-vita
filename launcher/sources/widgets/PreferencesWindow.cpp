// SPDX-License-Identifier: GPL-3.0-or-later
#include <widgets/PreferencesWindow.h>

#include <Preferences.h>
#include <Utils.h>
#include <Controller.h>
#include <Common.h>

#include <QApplication>
#include <QBoxLayout>
#include <QFormLayout>
#include <QComboBox>
#include <QScrollArea>
#include <QSpacerItem>
#include <QSignalBlocker>
#include <QTimer>
#include <QPushButton>
#include <QPainter>
#include <QLocale>

#include <oclero/qlementine/widgets/Label.hpp>
#include <oclero/qlementine/widgets/Switch.hpp>
#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <oclero/qlementine/style/ThemeManager.hpp>

namespace solarus::launcher {
namespace i18n {
static QString windowTitle() {
  return QApplication::translate("SolarusLauncher", "%1 Preferences");
}
static QString appPreferencesTitle() {
  return QApplication::translate("SolarusLauncher", "Application");
}
static QString appLanguage() {
  return QApplication::translate("SolarusLauncher", "Language");
}
static QString appLanguageCaption() {
  return QApplication::translate(
    "SolarusLauncher", "You must restart the application for it to take this change into account.");
}
static QString appPropertiesPanelVisible() {
  return QApplication::translate("SolarusLauncher", "Show Quest Information Panel");
}
static QString appPropertiesPanelVisibleCaption() {
  return QApplication::translate("SolarusLauncher", "Displays detailed information about the selected Quest.");
}
static QString appConsoleVisible() {
  return QApplication::translate("SolarusLauncher", "Show Console");
}
static QString appConsoleVisibleCaption() {
  return QApplication::translate("SolarusLauncher", "Very useful to spot bugs.");
}
static QString appWarnBeforeQuestRemoval() {
  return QApplication::translate("SolarusLauncher", "Ask before removing a quest");
}
static QString appWarnBeforeQuestRemovalCaption() {
  return QApplication::translate("SolarusLauncher", "Quests are only removed from the app's index, not from disk.");
}
static QString appTheme() {
  return QApplication::translate("SolarusLauncher", "Theme");
}
static QString appThemeCaption() {
  return QApplication::translate("SolarusLauncher", "Change the app's look. No need to restart.");
}
static QString questPreferencesTitle() {
  return QApplication::translate("SolarusLauncher", "Quests");
}
static QString questEnableAudio() {
  return QApplication::translate("SolarusLauncher", "Audio Enabled");
}
static QString questEnableAudioCaption() {
  return QApplication::translate("SolarusLauncher", "Plays the quest audio.");
}
static QString questForceSoftwareRendering() {
  return QApplication::translate("SolarusLauncher", "Force Software Rendering");
}
static QString questForceSoftwareRenderingCaption() {
  return QApplication::translate("SolarusLauncher", "Enable this if your machine doesn't support OpenGL.");
}
static QString questFullScreen() {
  return QApplication::translate("SolarusLauncher", "Full Screen");
}
static QString questFullScreenCaption() {
  return QApplication::translate("SolarusLauncher", "Start the quest as full screen.");
}
static QString questSuspendWhenUnfocused() {
  return QApplication::translate("SolarusLauncher", "Suspend Quest when unfocused");
}
static QString questSuspendWhenUnfocusedCaption() {
  return QApplication::translate("SolarusLauncher", "Pauses the game when the window is no longer active.");
}
static QString reset() {
  return QApplication::translate("SolarusLauncher", "Reset");
}
static QString resetTooltip() {
  return QApplication::translate("SolarusLauncher", "Reset to factory defaults.");
}
} // namespace i18n

namespace {

QWidget* makeRowLabel(const QString& label, const QString& caption, QWidget* parent) {
  auto* container = new QWidget(parent);
  container->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
  container->setFixedWidth(224);
  auto* layout = new QVBoxLayout(container);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(4);

  auto* qLabel = new QLabel(label, container);
  qLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  qLabel->setWordWrap(true);
  layout->addWidget(qLabel);

  auto* captionLabel = new oclero::qlementine::Label(caption, oclero::qlementine::TextRole::Caption, container);
  captionLabel->setWordWrap(true);
  captionLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  captionLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  layout->addWidget(captionLabel);

  layout->addStretch();

  return container;
}
} // namespace

class BottomWidget : public QWidget {
  using QWidget::QWidget;

protected:
  void paintEvent(QPaintEvent*) override {
    const auto* style = qobject_cast<oclero::qlementine::QlementineStyle*>(this->style());
    const auto& bgColor = style ? style->theme().backgroundColorMain3 : palette().base();
    QPainter p(this);
    p.fillRect(rect(), bgColor);
  }
};

PreferencesWindow::PreferencesWindow(Controller* controller, QWidget* parent)
  : QDialog(parent)
  , _controller(controller) {
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
  setWindowTitle(i18n::windowTitle().arg(QApplication::applicationDisplayName()));
  setWindowModality(Qt::WindowModality::ApplicationModal);
  setWindowFlag(Qt::WindowType::WindowContextHelpButtonHint, false);
  setWindowFlag(Qt::WindowType::WindowMaximizeButtonHint, false);
  setWindowFlag(Qt::WindowType::WindowMinimizeButtonHint, false);
  setWindowFlag(Qt::WindowType::WindowFullscreenButtonHint, false);

  setupUi();
  ensurePolished();
  const auto sh = sizeHint(); // + QSize(0, 40);
  setFixedWidth(sh.width());
  setMaximumHeight(sh.height());
  setMinimumHeight(200);

  QTimer::singleShot(0, this, [this, sh]() {
    resize(sh);
  });
}

void PreferencesWindow::setupUi() {
  auto* rootLayout = new QVBoxLayout(this);
  rootLayout->setSpacing(0);
  rootLayout->setContentsMargins(0, 0, 0, 0);
  setLayout(rootLayout);

  auto* scrollArea = new QScrollArea(this);
  scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  rootLayout->addWidget(scrollArea);

  auto* scrollAreaContent = new QWidget(scrollArea);
  scrollAreaContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  scrollArea->setWidgetResizable(true);
  scrollArea->setWidget(scrollAreaContent);

  auto* formLayout = new QFormLayout(scrollAreaContent);
  formLayout->setContentsMargins(32, 16, 32, 32);
  formLayout->setHorizontalSpacing(32);
  formLayout->setVerticalSpacing(16);
  formLayout->setRowWrapPolicy(QFormLayout::RowWrapPolicy::DontWrapRows);
  scrollAreaContent->setLayout(formLayout);

  // ---- App ----
  {
    auto* title = new oclero::qlementine::Label(this);
    title->setRole(oclero::qlementine::TextRole::H4);
    title->setText(i18n::appPreferencesTitle());
    formLayout->addRow(title);
  }
  {
    auto* languageComboBox = new QComboBox(this);
    if (auto* qlementine = qobject_cast<oclero::qlementine::QlementineStyle*>(languageComboBox->style())) {
      qlementine->setAutoIconColor(languageComboBox, oclero::qlementine::AutoIconColor::None);
    }
    languageComboBox->setSizeAdjustPolicy(QComboBox::SizeAdjustPolicy::AdjustToContents);
    for (const auto& lang : std::as_const(_controller->languages())) {
      languageComboBox->addItem(Common::languageIcon(lang), Common::languageName(lang), QVariant(lang));
    }

    const auto currentLanguage = _controller->preferences()->appLanguage();
    const auto currentIndex = languageComboBox->findData(currentLanguage);
    languageComboBox->setCurrentIndex(currentIndex);

    QObject::connect(languageComboBox, &QComboBox::currentIndexChanged, this, [this, languageComboBox](int index) {
      const auto lang = languageComboBox->itemData(index).toString();
      _controller->preferences()->setAppLanguage(lang);
    });

    formLayout->addRow(makeRowLabel(i18n::appLanguage(), i18n::appLanguageCaption(), this), languageComboBox);
  }
  {
    auto* themeComboBox = new QComboBox(this);
    themeComboBox->setMinimumWidth(100);

    const auto& themes = _controller->themeManager()->themes();
    const auto currentTheme = _controller->preferences()->appTheme();

    for (const auto& theme : themes) {
      const auto text = Controller::themeName(theme.meta.name);
      const auto icon = makeIcon(theme.meta.name == Common::darkThemeId() ? Icons16::Misc_Moon : Icons16::Misc_Sun);
      themeComboBox->addItem(icon, text, QString(theme.meta.name));
    }

    const auto index = themeComboBox->findData(currentTheme);
    themeComboBox->setCurrentIndex(index);

    QObject::connect(themeComboBox, &QComboBox::currentIndexChanged, this, [this, themeComboBox](int index) {
      const auto theme = themeComboBox->itemData(index).toString();
      _controller->preferences()->setAppTheme(theme);
    });

    formLayout->addRow(makeRowLabel(i18n::appTheme(), i18n::appThemeCaption(), this), themeComboBox);
  }
  {
    auto* propertiesPanelSwitch = new oclero::qlementine::Switch(this);
    formLayout->addRow(makeRowLabel(i18n::appPropertiesPanelVisible(), i18n::appPropertiesPanelVisibleCaption(), this),
      propertiesPanelSwitch);

    propertiesPanelSwitch->setChecked(_controller->preferences()->appPropertiesPanelVisible());
    QObject::connect(propertiesPanelSwitch, &QAbstractButton::clicked, this, [this](bool checked) {
      _controller->preferences()->setAppPropertiesPanelVisible(checked);
    });
    QObject::connect(_controller->preferences(), &Preferences::appPropertiesPanelVisibleChanged, this,
      [this, propertiesPanelSwitch]() {
        QSignalBlocker _(propertiesPanelSwitch);
        const auto value = _controller->preferences()->appPropertiesPanelVisible();
        propertiesPanelSwitch->setChecked(value);
      });
  }
  {
    auto* consoleSwitch = new oclero::qlementine::Switch(this);
    formLayout->addRow(makeRowLabel(i18n::appConsoleVisible(), i18n::appConsoleVisibleCaption(), this), consoleSwitch);

    consoleSwitch->setChecked(_controller->preferences()->appConsoleVisible());
    QObject::connect(consoleSwitch, &QAbstractButton::clicked, this, [this](bool checked) {
      _controller->preferences()->setAppConsoleVisible(checked);
    });
    QObject::connect(_controller->preferences(), &Preferences::appConsoleVisibleChanged, this, [this, consoleSwitch]() {
      QSignalBlocker _(consoleSwitch);
      const auto value = _controller->preferences()->appConsoleVisible();
      consoleSwitch->setChecked(value);
    });
  }
  {
    auto* warnBeforeRemovalSwitch = new oclero::qlementine::Switch(this);
    formLayout->addRow(makeRowLabel(i18n::appWarnBeforeQuestRemoval(), i18n::appWarnBeforeQuestRemovalCaption(), this),
      warnBeforeRemovalSwitch);

    warnBeforeRemovalSwitch->setChecked(_controller->preferences()->appWarnBeforeQuestRemoval());
    QObject::connect(warnBeforeRemovalSwitch, &QAbstractButton::clicked, this, [this](bool checked) {
      _controller->preferences()->setAppWarnBeforeQuestRemoval(checked);
    });
    QObject::connect(_controller->preferences(), &Preferences::appWarnBeforeQuestRemovalChanged, this,
      [this, warnBeforeRemovalSwitch]() {
        QSignalBlocker _(warnBeforeRemovalSwitch);
        const auto value = _controller->preferences()->appWarnBeforeQuestRemoval();
        warnBeforeRemovalSwitch->setChecked(value);
      });
  }
  {
    formLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Fixed, QSizePolicy::Fixed));
  }
  // ---- Quests ----
  {
    auto* title = new oclero::qlementine::Label(this);
    title->setRole(oclero::qlementine::TextRole::H4);
    title->setText(i18n::questPreferencesTitle());
    formLayout->addRow(title);
  }
  {
    auto* questAudiowitch = new oclero::qlementine::Switch(this);
    formLayout->addRow(makeRowLabel(i18n::questEnableAudio(), i18n::questEnableAudioCaption(), this), questAudiowitch);

    questAudiowitch->setChecked(_controller->preferences()->questEnableAudio());
    QObject::connect(questAudiowitch, &QAbstractButton::clicked, this, [this](bool checked) {
      _controller->preferences()->setQuestEnableAudio(checked);
    });
    QObject::connect(
      _controller->preferences(), &Preferences::questEnableAudioChanged, this, [this, questAudiowitch]() {
        QSignalBlocker _(questAudiowitch);
        const auto value = _controller->preferences()->questEnableAudio();
        questAudiowitch->setChecked(value);
      });
  }
  {
    auto* softwareRenderingSwitch = new oclero::qlementine::Switch(this);
    formLayout->addRow(
      makeRowLabel(i18n::questForceSoftwareRendering(), i18n::questForceSoftwareRenderingCaption(), this),
      softwareRenderingSwitch);

    softwareRenderingSwitch->setChecked(_controller->preferences()->questForceSoftwareRendering());
    QObject::connect(softwareRenderingSwitch, &QAbstractButton::clicked, this, [this](bool checked) {
      _controller->preferences()->setQuestForceSoftwareRendering(checked);
    });
    QObject::connect(_controller->preferences(), &Preferences::questForceSoftwareRenderingChanged, this,
      [this, softwareRenderingSwitch]() {
        QSignalBlocker _(softwareRenderingSwitch);
        const auto value = _controller->preferences()->questForceSoftwareRendering();
        softwareRenderingSwitch->setChecked(value);
      });
  }
  {
    auto* fullScreenSwitch = new oclero::qlementine::Switch(this);
    formLayout->addRow(makeRowLabel(i18n::questFullScreen(), i18n::questFullScreenCaption(), this), fullScreenSwitch);

    fullScreenSwitch->setChecked(_controller->preferences()->questFullScreen());
    QObject::connect(fullScreenSwitch, &QAbstractButton::clicked, this, [this](bool checked) {
      _controller->preferences()->setQuestFullScreen(checked);
    });
    QObject::connect(
      _controller->preferences(), &Preferences::questFullScreenChanged, this, [this, fullScreenSwitch]() {
        QSignalBlocker _(fullScreenSwitch);
        const auto value = _controller->preferences()->questFullScreen();
        fullScreenSwitch->setChecked(value);
      });
  }
  {
    auto* suspendSwitch = new oclero::qlementine::Switch(this);
    formLayout->addRow(
      makeRowLabel(i18n::questSuspendWhenUnfocused(), i18n::questSuspendWhenUnfocusedCaption(), this), suspendSwitch);

    suspendSwitch->setChecked(_controller->preferences()->questSuspendWhenUnfocused());
    QObject::connect(suspendSwitch, &QAbstractButton::clicked, this, [this](bool checked) {
      _controller->preferences()->setQuestSuspendWhenUnfocused(checked);
    });
    QObject::connect(
      _controller->preferences(), &Preferences::questSuspendWhenUnfocusedChanged, this, [this, suspendSwitch]() {
        QSignalBlocker _(suspendSwitch);
        const auto value = _controller->preferences()->questSuspendWhenUnfocused();
        suspendSwitch->setChecked(value);
      });
  }
  {
    auto* container = new BottomWidget(this);
    container->setFocusPolicy(Qt::NoFocus);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    auto* containerLayout = new QHBoxLayout(container);
    containerLayout->setContentsMargins(16, 12, 16, 12);
    container->setLayout(containerLayout);
    rootLayout->addWidget(container);

    auto* resetButton = new QPushButton(makeIcon(Icons16::Action_Reset), i18n::reset(), container);
    resetButton->setToolTip(i18n::resetTooltip());
    resetButton->setDefault(false);
    resetButton->setAutoDefault(false);
    resetButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    containerLayout->addWidget(resetButton);
    containerLayout->setAlignment(resetButton, Qt::AlignLeft);

    QObject::connect(resetButton, &QPushButton::clicked, this, [this]() {
      _controller->preferences()->resetToDefaults();
    });
  }
}

void PreferencesWindow::mouseReleaseEvent(QMouseEvent* evt) {
  QDialog::mouseReleaseEvent(evt);
  if (auto* widget = qApp->focusWidget()) {
    widget->clearFocus();
  }
}
} // namespace solarus::launcher
