#include "LayoutItem.h"

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
