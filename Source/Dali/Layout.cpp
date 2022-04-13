#include "Dali/Layout.hpp"
#include <sstream>
#include "Dali/LayoutBox.hpp"

using namespace Dali;

void build_constraints(const std::vector<LayoutBox*>& boxes, const std::string& property,
    Constraints& constraints) {
  auto is_horiztonal = [&] {
    if(property == "width") {
      return true;
    }
    return false;
  }();
  auto get_value = [=] (auto index) {
    if(is_horiztonal) {
      return boxes[index]->get_rect().width();
    } else {
      return boxes[index]->get_rect().height();
    }
  };
  auto expanding_box_without_constraint = std::vector<std::pair<int, int>>();
  auto sum_expression = std::string();
  for(auto i = 0; i < static_cast<int>(boxes.size()); ++i) {
    sum_expression += boxes[i]->get_name() + "." + property + " +";
    if(is_horiztonal) {
      if(boxes[i]->get_horizontal_size_policy() == SizePolicy::Fixed) {
        auto sstr = std::ostringstream();
        sstr << boxes[i]->get_name() << "." << property << "=" << get_value(i);
        constraints.add_local_constraint(Constraint(sstr.str()), true);
      } else if(!constraints.has_varaible_name_in_global(boxes[i]->get_name())) {
        expanding_box_without_constraint.push_back({get_value(i), i});
      }
    } else {
      if(boxes[i]->get_vertical_size_policy() == SizePolicy::Fixed) {
        auto sstr = std::ostringstream();
        sstr << boxes[i]->get_name() << "." << property << "=" << get_value(i);
        constraints.add_local_constraint(Constraint(sstr.str()), true);
      } else if(!constraints.has_varaible_name_in_global(boxes[i]->get_name())) {
        expanding_box_without_constraint.push_back({get_value(i), i});
      }
    }
    auto sstr = std::ostringstream();
    sstr << boxes[i]->get_name() << "." << property << ">= 0";
    constraints.add_local_constraint(Constraint(sstr.str()), true);
  }
  if(!expanding_box_without_constraint.empty()) {
    std::sort(expanding_box_without_constraint.begin(), expanding_box_without_constraint.end());
    for(auto iter = expanding_box_without_constraint.begin() + 1; iter != expanding_box_without_constraint.end(); ++iter) {
      auto sstr = std::ostringstream();
      sstr << boxes[iter->second]->get_name() << "." << property << "=" <<
        boxes[expanding_box_without_constraint.begin()->second]->get_name() <<
        "." << property << " * " <<
        static_cast<double>(iter->first) / expanding_box_without_constraint.begin()->first;
      constraints.add_local_constraint(Constraint(sstr.str()), false);
    }
  }
  sum_expression.pop_back();
  sum_expression += "= width";
  constraints.add_local_constraint(Constraint(sum_expression), true);
}

Layout::Layout()
  : m_min_size(0, 0),
    m_max_size(MAX_LAYOUT_SIZE, MAX_LAYOUT_SIZE) {}

Layout::~Layout() {
  for(auto box : m_boxes) {
    delete box;
  }
}

QRect Layout::get_rect() const {
  return m_rect;
}

void Layout::set_rect(const QRect& rect) {
  m_rect = rect;
}

void Layout::add_box(LayoutBox* box) {
  if(box->get_vertical_size_policy() == SizePolicy::Fixed) {
    m_rect = m_rect.united(box->get_rect());
  }
  if(box->get_name().empty()) {
    box->set_name("DALI_LAYOUT_BOX" + m_boxes.size());
    box->set_name_visible(false);
  }
  m_boxes.push_back(box);
  m_name_map[box->get_name()] = static_cast<int>(m_boxes.size()) - 1;
  m_min_pos.setX(std::min(m_min_pos.x(), box->get_rect().x()));
  m_min_pos.setY(std::min(m_min_pos.y(), box->get_rect().y()));
  m_max_pos.setX(std::max(m_max_pos.x(), box->get_rect().right()));
  m_max_pos.setY(std::max(m_max_pos.y(), box->get_rect().bottom()));
}

void Layout::add_width_constraint(const Constraint& constraint) {
  m_width_constraints.add_global_constraint(constraint);
}

void Layout::add_height_constraint(const Constraint& constraint) {
  m_height_constraints.add_global_constraint(constraint);
}

int Layout::get_box_count() const {
  return static_cast<int>(m_boxes.size());
}

LayoutBox* Layout::get_box(int index) const {
  if(index < 0 || index >= get_box_count()) {
    throw std::out_of_range("The index is out of range.");
  }
  return m_boxes[index];
}

QSize Layout::get_min_size() const {
  return m_min_size;
}

QSize Layout::get_max_size() const {
  return m_max_size;
}

