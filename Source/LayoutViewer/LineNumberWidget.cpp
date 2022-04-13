#include "LayoutViewer/LineNumberWidget.hpp"

using namespace Dali;

LineNumberWidget::LineNumberWidget(JsonEditor& editor)
  : QWidget(&editor),
    m_editor(&editor) {}

QSize LineNumberWidget::sizeHint() const {
  auto digits = 1;
  auto max = std::max(1, m_editor->blockCount());
  while(max >= 10) {
    max /= 10;
    ++digits;
  }
  return QSize(3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits,
    0);
}

void LineNumberWidget::paintEvent(QPaintEvent* event) {
  m_editor->paint_line_number_event(event);
}
