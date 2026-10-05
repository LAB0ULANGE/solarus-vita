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
#include "widgets/text_editor.h"
#include "widgets/text_editor_widget.h"
#include "editor_style.h"
#include <QApplication>
#include <QDebug>
#include <QMenu>
#include <QPainter>
#include <QTextBlock>
#include <QUndoStack>

namespace SolarusEditor {

namespace {

/**
 * @brief Undo command that acts as proxy to trigger undo/redo on a text widget.
 *
 * This class is needed because QPlainTextWidget does not know the QUndoCommand
 * framework (it has its own undo/redo implementation instead).
 */
class UndoCommandProxy : public QUndoCommand {

public:

  /**
   * @brief Creates an undo command proxy.
   * @param text_editor_widget The text editor widget to forward undo/redo
   * commands to.
   */
  explicit UndoCommandProxy(TextEditorWidget& text_editor_widget):
    QUndoCommand("text"),
    text_editor_widget(text_editor_widget),
    first_time(true) {

  }

  /**
   * @brief Forwards an undo command to the text editor.
   */
  virtual void undo() override {

    text_editor_widget.undo();
  }

  /**
   * @brief Forwards an redo command to the text editor.
   *
   * Does nothing the first time, because it is not a real redo,
   * it corresponds to the initial action.
   * This initial action is done before creating the UndoCommandProxy.
   */
  virtual void redo() override {

    if (first_time) {
      // Not a real redo.
      first_time = false;
      return;
    }

    text_editor_widget.redo();
  }

private:

  TextEditorWidget& text_editor_widget;     /**< The text editor widget to
                                             * forward undo/redo commands to. */
  bool first_time;                          /**< \c true if redo has not been called yet.*/
};

/**
 * @brief Vertical widget showing line numbers.
 */
class LineNumberArea : public QWidget {

public:

  /**
   * @brief Constructor.
   * @param text_editor_widget The text editor to show line numbers of.
   */
  explicit LineNumberArea(TextEditorWidget& text_editor_widget) :
    QWidget(&text_editor_widget),
    text_editor_widget(text_editor_widget) {
  }

  /**
   * @brief Returns an appropriate size for this line number area.
   * @return The size hint.
   */
  QSize sizeHint() const override {
    return QSize(text_editor_widget.get_line_number_area_width(), 0);
  }

protected:

  /**
   * @brief Draws this line number area.
   * @param event The paint event to handle.
   */
  void paintEvent(QPaintEvent* event) override {
    text_editor_widget.line_number_area_paint_event(event);
  }

private:

