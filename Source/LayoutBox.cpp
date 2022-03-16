#include "LayoutBox.h"

using namespace Dali;

QColor get_color(LayoutBase::SizePolicy policy) {
  if(policy == LayoutBase::SizePolicy::Fixed) {
    return QColor(0xFFBB00);
  }
  return QColor(0x0066FF);
}

LayoutBox::LayoutBox()
    : LayoutBase() { }

QString LayoutBox::get_name() const {
  return m_name;
}

void LayoutBox::set_name(const QString& name) {
  m_name = name;
}

void LayoutBox::paint(QPainter& painter) {
  auto& rect = get_rect();
  if(get_horizontal_size_policy() == get_vertical_size_policy()) {
    painter.fillRect(rect, get_color(get_horizontal_size_policy()));
  } else {
    painter.setPen(get_color(get_horizontal_size_policy()));
    painter.drawLine(rect.topLeft(), rect.bottomLeft());
    painter.drawLine(rect.topRight(), rect.bottomRight());
    painter.setPen(get_color(get_vertical_size_policy()));
    painter.drawLine(rect.topLeft(), rect.topRight());
    painter.drawLine(rect.bottomLeft(), rect.bottomRight());
  }
  painter.setPen(Qt::black);
  auto position = QString("\n(%1, %2, %3, %4)").arg(rect.x()).arg(rect.y()).arg(rect.width()).arg(rect.height());
  painter.drawText(rect, Qt::AlignCenter | Qt::TextWordWrap, m_name + position);
}
