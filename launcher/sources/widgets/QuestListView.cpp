// SPDX-License-Identifier: GPL-3.0-or-later
#include <widgets/QuestListView.h>

#include <Utils.h>
#include <Preferences.h>
#include <Controller.h>
#include <quests/QuestListItemDelegate.h>
#include <quests/QuestListModel.h>
#include <quests/QuestRunner.h>

#include <QMouseEvent>
#include <QSortFilterProxyModel>
#include <QMenu>
#include <QApplication>
#include <QPainter>

#include <oclero/qlementine/style/QlementineStyle.hpp>

namespace solarus::launcher {
namespace i18n {
static QString removeQuest() {
  return QApplication::translate("SolarusLauncher", "Remove Quest");
}
static QString playQuest() {
  return QApplication::translate("SolarusLauncher", "Play Quest");
}
static QString stopQuest() {
  return QApplication::translate("SolarusLauncher", "Stop Quest");
}
static QString showQuestInformation() {
  return QApplication::translate("SolarusLauncher", "Show Quest Information");
}
static QString showContaingFolder() {
  return QApplication::translate("SolarusLauncher", "Open Containing Folder");
}
static QString noQuestAdded() {
  return QApplication::translate("SolarusLauncher", "No Solarus Quests added yet.");
}
} // namespace i18n

QuestListView::QuestListView(Controller* controller, QWidget* parent)
  : QListView(parent)
  , _controller(controller) {
  setupUi();
}

void QuestListView::setupUi() {
  ensurePolished();
  setFlow(QListView::Flow::LeftToRight);
  setSpacing(16);
  setResizeMode(QListView::ResizeMode::Adjust);
  setViewMode(QListView::ViewMode::IconMode);
  setDragEnabled(false);
  setItemAlignment(Qt::AlignCenter);
  setMovement(QListView::Movement::Static);
  setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
  setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectItems);
  setWrapping(true);
  setContentsMargins(0, 0, 0, 0);
  setVerticalScrollMode(QAbstractItemView::ScrollMode::ScrollPerPixel);
  setUniformItemSizes(true);

  // Delegate.
  auto* listDelegate = new QuestListItemDelegate(this);
  setItemDelegate(listDelegate);

  // Model.
  auto* model = _controller->model();
  setModel(model->proxyModel());

  setCurrentIndex(model->proxyModel()->index(0, 0));

  // Synchronize everyone.
  QObject::connect(selectionModel(), &QItemSelectionModel::currentRowChanged, this,
    [this](const QModelIndex& current, const QModelIndex&) {
      const auto path = this->model()->data(current, QuestListModel::DataRole::QuestPath).toString();
      _controller->model()->setCurrentQuest(path);
    });

  QObject::connect(model, &QuestListModel::currentQuestChanged, this, [this](const QString& path) {
    const auto sourceRow = _controller->model()->questRow(path);
    const auto sourceIndex = _controller->model()->index(sourceRow);
    if (sourceIndex.isValid()) {
      const auto proxyIndex = _controller->model()->proxyModel()->mapFromSource(sourceIndex);
      setCurrentIndex(proxyIndex);
    }
  });

  // Commands.
  QObject::connect(this, &QListView::doubleClicked, this, [this](const QModelIndex& index) {
    const auto path = this->model()->data(index, QuestListModel::DataRole::QuestPath).toString();
    _controller->playQuest(path);
  });

  // Quest's context menu.
  setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
  QObject::connect(this, &QListView::customContextMenuRequested, this, [this](const QPoint& pos) {
    const auto index = indexAt(pos);
    if (index.isValid()) {
      const auto questPath = this->model()->data(index, QuestListModel::DataRole::QuestPath).toString();
      const auto isPlayingQuest = questPath == _controller->model()->currentPlayingQuest();

      QMenu menu(this);
      if (isPlayingQuest) {
        auto* stopAction = new QAction(makeIcon(Icons16::Media_Stop), i18n::stopQuest(), &menu);
        // stopAction->setShortcut(QKeySequence(Qt::Key_F5));
        menu.addAction(stopAction);
        QObject::connect(stopAction, &QAction::triggered, this, [this]() {
          _controller->stopQuest();
        });
      } else {
        auto* playAction = new QAction(makeIcon(Icons16::Media_Play), i18n::playQuest(), &menu);
        playAction->setShortcut(QKeySequence(Qt::Key_F5));
        menu.addAction(playAction);
        QObject::connect(playAction, &QAction::triggered, this, [this, questPath]() {
          _controller->playQuest(questPath);
        });
      }
      menu.addSeparator();
      {
        auto* infoAction = new QAction(makeIcon(Icons16::Misc_Info), i18n::showQuestInformation(), &menu);
        menu.addAction(infoAction);
        QObject::connect(infoAction, &QAction::triggered, this, [this, index]() {
          setCurrentIndex(index);
          _controller->preferences()->setAppPropertiesPanelVisible(true);
        });
      }
      {
        auto* folderAction = new QAction(makeIcon(Icons16::File_FolderOpen), i18n::showContaingFolder(), &menu);
        menu.addAction(folderAction);
        QObject::connect(folderAction, &QAction::triggered, this, [this, questPath]() {
          _controller->openQuestFolder(questPath);
        });
      }
      menu.addSeparator();
      {
        auto* removeAction = new QAction(makeIcon(Icons16::Action_Trash), i18n::removeQuest(), &menu);
#ifdef __APPLE__
        removeAction->setShortcut(Qt::Key_Backspace);
#else
        removeAction->setShortcut(QKeySequence::StandardKey::Delete);
#endif
        menu.addAction(removeAction);
        QObject::connect(removeAction, &QAction::triggered, this, [this, questPath]() {
          _controller->removeQuest(questPath);
        });
      }

      const auto globalPos = mapToGlobal(pos);
      menu.exec(globalPos);
    }
  });
}

