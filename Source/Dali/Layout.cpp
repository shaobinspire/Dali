#include "Dali/Layout.hpp"
#include <QWidget>

using namespace Dali;

Layout::Layout()
  : m_min_size(0, 0),
    m_max_size(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX),
    m_first_fixed_item(-1) {}

QRect Layout::get_rect() const {
  return m_rect;
}

void Layout::set_rect(const QRect& rect) {
  m_rect = rect;
}

void Layout::add_item(LayoutItem* item) {
  if(item->get_vertical_size_policy() == SizePolicy::Fixed) {
    m_rect = m_rect.united(item->get_rect());
  }
  m_items.push_back(item);
}

int Layout::get_item_size() const {
  return static_cast<int>(m_items.size());
}

LayoutItem* Layout::get_item(int index) const {
  if(index < 0 || index >= get_item_size()) {
    throw std::out_of_range("The index is out of range.");
  }
  return m_items[index];
}


void Layout::calculate_min_max_size() {
  if(preprocess_base_cases()) {
    return;
  }
  build_graph();
  get_paths();
  rebuild_layout();
}

QSize Layout::get_min_size() const {
  return m_min_size;
}

QSize Layout::get_max_size() const {
  return m_max_size;
}

bool Layout::preprocess_base_cases() {
  m_first_fixed_item = -1;
  m_fixed_item_count = 0;
  for(auto i = 0; i < get_item_size(); ++i) {
    if(m_items[i]->get_horizontal_size_policy() == SizePolicy::Fixed ||
      m_items[i]->get_vertical_size_policy() == SizePolicy::Fixed) {
      if(m_first_fixed_item < 0) {
        m_first_fixed_item = i;
      }
      ++m_fixed_item_count;
    }
    m_horizontal_map[m_items[i]->get_pos().x()].push_back(i);
    m_vertical_map[m_items[i]->get_pos().y()].push_back(i);
    m_reverse_horizontal_map[m_items[i]->get_rect().right()].push_back(i);
    m_reverse_vertical_map[m_items[i]->get_rect().bottom()].push_back(i);
  }
  auto has_fixed_column1 = [&] {
    for(auto index : m_horizontal_map.begin()->second) {
      if(m_items[index]->get_vertical_size_policy() == SizePolicy::Expanding) {
        return false;
      }
    }
    return true;
  }();
  auto has_fixed_column2 = [&] {
    for(auto index : m_reverse_horizontal_map.begin()->second) {
      if(m_items[index]->get_vertical_size_policy() == SizePolicy::Expanding) {
        return false;
      }
    }
    return true;
  }();
  if(has_fixed_column1 || has_fixed_column2) {
    m_min_size.setHeight(m_rect.height());
    m_max_size.setHeight(m_rect.height());
  }
  auto has_fixed_row1 = [&] {
    for(auto index : m_vertical_map.begin()->second) {
      if(m_items[index]->get_horizontal_size_policy() == SizePolicy::Expanding) {
        return false;
      }
    }
    return true;
  }();
  auto has_fixed_row2 = [&] {
    for(auto index : m_reverse_vertical_map.begin()->second) {
      if(m_items[index]->get_horizontal_size_policy() == SizePolicy::Expanding) {
        return false;
      }
    }
    return true;
  }();
  if(has_fixed_row1 || has_fixed_row2) {
    m_min_size.setWidth(m_rect.width());
    m_max_size.setWidth(m_rect.width());
    if(m_min_size.height() > 0) {
      return true;
    }
  }
  auto has_fixed_column = [&] {
    for(auto iter = m_horizontal_map.begin(); iter != m_horizontal_map.end(); ++iter) {
      auto count = 0;
      for(auto index : iter->second) {
        if(m_items[index]->get_vertical_size_policy() == SizePolicy::Fixed) {
          ++count;
        }
      }
      if(count == iter->second.size()) {
        return true;
      }
    }
    return false;
  }();
  auto has_fixed_row = [&] {
    for(auto iter = m_vertical_map.begin(); iter != m_vertical_map.end(); ++iter) {
      auto count = 0;
      for(auto index : iter->second) {
        if(m_items[index]->get_horizontal_size_policy() == SizePolicy::Fixed) {
          ++count;
        }
      }
      if(count == iter->second.size()) {
        return true;
      }
    }
    return false;
  }();
  if(has_fixed_column && has_fixed_row) {
    m_min_size = m_rect.size();
    m_max_size = m_min_size;
    return true;
  }
  return false;
}

