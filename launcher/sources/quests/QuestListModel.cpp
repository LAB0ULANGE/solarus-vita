// SPDX-License-Identifier: GPL-3.0-or-later
#include <quests/QuestListModel.h>

#include <QPixmap>
#include <QStringList>
#include <QRegularExpression>
#include <QPainter>
#include <QApplication>
#include <QDir>
#include <QFileSystemWatcher>
#include <QSortFilterProxyModel>

#include <string>
#include <vector>

#include <solarus/core/CurrentQuest.h>
#include <solarus/core/Debug.h>
#include <solarus/core/QuestFiles.h>
#include <solarus/core/QuestProperties.h>
#include <solarus/core/SolarusFatal.h>

namespace solarus::launcher {
namespace {
QStringList toStringList(const std::vector<std::string>& vector) {
  QStringList result;
  result.reserve(vector.size());
  std::transform(vector.begin(), vector.end(), std::back_inserter(result), [](const std::string& str) {
    return QString::fromStdString(str);
  });
  return result;
}

QPixmap loadPixmap(const std::string& fileNameInQuestFiles) {
  QPixmap result;
  if (Solarus::QuestFiles::data_file_exists(fileNameInQuestFiles)
      && !Solarus::QuestFiles::data_file_is_dir(fileNameInQuestFiles)) {
    const auto buffer = Solarus::QuestFiles::data_file_read(fileNameInQuestFiles);
    result.loadFromData(reinterpret_cast<const uchar*>(buffer.data()), static_cast<uint>(buffer.size()));
  }
  return result;
}

QPixmap getThumnailFromLogo(const QPixmap& logo) {
  if (logo.isNull())
    return {};

  constexpr auto thumbnailW = 700;
  constexpr auto thumbnailH = 360;
  constexpr auto thumbnailPadding = 8;
  constexpr auto logoW = thumbnailW - thumbnailPadding * 2;
  constexpr auto logoH = thumbnailH - thumbnailPadding * 2;

  QPixmap result{ thumbnailW, thumbnailH };
  result.fill(Qt::transparent);

  QPainter p(&result);
  p.setRenderHint(QPainter::Antialiasing, true);

  const auto newSize = (QSizeF(logoW, logoH) * logo.devicePixelRatioF()).toSize();
  const auto resizedLogo = logo.scaled(newSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  const auto resizedLogoSize = resizedLogo.deviceIndependentSize();
  const auto logoX = (thumbnailW - resizedLogoSize.width()) / 2.;
  const auto logoY = (thumbnailH - resizedLogoSize.height()) / 2.;
  const auto logoRect = QRectF{ QPointF{ logoX, logoY }, resizedLogoSize };
  p.drawPixmap(logoRect.toRect(), resizedLogo);
  return result;
}

QPixmap getQuestThumbnail() {
  // First, try to get the actual thumbnail.
  static const auto thumbnail_file_name = std::string{ "logos/thumbnail.png" };
  const auto thumbnail = loadPixmap(thumbnail_file_name);
  if (!thumbnail.isNull())
    return thumbnail;

  // Else, try the fallback pictures, ordered by priority.
  static const auto fallback_file_names = std::array<std::string, 3>{
    "logos/logo_2x.png",
    "logos/logo@2x.png",
    "logos/logo.png",
  };
  QPixmap logo;
  for (const auto& file_name : fallback_file_names) {
    logo = loadPixmap(file_name);
    if (!logo.isNull())
      break;
  }

  // We reduce the logo size to make sure it fits the bounds entirely.
  return getThumnailFromLogo(logo);
}

Common::Controls getControls(const Solarus::FlagSet<Solarus::QuestProperties::Control>& controls) {
  using Control = Solarus::QuestProperties::Control;

  // TODO: check if there is a better way to have this enum in a QVariant.
  // Idealy, use directly the enum from Solarus.
  Common::Controls result;
  for (const auto control : { Control::Keyboard, Control::Mouse, Control::Joypad, Control::Other }) {
    if (controls.has_flag(control)) {
      result.setFlag(static_cast<Common::Control>(control));
    }
  }
  return result;
}

void initializeFromProperties(QuestData& questData, const Solarus::QuestProperties& properties) {
  static const QRegularExpression listSplitRE("\\s*,\\s*");

  questData.title = QString::fromStdString(properties.get_title());
  questData.authors =
    QString::fromStdString(properties.get_author()).split(listSplitRE, Qt::SplitBehaviorFlags::SkipEmptyParts);
  questData.initialReleaseDate =
    QDate::fromString(QString::fromStdString(properties.get_initial_release_date()), QStringLiteral("yyyyMMdd"));
  questData.latestReleaseDate =
    QDate::fromString(QString::fromStdString(properties.get_release_date()), QStringLiteral("yyyyMMdd"));
  questData.description = QString::fromStdString(properties.get_long_description());
  questData.version = QVersionNumber::fromString(QString::fromStdString(properties.get_quest_version()));
  questData.engineVersion = QVersionNumber::fromString(QString::fromStdString(properties.get_solarus_version()));
  questData.licenses =
    QString::fromStdString(properties.get_license()).split(listSplitRE, Qt::SplitBehaviorFlags::SkipEmptyParts);
  questData.minPlayers = properties.get_min_players();
  questData.maxPlayers = properties.get_max_players();
  questData.languages = toStringList(properties.get_languages());
  questData.genres = toStringList(properties.get_genres());
  questData.controls = getControls(properties.get_controls());
  questData.website = QString::fromStdString(properties.get_website());
  questData.id = QString::fromStdString(properties.get_quest_write_dir());

  questData.isValid = true;
}

QuestData makeQuestData(const QString& path) {
  QuestData result;
  result.path = path;

  // Open the quest to get its quest.dat file.
  const auto arguments = QApplication::arguments();
  const auto& program_name = arguments.isEmpty() ? QString{} : arguments.first();

  // Prevent Solarus from closing the launcher.
  Solarus::Debug::set_die_on_error(false);
  Solarus::Debug::set_show_popup_on_die(false);
  Solarus::Debug::set_abort_on_die(false);

  try {
    if (Solarus::QuestFiles::open_quest(
        std::string(program_name.toUtf8().constData()),
        std::string(path.toUtf8().constData()))) {
      // Load all properties.
      const auto properties = Solarus::QuestProperties{ Solarus::CurrentQuest::get_properties() };
      initializeFromProperties(result, properties);

      // Load thumbnail.
      result.thumbnail = getQuestThumbnail();
    }
    Solarus::QuestFiles::close_quest();
  } catch (const Solarus::SolarusFatal&) {
    // Solarus always throws an exception when loading a quest isn't successful.
    // Let's just catch it, and ignore this quest.
  }

  return result;
}
} // namespace

QuestListModel::QuestListModel(QObject* parent)
  : QAbstractListModel(parent) {
  _watcher = new QFileSystemWatcher(this);

  _proxyModel = new QSortFilterProxyModel(this);
  _proxyModel->setSourceModel(this);
  _proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
  _proxyModel->setSortRole(Qt::DisplayRole);
  _proxyModel->setDynamicSortFilter(true);
  _proxyModel->sort(0, Qt::AscendingOrder);

  QObject::connect(_watcher, &QFileSystemWatcher::fileChanged, this, [this](const QString& path) {
    if (QFile::exists(path)) {
      addQuest(path);
    } else {
      removeQuest(path);
    }
  });
}

void QuestListModel::addQuest(const QString& path) {
  const auto questData = makeQuestData(path);
  if (!questData.isValid)
    return;

  // Check if already in the list. If not, add it. Else, replace it.
  const auto it = std::find_if(_quests.begin(), _quests.end(), [&questData](const QuestData& other) {
    return /*other.id == questData.id ||*/ other.path == questData.path;
  });
  if (it == _quests.end()) {
    _watcher->addPath(path);

    const auto row = static_cast<int>(_quests.size());
    beginInsertRows({}, row, row);
    _quests.emplace_back(questData);
    endInsertRows();

    emit rowCountChanged();
  } else {
    const auto row = std::distance(_quests.begin(), it);
    _quests[row] = questData;
    const auto modelIndex = index(row);
    emit dataChanged(modelIndex, modelIndex);
  }

  emit questListChanged();
}

void QuestListModel::addQuestFolder(const QString& path) {
  QDir dir(path);
  if (dir.exists()) {
    const auto filePaths =
      dir.entryList({ "*.solarus" }, QDir::Filter::NoDotAndDotDot | QDir::Filter::Files | QDir::Filter::Readable,
        QDir::SortFlag::IgnoreCase | QDir::SortFlag::Name);
    for (const auto& filePath : filePaths) {
      const auto absoluteFilePath = dir.absoluteFilePath(filePath);
      addQuest(absoluteFilePath);
    }
  }
}

void QuestListModel::removeQuest(const QString& path) {
  const auto row = questRow(path);
  if (row >= 0 && row < _quests.size()) {
    beginRemoveRows({}, row, row);
    _quests.removeAt(row);
    endRemoveRows();

    emit rowCountChanged();
    emit questListChanged();
  }
}

const QuestData& QuestListModel::questData(const QString& path) const {
  static const QuestData invalid;
  const auto row = questRow(path);
  if (row >= 0 && row < static_cast<int>(_quests.size())) {
    const auto& quest = _quests.at(row);
    return quest;
  }
  return invalid;
}

int QuestListModel::rowCount(const QModelIndex&) const {
  return static_cast<int>(_quests.size());
}

QVariant QuestListModel::data(const QModelIndex& index, int role) const {
  if (!index.isValid())
    return {};

  const auto row = index.row();
  if (row < 0 || row >= static_cast<int>(_quests.size()))
    return {};

  const auto& quest = _quests.at(row);
  switch (role) {
    case static_cast<int>(Qt::DisplayRole):
    case static_cast<int>(Qt::ToolTipRole):
      return QVariant::fromValue(quest.title);
    case static_cast<int>(Qt::DecorationRole):
      return QVariant::fromValue(quest.thumbnail);
    case static_cast<int>(DataRole::QuestPath):
      return QVariant::fromValue(quest.path);
    case static_cast<int>(DataRole::IsPlaying):
      return QVariant::fromValue(quest.path == _currentPlayingQuest);
    default:
      return {};
  }
}

const QString& QuestListModel::currentQuest() const {
  return _currentQuest;
}

void QuestListModel::setCurrentQuest(const QString& path) {
  if (path != _currentQuest) {
    _currentQuest = path;
    emit currentQuestChanged(_currentQuest);
  }
}

const QString& QuestListModel::currentPlayingQuest() const {
  return _currentPlayingQuest;
}

void QuestListModel::setCurrentPlayingQuest(const QString& path) {
  if (path != _currentPlayingQuest) {
    const auto oldIndex = index(questRow(_currentPlayingQuest));
    _currentPlayingQuest = path;
    emit currentPlayingQuestChanged(_currentPlayingQuest);

    // Signal for old and new one.
    if (oldIndex.isValid()) {
      emit dataChanged(oldIndex, oldIndex);
    }
    const auto newIndex = index(questRow(_currentPlayingQuest));
    emit dataChanged(newIndex, newIndex);
  }
}

QSortFilterProxyModel* QuestListModel::proxyModel() {
  return _proxyModel;
}

int QuestListModel::questRow(const QString& path) const {
  const auto it = std::find_if(_quests.begin(), _quests.end(), [&path](const QuestData& other) {
    return other.path == path;
  });
  if (it != _quests.end()) {
    const auto row = std::distance(_quests.begin(), it);
    return row;
  }
  return -1;
}

QStringList QuestListModel::questPathList() const {
  QStringList result(_quests.count());
  for (auto i = 0; i < result.count(); ++i) {
    result[i] = _quests.at(i).path;
  }
  return result;
}

void QuestListModel::setQuestPathList(const QStringList& list) {
  beginResetModel();
  for (const auto& path : std::as_const(list)) {
    QSignalBlocker _(this);
    addQuest(path);
  }
  endResetModel();
  emit rowCountChanged();
  emit questListChanged();
}
} // namespace solarus::launcher