void QuestListView::mousePressEvent(QMouseEvent* event) {
  const auto index = indexAt(event->pos());
  if (!index.isValid()) {
    event->ignore();
    if (focusPolicy() & Qt::FocusPolicy::ClickFocus) {
      setFocus(Qt::FocusReason::MouseFocusReason);
    }
    return;
  }

  QListView::mousePressEvent(event);
}

void QuestListView::mouseReleaseEvent(QMouseEvent* event) {
  const auto index = indexAt(event->pos());
  if (index.isValid()) {
    const auto unselect_shortcut = event->modifiers() == Qt::ControlModifier;
    if (unselect_shortcut) {
      // Ignore CTRL+click.
      event->ignore();
      return;
    }
  } else {
    // Ignore clicks on empty areas.
    event->ignore();
    if (focusPolicy() & Qt::FocusPolicy::ClickFocus) {
      setFocus(Qt::FocusReason::MouseFocusReason);
    }
    return;
  }

  QListView::mouseReleaseEvent(event);
}

void QuestListView::mouseMoveEvent(QMouseEvent* event) {
  if (event->buttons() != Qt::NoButton) {
    // Ignore clicks on empty areas.
    event->ignore();
    return;
  }
  QListView::mouseMoveEvent(event);
}

void QuestListView::currentChanged(const QModelIndex& current, const QModelIndex& previous) {
  QListView::currentChanged(current, previous);
  const auto path = model()->data(current, QuestListModel::DataRole::QuestPath).toString();
  _controller->model()->setCurrentQuest(path);
}

void QuestListView::keyPressEvent(QKeyEvent* event) {
  if (!hasFocus() || !isActiveWindow())
    return;

  const auto key = event->key();
  if (key == Qt::Key_Enter || key == Qt::Key_Return || key == Qt::Key_Space) {
    _pressedKey = key;
  } else {
    QListView::keyPressEvent(event);
  }
}

void QuestListView::keyReleaseEvent(QKeyEvent* event) {
  if (!hasFocus() || !isActiveWindow())
    return;

  const auto key = event->key();
  if (key == _pressedKey && (key == Qt::Key_Enter || key == Qt::Key_Return || key == Qt::Key_Space)
      && !event->isAutoRepeat()) {
    const auto currentIndex = this->currentIndex();
    if (currentIndex.isValid()) {
      const auto path = model()->data(currentIndex, QuestListModel::DataRole::QuestPath).toString();
      _controller->playQuest(path);
    }
  } else {
    QListView::keyReleaseEvent(event);
  }

  _pressedKey = Qt::Key::Key_unknown;
}

void QuestListView::paintEvent(QPaintEvent* event) {
  QListView::paintEvent(event);

  if (model() && model()->rowCount() == 0) {
    QPainter painter(viewport());
    painter.setRenderHint(QPainter::RenderHint::Antialiasing, true);

    const auto* qlementine = qobject_cast<oclero::qlementine::QlementineStyle*>(style());
    const auto& theme = qlementine ? qlementine->theme() : oclero::qlementine::Theme{};

    const auto rect = viewport()->rect();
    const auto titleText = i18n::noQuestAdded();
    const auto& titleFont = theme.fontH2;
    const auto& titleColor = theme.secondaryAlternativeColor;
    const auto titleRect = rect;

    painter.setBrush(Qt::NoBrush);
    painter.setPen(titleColor);
    painter.setFont(titleFont);
    painter.drawText(titleRect, titleText, { Qt::AlignCenter });
  }
}
} // namespace solarus::launcher
