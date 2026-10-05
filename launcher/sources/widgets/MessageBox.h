// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QDialog>
#include <QMap>

class QLayout;
class QLabel;
class QCheckBox;

namespace oclero::qlementine {
class Label;
class StatusBadgeWidget;
} // namespace oclero::qlementine

namespace solarus::launcher {
/// Reimplementation of QMessageBox, but prettier.
class MessageBox : public QDialog {
  Q_OBJECT

public:
  enum class Type {
    None,
    Question,
    Information,
    Warning,
    Error,
    Success,
  };

  enum Button : int {
    None = 0,
    Ok = 1 << 0,
    Yes = 1 << 1,
    Cancel = 1 << 2,
  };
  Q_ENUM(Button);
  Q_DECLARE_FLAGS(Buttons, Button)

public:
  explicit MessageBox(QWidget* parent = nullptr);
  virtual ~MessageBox() = default;

  Type type() const;
  void setType(Type value);

  QString title() const;
  void setTitle(const QString& value);

  QString text() const;
  void setText(const QString& value);

  Buttons buttons() const;
  void setButtons(Buttons value);

  void setButtonIcon(Button, const QIcon& icon);
  QIcon iconForButton(Button button) const;

  void setCheckBox(const QString& text, bool checked);
  void setCheckBoxText(const QString& text);
  void setCheckBoxChecked(bool checked);

  QString checkBoxText() const;
  bool checkBoxChecked() const;

  static Button exec(
    QWidget* parent, Type type, const QString& title, const QString& text, const Buttons buttons = { Ok | Cancel });

  static Button buttonResult(int result);

protected:
  void showEvent(QShowEvent*) override;
  void hideEvent(QHideEvent*) override;

private:
  void setupUi();
  QPushButton* getButtonWidget(Button button) const;

private:
  Type _type{ Type::None };
  struct {
    oclero::qlementine::StatusBadgeWidget* statusBadge{ nullptr };
    oclero::qlementine::Label* titleLabel{ nullptr };
    QLabel* textLabel{ nullptr };
    QCheckBox* checkBox{ nullptr };
    QLayout* buttonsLayout{ nullptr };
  } _ui;
  Buttons _buttons{ Button::None };
  QMap<Button, QPushButton*> _widgets{};
};
} // namespace solarus::launcher

Q_DECLARE_OPERATORS_FOR_FLAGS(solarus::launcher::MessageBox::Buttons)
