// SPDX-License-Identifier: GPL-3.0-or-later
#include <widgets/QuestPropertiesPanel.h>

#include <quests/QuestData.h>
#include <quests/QuestListModel.h>

#include <QAbstractTableModel>
#include <QVBoxLayout>
#include <QLabel>
#include <QTableView>
#include <QHeaderView>
#include <QScrollArea>
#include <QApplication>
#include <QScrollBar>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QMouseEvent>
#include <QDesktopServices>

#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <oclero/qlementine/utils/StateUtils.hpp>

namespace solarus::launcher {
enum TableColumn {
  Value,
  ColumnCount,
};

enum TableRow {
  Title,
  Authors,
  InitialReleaseDate,
  LatestReleaseDate,
  Version,
  EngineVersion,
  Licenses,
  Languages,
  Players,
  Genres,
  Website,
  Age,
  Controls,
  RowCount,
};

namespace i18n {
static QString ageToString(const Common::AgeRating value) {
  switch (value) {
    case Common::AgeRating::All:
      return QApplication::translate("SolarusLauncher", "All");
    case Common::AgeRating::Restricted:
      return QApplication::translate("SolarusLauncher", "Restricted");
    case Common::AgeRating::Warning:
      return QApplication::translate("SolarusLauncher", "Warning");
    default:
      return "";
  }
}

static QString controlToString(const Common::Control value) {
  switch (value) {
    case Common::Control::Joypad:
      return QApplication::translate("SolarusLauncher", "Joypad");
    case Common::Control::Keyboard:
      return QApplication::translate("SolarusLauncher", "Keyboard");
    case Common::Control::Mouse:
      return QApplication::translate("SolarusLauncher", "Mouse");
    case Common::Control::Other:
      return QApplication::translate("SolarusLauncher", "Other");
    default:
      return "";
  }
}

static QString tableRowHeader(const int row) {
  switch (row) {
    case TableRow::Title:
      return QApplication::translate("SolarusLauncher", "Title");
    case TableRow::Authors:
      return QApplication::translate("SolarusLauncher", "Authors");
    case TableRow::InitialReleaseDate:
      return QApplication::translate("SolarusLauncher", "Release Date");
    case TableRow::LatestReleaseDate:
      return QApplication::translate("SolarusLauncher", "Latest Update");
    case TableRow::Version:
      return QApplication::translate("SolarusLauncher", "Version");
    case TableRow::EngineVersion:
      return QApplication::translate("SolarusLauncher", "Solarus Version");
    case TableRow::Licenses:
      return QApplication::translate("SolarusLauncher", "License");
    case TableRow::Languages:
      return QApplication::translate("SolarusLauncher", "Languages");
    case TableRow::Players:
      return QApplication::translate("SolarusLauncher", "Players");
    case TableRow::Genres:
      return QApplication::translate("SolarusLauncher", "Genres");
    case TableRow::Website:
      return QApplication::translate("SolarusLauncher", "Website");
    case TableRow::Age:
      return QApplication::translate("SolarusLauncher", "Age");
    case TableRow::Controls:
      return QApplication::translate("SolarusLauncher", "Controls");
    default:
      return QString();
  }
}
} // namespace i18n

namespace {
static QString controlsToString(const Common::Controls controls) {
  QStringList strings;
  for (const auto control : {
         Common::Control::Keyboard,
         Common::Control::Mouse,
         Common::Control::Joypad,
         Common::Control::Other,
       }) {
    if (controls.testFlag(control)) {
      strings.append(i18n::controlToString(control));
    }
  }
  return strings.join(", ");
}

static QString playersToString(const int minPlayers, const int maxPlayers) {
  return maxPlayers == minPlayers ? (maxPlayers > 0 ? QString::number(maxPlayers) : QString())
                                  : QString("%1-%2").arg(minPlayers, maxPlayers);
}
} // namespace

class LinkDelegate : public QStyledItemDelegate {
private:
  QModelIndex pressedIndex;

public:
  explicit LinkDelegate(QObject* parent = nullptr)
    : QStyledItemDelegate(parent) {}

