// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <oclero/qlementine/icons/Icons16.hpp>
#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <QIcon>

using Icons16 = oclero::qlementine::icons::Icons16;

/**
 * @brief Convenient function to make an icon from the given Qlementine ID.
 * @param id The ID of the icon.
 * @param macOSMenu Whether the icon is for a macOS menu.
 * @param size The size of the icon.
 * @return QIcon The icon.
 */
QIcon makeIcon(Icons16 id, bool macOSMenu = false, const QSize& size = { 16, 16 });
