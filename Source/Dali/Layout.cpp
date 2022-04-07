#include "Dali/Layout.hpp"
#include <stack>
#include <QWidget>
#include "Dali/ConstraintGraph.hpp"
#include "Dali/LayoutBox.hpp"

using namespace Dali;

Layout::Layout()
  : m_min_size(0, 0),
  m_max_size(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX),
  m_fixed_size(0)
  /*,
  m_first_fixed_item(-1)*/ {}

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
  m_boxes.push_back(box);
  m_name_map[box->get_name()] = static_cast<int>(m_boxes.size()) - 1;
  m_min_pos.setX(std::min(m_min_pos.x(), box->get_rect().x()));
  m_min_pos.setY(std::min(m_min_pos.y(), box->get_rect().y()));
  m_max_pos.setX(std::max(m_max_pos.x(), box->get_rect().right()));
  m_max_pos.setY(std::max(m_max_pos.y(), box->get_rect().bottom()));
}

void Layout::add_width_constraint(const Constraint& constraint) {
  m_additional_width_constraints.push_back(constraint);
  auto& variable_names = constraint.get_variable_names();
  for(auto& name : variable_names) {
    m_additional_width_variable_names.insert(name);
  }
  //m_width_constraints.add(constraint);
}

void Layout::add_height_constraint(const Constraint& constraint) {
  m_additional_height_constraints.push_back(constraint);
  //m_height_constraints.add(constraint);
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
  resize_width(size.width());
  resize_height(size.height());
  m_rect.setSize(size);
}

bool Layout::build() {
  //auto horizontal_expanding_boxes = std::vector<QString>();
  //auto vertical_expanding_boxes = std::vector<QString>();
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
    auto expanding_box_without_constraint = std::map<int, int>();
    for(auto i = 0; i < get_box_count(); ++i) {
      m_expanding_boxes.push_back(m_boxes[i]->get_name());
      if(m_boxes[i]->get_horizontal_size_policy() == SizePolicy::Fixed) {
        //m_fixed_size += m_boxes[i]->get_rect().width();
        auto expression = QString("%1.%2 = %3").
          arg(m_boxes[i]->get_name()).arg("width").
          arg(m_boxes[i]->get_rect().width());
        m_initial_width_constraints.push_back(Constraint(expression));
        //m_width_constraints.add(Constraint(expression));
      } else {
        //m_expanding_boxes.push_back(m_boxes[i]->get_name());
        expanding_box_without_constraint[m_boxes[i]->get_rect().width()] = i;
      }
    }
    for(auto iter = std::next(expanding_box_without_constraint.begin(), 1); iter != expanding_box_without_constraint.end(); ++iter) {
      auto expression = QString("%1.%2 = %3.%2 * %4").
        arg(m_boxes[iter->second]->get_name()).arg("width").
        arg(m_boxes[expanding_box_without_constraint.begin()->second]->get_name()).
        arg(static_cast<double>(iter->first) / expanding_box_without_constraint.begin()->first);
      m_initial_width_constraints.push_back(Constraint(expression));
      //m_width_constraints.add(Constraint(expression));
    }
    auto constraints = std::vector<Constraint>(m_additional_width_constraints.begin(), m_additional_width_constraints.end());
    for(auto& constraint : m_initial_width_constraints) {
      auto& variable_names = constraint.get_variable_names();
      auto is_contained = [&] {
        for(auto& name : variable_names) {
          if(m_additional_width_variable_names.contains(name)) {
            return true;
          }
        }
        return false;
      }();
      if(!is_contained) {
        constraints.push_back(constraint);
      }
    }
    for(auto& constraint : constraints) {
      m_width_constraints.add(constraint);
    }
  } else if(is_vertical_one_column()) {
    auto expanding_box_without_constraint = std::map<int, int>();
    for(auto i = 0; i < get_box_count(); ++i) {
      if(m_boxes[i]->get_vertical_size_policy() == SizePolicy::Fixed) {
        m_fixed_size += m_boxes[i]->get_rect().height();
      } else {
        m_expanding_boxes.push_back(m_boxes[i]->get_name());
        expanding_box_without_constraint[m_boxes[i]->get_rect().height()] = i;
      }
    }
    for(auto iter = std::next(expanding_box_without_constraint.begin(), 1); iter != expanding_box_without_constraint.end(); ++iter) {
      auto expression = QString("%1.%2 = %3.%2 * %4").
        arg(m_boxes[iter->second]->get_name()).arg("height").
        arg(m_boxes[expanding_box_without_constraint.begin()->second]->get_name()).
        arg(static_cast<double>(iter->first) / expanding_box_without_constraint.begin()->first);
      m_width_constraints.add(Constraint(expression));
    }
  }
  calculate_min_max_size();
  //if(is_horizontal_one_row()) {
  //  m_width_constraints.add_sum_constraint("width", horizontal_expanding_boxes);
  //} else if(is_vertical_one_column()) {
  //  m_height_constraints.add_sum_constraint("height", vertical_expanding_boxes);
  //}

  return true;
}