void Layout::build_graph() {
  m_graph.resize(get_item_size());
  for(auto i = 0; i < get_item_size(); ++i) {
    auto pre_rect = m_items[i]->get_rect();
    if(pre_rect.right() == m_rect.right()) {
      continue;
    }
    for(auto j = 0; j < get_item_size(); ++j) {
      auto rect = m_items[j]->get_rect();
      if(pre_rect.right() + 1 == rect.left()) {
        if(pre_rect.y() == rect.y() && pre_rect.height() == rect.height()) {
          add_edge(i, j, Direction::RIGHT);
          break;
        } else if(pre_rect.y() >= rect.y() && pre_rect.y() < rect.bottom() ||
          pre_rect.bottom() > rect.y() && pre_rect.bottom() <= rect.bottom()) {
          add_edge(i, j, Direction::RIGHT);
          if(pre_rect.bottom() > rect.y() && pre_rect.bottom() < rect.bottom()) {
            break;
          }
        }
      }
    }
  }
  for(auto i = 0; i < get_item_size(); ++i) {
    auto pre_rect = m_items[i]->get_rect();
    if(pre_rect.bottom() == m_rect.bottom()) {
      continue;
    }
    for(auto j = 0; j < get_item_size(); ++j) {
      auto rect = m_items[j]->get_rect();
      if(pre_rect.bottom() + 1 == rect.top()) {
        if(pre_rect.x() == rect.x() && pre_rect.width() == rect.width()) {
          add_edge(i, j, Direction::DOWN);
          break;
        } else if(pre_rect.x() >= rect.x() && pre_rect.x() < rect.width() ||
          pre_rect.width() > rect.x() && pre_rect.width() < rect.width()) {
          add_edge(i, j, Direction::DOWN);
          if(pre_rect.width() > rect.x() && pre_rect.width() < rect.width()) {
            break;
          }
        }
      }
    }
  }
}

void Layout::get_paths() {
  auto path = std::vector<GraphItem>();
  auto fixed_item_count = 0;
  auto d = m_items.size() - 1;
  dfs(m_first_fixed_item, d, Direction::NONE, path, fixed_item_count);
}

void Layout::dfs(int u, int d, Direction direction, std::vector<GraphItem>& path, int& fixed_item_count) {
  m_visited[u] = true;
  if(m_items[u]->get_horizontal_size_policy() == SizePolicy::Fixed ||
      m_items[u]->get_vertical_size_policy() == SizePolicy::Fixed) {
    ++fixed_item_count;
  }
  path.push_back({u, direction});
  if(u == d) {
    if(fixed_item_count >= m_fixed_item_count) {
      m_paths.push_back(path);
    }
  } else {
    for(auto g : m_graph[u]) {
      if(!m_visited[g.m_index]) {
        dfs(g.m_index, d, g.m_direction, path, fixed_item_count);
      }
    }
  }
  if(m_items[u]->get_horizontal_size_policy() == SizePolicy::Fixed ||
    m_items[u]->get_vertical_size_policy() == SizePolicy::Fixed) {
    --fixed_item_count;
  }
  path.pop_back();
  m_visited[u] = false;
}

void Layout::rebuild_layout() {
  auto min_size = QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
  auto max_size = QSize(0, 0);
  for(auto& path : m_paths) {
    auto fixed_item_count = 0;
    auto layout_rect = QRect();
    auto pre_rect = QRect();
    auto direction = Direction::NONE;
    auto pre_direction = direction;
    auto rects = std::map<int, QRect>();
    auto is_valid = true;
    for(auto i = 0; i < static_cast<int>(path.size()); ++i) {
      auto index = path[i].m_index;
      if(direction == Direction::NONE) {
        direction = path[i].m_direction;
      }
      if(direction == Direction::NONE) {
        pre_rect = m_items[index]->get_rect();
        layout_rect = pre_rect;
        rects[index] = pre_rect;
      } else if(direction == Direction::RIGHT || direction == Direction::LEFT) {
        if(m_items[index]->get_horizontal_size_policy() == SizePolicy::Expanding &&
            m_items[index]->get_vertical_size_policy() == SizePolicy::Expanding) {
          continue;
        }
        auto rect = m_items[index]->get_rect();
        if(direction == Direction::RIGHT) {
          rect.moveTopLeft({pre_rect.right() + 1, pre_rect.top()});
        } else {
          rect.moveTopRight({pre_rect.left() - 1, pre_rect.top()});
          if(rect.left() < 0) {
            is_valid = false;
            break;
          }
        }
        if(m_items[index]->get_horizontal_size_policy() == SizePolicy::Fixed &&
            m_items[index]->get_vertical_size_policy() == SizePolicy::Expanding) {
          rect.setHeight(0);
        } else if(m_items[index]->get_horizontal_size_policy() == SizePolicy::Expanding && 
            m_items[index]->get_vertical_size_policy() == SizePolicy::Fixed) {
          rect.setWidth(0);
        }
        rects[index] = rect;
        layout_rect = layout_rect.united(rect);
        pre_rect = rect;
        pre_direction = direction;
        direction = Direction::NONE;
      } else if(direction == Direction::DOWN || direction == Direction::UP) {
        if(m_items[index]->get_horizontal_size_policy() == SizePolicy::Expanding &&
            m_items[index]->get_vertical_size_policy() == SizePolicy::Expanding) {
          continue;
        }
        auto rect = m_items[index]->get_rect();
        if(direction == Direction::DOWN) {
          rect.moveTopLeft({pre_rect.left(), pre_rect.bottom() + 1});
        } else {
          rect.moveBottomLeft({pre_rect.left(), pre_rect.top() - 1});
          if(rect.top() < 0) {
            is_valid = false;
            break;
          }
        }
        if(m_items[index]->get_horizontal_size_policy() == SizePolicy::Fixed && 
            m_items[index]->get_vertical_size_policy() == SizePolicy::Expanding) {
          rect.setHeight(0);
        } else if(m_items[index]->get_horizontal_size_policy() == SizePolicy::Expanding &&
            m_items[index]->get_vertical_size_policy() == SizePolicy::Fixed) {
          rect.setWidth(0);
        }
        rects[index] = rect;
        layout_rect = layout_rect.united(rect);
        pre_rect = rect;
        pre_direction = direction;
        direction = Direction::NONE;
      }
    }
    if(!is_valid) {
      continue;
    }
    if(!check_valid_align_left(rects) || !check_valid_align_right(rects) || !check_valid_align_bottom(rects)) {
      continue;
    }
    min_size.setWidth(std::min(min_size.width(), layout_rect.width()));
    min_size.setHeight(std::min(min_size.height(), layout_rect.height()));
  }
  if(m_min_size.width() == 0) {
    if(min_size.width() != QWIDGETSIZE_MAX) {
      m_min_size.setWidth(min_size.width());
    } else {
      m_min_size.setWidth(m_rect.width());
    }
  }
  if(m_min_size.height() == 0) {
    if(min_size.height() != QWIDGETSIZE_MAX) {
      m_min_size.setHeight(min_size.height());
    }
  }
}