  void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override {
    auto linkColor = option.palette.link().color();
    if (const auto* qlementine = qobject_cast<oclero::qlementine::QlementineStyle*>(option.widget->style())) {
      // const auto mouseOver = option.state.testFlag(QStyle::State_MouseOver);
      const auto mouse = oclero::qlementine::getMouseState(option.state);
      linkColor = qlementine->color(mouse, oclero::qlementine::ColorRole::Primary);
    }

    const auto linkText = index.data(Qt::DisplayRole).toString();
    const auto contentRect = option.rect.marginsRemoved(QMargins(4, 0, 8, 0));
    const auto elidedText = option.fontMetrics.elidedText(linkText, Qt::TextElideMode::ElideRight, contentRect.width());

    painter->save();
    {
      auto font = painter->font();
      font.setUnderline(true);
      painter->setFont(font);
      painter->setPen(linkColor);
      painter->drawText(contentRect, Qt::AlignLeft | Qt::AlignVCenter, elidedText);
    }
    painter->restore();
  }

  bool editorEvent(
    QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) override {
    const auto type = event->type();
    switch (type) {
      case QEvent::MouseButtonPress: {
        const auto* mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
          pressedIndex = index;
        }
      } break;
      case QEvent::MouseButtonRelease: {
        const auto* mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
          if (pressedIndex == index) {
            pressedIndex = {};
            const auto link = index.data(Qt::DisplayRole).toString();
            QDesktopServices::openUrl(QUrl{ link });
            return true;
          } else {
            pressedIndex = {};
          }
        }
      } break;
      default:
        break;
    }

    return false;
  }
};

class CustomTableView : public QTableView {
private:
  QMap<int, Qt::CursorShape> _cursorForRowMap;

public:
  explicit CustomTableView(const QMap<int, Qt::CursorShape>& map, QWidget* parent = nullptr)
    : QTableView(parent)
    , _cursorForRowMap(map) {}

protected:
  bool viewportEvent(QEvent* event) override {
    if (event->type() == QEvent::MouseMove) {
      const auto* mouseEvent = static_cast<QMouseEvent*>(event);
      const auto index = indexAt(mouseEvent->pos());
      if (index.isValid()) {
        const auto row = index.row();
        const auto it = _cursorForRowMap.constFind(row);
        if (it != _cursorForRowMap.constEnd()) {
          setCursor(it.value());
        } else {
          unsetCursor();
        }
      } else {
        unsetCursor();
      }
    }
    return QTableView::viewportEvent(event);
  }
};

class QuestPropsModel : public QAbstractTableModel {
public:
  using QAbstractTableModel::QAbstractTableModel;

public:
  QuestData quest;

  void setQuest(const QuestData& quest) {
    this->quest = quest;
    const auto index1 = index(TableRow(), TableColumn::Value);
    const auto index2 = index(TableRow::RowCount - 1, TableColumn::Value);
    emit dataChanged(index1, index2);
  }

  int rowCount(const QModelIndex& parent) const override {
    return TableRow::RowCount;
  }

  int columnCount(const QModelIndex& parent) const override {
    return TableColumn::ColumnCount;
  }

  QVariant data(const QModelIndex& index, int role) const override {
    switch (role) {
      case Qt::ItemDataRole::DisplayRole:
        if (index.column() == TableColumn::Value) {
          return rowValue(index.row());
        }
        break;
      case Qt::ItemDataRole::DecorationRole:
        if (index.column() == TableColumn::Value) {
          return rowValueIcon(index.row());
        }
        break;
      default:
        break;
    }

    return {};
  }

  QString rowValue(int row) const {
    if (!quest.isValid)
      return QString();

    switch (row) {
      case TableRow::Title:
        return quest.title;
      case TableRow::Authors:
        return quest.authors.join(", ");
      case TableRow::InitialReleaseDate:
        return quest.initialReleaseDate.toString(Qt::DateFormat::ISODateWithMs);
      case TableRow::LatestReleaseDate:
        return quest.latestReleaseDate.toString(Qt::DateFormat::ISODateWithMs);
      case TableRow::Version:
        return quest.version.toString();
      case TableRow::EngineVersion:
        return quest.engineVersion.toString();
      case TableRow::Licenses:
        return quest.licenses.join(", ");
      case TableRow::Languages:
        return quest.languages.join(", ");
      case TableRow::Players:
        return playersToString(quest.minPlayers, quest.maxPlayers);
      case TableRow::Genres:
        return quest.genres.join(", ");
      case TableRow::Website:
        return quest.website.toString();
      case TableRow::Age:
        return i18n::ageToString(quest.ageRating);
      case TableRow::Controls:
        return controlsToString(quest.controls);
      default:
        return QString();
    }
  }

