// SPDX-License-Identifier: GPL-3.0-or-later
#include <QWidget>
#include <QVariantAnimation>
#include <QIcon>

namespace solarus::launcher {
class DropArea : public QWidget {
  Q_OBJECT

public:
  explicit DropArea(QWidget* parent = nullptr);
  virtual ~DropArea() = default;

  void animateVisibility(bool visible);

protected:
  void paintEvent(QPaintEvent* event) override;

private:
  bool _visible{ false };
  QVariantAnimation _opacityAnimation;
  QPixmap _pixmapCache;
};
} // namespace solarus::launcher