void Layout::resize(const QSize& size) {
  if(is_horizontal_one_row()) {
    auto result = m_width_solver.solve(size.width());
    for(auto& pos : result) {
      if(!m_name_map.contains(pos.first)) {
        continue;
      }
      auto index = m_name_map[pos.first];
      m_boxes[index]->set_size({static_cast<int>(pos.second), size.height()});
    }
    for(auto i = 1; i < get_box_count(); ++i) {
      m_boxes[i]->set_pos({m_boxes[i - 1]->get_rect().right() + 1, m_boxes[i]->get_rect().y()});
    }
  } else if(is_vertical_one_column()) {
    auto result = m_height_solver.solve(size.height());
    for(auto& pos : result) {
      if(!m_name_map.contains(pos.first)) {
        continue;
      }
      auto index = m_name_map[pos.first];
      m_boxes[index]->set_size({size.width(), static_cast<int>(pos.second)});
    }
    for(auto i = 1; i < get_box_count(); ++i) {
      m_boxes[i]->set_pos({m_boxes[i]->get_rect().x(), m_boxes[i - 1]->get_rect().bottom() + 1});
    }
  }
  //resize_width(size.width());
  //resize_height(size.height());
  m_rect.setSize(size);
}

bool Layout::build() {
  for(auto i = 0; i < get_box_count(); ++i) {
    if(m_boxes[i]->get_rect().y() == m_min_pos.y()) {
      m_top_row.push_back(i);
    }
    if(m_boxes[i]->get_rect().x() == m_min_pos.x()) {
      m_left_column.push_back(i);
    }
    if(m_boxes[i]->get_rect().bottom() == m_max_pos.y()) {
      m_bottom_row.push_back(i);
    }
    if(m_boxes[i]->get_rect().right() == m_min_pos.x()) {
      m_right_column.push_back(i);
    }
  }
  if(is_horizontal_one_row()) {
    build_constraints(m_boxes, "width", m_width_constraints);
    m_width_solver.add_constraints(m_width_constraints);
    calculate_one_row_min_max_size();
  } else if(is_vertical_one_column()) {
    build_constraints(m_boxes, "height", m_height_constraints);
    m_height_solver.add_constraints(m_height_constraints);
    calculate_one_column_min_max_size();
  }
  return true;
}

int Layout::get_index_by_name(const std::string& name) {
  if(auto iter = m_name_map.find(name); iter != m_name_map.end()) {
    return iter->second;
  }
  return -1;
}

bool Layout::is_horizontal_one_row() const {
  return m_top_row.size() == get_box_count();
}

bool Layout::is_vertical_one_column() const {
  return m_left_column.size() == get_box_count();
}

void Layout::calculate_one_row_min_max_size() {
  m_min_size.setWidth(m_width_solver.get_min_value());
  auto min_height = 0;
  for(auto index : m_top_row) {
    if(m_boxes[index]->get_vertical_size_policy() == SizePolicy::Fixed) {
      min_height = m_boxes[index]->get_rect().height();
    }
  }
  m_min_size.setHeight(min_height);
  auto max_width = m_width_solver.get_max_value();
  if(max_width == -1) {
    m_max_size.setWidth(m_min_size.width());
  } else {
    m_max_size.setWidth(max_width);
  }
  if(m_min_size.height() != 0) {
    m_max_size.setHeight(m_min_size.height());
  }
}

void Layout::calculate_one_column_min_max_size() {
  m_min_size.setHeight(m_height_solver.get_min_value());
  auto min_width = 0;
  for(auto index : m_left_column) {
    if(m_boxes[index]->get_horizontal_size_policy() == SizePolicy::Fixed) {
      min_width = m_boxes[index]->get_rect().width();
    }
  }
  m_min_size.setWidth(min_width);
  auto max_height = m_width_solver.get_max_value();
  if(max_height == -1) {
    m_max_size.setHeight(m_min_size.width());
  } else {
    m_max_size.setHeight(max_height);
  }
  if(m_min_size.width() != 0) {
    m_max_size.setWidth(m_min_size.width());
  }
}

void Layout::resize_width(int width) {
  if(is_horizontal_one_row()) {
    auto result = m_width_solver.solve(width);
    for(auto& pos : result) {
      if(!m_name_map.contains(pos.first)) {
        continue;
      }
      auto index = m_name_map[pos.first];
      m_boxes[index]->set_size({static_cast<int>(pos.second), m_boxes[index]->get_rect().height()});
    }
    for(auto i = 1; i < get_box_count(); ++i) {
      m_boxes[i]->set_pos({m_boxes[i - 1]->get_rect().right() + 1, m_boxes[i]->get_rect().y()});
    }
  }
}

void Layout::resize_height(int height) {
  if(is_vertical_one_column()) {
    auto result = m_height_solver.solve(height);
    for(auto& pos : result) {
      if(!m_name_map.contains(pos.first)) {
        continue;
      }
      auto index = m_name_map[pos.first];
      m_boxes[index]->set_size({m_boxes[index]->get_rect().width(), static_cast<int>(pos.second)});
    }
    for(auto i = 1; i < get_box_count(); ++i) {
      m_boxes[i]->set_pos({m_boxes[i]->get_rect().x(), m_boxes[i - 1]->get_rect().bottom() + 1});
    }
  }
}

