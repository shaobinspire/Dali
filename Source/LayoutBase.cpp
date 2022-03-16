#include "LayoutBase.h"

using namespace Dali;

const QRect& LayoutBase::get_rect() const {
  return m_rect;
}

void LayoutBase::set_rect(const QRect& rect) {
  m_rect = rect;
}

void LayoutBase::set_pos(const QPoint& pos) {
  m_rect.setTopLeft(pos);
}

void LayoutBase::set_size(const QSize& size) {
  m_rect.setSize(size);
}

LayoutBase::SizePolicy LayoutBase::get_horizontal_size_policy() const {
  return m_horizontal_size_policy;
}

LayoutBase::SizePolicy LayoutBase::get_vertical_size_policy() const {
  return m_vertical_size_policy;
}

void LayoutBase::set_horizontal_size_policy(SizePolicy policy) {
  m_horizontal_size_policy = policy;
}

void LayoutBase::set_vertical_size_policy(SizePolicy policy) {
  m_vertical_size_policy = policy;
}

void LayoutBase::paint(QPainter& painter) {
}
