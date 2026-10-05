// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QAbstractListModel>

#include <quests/QuestData.h>

class QFileSystemWatcher;
class QSortFilterProxyModel;

namespace solarus::launcher {
class QuestListModel : public QAbstractListModel {
  Q_OBJECT

public:
  enum DataRole {
    IsPlaying = Qt::UserRole + 1,
    QuestPath,
  };

public:
  explicit QuestListModel(QObject* parent = nullptr);
  virtual ~QuestListModel() = default;

public:
  void addQuest(const QString& path);
  void addQuestFolder(const QString& path);
  void removeQuest(const QString& path);

  const QuestData& questData(const QString& path) const;

  const QString& currentQuest() const;
  void setCurrentQuest(const QString& path);
  Q_SIGNAL void currentQuestChanged(const QString& path);

  const QString& currentPlayingQuest() const;
  void setCurrentPlayingQuest(const QString& path);
  Q_SIGNAL void currentPlayingQuestChanged(const QString& index);

  QSortFilterProxyModel* proxyModel();

  QStringList questPathList() const;
  void setQuestPathList(const QStringList& list);

  int questRow(const QString& path) const;

public:
  int rowCount(const QModelIndex& parent) const override;
  QVariant data(const QModelIndex& index, int role) const override;

signals:
  void rowCountChanged();
  void questListChanged();

private:
  QList<QuestData> _quests;
  QFileSystemWatcher* _watcher{ nullptr };
  QString _currentQuest;
  QString _currentPlayingQuest;
  QSortFilterProxyModel* _proxyModel{ nullptr };
};
} // namespace solarus::launcher
