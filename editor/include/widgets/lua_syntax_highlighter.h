/*
 * Copyright (C) 2014-2018 Christopho, Solarus - http://www.solarus-games.org
 *
 * Solarus Quest Editor is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Solarus Quest Editor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef SOLARUSEDITOR_LUA_SYNTAX_HIGHLIGHTER_H
#define SOLARUSEDITOR_LUA_SYNTAX_HIGHLIGHTER_H

#include <QRegularExpression>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QVector>

namespace SolarusEditor {

/**
 * @brief A simple syntax highlighter for Lua code.
 *
 * This class is highly inspired from Qt syntax highlighter example:
 * http://qt-project.org/doc/qt-4.8/richtext-syntaxhighlighter.html
 */
class LuaSyntaxHighlighter : public QSyntaxHighlighter {
    Q_OBJECT

public:

  /**
   * @brief List of colors for Lua syntax highlighting depending on the theme.
   */
  struct SyntaxColors {
    QColor keyword_color;  /**< Color for Lua keywords. */
    QColor string_color;   /**< Color for literal strings. */
    QColor comment_color;  /**< Color for comments. */
  };

  explicit LuaSyntaxHighlighter(QTextDocument* document = nullptr);

protected:

  void highlightBlock(const QString& text) override;

private:

  void update_colors();
  void create_rules();

  /**
   * @brief A single-line highlighting rule.
   */
  struct HighlightingRule {
    QRegularExpression pattern;                             /**< A regexp. If it contains a capture, only the
                                                  * captured text will be highlighted. */
    QTextCharFormat format;                      /**< Format to apply to text matching the regexp. */
  };

  QVector<HighlightingRule> rules;               /**< The rule set. */
  QRegularExpression comment_start_pattern;                 /**< Regexp of multi-line comments start. */
  QRegularExpression comment_end_pattern;                   /**< Regexp of multi-line comments end. */
};

}

#endif
