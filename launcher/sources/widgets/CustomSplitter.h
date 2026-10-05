// SPDX-License-Identifier: GPL-3.0-or-later
#include <QSplitter>

namespace solarus::launcher {
class CustomSplitter : public QSplitter {
  Q_OBJECT

public:
  using QSplitter::QSplitter;

  Q_SIGNAL void handleMouseReleased();

protected:
  QSplitterHandle* createHandle() override;
};
} // namespace solarus::launcher
