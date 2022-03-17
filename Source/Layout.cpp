#include "Layout.h"

using namespace Dali;

Layout::Layout()
  : Layout(Direction::HORIZONTAL) {}

Layout::Layout(Direction direction)
  : LayoutBase(),
    m_direction(direction) {}

Layout::~Layout() {
  for(auto child : m_children) {
    delete child;
  }
}

Layout::Direction Layout::get_direction() const {
  return m_direction;
}

void Layout::set_direction(Direction direction) {
  m_direction = direction;
}

void Layout::add_child(LayoutBase* child) {
  m_children.push_back(child);
}

int Layout::get_size() const {
  return static_cast<int>(m_children.size());
}

LayoutBase* Layout::get_child(int index) {
  if(index < 0 || index >= get_size()) {
    return nullptr;
  }
  return m_children[index];
}

void Layout::draw(QPainter& painter) {
  for(auto child : m_children) {
    child->draw(painter);
  }
}
