#include "LayoutBase.h"

using namespace Dali;

QRect LayoutBase::get_rect() const {
  return {m_pos, m_size};
}

void LayoutBase::set_rect(const QRect& rect) {
  m_pos = rect.topLeft();
  m_size = rect.size();
}

QPoint LayoutBase::get_pos() const {
  return m_pos;
}

void LayoutBase::set_pos(const QPoint& pos) {
  m_pos = pos;
}

QSize LayoutBase::get_size() const {
  return m_size;
}

void LayoutBase::set_size(const QSize& size) {
  m_size = size;
}

int LayoutBase::get_x() const {
  return m_pos.x();
}

void LayoutBase::set_x(int x) {
  m_pos.setX(x);
}

int LayoutBase::get_y() const {
  return m_pos.y();
}

void LayoutBase::set_y(int y) {
  m_pos.setY(y);
}

int LayoutBase::get_width() const {
  return m_size.width();
}

void LayoutBase::set_width(int width) {
  m_size.setWidth(width);
}

int LayoutBase::get_height() const {
  return m_size.height();
}

void LayoutBase::set_height(int height) {
  m_size.setHeight(height);
}

LayoutBase::LayoutBase()
  : m_pos(0, 0),
    m_size(0, 0) {}
