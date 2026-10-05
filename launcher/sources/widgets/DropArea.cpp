// SPDX-License-Identifier: GPL-3.0-or-later
#include <widgets/DropArea.h>
#include <Utils.h>

#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <oclero/qlementine/utils/ColorUtils.hpp>

#include <QPainter>
#include <QApplication>
#include <QSvgRenderer>

namespace solarus::launcher {
namespace i18n {
static QString dropAreaTitle() {
  return QApplication::translate("SolarusLauncher", "Drop Files Here");
}
static QString dropAreaLabel() {
  return QApplication::translate("SolarusLauncher", "Accepts folders and Solarus Quests");
}
} // namespace i18n

DropArea::DropArea(QWidget* parent)
  : QWidget(parent) {
  setAcceptDrops(false);
  setAttribute(Qt::WA_TransparentForMouseEvents);
  setAutoFillBackground(false);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  setFocusPolicy(Qt::NoFocus);
  setVisible(false);

  // Opacity animation.
  const auto opacityAnimDuration = style()->styleHint(QStyle::SH_Widget_Animation_Duration) * 0.5;
  constexpr auto startOpacity = 0.;
  const auto startOpacityVar = QVariant::fromValue<double>(startOpacity);
  _opacityAnimation.setDuration(opacityAnimDuration);
  _opacityAnimation.setStartValue(startOpacityVar);
  _opacityAnimation.setEndValue(startOpacityVar);
  QObject::connect(&_opacityAnimation, &QVariantAnimation::valueChanged, this, [this]() {
    update();
  });
  QObject::connect(&_opacityAnimation, &QVariantAnimation::finished, this, [this]() {
    if (!_visible) {
      hide();
    }
  });

  // Generate image and cache it.
  const auto svgPath = QString(oclero::qlementine::icons::iconPath(Icons16::Action_Download));
  QSvgRenderer svgRenderer(svgPath);
  svgRenderer.setAspectRatioMode(Qt::AspectRatioMode::KeepAspectRatio);

  const auto pixelRatio = 2;
  QPixmap pixmap(QSize(64, 64) * pixelRatio);
  pixmap.fill(Qt::transparent);
  {
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    svgRenderer.render(&painter, pixmap.rect());
  }
  pixmap.setDevicePixelRatio(static_cast<double>(pixelRatio));
  const auto* qlementine = qobject_cast<oclero::qlementine::QlementineStyle*>(style());
  const auto& theme = qlementine ? qlementine->theme() : oclero::qlementine::Theme{};
  const auto coloredPixmap = oclero::qlementine::getColorizedPixmap(pixmap, theme.primaryColor);
  _pixmapCache = coloredPixmap;
}

void DropArea::paintEvent(QPaintEvent* event) {
  QPainter painter(this);
  painter.setRenderHint(QPainter::RenderHint::Antialiasing, true);

  const auto* qlementine = qobject_cast<oclero::qlementine::QlementineStyle*>(style());
  const auto& theme = qlementine ? qlementine->theme() : oclero::qlementine::Theme{};
  const auto& bgColor = oclero::qlementine::colorWithAlphaF(theme.backgroundColorWorkspace, 0.95);
  const auto& borderColor = theme.primaryColor;
  const auto penWidth = theme.borderWidth * 2.;
  const auto halfPenWidth = penWidth / 2.;
  const auto radius = theme.borderRadius * 2.;
  const auto padding = theme.spacing * 3;
  const auto rect = this->rect();

  // Global opacity.
  const auto opacity = _opacityAnimation.currentValue().toDouble();
  painter.setOpacity(opacity);

  // Background.
  painter.setPen(Qt::NoPen);
  painter.setBrush(bgColor);
  painter.drawRect(rect);

  // Border;
  auto pen = QPen(borderColor, penWidth, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
  pen.setDashPattern({ penWidth * 2., penWidth * 2. });
  const auto border_rect = rect.toRectF().adjusted(
    padding + halfPenWidth, padding + halfPenWidth, -padding - halfPenWidth, -padding - halfPenWidth);
  painter.setBrush(Qt::NoBrush);
  painter.setPen(pen);
  painter.drawRoundedRect(border_rect, radius, radius);

  // Texts.
  const auto textSpacing = theme.spacing * 2;
  const auto titleText = i18n::dropAreaTitle();
  const auto& titleFont = theme.fontH2;
  const auto titleFm = QFontMetricsF(titleFont);
  const auto titleHeight = titleFm.height();
  const auto& titleColor = theme.primaryColor;
  const auto& labelText = QString("%1 (*.solarus).").arg(i18n::dropAreaLabel());
  const auto& labelFont = theme.fontRegular;
  const auto labelFm = QFontMetricsF(labelFont);
  const auto labelHeight = labelFm.height();
  const auto labelColor = theme.secondaryColor;

  constexpr auto iconHeight = 64;
  const auto totalTextHeight = iconHeight + textSpacing + titleHeight + textSpacing + labelHeight;
  const auto totalTextY = (rect.height() - totalTextHeight) / 2;
  const auto pixmapY = totalTextY;
  const auto titleY = pixmapY + iconHeight + textSpacing;
  const auto labelY = titleY + titleHeight + textSpacing;

  const auto titleRect = QRect(0, titleY, rect.width(), titleHeight);
  const auto labelRect = QRect(0, labelY, rect.width(), labelHeight);
  const auto pixmapRect = QRect((rect.width() - 64) / 2, pixmapY, 64, 64);

  painter.drawPixmap(pixmapRect, _pixmapCache);

  painter.setPen(titleColor);
  painter.setFont(titleFont);
  painter.drawText(titleRect, titleText, { Qt::AlignHCenter | Qt::AlignTop });

  painter.setPen(labelColor);
  painter.setFont(labelFont);
  painter.drawText(labelRect, labelText, { Qt::AlignHCenter | Qt::AlignTop });
}

void DropArea::animateVisibility(bool visible) {
  if (visible != _visible) {
    _visible = visible;

    if (_visible) {
      show();
    }
    const auto currentOpacity = _opacityAnimation.currentValue().toDouble();
    _opacityAnimation.stop();
    _opacityAnimation.setStartValue(QVariant::fromValue<double>(currentOpacity));
    _opacityAnimation.setEndValue(QVariant::fromValue<double>(_visible ? 1. : 0.));
    _opacityAnimation.start();
  }
}
} // namespace solarus::launcher
