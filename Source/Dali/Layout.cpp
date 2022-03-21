#include "Dali/Layout.hpp"

using namespace Dali;

QRect Layout::get_rect() const {
  return m_rect;
}

void Layout::set_rect(const QRect& rect) {
  m_rect = rect;
}

void Layout::add_item(const LayoutItem& item) {
  m_items.push_back(item);
}

int Layout::get_item_size() const {
  return static_cast<int>(m_items.size());
}

const LayoutItem& Layout::get_item(int index) const {
  return m_items[index];
}
