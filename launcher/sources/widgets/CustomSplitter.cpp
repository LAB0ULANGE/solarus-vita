// SPDX-License-Identifier: GPL-3.0-or-later
#include <widgets/CustomSplitter.h>

#include <QSplitterHandle>
#include <QMouseEvent>

namespace solarus::launcher {
class CustomSplitterHandle : public QSplitterHandle {
  using QSplitterHandle::QSplitterHandle;

public:
  std::function<void()> onMouseReleased;

protected:
  void mouseReleaseEvent(QMouseEvent* event) override {
    QSplitterHandle::mouseReleaseEvent(event);

    if (event->button() == Qt::LeftButton && onMouseReleased) {
      onMouseReleased();
    }
  }
};

QSplitterHandle* CustomSplitter::createHandle() {
  auto* handle = new CustomSplitterHandle(orientation(), this);
  handle->onMouseReleased = [this]() {
    emit handleMouseReleased();
  };
  return handle;
}
} // namespace solarus::launcher
