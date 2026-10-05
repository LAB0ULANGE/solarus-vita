// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QScrollArea>

class QLabel;
class QTableView;
class QModelIndex;

namespace solarus::launcher {
class QuestPropsModel;
class QuestListModel;

class QuestPropertiesPanel : public QScrollArea {
  Q_OBJECT

public:
  explicit QuestPropertiesPanel(QuestListModel* model, QWidget* parent = nullptr);
  virtual ~QuestPropertiesPanel() = default;

private:
  void setupUi();
  void updateUi(const QString& questPath);

private:
  QuestListModel* _model{ nullptr };
  QLabel* _thumbnailLabel{ nullptr };
  QLabel* _descriptionLabel{ nullptr };
  QTableView* _tableView{ nullptr };
  QuestPropsModel* _tableModel{ nullptr };
};
} // namespace solarus::launcher
