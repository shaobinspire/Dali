#include "LayoutBox.h"

using namespace Dali;

auto get_color(LayoutBox::SizePolicy policy) {
  if(policy == LayoutBox::SizePolicy::Fixed) {
    static auto color = QColor(0xFFBB00);
    return color;
  }
  static auto color = QColor(0x0066FF);
  return color;
}

LayoutBox::LayoutBox()
  : LayoutItem(),
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
  painter.save();
  auto rect = get_rect();
  if(get_horizontal_size_policy() == get_vertical_size_policy()) {
    painter.fillRect(rect, get_color(get_horizontal_size_policy()));
  } else {
    painter.setPen(get_color(get_horizontal_size_policy()));
    auto top_right = rect.topRight() + QPoint(1, 0);
    auto bottom_left = rect.bottomLeft() + QPoint(0, 1);
    auto bottom_right = rect.bottomRight() + QPoint(1, 1);
    painter.drawLine(QLineF(rect.topLeft(), rect.bottomLeft()));
    painter.drawLine(QLineF(top_right, bottom_right));
    painter.setPen(get_color(get_vertical_size_policy()));
    painter.drawLine(QLineF(rect.topLeft(), rect.topRight()));
    painter.drawLine(QLineF(bottom_left, bottom_right));
  }
  painter.setPen(Qt::black);
  auto position = QString("\n(%1, %2, %3, %4)").
    arg(rect.x()).arg(rect.y()).arg(rect.width()).arg(rect.height());
  painter.drawText(rect, Qt::AlignCenter | Qt::TextWordWrap, m_name + position);
  painter.restore();
}
