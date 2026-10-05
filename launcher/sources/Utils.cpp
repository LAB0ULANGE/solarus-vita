// SPDX-License-Identifier: GPL-3.0-or-later
#include <Utils.h>

QIcon makeIcon(Icons16 id, bool macOSMenu, const QSize& size) {
  if (macOSMenu)
    return QIcon{};

  const auto svgPath = oclero::qlementine::icons::iconPath(id);
  if (auto* style = oclero::qlementine::appStyle()) {
    return style->makeThemedIcon(svgPath, size);
  } else {
    return QIcon(svgPath);
  }
}
