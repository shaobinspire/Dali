#include "Dali/Layout.hpp"

using namespace Dali;

Layout::Layout()
  : Layout(Direction::HORIZONTAL) {}

Layout::Layout(Direction direction)
  : LayoutItem(),
    m_direction(direction) {}

Layout::Direction Layout::get_direction() const {
  return m_direction;
}

void Layout::set_direction(Direction direction) {
  m_direction = direction;
}

void Layout::add_item(std::unique_ptr<LayoutItem> item) {
  if(m_direction == Layout::Direction::HORIZONTAL) {
    set_size({get_size().width() + item->get_size().width(),
      std::max(item->get_size().height(), get_size().height())});
  } else {
    set_size({std::max(item->get_size().width(), get_size().width()),
      get_size().height() + item->get_size().height()});
  }
  m_items.push_back(std::move(item));
}

int Layout::get_item_size() const {
  return static_cast<int>(m_items.size());
}

void Layout::draw(QPainter& painter) {
  for(auto& item : m_items) {
    item->draw(painter);
  }
}