  QIcon rowValueIcon(int row) const {
    if (!quest.isValid)
      return QIcon();

    if (row == TableRow::Age) {
      return Common::ageIcon(quest.ageRating);
    }
    return QIcon();
  }

  QVariant headerData(int section, Qt::Orientation orientation, int role) const override {
    if (role == Qt::ItemDataRole::DisplayRole && orientation == Qt::Orientation::Vertical) {
      return i18n::tableRowHeader(section);
    }
    return {};
  }
};

QuestPropertiesPanel::QuestPropertiesPanel(QuestListModel* model, QWidget* parent)
  : QScrollArea(parent)
  , _model(model) {
  setupUi();
}

void QuestPropertiesPanel::setupUi() {
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
  setFixedWidth(300);
  setWidgetResizable(true);

  auto* scrollAreaContent = new QWidget(this);
  setWidget(scrollAreaContent);

  auto* layout = new QVBoxLayout(scrollAreaContent);
  layout->setSpacing(0);
  layout->setContentsMargins(0, 0, 0, 0);
  scrollAreaContent->setLayout(layout);

  // Quest thumbnail.
  constexpr auto thumbnailRatio = 360. / 700.;
  const auto thumbnailWidth = width();
  _thumbnailLabel = new QLabel(scrollAreaContent);
  _thumbnailLabel->setFixedSize(thumbnailWidth, thumbnailWidth * thumbnailRatio);
  _thumbnailLabel->setScaledContents(true);
  layout->addWidget(_thumbnailLabel);
  layout->setAlignment(_thumbnailLabel, Qt::AlignHCenter);

  // Quest description.
  _descriptionLabel = new QLabel(scrollAreaContent);
  _descriptionLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  _descriptionLabel->setMargin(16);
  _descriptionLabel->setWordWrap(true);
  layout->addWidget(_descriptionLabel);

  // Quest properties.
  _tableModel = new QuestPropsModel(this);

  _tableView = new CustomTableView(
    {
      { TableRow::Website, Qt::PointingHandCursor },
    },
    scrollAreaContent);
  if (auto* qlementine = qobject_cast<oclero::qlementine::QlementineStyle*>(_tableView->style())) {
    qlementine->setAutoIconColor(_tableView, oclero::qlementine::AutoIconColor::None);
  }

  _tableView->setSizeAdjustPolicy(QTableView::SizeAdjustPolicy::AdjustIgnored);
  _tableView->setFixedWidth(width());
  _tableView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  _tableView->setSortingEnabled(false);
  _tableView->setCornerButtonEnabled(false);
  _tableView->horizontalHeader()->hide();
  _tableView->verticalHeader()->show();
  _tableView->setWordWrap(true);
  _tableView->setModel(_tableModel);
  _tableView->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);
  _tableView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
  _tableView->setDragEnabled(false);
  _tableView->setVerticalScrollMode(QAbstractItemView::ScrollMode::ScrollPerPixel);
  _tableView->setTextElideMode(Qt::TextElideMode::ElideNone);
  _tableView->setLineWidth(0);
  _tableView->setAlternatingRowColors(true);
  _tableView->setFrameShape(QFrame::Shape::NoFrame);
  _tableView->setFrameShadow(QFrame::Shadow::Plain);

  layout->addWidget(_tableView);

  auto* linkDelegate = new LinkDelegate(_tableView);
  _tableView->setItemDelegateForRow(TableRow::Website, linkDelegate);

  updateUi(_model->currentQuest());
  QObject::connect(_model, &QuestListModel::currentQuestChanged, this, &QuestPropertiesPanel::updateUi);
}

void QuestPropertiesPanel::updateUi(const QString& path) {
  const auto& quest = _model->questData(path);
  _tableModel->setQuest(quest);
  _thumbnailLabel->setPixmap(_tableModel->quest.thumbnail);
  _descriptionLabel->setText(quest.description);

  if (auto* hHeader = _tableView->horizontalHeader()) {
    hHeader->setSectionResizeMode(TableColumn::Value, QHeaderView::Stretch);
  }
  if (auto* vHeader = _tableView->verticalHeader()) {
    vHeader->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
  }

  _tableView->resizeColumnsToContents();
  _tableView->resizeRowsToContents();
  _tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  _tableView->setFixedHeight(_tableView->verticalHeader()->length() + _tableView->horizontalHeader()->height());

  setEnabled(quest.isValid);
  verticalScrollBar()->setValue(0);
}
} // namespace solarus::launcher
