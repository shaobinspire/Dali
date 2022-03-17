#include "LayoutBox.h"

using namespace Dali;

QColor get_color(LayoutBox::SizePolicy policy) {
  if(policy == LayoutBox::SizePolicy::Fixed) {
    return QColor(0xFFBB00);
  }
  return QColor(0x0066FF);
}

LayoutBox::LayoutBox()
  : LayoutBase(),
    m_horizontal_size_policy(SizePolicy::Fixed),
    m_vertical_size_policy(SizePolicy::Fixed) {}

QString LayoutBox::get_name() const {
  return m_name;
}

LayoutBox::SizePolicy LayoutBox::get_horizontal_size_policy() const {
  return m_horizontal_size_policy;
}

void LayoutBox::set_horizontal_size_policy(SizePolicy policy) {
  m_horizontal_size_policy = policy;
}

LayoutBox::SizePolicy LayoutBox::get_vertical_size_policy() const {
  return m_vertical_size_policy;
}

void LayoutBox::set_vertical_size_policy(SizePolicy policy) {
  m_vertical_size_policy = policy;
}

void LayoutBox::set_name(const QString& name) {
  m_name = name;
}

void LayoutBox::draw(QPainter& painter) {
  auto rect = get_rect();
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
  auto position = QString("\n(%1, %2, %3, %4)").
    arg(rect.x()).arg(rect.y()).arg(rect.width()).arg(rect.height());
  painter.drawText(rect, Qt::AlignCenter | Qt::TextWordWrap, m_name + position);
}
