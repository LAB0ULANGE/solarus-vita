// SPDX-License-Identifier: GPL-3.0-or-later
#include <quests/QuestListItemDelegate.h>

#include <quests/QuestListModel.h>

#include <QPainter>
#include <QEvent>
#include <QMouseEvent>
#include <QApplication>

#include <oclero/qlementine/utils/ImageUtils.hpp>
#include <oclero/qlementine/utils/StateUtils.hpp>
#include <oclero/qlementine/style/QlementineStyle.hpp>

namespace solarus::launcher {
namespace i18n {
static QString playing() {
  return QApplication::translate("SolarusLauncher", "Playing");
}
} // namespace i18n

namespace {
QPixmap getPixmap(const QModelIndex& index, const qreal radius, const qreal pixelRatio, const qreal height) {
  auto thumbnail =
    index.data(Qt::DecorationRole).value<QPixmap>().scaledToHeight(height * pixelRatio, Qt::SmoothTransformation);
  thumbnail.setDevicePixelRatio(pixelRatio);
  const auto roundedThumbnail = oclero::qlementine::makeRoundedPixmap(thumbnail, radius * pixelRatio);
  return roundedThumbnail;
}
} // namespace

QuestListItemDelegate::QuestListItemDelegate(QObject* parent)
  : QStyledItemDelegate(parent) {}

void QuestListItemDelegate::paint(
  QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
  const auto pixelRatio = option.widget->devicePixelRatioF();
  const auto* qlementine = qobject_cast<oclero::qlementine::QlementineStyle*>(option.widget->style());
  const auto& theme = qlementine ? qlementine->theme() : oclero::qlementine::Theme{};
  const auto radius = theme.borderRadius;
  const auto& bgColor = theme.backgroundColorMain2;
  const auto thumbnail = getPixmap(index, radius, pixelRatio, option.rect.height());
  const auto thumbnailSize = thumbnail.deviceIndependentSize();
  const auto thumbnailX = option.rect.x() + (option.rect.width() - thumbnailSize.width()) / 2;
  const auto thumbnailY = option.rect.y() + (option.rect.height() - thumbnailSize.height()) / 2;
  const auto isPlaying = index.data(QuestListModel::DataRole::IsPlaying).toBool();

  painter->save();
  {
    painter->setRenderHint(QPainter::Antialiasing, true);

    // Background in case of a logo (Solarus 1.6).
    painter->setPen(Qt::NoPen);
    painter->setBrush(bgColor);
    painter->drawRoundedRect(option.rect, radius, radius);

    // Thumbnail.
    painter->setClipRect(option.rect, Qt::ReplaceClip);
    painter->setClipping(true);
    painter->drawPixmap(thumbnailX, thumbnailY, thumbnail);

    // Overlay to highlight hovered element.
    const auto hasMouseOver = option.state.testFlag(QStyle::State_MouseOver);
    const auto hasMousePress = option.state.testFlag(QStyle::State_Sunken);
    if (hasMouseOver || hasMousePress) {
      const auto overlayColor = QColor(255, 255, 255, hasMousePress ? 64 : 32);
      painter->setCompositionMode(QPainter::CompositionMode_Overlay);
      painter->setPen(Qt::NoPen);
      painter->setBrush(overlayColor);
      painter->drawRoundedRect(option.rect, radius, radius);
      painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
    }
    painter->setClipRect(QRect{}, Qt::NoClip);
    painter->setClipping(false);

    // Indicator for the current currently running.
    if (isPlaying) {
      const auto textRadius = radius / 1.5;
      const auto text = i18n::playing();
      const auto textBoundingRect = option.fontMetrics.boundingRect(text);
      const auto textBgPosition = option.rect.topLeft() + QPoint{ 9, 9 };
      constexpr auto padding = QMargins{ 8, 4, 8, 4 };
      const auto textBgRect = QRect(textBgPosition, textBoundingRect.size().grownBy(padding));

      painter->setPen(Qt::NoPen);
      painter->setBrush(theme.primaryColor);
      painter->drawRoundedRect(textBgRect, textRadius, textRadius);

      painter->setBrush(Qt::NoBrush);
      painter->setPen(theme.primaryColorForeground);
      painter->drawText(textBgRect, text, { Qt::AlignCenter });
    }

    // Border for selected element.
    const auto isCurrent = option.state.testFlag(QStyle::State_Selected);
    const auto hasFocus = option.state.testFlag(QStyle::State_HasFocus);
    if (isCurrent) {
      const auto strokeColor = hasFocus ? theme.primaryColorHovered : theme.primaryColor;
      const auto strokeWidth = hasFocus ? theme.focusBorderWidth + theme.borderWidth : theme.borderWidth;
      const auto halfStrokeWidth = strokeWidth / 2.;
      const auto strokeRect =
        option.rect.toRectF().adjusted(halfStrokeWidth, halfStrokeWidth, -halfStrokeWidth, -halfStrokeWidth);
      const auto strokeRadius = radius - halfStrokeWidth;

      painter->setBrush(Qt::NoBrush);
      painter->setPen(QPen(strokeColor, strokeWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
      painter->drawRoundedRect(strokeRect, strokeRadius, strokeRadius);
    }
  }
  painter->restore();
}

QSize QuestListItemDelegate::sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const {
  return { 200, 102 };
}
} // namespace solarus::launcher
