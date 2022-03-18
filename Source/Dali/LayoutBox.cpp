#include "Dali/LayoutBox.hpp"

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
    auto top_left = rect.topLeft() + QPoint(1, 1);
    auto top_right = rect.topRight() + QPoint(0, 1);
    auto bottom_left = rect.bottomLeft() + QPoint(1, 0);
    painter.setPen(QPen(QBrush(get_color(get_horizontal_size_policy())), 2,
      Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
    painter.drawLine(QLineF(top_left, bottom_left));
    painter.drawLine(QLineF(top_right, rect.bottomRight()));
    painter.setPen(QPen(QBrush(get_color(get_vertical_size_policy())), 2,
      Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
    painter.drawLine(QLineF(top_left, top_right));
    painter.drawLine(QLineF(bottom_left, rect.bottomRight()));
  }
  painter.setPen(Qt::black);
  auto position = QString("\n(%1, %2, %3, %4)").
    arg(rect.x()).arg(rect.y()).arg(rect.width()).arg(rect.height());
  painter.drawText(rect, Qt::AlignCenter | Qt::TextWordWrap, m_name + position);
  painter.restore();
}
