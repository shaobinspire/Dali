#include "Dali/LayoutItem.hpp"

using namespace Dali;

QRect LayoutItem::get_rect() const {
  return m_rect;
}

void LayoutItem::set_rect(const QRect& rect) {
  m_rect = rect;
}

QPoint LayoutItem::get_pos() const {
  return m_rect.topLeft();
}

void LayoutItem::set_pos(const QPoint& pos) {
  m_rect.moveTopLeft(pos);
}

QSize LayoutItem::get_size() const {
  return m_rect.size();
}

void LayoutItem::set_size(const QSize& size) {
  m_rect.setSize(size);
}

SizePolicy LayoutItem::get_horizontal_size_policy() const {
  return m_horizontal_size_policy;
}

void LayoutItem::set_horizontal_size_policy(SizePolicy policy) {
  m_horizontal_size_policy = policy;
}

SizePolicy LayoutItem::get_vertical_size_policy() const {
  return m_vertical_size_policy;
}

void LayoutItem::set_vertical_size_policy(SizePolicy policy) {
  m_vertical_size_policy = policy;
}

QString LayoutItem::get_name() const {
  return m_name;
}

void LayoutItem::set_name(const QString& name) {
  m_name = name;
}