Layout::Direction Layout::flip_direction(Direction direction) {
  if(direction == Direction::LEFT) {
    return Direction::RIGHT;
  } else if(direction == Direction::RIGHT) {
    return Direction::LEFT;
  } else if(direction == Direction::UP) {
    return Direction::DOWN;
  }
  return Direction::UP;
}

void Layout::add_edge(int u, int v, Direction direction) {
  m_graph[u].push_back({v, direction});
  m_graph[v].push_back({u, flip_direction(direction)});
}

bool Layout::check_valid_align_left(const std::map<int, QRect>& rects) {
  auto lefts = std::vector<int>();
  for(auto index : m_horizontal_map.begin()->second) {
    if(m_items[index]->get_horizontal_size_policy() == SizePolicy::Fixed) {
      if(rects.contains(index)) {
        lefts.push_back(rects.at(index).left());
      }
    }
  }
  if(!lefts.empty()) {
    auto left = *lefts.begin();
    for(auto iter = lefts.begin() + 1; iter != lefts.end(); ++iter) {
      if(left != *iter) {
        return false;
      }
    }
  }
  return true;
}

bool Layout::check_valid_align_right(const std::map<int, QRect>& rects) {
  auto rights = std::vector<int>();
  for(auto index : m_reverse_horizontal_map.begin()->second) {
    if(m_items[index]->get_horizontal_size_policy() == SizePolicy::Fixed) {
      if(rects.contains(index)) {
        rights.push_back(rects.at(index).right());
      }
    }
  }
  if(!rights.empty()) {
    auto right = *rights.begin();
    for(auto iter = rights.begin() + 1; iter != rights.end(); ++iter) {
      if(right != *iter) {
        return false;
      }
    }
  }
  return true;
}

bool Layout::check_valid_align_bottom(const std::map<int, QRect>& rects) {
  auto bottoms = std::vector<int>();
  for(auto index : m_reverse_vertical_map.begin()->second) {
    if(m_items[index]->get_horizontal_size_policy() == SizePolicy::Fixed) {
      if(rects.contains(index)) {
        bottoms.push_back(rects.at(index).bottom());
      }
    }
  }
  if(!bottoms.empty()) {
    auto bottom = *bottoms.begin();
    for(auto iter = bottoms.begin() + 1; iter != bottoms.end(); ++iter) {
      if(bottom != *iter) {
        return false;
      }
    }
  }
  return true;
}

bool Layout::is_opposite_direction(Direction direction1, Direction direction2) {
  if(direction1 == Direction::RIGHT && direction2 == Direction::LEFT ||
    direction2 == Direction::RIGHT && direction1 == Direction::LEFT) {
    return true;
  }
  if(direction1 == Direction::DOWN && direction2 == Direction::UP ||
    direction2 == Direction::DOWN && direction1 == Direction::UP) {
    return true;
  }
  return false;
}
