// SPDX-License-Identifier: GPL-3.0-or-later
#include <widgets/MessageBox.h>

#include <Utils.h>

#include <oclero/qlementine/widgets/Label.hpp>
#include <oclero/qlementine/utils/LayoutUtils.hpp>
#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <oclero/qlementine/widgets/StatusBadgeWidget.hpp>

#include <QHBoxLayout>
#include <QPushButton>
#include <QApplication>
#include <QPainter>
#include <QCheckBox>

#include <optional>

namespace solarus::launcher {
namespace i18n {
// Gets Qt's embedded default translations.
static QString buttonLabel(MessageBox::Button button) {
  switch (button) {
    case MessageBox::Button::Ok:
      return QObject::tr("OK");
    case MessageBox::Button::Yes:
      return QObject::tr("Yes");
    case MessageBox::Button::Cancel:
      return QObject::tr("Cancel");
    default:
      return {};
  }
}
} // namespace i18n

namespace {
std::optional<oclero::qlementine::StatusBadge> typeToStatus(MessageBox::Type type) {
  switch (type) {
    case MessageBox::Type::Error:
      return oclero::qlementine::StatusBadge::Error;
    case MessageBox::Type::Warning:
      return oclero::qlementine::StatusBadge::Warning;
    case MessageBox::Type::Information:
      return oclero::qlementine::StatusBadge::Info;
    case MessageBox::Type::Success:
      return oclero::qlementine::StatusBadge::Success;
    case MessageBox::Type::None:
    default:
      return std::nullopt;
  }
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

MessageBox::MessageBox(QWidget* parent)
  : QDialog(parent) {
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  setWindowModality(Qt::WindowModality::ApplicationModal);
  setWindowFlag(Qt::WindowType::MSWindowsFixedSizeDialogHint, true);
  setWindowFlag(Qt::WindowType::WindowContextHelpButtonHint, false);
  setWindowFlag(Qt::WindowType::WindowMaximizeButtonHint, false);
  setWindowFlag(Qt::WindowType::WindowMinimizeButtonHint, false);
  setWindowFlag(Qt::WindowType::WindowFullscreenButtonHint, false);

  setupUi();
}

void MessageBox::setupUi() {
  auto* rootLayout = new QVBoxLayout(this);
  setLayout(rootLayout);
  rootLayout->setContentsMargins(0, 0, 0, 0);
  rootLayout->setSpacing(0);

  auto* topLayout = new QVBoxLayout();
  topLayout->setContentsMargins(32, 16, 32, 16);
  topLayout->setSpacing(16);
  rootLayout->addLayout(topLayout);

  _ui.statusBadge = new oclero::qlementine::StatusBadgeWidget(this);
  topLayout->addWidget(_ui.statusBadge);
  topLayout->setAlignment(_ui.statusBadge, Qt::AlignCenter);

  _ui.titleLabel = new oclero::qlementine::Label(this);
  _ui.titleLabel->setWordWrap(true);
  _ui.titleLabel->setRole(oclero::qlementine::TextRole::H4);
  topLayout->addWidget(_ui.titleLabel);

  _ui.textLabel = new QLabel(this);
  _ui.textLabel->setWordWrap(true);
  _ui.textLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  topLayout->addWidget(_ui.textLabel);

  ensurePolished();
  if (const auto* qlementineStyle = qobject_cast<oclero::qlementine::QlementineStyle*>(style())) {
    const auto updatePalette = [qlementineStyle, this]() {
      const auto palette = qlementineStyle->paletteForTextRole(oclero::qlementine::TextRole::Caption);
      _ui.textLabel->setPalette(palette);
    };
    updatePalette();
    QObject::connect(qlementineStyle, &oclero::qlementine::QlementineStyle::themeChanged, this, [updatePalette]() {
      updatePalette();
    });
  }

  _ui.checkBox = new QCheckBox(this);
  _ui.checkBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  _ui.checkBox->setChecked(false);
  _ui.checkBox->setVisible(false);
  topLayout->addWidget(_ui.checkBox);

  topLayout->addStretch();

  auto* bottomWidget = new BottomWidget(this);
  bottomWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  rootLayout->addWidget(bottomWidget);

  _ui.buttonsLayout = new QHBoxLayout(bottomWidget);
  bottomWidget->setLayout(_ui.buttonsLayout);
  _ui.buttonsLayout->setContentsMargins(32, 16, 32, 16);
  _ui.buttonsLayout->setSpacing(16);
}

void MessageBox::showEvent(QShowEvent* evt) {
  QDialog::showEvent(evt);

  _ui.checkBox->setVisible(!_ui.checkBox->text().isEmpty());

  constexpr auto defaultWidth = 380;
  setFixedWidth(defaultWidth);
  const auto h = heightForWidth(defaultWidth);
  setFixedHeight(h);
}

void MessageBox::hideEvent(QHideEvent* evt) {
  QDialog::hideEvent(evt);
  deleteLater();
}

MessageBox::Type MessageBox::type() const {
  return _type;
}

void MessageBox::setType(Type value) {
  if (value != _type) {
    _type = value;
    const auto status = typeToStatus(_type);
    if (status.has_value()) {
      _ui.statusBadge->setVisible(true);
      _ui.statusBadge->setBadge(status.value());
    } else {
      _ui.statusBadge->setVisible(false);
    }
  }
}

QString MessageBox::title() const {
  return _ui.titleLabel->text();
}

void MessageBox::setTitle(const QString& value) {
  _ui.titleLabel->setText(value);
}

QString MessageBox::text() const {
  return _ui.textLabel->text();
}

void MessageBox::setText(const QString& value) {
  _ui.textLabel->setText(value);
}

MessageBox::Buttons MessageBox::buttons() const {
  return _buttons;
}

void MessageBox::setButtons(Buttons value) {
  if (value != _buttons) {
    _buttons = value;
    _widgets.clear();
    oclero::qlementine::clearLayout(_ui.buttonsLayout);

    for (const auto btn : { Cancel, Yes, Ok }) {
      if (_buttons.testFlag(btn)) {
        auto* button = new QPushButton(this);
        button->setText(i18n::buttonLabel(btn));
        button->setAutoDefault(false);
        button->setDefault(btn == Yes || btn == Ok);
        button->setIcon(iconForButton(btn));
        QObject::connect(button, &QPushButton::clicked, this, [this, btn]() {
          done(btn);
        });
        _ui.buttonsLayout->addWidget(button);
        _widgets[btn] = button;
      }
    }
  }

  // Find default button.
  // TODO
}

QPushButton* MessageBox::getButtonWidget(Button button) const {
  const auto it = _widgets.find(button);
  if (it != _widgets.end()) {
    auto* widget = it.value();
    return widget;
  }
  return nullptr;
}

void MessageBox::setButtonIcon(Button button, const QIcon& icon) {
  if (auto* widget = getButtonWidget(button)) {
    widget->setIcon(icon);
  }
}

QIcon MessageBox::iconForButton(Button button) const {
  switch (button) {
    case Button::Ok:
      return makeIcon(Icons16::Misc_Success);
    case Button::Cancel:
      return makeIcon(Icons16::Action_Clear);
    default:
      return QIcon{};
  }
}

void MessageBox::setCheckBox(const QString& text, bool checked) {
  _ui.checkBox->setText(text);
  _ui.checkBox->setChecked(checked);
}

void MessageBox::setCheckBoxText(const QString& text) {
  _ui.checkBox->setText(text);
}

void MessageBox::setCheckBoxChecked(bool checked) {
  _ui.checkBox->setChecked(checked);
}

QString MessageBox::checkBoxText() const {
  return _ui.checkBox->text();
}

bool MessageBox::checkBoxChecked() const {
  return _ui.checkBox->isChecked();
}

MessageBox::Button MessageBox::exec(
  QWidget* parent, Type type, const QString& title, const QString& text, const Buttons buttons) {
  MessageBox msgBox(parent);
  msgBox.setType(type);
  msgBox.setTitle(title);
  msgBox.setText(text);
  msgBox.setButtons(buttons);
  const auto result = msgBox.QDialog::exec();
  return static_cast<Button>(result);
}

MessageBox::Button MessageBox::buttonResult(int result) {
  return static_cast<MessageBox::Button>(result);
}
} // namespace solarus::launcher
