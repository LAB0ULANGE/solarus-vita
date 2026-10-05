#include "editor_settings.h"
#include "editor_style.h"
#include "widgets/resource_selector.h"
#include <map>
#include <QApplication>
#include <QStyleHints>
#include <QStyleOptionComboBox>
#include <QWidget>
#include <QLayout>
#include <QLineEdit>
#include <QListView>
#include <QPlainTextEdit>

namespace SolarusEditor {

namespace {

const std::map<Mode, ModeInfo> mode_info = {
    {
        Mode::LIGHT,
        {
            Mode::LIGHT,
            ":/themes/light.json",
            QColor(0xCB6C7E),     // Lua keyword.
            QColor(0xFF9200),     // Literal string.
            QColor(0x9486FF),     // Comment.
            QColor(0xEEEDFA),     // Current line.
            QColor(0x66608F),     // Debug log text.
            QColor(0x4275F6),     // Info log text.
            QColor(0xFF9749),     // Warning log text.
            QColor(0xEE4D6E),     // Error log text.
            QColor(0xFFE1B6),     // Lua console background when invalid.
            QColor(0xFFBFC2),     // Lua console background when error.
        },
    },
    {
        Mode::DARK,
        {
            Mode::DARK,
            ":/themes/dark.json",
            QColor(0xff9197),     // Lua keyword.
            QColor(0xfbc064),     // Literal string.
            QColor(0xABA3E8),     // Comment.
            QColor(0x2E3149),     // Current line.
            QColor(0xd3d2d7),     // Debug log text.
            QColor(0x6c89ff),     // Info log text.
            QColor(0xfbc064),     // Warning log text.
            QColor(0xe96b72),     // Error log text.
            QColor(0x332C2A),     // Lua console background when invalid.
            QColor(0x3E2731),     // Lua console background when error.
        },
    },
};

}  // Anonymous namespace.

/**
 * @brief Creates a style for the editor.
 * @param parent Parent object or @c nullptr.
 */
EditorStyle::EditorStyle(QObject *parent):
  QlementineStyle(parent) {
  // Do not color icons, except for the widgets it is explicitely set to do so.
  setAutoIconColor(AutoIconColor::None);

  connect(QGuiApplication::styleHints(), &QStyleHints::colorSchemeChanged,
          this, &EditorStyle::osThemeChanged);

  EditorSettings settings;
  const QString& theme_name = settings.get_value_string(EditorSettings::theme);
  Mode mode = Mode::AUTOMATIC;
  if (theme_name == "light") {
    mode = Mode::LIGHT;
  } else if (theme_name == "dark") {
    mode = Mode::DARK;
  }
  set_mode(mode);

  // Allows to customize the icons in the QLineEdit/QPlainTextEdit context menu.
  setIconPathGetter([](const QString& freeDesktopName){
    static const std::map<QString, QString> mapping{
      {"edit-undo",":/images/icon_undo.svg"},
      {"edit-redo",":/images/icon_redo.svg"},
      {"edit-cut",":/images/icon_cut.svg"},
      {"edit-copy",":/images/icon_copy.svg"},
      {"edit-paste",":/images/icon_paste.svg"},
      {"edit-delete",":/images/icon_delete.svg"},
      {"edit-select-all",":/images/icon_select_all.svg"},
      {"go-up",":/images/icon_go_up.svg"},
      {"go-down",":/images/icon_go_down.svg"},
    };
    const auto it = mapping.find(freeDesktopName);
    return it != mapping.end() ? it->second : "";
  });
}

/**
 * @brief Returns a fixed-width font used in the editor.
 * @return A fixed-width font.
 */
QFont EditorStyle::get_fixed_font() const {
  return theme().fontMonospace;
}

/**
 * @brief Returns a fixed-width font used in the editor, or a fallback if the style is not an EditorStyle.
 * @return A fixed-width font.
 */
QFont EditorStyle::get_fixed_font_or_fallback() {
  if (const EditorStyle* style = qobject_cast<const EditorStyle*>(qApp->style())) {
    return style->get_fixed_font();
  } else {
    QFont font = QFontDatabase::systemFont(QFontDatabase::SystemFont::FixedFont);
    font.setPointSize(11);
    return font;
  }
}

/**
 * @brief Returns the current mode of the editor.
 * @return The current mode (possibly @c Mode::AUTOMATIC).
 */
Mode EditorStyle::get_mode() const {
  return mode;
}

/**
 * @brief Sets the mode of the editor.
 * @param mode The mode to set (possibly @c Mode::AUTOMATIC).
 */
void EditorStyle::set_mode(Mode mode) {
  this->mode = mode;

  Mode actual_mode = (mode == Mode::AUTOMATIC) ? get_os_mode() : mode;
  if (actual_mode != this->actual_mode) {
    this->actual_mode = actual_mode;
    setThemeJsonPath(mode_info.at(actual_mode).path);
    emit actual_mode_changed(actual_mode);
  }
}

/**
 * @brief Returns the final mode, i.e. with automatic replaced by the actual one.
 * @return The actual mode.
 */
Mode EditorStyle::get_actual_mode() const {
  return actual_mode;
}

/**
 * @brief Returns details of the actual current mode.
 */
const ModeInfo& EditorStyle::get_mode_info() {
  const EditorStyle* style = qobject_cast<const EditorStyle*>(qApp->style());
  if (style == nullptr) {
    return mode_info.at(Mode::LIGHT);
  }
  return mode_info.at(style->get_actual_mode());
}

/**
 * @brief Returns the light or dark mode setting from the operating system.
 * @return @c Mode::LIGHT or @c Mode::DARK.
 */
Mode EditorStyle::get_os_mode() {
  return QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark ? Mode::DARK : Mode::LIGHT;
}

/**
 * @brief Called when the operating system theme has changed (light or dark).
 */
void EditorStyle::osThemeChanged() {
  set_mode(mode);
}

void EditorStyle::polish(QWidget* widget) {

  QlementineStyle::polish(widget);

  // Tweak the icon colors in the text widget's menus.
  if (QLineEdit* line_edit = qobject_cast<QLineEdit*>(widget)) {
    QlementineStyle::setAutoIconColor(line_edit, AutoIconColor::TextColor);
  }
  if (QPlainTextEdit* plain_text_edit = qobject_cast<QPlainTextEdit*>(widget)) {
    QlementineStyle::setAutoIconColor(plain_text_edit, AutoIconColor::TextColor);
  }
}


/**
 * @brief Allows to get the Status property undirectly set on a widget.
 * @param widget The widget to get the set Status.
 * @return The widget's Status.
 */
EditorStyle::Status EditorStyle::widgetStatus(QWidget const* widget) const {

  if (widget == nullptr) {
    return Status::Default;
  }
  const QVariant& status = widget->property("status");
  return status.isValid() ? status.value<Status>() : Status::Default;
}

/**
 * @brief Allows to get the background color property undirectly set on a text field.
 * @param mouse The state of the widget.
 * @param status The status of the widget.
 * @return The text field background color.
 */
QColor const& EditorStyle::textFieldBackgroundColor(MouseState const mouse, Status const status) const {

  switch (status) {
  case Status::Error:
    return get_mode_info().console_background_error_color;
  case Status::Warning:
    return get_mode_info().console_background_invalid_color;
  case Status::Success:
  case Status::Info:
  case Status::Default:
  default:
    return QlementineStyle::textFieldBackgroundColor(mouse, status);
  }
}

/**
 * @brief Workaround for Qlementine bug #63 that crops the left of QLineEdit.
 * Assumes that editable comboboxes that don't have icons in their items.
 */
QRect EditorStyle::subControlRect(
    ComplexControl control, const QStyleOptionComplex* option, SubControl subControl, const QWidget* widget) const {

  switch (control) {
  case CC_ComboBox:
    if (const auto* comboBoxOpt = qstyleoption_cast<const QStyleOptionComboBox*>(option)) {
      switch (subControl) {
      case SC_ComboBoxEditField:
        if (comboBoxOpt->editable) {
          const auto indicatorSize = theme().iconSize;
          const auto spacing = theme().spacing;
          const auto indicatorButtonW = spacing * 2 + indicatorSize.width();
          const auto editFieldW = comboBoxOpt->rect.width() - indicatorButtonW;
          return QRect{ comboBoxOpt->rect.x(), comboBoxOpt->rect.y(), editFieldW, comboBoxOpt->rect.height() };
        }
        break;
      default:
        break;
      }
    }
  default:
    break;
  }
  return QlementineStyle::subControlRect(control, option, subControl, widget);
}

}  // namespace Solarus Editor
