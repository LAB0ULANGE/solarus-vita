// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QStyledItemDelegate>

class QListView;

namespace solarus::launcher {
class QuestListItemDelegate : public QStyledItemDelegate {
  Q_OBJECT

public:
  explicit QuestListItemDelegate(QObject *parent = nullptr);

  void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
  QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};
}  // namespace solarus::launcher
