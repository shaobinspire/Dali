#include "Dali/Layout.hpp"
#include <stack>
#include <QWidget>
#include "Dali/ConstraintGraph.hpp"
#include "Dali/LayoutBox.hpp"

using namespace Dali;

Layout::Layout()
  : m_min_size(0, 0),
  m_max_size(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX)/*,
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
  calculate_min_max_size();
  if(!build_constraints()) {
    return false;
  }
  return true;
}

bool Layout::build_constraints() {
  ConstraintGraph width_constraint_graph(m_boxes.size());
  ConstraintGraph height_constraint_graph(m_boxes.size());
  for(auto i = 0; i < get_box_count(); ++i) {
    build_constraint_graph(width_constraint_graph, i,
      m_boxes[i]->get_width_constraint());
    build_constraint_graph(height_constraint_graph, i,
      m_boxes[i]->get_height_constraint());
  }
  if(!width_constraint_graph.topological_sort() ||
      !height_constraint_graph.topological_sort()) {
    return false;
  }
  m_width_sorted_constraint = width_constraint_graph.get_sorted_list();
  m_height_sorted_constraint = height_constraint_graph.get_sorted_list();
  return true;
}

void Layout::build_constraint_graph(ConstraintGraph& graph, int box_index,
    ConstraintExpression& constraint_expression) {
  for(auto i = 0; i < constraint_expression.get_element_count(); ++i) {
    auto& element = constraint_expression.get_element(i);
    if(auto variable = std::get_if<ConstraintExpression::Variable>(&element)) {
      if(m_name_map.contains(variable->m_name)) {
        auto index = m_name_map[variable->m_name];
        variable->m_index = index;
        graph.add_edge(box_index, index);
      }
    }
  }
}

void Layout::calculate_min_max_size() {
  //if(preprocess_base_cases()) {
  //  return;
  //}
  //build_graph();
  //get_paths();
  //rebuild_layout();
}

void Layout::resize_width(int width) {
  if(m_top_row.size() != get_box_count() && m_left_column.size() != get_box_count()) {
    return;
  }
  for(auto index : m_width_sorted_constraint) {
    auto box_width = m_boxes[index]->get_width_constraint().evaluate(
      [&] (auto i) {
        return m_boxes[i]->get_rect().width();
      },
      [&] {
        return width;
      });
    if(box_width < 0) {
      box_width = 0;
    }
    m_boxes[index]->set_size({static_cast<int>(box_width), m_boxes[index]->get_size().height()});
  }
  if(m_top_row.size() == get_box_count()) {
    for(auto i = 1; i < get_box_count(); ++i) {
      m_boxes[i]->set_pos({m_boxes[i - 1]->get_rect().right() + 1, m_boxes[i]->get_rect().y()});
    }
  }
}

void Layout::resize_height(int height) {
  if(m_top_row.size() != get_box_count() && m_left_column.size() != get_box_count()) {
    return;
  }
  for(auto index : m_height_sorted_constraint) {
    auto box_height = m_boxes[index]->get_height_constraint().evaluate(
      [&] (auto i) {
        return m_boxes[i]->get_rect().height();
      },
      [&] {
        return height;
      });
    if(box_height < 0) {
      box_height = 0;
    }
    m_boxes[index]->set_size({m_boxes[index]->get_size().width(), static_cast<int>(box_height)});
  }
  if(m_left_column.size() == get_box_count()) {
    for(auto i = 1; i < get_box_count(); ++i) {
      m_boxes[i]->set_pos({m_boxes[i]->get_rect().x(), m_boxes[i - 1]->get_rect().bottom() + 1});
    }
  }
}