bool Layout::build_global_constraints() {
  //ConstraintGraph width_constraint_graph(m_boxes.size());
  //ConstraintGraph height_constraint_graph(m_boxes.size());
  //auto constraints = m_width_constraints.get_constraints();
  //for(auto& constraint : constraints) {
  //  build_constraint_graph(width_constraint_graph, constraint);
  //  build_constraint_graph(height_constraint_graph, constraint);
  //}
  //if(!width_constraint_graph.topological_sort() ||
  //    !height_constraint_graph.topological_sort()) {
  //  return false;
  //}
  //m_width_sorted_constraint = width_constraint_graph.get_sorted_list();
  //m_height_sorted_constraint = height_constraint_graph.get_sorted_list();
  return true;
}

void Layout::build_constraint_graph(ConstraintGraph& graph, Constraint& constraint) {
  //for(auto i = 0; i < constraint.get_element_count(); ++i) {
  //  auto& element = constraint.get_element(i);
  //  if(auto variable = std::get_if<Constraint::Variable>(&element)) {
  //    if(m_name_map.contains(variable->m_name)) {
  //      auto index = m_name_map[variable->m_name];
  //      graph.add_edge(box_index, index);
  //    }
  //  }
  //}
}

int Layout::get_index_by_name(const QString& name) {
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

void Layout::calculate_min_max_size() {
  if(is_horizontal_one_row()) {
    calculate_one_row_min_max_size();
  } else if(is_vertical_one_column()) {
    calculate_one_column_min_max_size();
  }
  //if(preprocess_base_cases()) {
  //  return;
  //}
  //build_graph();
  //get_paths();
  //rebuild_layout();
}

void Layout::calculate_one_row_min_max_size() {
  auto fixed_count = 0;
  for(auto index : m_top_row) {
    if(m_boxes[index]->get_horizontal_size_policy() == SizePolicy::Fixed) {
      m_min_size.setWidth(m_min_size.width() + m_boxes[index]->get_rect().width());
      ++fixed_count;
    }
    if(m_boxes[index]->get_vertical_size_policy() == SizePolicy::Fixed) {
      m_min_size.setHeight(m_boxes[index]->get_rect().height());
    }
  }
  if(fixed_count == get_box_count()) {
    m_max_size.setWidth(m_min_size.width());
  }
  if(m_min_size.height() != QWIDGETSIZE_MAX) {
    m_max_size.setHeight(m_min_size.height());
  }
}

void Layout::calculate_one_column_min_max_size() {
  auto fixed_count = 0;
  for(auto index : m_left_column) {
    if(m_boxes[index]->get_vertical_size_policy() == SizePolicy::Fixed) {
      m_min_size.setHeight(m_min_size.height() + m_boxes[index]->get_rect().height());
      ++fixed_count;
    }
    if(m_boxes[index]->get_horizontal_size_policy() == SizePolicy::Fixed) {
      m_min_size.setWidth(m_boxes[index]->get_rect().width());
    }
  }
  if(fixed_count == get_box_count()) {
    m_max_size.setHeight(m_min_size.height());
  }
  if(m_min_size.width() != QWIDGETSIZE_MAX) {
    m_max_size.setWidth(m_min_size.width());
  }
}

void Layout::resize_width(int width) {
  if(is_horizontal_one_row()) {
    if(width - m_fixed_size <= 0) {
      return;
    }
    auto result = m_width_constraints.solve(width/* - m_fixed_size*/, m_expanding_boxes);
    for(auto& pos : result) {
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
    if(height - m_fixed_size <= 0) {
      return;
    }
    auto result = m_width_constraints.solve(height - m_fixed_size, m_expanding_boxes);
    for(auto& pos : result) {
      auto index = m_name_map[pos.first];
      m_boxes[index]->set_size({m_boxes[index]->get_rect().width(), static_cast<int>(pos.second)});
    }
    for(auto i = 1; i < get_box_count(); ++i) {
      m_boxes[i]->set_pos({m_boxes[i]->get_rect().x(), m_boxes[i - 1]->get_rect().bottom() + 1});
    }
  }
}
