#include "LayoutViewer/JsonEditor.hpp"
#include <iomanip>
#include <QFont>
#include <QFontMetrics>
#include <QPainter>
#include <QTextBlock>
#include <fstream>
#include "LayoutViewer/LineNumberWidget.hpp"

using namespace Dali;

JsonEditor::JsonEditor(QWidget* parent)
    : QPlainTextEdit(parent) {
  m_line_number_widget = new LineNumberWidget(*this);
  update_line_number_widget_width();
  connect(this, &JsonEditor::updateRequest,
    this, &JsonEditor::update_line_number_widget);
  connect(this, &JsonEditor::blockCountChanged,
    this, &JsonEditor::update_line_number_widget_width);
  setTabStopDistance(4.0 * QFontMetrics(font()).horizontalAdvance(' '));
}

void JsonEditor::load(const QString& file_name) {
  auto ifs = std::ifstream(file_name.toStdString());
  auto text = std::string(std::istreambuf_iterator<char>(ifs),
                 std::istreambuf_iterator<char>());
  setPlainText(QString::fromStdString(text));
}

void JsonEditor::paint_line_number_event(QPaintEvent* event) {
  auto painter = QPainter(m_line_number_widget);
  painter.setPen(Qt::black);
  painter.fillRect(event->rect(), Qt::lightGray);
  auto block = firstVisibleBlock();
  auto block_number = block.blockNumber();
  auto top =
    qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
  auto bottom = top + qRound(blockBoundingRect(block).height());
  while(block.isValid() && top <= event->rect().bottom()) {
    if(block.isVisible() && bottom >= event->rect().top()) {
      auto number = QString::number(block_number + 1);
      painter.drawText(0, top, m_line_number_widget->width(),
        fontMetrics().height(), Qt::AlignRight, number);
    }
    block = block.next();
    top = bottom;
    bottom = top + qRound(blockBoundingRect(block).height());
    ++block_number;
  }
}

void JsonEditor::resizeEvent(QResizeEvent* event) {
  QPlainTextEdit::resizeEvent(event);
  auto rect = contentsRect();
  m_line_number_widget->setGeometry(QRect(rect.left(), rect.top(),
    m_line_number_widget->sizeHint().width(), rect.height()));
}

void JsonEditor::update_line_number_widget(const QRect& rect, int dy) {
  if(dy) {
    m_line_number_widget->scroll(0, dy);
  } else {
    m_line_number_widget->update(0, rect.y(), m_line_number_widget->width(),
      rect.height());
  }
}

void JsonEditor::update_line_number_widget_width() {
  setViewportMargins(m_line_number_widget->sizeHint().width(), 0, 0, 0);
}

