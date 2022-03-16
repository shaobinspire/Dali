#include "Layout.h"

using namespace Dali;

Layout::Layout()
  : Layout(Direction::HORIZONTAL) {}

Layout::Layout(Direction direction)
  : LayoutBase(),
    m_direction(direction) {}

Layout::Direction Layout::get_direction() const {
  return m_direction;
}

void Layout::set_direction(Direction direction) {
  m_direction = direction;
}

void Layout::add_child(std::unique_ptr<LayoutBase> child) {
  m_children.push_back(std::move(child));
}

void Layout::paint(QPainter& painter) {
  for(auto& child : m_children) {
    child->paint(painter);
  }
}