  TextEditorWidget& text_editor_widget;     /**< The text editor to show line numbers of. */
};

}

/**
 * @brief Constructor.
 * @param file_path Path of the file to open.
 * @param editor The parent object.
 */
TextEditorWidget::TextEditorWidget(const QString& file_path, TextEditor& editor):
  QPlainTextEdit(file_path, &editor),
  line_number_area(new LineNumberArea(*this)),
  undo_stack(editor.get_undo_stack()),
  tab_length(2),
  replace_tab_by_spaces(false) {

  // Undo/redo system.
  connect(document(), &QTextDocument::undoCommandAdded,
          this, &TextEditorWidget::undo_command_added);

  // Line number displaying.
  connect(this, &QPlainTextEdit::blockCountChanged,
          this, &TextEditorWidget::update_line_number_area_width);
  connect(this, &QPlainTextEdit::updateRequest,
          this, &TextEditorWidget::update_line_number_area);
  connect(this, &QPlainTextEdit::cursorPositionChanged,
          this, &TextEditorWidget::highlight_current_line);

  const EditorStyle* style = qobject_cast<const EditorStyle*>(qApp->style());
  if (style != nullptr) {
    connect(style, &EditorStyle::actual_mode_changed,
            this, &TextEditorWidget::highlight_current_line);
  }

  update_line_number_area_width(0);
  highlight_current_line();

  setFrameStyle(QFrame::Shape::Panel | QFrame::Shadow::Sunken);
}

/**
 * @brief Slot called when the text edit creates an internal undo command.
 *
 * This is called for example when the user enters or deletes some text.
 * A special command is added to the undo stack to reflect it.
 */
void TextEditorWidget::undo_command_added() {

  undo_stack.push(new UndoCommandProxy(*this));
}

/**
 * @brief Returns the width needed for the line number area.
 *
 * It depends on the number of lines.
 *
 * @return The line number area width.
 */
int TextEditorWidget::get_line_number_area_width() {

  int digits = 1;
  int max = qMax(1, blockCount());
  while (max >= 10) {
    max /= 10;
    ++digits;
  }

  ++digits;  // Add space equivalent to an extra digit to the right.

  int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

  return space;
}

/**
 * @brief Slot called when the number of blocks has changed.
 *
 * Updates the viewport margin depending on the line number area width.
 *
 * @param new_block_count The new block count.
 */
void TextEditorWidget::update_line_number_area_width(int new_block_count) {

  Q_UNUSED(new_block_count);
  setViewportMargins(get_line_number_area_width(), 0, 0, 0);
}

/**
 * @brief Called when the viewport has been scrolled.
 * @param rect Region to be redrawn.
 * @param dy Number of pixels scrolled vertically.
 */
void TextEditorWidget::update_line_number_area(const QRect& rect, int dy) {

  if (dy != 0) {
    line_number_area->scroll(0, dy);
  }
  else {
    line_number_area->update(0, rect.y(), line_number_area->width(), rect.height());
  }

  if (rect.contains(viewport()->rect())) {
    update_line_number_area_width(0);
  }
}

/**
 * @brief Receives a resize event.
 *
 * Updates the line number area.
 *
 * @param event The event to handle.
 */
void TextEditorWidget::resizeEvent(QResizeEvent* event) {
  QPlainTextEdit::resizeEvent(event);

  QRect contents_rect = contentsRect();
  line_number_area->setGeometry(
        QRect(contents_rect.left(), contents_rect.top(),
              get_line_number_area_width(), contents_rect.height()
              )
        );
}

/**
 * @brief Returns the tabulation length.
 * @return The length.
 */
int TextEditorWidget::get_tab_length() const {

  return tab_length;
}

/**
 * @brief Change the tabulation length.
 * @param length The new length.
 */
void TextEditorWidget::set_tab_length(int length) {

  tab_length = length;
  setTabStopDistance(fontMetrics().horizontalAdvance(" ") * tab_length);
}

/**
 * @brief Returns whether the widget replace tabulation by spaces.
 * @return @c true if the widget replace tabulation by spaces.
 */
bool TextEditorWidget::get_replace_tab_by_spaces() const {

  return replace_tab_by_spaces;
}

/**
 * @brief Changes whether the widget should replace tabulation by spaces.
 * @param replace @c true to replace tabulation by spaces.
 */
void TextEditorWidget::set_replace_tab_by_spaces(bool replace) {

  replace_tab_by_spaces = replace;
}

/**
 * @brief Highlights the current line of text.
 */
void TextEditorWidget::highlight_current_line() {

  QList<QTextEdit::ExtraSelection> extraSelections;

  if (!isReadOnly()) {
    QTextEdit::ExtraSelection selection;

    QColor line_color = EditorStyle::get_mode_info().current_line_background_color;

    selection.format.setBackground(line_color);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = textCursor();
    selection.cursor.clearSelection();
    extraSelections.append(selection);
  }

  setExtraSelections(extraSelections);
}

/**
 * @brief Draws the line number area.
 * @param event The paint event to handle.
 */
void TextEditorWidget::line_number_area_paint_event(QPaintEvent* event) {

  QPainter painter(line_number_area);

  QTextBlock block = firstVisibleBlock();
  int block_number = block.blockNumber();
  int top = static_cast<int>(blockBoundingGeometry(block).translated(contentOffset()).top());
  int bottom = top + static_cast<int>(blockBoundingRect(block).height());
  const EditorStyle* style = qobject_cast<const EditorStyle*>(qApp->style());

  while (block.isValid() && top <= event->rect().bottom()) {
    if (block.isVisible() && bottom >= event->rect().top()) {
      QString number = QString::number(block_number + 1) + " ";
      if (style != nullptr) {
        // Make line number color less flashy than the main text.
        painter.setPen(style->theme().secondaryAlternativeColor);
      }
      painter.drawText(0, top, line_number_area->width(), fontMetrics().height(),
                       Qt::AlignRight, number);
    }

    block = block.next();
    top = bottom;
    bottom = top + static_cast<int>(blockBoundingRect(block).height());
    ++block_number;
  }
}

/**
 * @brief Shows a context menu.
 *
 * Reimplemented to replace the built-in undo/redo actions by actions that
 * pass through the QUndoStack.
 *
 * @param event The context menu event.
 */
void TextEditorWidget::contextMenuEvent(QContextMenuEvent* event) {

  // Create our own context menu with correct undo/redo actions.
  QMenu menu;
  EditorStyle::setAutoIconColor(&menu, EditorStyle::AutoIconColor::TextColor);

  QAction* action = nullptr;

  // Undo/Redo actions that use the undo stack.
  action = undo_stack.createUndoAction(this);
  action->setIcon(QIcon(":/images/icon_undo.svg"));
  action->setShortcut(QKeySequence::Undo);
  menu.addAction(action);

  action = undo_stack.createRedoAction(this);
  action->setIcon(QIcon(":/images/icon_redo.svg"));
  action->setShortcut(QKeySequence::Redo);
  menu.addAction(action);

  // --- Separator. ---
  menu.addSeparator();

  // Cut/Copy/Paste.
  action = new QAction(
      QIcon(":/images/icon_cut.svg"),
      tr("Cut"), this);
  action->setShortcut(QKeySequence::Cut);
  if (textCursor().selectedText().isEmpty()) {
    action->setEnabled(false);
  }
  else {
    connect(action, SIGNAL(triggered()),
            this, SLOT(cut()));
  }
  menu.addAction(action);

  action = new QAction(
      QIcon(":/images/icon_copy.svg"),
      tr("Copy"), this);
  action->setShortcut(QKeySequence::Copy);
  if (textCursor().selectedText().isEmpty()) {
    action->setEnabled(false);
  }
  else {
    connect(action, SIGNAL(triggered()),
            this, SLOT(copy()));
  }
  menu.addAction(action);

  action = new QAction(
      QIcon(":/images/icon_paste.svg"),
      tr("Paste"), this);
  action->setShortcut(QKeySequence::Paste);
  if (!canPaste()) {
    action->setEnabled(false);
  }
  else {
    connect(action, SIGNAL(triggered()),
            this, SLOT(paste()));
  }
  menu.addAction(action);

  // --- Separator. ---
  menu.addSeparator();

  // Select all.
  action = new QAction(
      QIcon(":/images/icon_select_all.svg"),
      tr("Select all"), this);
  action->setShortcut(QKeySequence::SelectAll);
  connect(action, SIGNAL(triggered()),
          this, SLOT(selectAll()));
  menu.addAction(action);

  menu.exec(event->globalPos());
}

/**
 * @brief Receives a key press event.
 *
 * This overrides the implementation of QPlainTextEdit to block the built-in
 * undo/redo actions and use our own ones instead, to pass through the
 * QUndoStack.
 *
 * @param event The event to handle.
 */
void TextEditorWidget::keyPressEvent(QKeyEvent* event) {

  if (event == QKeySequence::Undo) {
    undo_stack.undo();
    event->accept();
    return;
  }

  if (event == QKeySequence::Redo) {
    undo_stack.redo();
    event->accept();
    return;
  }

  const bool control = event->modifiers() & Qt::ControlModifier;
  if (event->key() == Qt::Key_Tab) {
    if (!control) {
      // Indent to the right.
      insert_tab();
      event->accept();
    }
    else {
      // Don't eat the event, let it propagate to the tab widget.
      event->ignore();
    }
    return;
  }

  if (event->key() == Qt::Key_Backtab) {
    if (!control) {
      // Indent to the left.
      remove_tab();
      event->accept();
    }
    else {
      // Don't eat the event, let it propagate to the tab widget.
      event->ignore();
    }
    return;
  }

  QPlainTextEdit::keyPressEvent(event);
}

/**
 * @brief Inserts tabulation(s) at the cursor position.
 */
void TextEditorWidget::insert_tab() {

  QTextCursor cursor = textCursor();

  // Get tabulation character(s).
  QString tab = "\t";
  if (replace_tab_by_spaces) {
    int length = tab_length;
    if (!cursor.hasSelection()) {
      length -= (cursor.columnNumber() % tab_length);
    }
    tab = QString(" ").repeated(length);
  }

  cursor.beginEditBlock();

  // Insert tab for all selected lines.
  if (cursor.hasSelection()) {

    // Remember the original selection.
    int original_start = cursor.selectionStart();
    int original_end = cursor.selectionEnd();

    // Store the document's last valid position
    cursor.movePosition(QTextCursor::End);
    int document_end = cursor.position();

    // Return to the selection.
    cursor.setPosition(original_start);
    cursor.movePosition(QTextCursor::StartOfLine);
    int current_line_start = cursor.position();

    // Find the block that contains the end of the selection.
    cursor.setPosition(original_end);
    int end_block = cursor.blockNumber();

    // Go back to start position.
    cursor.setPosition(current_line_start);
    int start_block = cursor.blockNumber();

    // Process each block from start to end.
    for (int block = start_block; block <= end_block; ++block) {
      // Make sure we are at the start of the line.
      cursor.movePosition(QTextCursor::StartOfLine);

      // Only add indentation if the line is not empty.
      if (!cursor.atBlockEnd()) {
        cursor.insertText(tab);
        // Since we are adding text, we need to adjust our original end position.
        if (cursor.position() <= original_end) {
          original_end += tab.size();
        }
      }

      // Move to the next block if we ae not at the end of the document.
      if (cursor.position() < document_end) {
        cursor.movePosition(QTextCursor::NextBlock);
      } else {
        // We have reached the end of the document.
        break;
      }
    }

    // Restore the selection, adjusted for any inserted tabs.
    cursor.setPosition(original_start);
    cursor.setPosition(original_end, QTextCursor::KeepAnchor);
  }
  // Insert tab before the cursor.
  else {
    cursor.insertText(tab);
  }

  cursor.endEditBlock();

  // Set the modified cursor back to the editor.
  setTextCursor(cursor);

}

/**
 * @brief Removes tabulation(s) at the cursor position.
 */
void TextEditorWidget::remove_tab() {

  QTextCursor cursor = textCursor();
  cursor.beginEditBlock();

  // Check all selected lines.
  if (cursor.hasSelection()) {
    // Remember the original selection.
    int original_start = cursor.selectionStart();
    int original_end = cursor.selectionEnd();

    // Store the document's last valid position.
    cursor.movePosition(QTextCursor::End);
    int document_end = cursor.position();

    // Find the block numbers that contain the selection.
    cursor.setPosition(original_start);
    cursor.movePosition(QTextCursor::StartOfLine);
    int start_block = cursor.blockNumber();

    cursor.setPosition(original_end);
    int end_block = cursor.blockNumber();

    // Go back to the start position.
    cursor.setPosition(original_start);
    cursor.movePosition(QTextCursor::StartOfLine);

    // Process each block from start to end.
    for (int block = start_block; block <= end_block; ++block) {
      // Make sure we are at the start of the line.
      cursor.movePosition(QTextCursor::StartOfBlock);
      int start_of_current_line = cursor.position();

      // Remove first space(s) character(s) of this line.
      if (replace_tab_by_spaces) {
        // Get the number of spaces to remove.
        do {
          cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
        } while (cursor.selectedText().endsWith(" ") &&
                 !cursor.atBlockEnd() &&
                 cursor.selectedText().length() <= tab_length);

        const int length = static_cast<int>(cursor.selectedText().length()) - 1;

        // Select and remove the space(s).
        if (length > 0) {
          cursor.movePosition(QTextCursor::StartOfBlock);
          cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, length);
          cursor.removeSelectedText();

          // Adjust the end position since we removed text.
          if (start_of_current_line < original_end) {
            original_end -= length;
          }
        }
      }
      // Remove the first tab character of this line.
      else {
        cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
        if (cursor.selectedText() == "\t") {
          cursor.removeSelectedText();

          // Adjust the end position since we removed text.
          if (start_of_current_line < original_end) {
            original_end -= 1;
          }
        }
      }

      // Move to the next block if we are not at the end of the document.
      if (cursor.position() < document_end && block < end_block) {
        cursor.movePosition(QTextCursor::NextBlock);
      } else {
        // We have reached the end of the document or processed all blocks.
        break;
      }
    }

    // Restore the selection, adjusted for removed tabs/spaces.
    cursor.setPosition(original_start);
    cursor.setPosition(original_end, QTextCursor::KeepAnchor);
  }
  else {
    // Remove previous space(s) character(s).
    if (replace_tab_by_spaces) {
      int pos = cursor.position();
      // Get space count before the cursor.
      do {
        cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
      } while (cursor.selectedText().startsWith(" ") && !cursor.atStart());

      const int space_count = static_cast<int>(cursor.selectedText().length()) - 1;
      // Select and remove previous space(s).
      if (space_count > 0) {
        int length = space_count;
        if (space_count > tab_length) {
          length = space_count % tab_length;
          if (length == 0) {
            length = tab_length;
          }
        }
        cursor.setPosition(pos);
        cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor, length);
        cursor.removeSelectedText();
      }
    }
    // Remove the previous tab character.
    else {
      cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
      if (cursor.selectedText() == "\t") {
        cursor.removeSelectedText();
      }
    }
  }

  cursor.endEditBlock();

  // Set the modified cursor back to the editor.
  setTextCursor(cursor);
}

}
