#include "Dali/Layout.hpp"
#include <chrono>
#include <map>
#include <fmt/core.h>
#include "Dali/LayoutBox.hpp"

using namespace Dali;
using namespace z3;

//void build_constraints(const std::vector<LayoutBox*>& boxes, const std::string& property,
//    Constraints& constraints) {
//  auto is_horiztonal = [&] {
//    if(property == "width") {
//      return true;
//    }
//    return false;
//  }();
//  auto get_value = [=] (auto index) {
//    if(is_horiztonal) {
//      return boxes[index]->get_rect().width();
//    } else {
//      return boxes[index]->get_rect().height();
//    }
//  };
//  auto expanding_box_without_constraint = std::vector<std::pair<int, int>>();
//  auto sum_expression = std::string();
//  for(auto i = 0; i < static_cast<int>(boxes.size()); ++i) {
//    sum_expression += boxes[i]->get_name() + "." + property + " +";
//    if(is_horiztonal) {
//      if(boxes[i]->get_horizontal_size_policy() == SizePolicy::Fixed) {
//        auto sstr = std::ostringstream();
//        sstr << boxes[i]->get_name() << "." << property << "=" << get_value(i);
//        constraints.add_local_constraint(Constraint(sstr.str()), true);
//      } else if(!constraints.has_varaible_name_in_global(boxes[i]->get_name())) {
//        expanding_box_without_constraint.push_back({get_value(i), i});
//      }
//    } else {
//      if(boxes[i]->get_vertical_size_policy() == SizePolicy::Fixed) {
//        auto sstr = std::ostringstream();
//        sstr << boxes[i]->get_name() << "." << property << "=" << get_value(i);
//        constraints.add_local_constraint(Constraint(sstr.str()), true);
//      } else if(!constraints.has_varaible_name_in_global(boxes[i]->get_name())) {
//        expanding_box_without_constraint.push_back({get_value(i), i});
//      }
//    }
//    auto sstr = std::ostringstream();
//    sstr << boxes[i]->get_name() << "." << property << ">= 0";
//    constraints.add_local_constraint(Constraint(sstr.str()), true);
//  }
//  if(!expanding_box_without_constraint.empty()) { //    std::sort(expanding_box_without_constraint.begin(), expanding_box_without_constraint.end());
//    for(auto iter = expanding_box_without_constraint.begin() + 1; iter != expanding_box_without_constraint.end(); ++iter) {
//      auto sstr = std::ostringstream();
//      sstr << boxes[iter->second]->get_name() << "." << property << "=" <<
//        boxes[expanding_box_without_constraint.begin()->second]->get_name() <<
//        "." << property << " * " <<
//        static_cast<double>(iter->first) / expanding_box_without_constraint.begin()->first;
//      constraints.add_local_constraint(Constraint(sstr.str()), false);
//    }
//  }
//  sum_expression.pop_back();
//  sum_expression += "= width";
//  constraints.add_local_constraint(Constraint(sum_expression), true);
//}

Layout::Layout()
  : m_min_size(0, 0),
    m_max_size(MAX_LAYOUT_SIZE, MAX_LAYOUT_SIZE),
    m_status(Status::NONE),
    m_min_fixed_box_width(MAX_LAYOUT_SIZE),
    m_min_fixed_box_height(MAX_LAYOUT_SIZE),
    m_total_fixed_box_width(0),
    m_total_fixed_box_height(0) {}

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
  if(box->get_horizontal_size_policy() == SizePolicy::Fixed) {
    m_min_fixed_box_width = std::min(m_min_fixed_box_width, box->get_rect().width());
    m_total_fixed_box_width += box->get_rect().width();
  }
  if(box->get_vertical_size_policy() == SizePolicy::Fixed) {
    m_min_fixed_box_height = std::min(m_min_fixed_box_height, box->get_rect().height());
    m_total_fixed_box_height += box->get_rect().height();
  }
  m_rect = m_rect.united(box->get_rect());
  if(box->get_name().empty()) {
    box->set_name("DALI_LAYOUT_BOX" + m_boxes.size());
    box->set_name_visible(false);
  }
  m_boxes.push_back(box);
  //m_box_rects.push_back(box->get_rect());
  m_name_map[box->get_name()] = static_cast<int>(m_boxes.size()) - 1;
  m_min_pos.setX(std::min(m_min_pos.x(), box->get_rect().x()));
  m_min_pos.setY(std::min(m_min_pos.y(), box->get_rect().y()));
  m_max_pos.setX(std::max(m_max_pos.x(), box->get_rect().right()));
  m_max_pos.setY(std::max(m_max_pos.y(), box->get_rect().bottom()));
}

void Layout::add_width_constraint(const Constraint& constraint) {
  m_horizontal_constraints.add_global_constraint(constraint);
}

void Layout::add_height_constraint(const Constraint& constraint) {
  m_vertical_constraints.add_global_constraint(constraint);
}

void Layout::add_position_constraint(const Constraint& constraint) {
  m_position_constraints.add_local_constraint(constraint, true);
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

QSize Layout::get_min_size() {
  //m_min_size.setWidth(m_width_solver.get_min_value());
  //m_min_size.setHeight(m_height_solver.get_min_value());
  return m_min_size;
}

QSize Layout::get_max_size() {
  //auto max_width = m_width_solver.get_max_value();
  //if(max_width == -1) {
  //  m_max_size.setWidth(m_min_size.width());
  //} else {
  //  m_max_size.setWidth(max_width);
  //}
  return m_max_size;
}

void Layout::resize(const QSize& size) {
  auto boxes_rects = std::vector<QRect>();
  for(auto box : m_boxes) {
    boxes_rects.push_back(box->get_rect());
  }
  auto rows = build_rows(boxes_rects);
  auto horizontal_formulas = build_horizontal_formulas(rows);
  qDebug() << "horizontal:";
  auto widths = m_horizontal_solver.solve(horizontal_formulas, size.width());
  adjust_horizontal_layout(widths, rows, boxes_rects);
  auto columns = build_columns(boxes_rects);
  auto vertical_formulas = build_vertical_formulas(columns);
  qDebug() << "vertical:";
  auto heights = m_vertical_solver.solve(vertical_formulas, size.height());
  adjust_vertical_layout(heights, columns, boxes_rects);
  auto formulas = expr_vector(m_position_solver.get_context());
  for(auto i = 0; i < m_position_constraints.get_constraint_count(); ++i) {
    auto formula = set_position_variable_value(boxes_rects, m_position_constraints.get_constraint(i).get_variables());
    for(auto iter = formula.begin(); iter != formula.end(); ++iter) {
      formulas.push_back(*iter);
    }
  }
  if(!m_position_solver.check(formulas)) {
    m_status = Status::NONE;
    return;
  }
  //{
  //  auto rows = build_rows(boxes_rects);
  //  auto horizontal_formulas = build_horizontal_formulas(rows);
  //  qDebug() << "horizontal:";
  //  auto widths = m_horizontal_solver.solve(horizontal_formulas, size.width());
  //  adjust_horizontal_layout(widths, rows, boxes_rects);
  //  auto columns = build_columns(boxes_rects);
  //  auto vertical_formulas = build_vertical_formulas(columns);
  //  qDebug() << "vertical:";
  //  auto heights = m_vertical_solver.solve(vertical_formulas, size.height());
  //  adjust_vertical_layout(heights, columns, boxes_rects);
  //}
  m_boxes_rects = boxes_rects;
  auto rect = QRect();
  auto area = 0;
  for(auto i = 0; i < get_box_count(); ++i) {
    area += boxes_rects[i].width() * boxes_rects[i].height();
    rect = rect.united(boxes_rects[i]);
  }
  auto area2 = (boxes_rects.back().bottom() + 1) * (boxes_rects.back().right() + 1);
  if(area > area2) {
    m_status = Status::LAYOUT_OVERFLOW;
    return;
  } else if(area < area2) {
    m_status = Status::LAYOUT_UNDERFLOW;
    return;
  }
  m_status = Status::NONE;
  for(auto i = 0; i < get_box_count(); ++i) {
    m_boxes[i]->set_rect(boxes_rects[i]);
  }
  m_rect.setSize(rect.size());

  //qDebug() << size;
  //static auto start = std::chrono::high_resolution_clock::now();
  //auto t = std::chrono::high_resolution_clock::now();
  //auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t - start);
  //qDebug() << "stat: " << start.time_since_epoch().count() << " end:" << t.time_since_epoch().count() << " resize time : " << duration.count() << " " << size;
  //start = t;
  //auto start = std::chrono::high_resolution_clock::now();
  //rebuild_constraints();
  //if(m_last_size.width() != size.width()) {
  //  resize_width(size.width());
  //} else if(m_last_size.height() != size.height()) {
  //  resize_height(size.height());
  //}
  //m_last_size = size;
  //auto stop = std::chrono::high_resolution_clock::now();
  //auto duration = duration_cast<std::chrono::microseconds>(stop - start);
  //qDebug() << "rebuild time:" << duration.count();
  //start = std::chrono::high_resolution_clock::now();
  //auto width_result = m_horizontal_solver.solve(size.width());
  //auto height_result = m_vertical_solver.solve(size.height());
  //stop = std::chrono::high_resolution_clock::now();
  //duration = duration_cast<std::chrono::microseconds>(stop - start);
  //qDebug() << "solve time:" << duration.count();
  ////start = std::chrono::high_resolution_clock::now();
  //for(auto& pos : width_result) {
  //  if(!m_name_map.contains(pos.first)) {
  //    continue;
  //  }
  //  auto index = m_name_map[pos.first];
  //  if(m_boxes[index]->get_horizontal_size_policy() != SizePolicy::Fixed) {
  //    m_box_rects[index].setWidth(static_cast<int>(pos.second));
  //  }
  //}
  //if(!width_result.empty()) {
  //  for(auto& row : m_rows) {
  //    for(auto i = 1; i < static_cast<int>(row.size()); ++i) {
  //      auto index = row[i];
  //      m_box_rects[index].moveLeft(m_box_rects[row[i - 1]].right() + 1);
  //    }
  //  }
  //}
  //for(auto& pos : height_result) {
  //  if(!m_name_map.contains(pos.first)) {
  //    continue;
  //  }
  //  auto index = m_name_map[pos.first];
  //  if(m_boxes[index]->get_vertical_size_policy() != SizePolicy::Fixed) {
  //    m_box_rects[index].setHeight(static_cast<int>(pos.second));
  //  }
  //}
  //if(!height_result.empty()) {
  //  for(auto& column : m_columns) {
  //    for(auto i = 1; i < static_cast<int>(column.size()); ++i) {
  //      auto index = column[i];
  //      m_box_rects[index].moveTop(m_box_rects[column[i - 1]].bottom() + 1);
  //    }
  //  }
  //}
  //auto stop = std::chrono::high_resolution_clock::now();
  //auto duration = duration_cast<std::chrono::microseconds>(stop - start);
  //qDebug() << "resize time:" << duration.count();
  //auto rect = QRect();
  //auto area = 0;
  //for(auto i = 0; i < get_box_count(); ++i) {
  //  area += m_box_rects[i].width() * m_box_rects[i].height();
  //  rect = rect.united(m_box_rects[i]);
  //}
  //auto area3 = rect.width() * rect.height();
  //auto area2 = (m_box_rects.back().bottom() + 1) * (m_box_rects.back().right() + 1);
  //if(area > area2) {
  //  m_status = Status::LAYOUT_OVERFLOW;
  //  return;
  //} else if(area < area2) {
  //  m_status = Status::LAYOUT_UNDERFLOW;
  //  return;
  //}
  //m_status = Status::NONE;
  //for(auto i = 0; i < get_box_count(); ++i) {
  //  m_boxes[i]->set_rect(m_box_rects[i]);
  //}
  //m_rect.setSize(rect.size());
}

bool Layout::build() {
  m_total_fixed_box_width = std::max(m_total_fixed_box_width, m_rect.width());
  m_total_fixed_box_height = std::max(m_total_fixed_box_height, m_rect.height());
  m_position_solver.add_const_formula(m_position_constraints.convert(m_position_solver.get_context()));
  auto horizontal_expanding_box = std::vector<std::pair<int, int>>();
  auto vertical_expanding_box = std::vector<std::pair<int, int>>();
  auto area = 0;
  for(auto i = 0; i < get_box_count(); ++i) {
    auto box = m_boxes[i];
    area += box->get_rect().width() * box->get_rect().height();
    if(box->get_horizontal_size_policy() == SizePolicy::Fixed) {
      m_horizontal_constraints.add_local_constraint(Constraint(
        fmt::format("{}.width = {}", box->get_name(), box->get_rect().width())), true);
    } else {
      m_horizontal_constraints.add_local_constraint(Constraint(
        fmt::format("{}.width >= 0", box->get_name())), true);
      if(!m_horizontal_constraints.has_varaible_name_in_global(box->get_name())) {
        horizontal_expanding_box.push_back({box->get_rect().width(), i});
      }
    }
    if(box->get_vertical_size_policy() == SizePolicy::Fixed) {
      m_vertical_constraints.add_local_constraint(Constraint(
        fmt::format("{}.height = {}", box->get_name(), box->get_rect().height())), true);
    } else {
      m_vertical_constraints.add_local_constraint(Constraint(
       fmt::format("{}.height >= 0", box->get_name())), true);
      if(!m_vertical_constraints.has_varaible_name_in_global(box->get_name())) {
        vertical_expanding_box.push_back({box->get_rect().height(), i});
      }
    }
  }
  if(area != m_rect.width() * m_rect.height()) {
    return false;
  }
  if(!horizontal_expanding_box.empty()) {
    std::sort(horizontal_expanding_box.begin(), horizontal_expanding_box.end());
    for(auto iter = horizontal_expanding_box.begin() + 1; iter != horizontal_expanding_box.end(); ++iter) {
      m_horizontal_constraints.add_local_constraint(Constraint(
        fmt::format("{}.width = {}.width * {}", m_boxes[iter->second]->get_name(),
        m_boxes[horizontal_expanding_box.begin()->second]->get_name(),
        static_cast<double>(iter->first) / horizontal_expanding_box.begin()->first)), false);
    }
  }
  if(!vertical_expanding_box.empty()) {
    std::sort(vertical_expanding_box.begin(), vertical_expanding_box.end());
    for(auto iter = vertical_expanding_box.begin() + 1; iter != vertical_expanding_box.end(); ++iter) {
      m_vertical_constraints.add_local_constraint(Constraint(
        fmt::format("{}.height = {}.height * {}", m_boxes[iter->second]->get_name(),
        m_boxes[vertical_expanding_box.begin()->second]->get_name(),
        static_cast<double>(iter->first) / vertical_expanding_box.begin()->first)), false);
    }
  }
  qDebug() << "horizontal const fromula:";
  m_horizontal_solver.add_const_formula(m_horizontal_constraints.convert(m_horizontal_solver.get_context()));
  qDebug() << "vertcial const fromula:";
  m_vertical_solver.add_const_formula(m_vertical_constraints.convert(m_vertical_solver.get_context()));
  calculate_min_max_size();
  return true;
}

int Layout::get_index_by_name(const std::string& name) {
  if(auto iter = m_name_map.find(name); iter != m_name_map.end()) {
    return iter->second;
  }
  return -1;
}

const std::vector<QRect>& Layout::get_box_rect() const {
  return m_boxes_rects;
}

Layout::Status Layout::get_status() const {
  return m_status;
}

//bool Layout::is_horizontal_one_row() const {
//  return m_top_row.size() == get_box_count();
//}
//
//bool Layout::is_vertical_one_column() const {
//  return m_left_column.size() == get_box_count();
//}
//
//void Layout::calculate_one_row_min_max_size() {
//  m_min_size.setWidth(m_horizontal_solver.get_min_value());
//  auto min_height = 0;
//  for(auto index : m_top_row) {
//    if(m_boxes[index]->get_vertical_size_policy() == SizePolicy::Fixed) {
//      min_height = m_boxes[index]->get_rect().height();
//    }
//  }
//  m_min_size.setHeight(min_height);
//  auto max_width = m_horizontal_solver.get_max_value();
//  if(max_width == -1) {
//    m_max_size.setWidth(m_min_size.width());
//  } else {
//    m_max_size.setWidth(max_width);
//  }
//  if(m_min_size.height() != 0) {
//    m_max_size.setHeight(m_min_size.height());
//  }
//}
//
//void Layout::calculate_one_column_min_max_size() {
//  m_min_size.setHeight(m_vertical_solver.get_min_value());
//  auto min_width = 0;
//  for(auto index : m_left_column) {
//    if(m_boxes[index]->get_horizontal_size_policy() == SizePolicy::Fixed) {
//      min_width = m_boxes[index]->get_rect().width();
//    }
//  }
//  m_min_size.setWidth(min_width);
//  auto max_height = m_horizontal_solver.get_max_value();
//  if(max_height == -1) {
//    m_max_size.setHeight(m_min_size.width());
//  } else {
//    m_max_size.setHeight(max_height);
//  }
//  if(m_min_size.width() != 0) {
//    m_max_size.setWidth(m_min_size.width());
//  }
//}

//void Layout::rebuild_constraints() {
//  auto rows = std::vector<int>();
//  auto columns = std::vector<int>();
//  for(auto i = 0; i < get_box_count(); ++i) {
//    if(m_box_rect[i].width() != 0) {
//      columns.push_back(m_box_rect[i].x());
//    }
//    if(m_box_rect[i].height() != 0) {
//      rows.push_back(m_box_rect[i].y());
//    }
//  }
//  std::sort(rows.begin(), rows.end());
//  std::sort(columns.begin(), columns.end());
//  rows.erase(std::unique(rows.begin(), rows.end()), rows.end());
//  columns.erase(std::unique(columns.begin(), columns.end()), columns.end());
//  auto rows = std::vector<std::vector<int>>();
//  auto columns = std::vector<std::vector<int>>();
//  rows.resize(rows.size());
//  columns.resize(columns.size());
//  for(auto i = 0; i < get_box_count(); ++i) {
//    auto rect = m_box_rect[i];
//    for(auto j = 0; j != static_cast<int>(columns.size()); ++j) {
//      auto x = columns[j];
//      if(rect.width() != 0 && (x >= rect.x() && x < rect.right() ||
//          x == rect.x() && rect.width() == 0)) {
//        m_columns[j].push_back(i);
//      } else if(x > rect.right()) {
//        break;
//      }
//    }
//    for(auto j = 0; j != static_cast<int>(rows.size()); ++j) {
//      auto y = rows[j];
//      if(rect.height() != 0 && (y >= rect.y() && y < rect.bottom() ||
//          y == rect.y() && rect.height() == 0)) {
//        m_rows[j].push_back(i);
//      } else if(y > rect.bottom()) {
//        break;
//      }
//    }
//  }
//  auto m_width_sum_formula = expr_vector(m_horizontal_solver.get_context());
//  //auto start = std::chrono::high_resolution_clock::now();
//  for(auto& row : m_rows) {
//    if(!row.empty()) {
//      std::sort(row.begin(), row.end(), [=] (int a, int b) {
//        return m_boxes[a]->get_rect().x() < m_boxes[b]->get_rect().x();
//      });
//      auto formula = m_horizontal_solver.declare_variable(m_boxes[row[0]]->get_name());
//      for(auto i = 1; i < static_cast<int>(row.size()); ++i) {
//        formula = formula + m_horizontal_solver.declare_variable(m_boxes[row[i]]->get_name());
//      }
//      m_width_sum_formula.push_back(formula == m_horizontal_solver.declare_variable(LAYOUT_NAME));
//    }
//  }
//  m_horizontal_solver.add_varying_formula(m_width_sum_formula);
//  //auto stop = std::chrono::high_resolution_clock::now();
//  //auto duration = duration_cast<std::chrono::microseconds>(stop - start);
//  //qDebug() << "build constraints time:" << duration.count();
//  auto m_height_sum_formula = expr_vector(m_vertical_solver.get_context());
//  for(auto& column : m_columns) {
//    if(!column.empty()) {
//      std::sort(column.begin(), column.end(), [=] (int a, int b) {
//        return m_boxes[a]->get_rect().y() < m_boxes[b]->get_rect().y();
//      });
//      auto formula = m_vertical_solver.declare_variable(m_boxes[column[0]]->get_name());
//      for(auto i = 1; i < static_cast<int>(column.size()); ++i) {
//        formula = formula + m_vertical_solver.declare_variable(m_boxes[column[i]]->get_name());
//      }
//      m_height_sum_formula.push_back(formula == m_vertical_solver.declare_variable(LAYOUT_NAME));
//    }
//  }
//  m_vertical_solver.add_varying_formula(m_height_sum_formula);
//}

//void Layout::rebuild_constraints(bool is_horizontal, Solver& solver, std::vector<std::vector<int>>& row_or_column) {
//  auto list = std::vector<int>();
//  for(auto i = 0; i < static_cast<int>(m_boxes.size()); ++i) {
//    if(is_horizontal) {
//      if(m_boxes[i]->get_rect().height() != 0) {
//        list.push_back(m_boxes[i]->get_pos().y());
//      }
//    } else {
//      if(m_boxes[i]->get_rect().width() != 0) {
//        list.push_back(m_boxes[i]->get_pos().x());
//      }
//    }
//  }
//  std::sort(list.begin(), list.end());
//  list.erase(std::unique(list.begin(), list.end()), list.end());
//  row_or_column.resize(list.size());
//  for(auto i = 0; i < static_cast<int>(m_boxes.size()); ++i) {
//    auto box = m_boxes[i];
//    for(auto j = 0; j != static_cast<int>(list.size()); ++j) {
//      auto value = list[j];
//      if(is_horizontal) {
//        if(box->get_rect().height() != 0 &&
//          (value >= box->get_rect().y() && value < box->get_rect().bottom() ||
//          value == box->get_rect().y() && box->get_rect().height() == 0)) {
//          row_or_column[j].push_back(i);
//        } else if(value > box->get_rect().bottom()) {
//          break;
//        }
//      } else {
//        if(box->get_rect().width() != 0 &&
//          (value >= box->get_rect().x() && value < box->get_rect().right() ||
//          value == box->get_rect().x() && box->get_rect().width() == 0)) {
//          row_or_column[j].push_back(i);
//        } else if(value > box->get_rect().right()) {
//          break;
//        }
//      }
//    }
//  }
//  auto sum_formula = expr_vector(solver.get_context());
//  for(auto& v : row_or_column) {
//    if(!v.empty()) {
//      std::sort(v.begin(), v.end(), [&] (int a, int b) {
//        return m_boxes[a]->get_rect().x() < m_boxes[b]->get_rect().x();
//      });
//      auto formula = solver.declare_variable(m_boxes[v[0]]->get_name());
//      for(auto i = 1; i < static_cast<int>(v.size()); ++i) {
//        formula = formula + solver.declare_variable(m_boxes[v[i]]->get_name());
//      }
//      sum_formula.push_back(formula == solver.declare_variable(LAYOUT_NAME));
//    }
//  }
//  solver.add_varying_formula(sum_formula);
//}
//
//void Layout::rebuild_horizontal_constraints() {
//  m_rows.clear();
//  rebuild_constraints(true, m_horizontal_solver, m_rows);
//  //auto rows = std::vector<int>();
//  //auto columns = std::vector<int>();
//  //for(auto i = 0; i < get_box_count(); ++i) {
//  //  if(m_boxes[i]->get_rect().width() != 0) {
//  //    columns.push_back(m_boxes[i]->get_pos().x());
//  //  }
//  //  if(m_boxes[i]->get_rect().height() != 0) {
//  //    rows.push_back(m_boxes[i]->get_pos().y());
//  //  }
//  //}
//  //std::sort(rows.begin(), rows.end());
//  //std::sort(columns.begin(), columns.end());
//  //rows.erase(std::unique(rows.begin(), rows.end()), rows.end());
//  //columns.erase(std::unique(columns.begin(), columns.end()), columns.end());
//  //m_rows.clear();
//  //m_columns.clear();
//  //m_rows.resize(rows.size());
//  //m_columns.resize(columns.size());
//  //for(auto i = 0; i < get_box_count(); ++i) {
//  //  auto box = m_boxes[i];
//  //  for(auto j = 0; j != static_cast<int>(columns.size()); ++j) {
//  //    auto x = columns[j];
//  //    if(box->get_rect().width() != 0 &&
//  //        (x >= box->get_rect().x() && x < box->get_rect().right() ||
//  //        x == box->get_rect().x() && box->get_rect().width() == 0)) {
//  //      m_columns[j].push_back(i);
//  //    } else if(x > box->get_rect().right()) {
//  //      break;
//  //    }
//  //  }
//  //  for(auto j = 0; j != static_cast<int>(rows.size()); ++j) {
//  //    auto y = rows[j];
//  //    if(box->get_rect().height() != 0 &&
//  //        (y >= box->get_rect().y() && y < box->get_rect().bottom() ||
//  //        y == box->get_rect().y() && box->get_rect().height() == 0)) {
//  //      m_rows[j].push_back(i);
//  //    } else if(y > box->get_rect().bottom()) {
//  //      break;
//  //    }
//  //  }
//  //}
//  //auto m_width_sum_formula = expr_vector(m_horizontal_solver.get_context());
//  ////auto start = std::chrono::high_resolution_clock::now();
//  //for(auto& row : m_rows) {
//  //  if(!row.empty()) {
//  //    std::sort(row.begin(), row.end(), [=] (int a, int b) {
//  //      return m_boxes[a]->get_rect().x() < m_boxes[b]->get_rect().x();
//  //    });
//  //    auto formula = m_horizontal_solver.declare_variable(m_boxes[row[0]]->get_name());
//  //    for(auto i = 1; i < static_cast<int>(row.size()); ++i) {
//  //      formula = formula + m_horizontal_solver.declare_variable(m_boxes[row[i]]->get_name());
//  //    }
//  //    m_width_sum_formula.push_back(formula == m_horizontal_solver.declare_variable(LAYOUT_NAME));
//  //  }
//  //}
//  //m_horizontal_solver.add_varying_formula(m_width_sum_formula);
//  ////auto stop = std::chrono::high_resolution_clock::now();
//  ////auto duration = duration_cast<std::chrono::microseconds>(stop - start);
//  ////qDebug() << "build constraints time:" << duration.count();
//  //auto m_height_sum_formula = expr_vector(m_vertical_solver.get_context());
//  //for(auto& column : m_columns) {
//  //  if(!column.empty()) {
//  //    std::sort(column.begin(), column.end(), [=] (int a, int b) {
//  //      return m_boxes[a]->get_rect().y() < m_boxes[b]->get_rect().y();
//  //    });
//  //    auto formula = m_vertical_solver.declare_variable(m_boxes[column[0]]->get_name());
//  //    for(auto i = 1; i < static_cast<int>(column.size()); ++i) {
//  //      formula = formula + m_vertical_solver.declare_variable(m_boxes[column[i]]->get_name());
//  //    }
//  //    m_height_sum_formula.push_back(formula == m_vertical_solver.declare_variable(LAYOUT_NAME));
//  //  }
//  //}
//  //m_vertical_solver.add_varying_formula(m_height_sum_formula);
//}
//
//void Layout::rebuild_vertical_constraints() {
//  m_columns.clear();
//  rebuild_constraints(false, m_vertical_solver, m_columns);
//}
//
//void Layout::resize_width(int width) {
//  auto start = std::chrono::high_resolution_clock::now();
//  rebuild_horizontal_constraints();
//  auto width_result = m_horizontal_solver.solve(width);
//  for(auto& pos : width_result) {
//    if(!m_name_map.contains(pos.first)) {
//      continue;
//    }
//    auto index = m_name_map[pos.first];
//    if(m_boxes[index]->get_horizontal_size_policy() != SizePolicy::Fixed) {
//      m_box_rects[index].setWidth(static_cast<int>(pos.second));
//    }
//  }
//  if(!width_result.empty()) {
//    for(auto& row : m_rows) {
//      for(auto i = 1; i < static_cast<int>(row.size()); ++i) {
//        auto index = row[i];
//        m_box_rects[index].moveLeft(m_box_rects[row[i - 1]].right() + 1);
//      }
//    }
//  }
//  auto stop = std::chrono::high_resolution_clock::now();
//  auto duration = duration_cast<std::chrono::microseconds>(stop - start);
//  qDebug() << "resize width time:" << duration.count();
//}
//
//void Layout::resize_height(int height) {
//  auto start = std::chrono::high_resolution_clock::now();
//  rebuild_vertical_constraints();
//  auto height_result = m_vertical_solver.solve(height);
//  for(auto& pos : height_result) {
//    if(!m_name_map.contains(pos.first)) {
//      continue;
//    }
//    auto index = m_name_map[pos.first];
//    if(m_boxes[index]->get_vertical_size_policy() != SizePolicy::Fixed) {
//      m_box_rects[index].setHeight(static_cast<int>(pos.second));
//    }
//  }
//  if(!height_result.empty()) {
//    for(auto& column : m_columns) {
//      for(auto i = 1; i < static_cast<int>(column.size()); ++i) {
//        auto index = column[i];
//        m_box_rects[index].moveTop(m_box_rects[column[i - 1]].bottom() + 1);
//      }
//    }
//  }
//  auto stop = std::chrono::high_resolution_clock::now();
//  auto duration = duration_cast<std::chrono::microseconds>(stop - start);
//  qDebug() << "resize height time:" << duration.count();
//}

void Layout::build_constraints(const std::vector<int>& range,
  const std::string& property, Constraints& constraints) {
  //auto is_horiztonal = [&] {
  //  if(property == "width") {
  //    return true;
  //  }
  //  return false;
  //}();
  //auto get_value = [=] (auto index) {
  //  if(is_horiztonal) {
  //    return m_boxes[index]->get_rect().width();
  //  } else {
  //    return m_boxes[index]->get_rect().height();
  //  }
  //};
  //auto expanding_box_without_constraint = std::vector<std::pair<int, int>>();
  auto sum_expression = std::string();
  for(auto i : range) {
    auto box = m_boxes[i];
    sum_expression += box->get_name() + "." + property + " +";
    //if(is_horiztonal) {
    //  if(box->get_horizontal_size_policy() == SizePolicy::Fixed) {
    //    auto sstr = std::ostringstream();
    //    sstr << box->get_name() << "." << property << "=" << get_value(i);
    //    constraints.add_local_constraint(Constraint(sstr.str()), true);
    //  } else if(!constraints.has_varaible_name_in_global(box->get_name()) && get_value(i) != 0) {
    //    expanding_box_without_constraint.push_back({get_value(i), i});
    //  }
    //} else {
    //  if(box->get_vertical_size_policy() == SizePolicy::Fixed) {
    //    auto sstr = std::ostringstream();
    //    sstr << box->get_name() << "." << property << "=" << get_value(i);
    //    constraints.add_local_constraint(Constraint(sstr.str()), true);
    //  } else if(!constraints.has_varaible_name_in_global(box->get_name()) && get_value(i) != 0) {
    //    expanding_box_without_constraint.push_back({get_value(i), i});
    //  }
    //}
    //auto sstr = std::ostringstream();
    //sstr << box->get_name() << "." << property << ">= 0";
    //constraints.add_local_constraint(Constraint(sstr.str()), true);
  }
  //if(!expanding_box_without_constraint.empty()) {
  //  std::sort(expanding_box_without_constraint.begin(), expanding_box_without_constraint.end());
  //  for(auto iter = expanding_box_without_constraint.begin() + 1; iter != expanding_box_without_constraint.end(); ++iter) {
  //    auto sstr = std::ostringstream();
  //    sstr << m_boxes[iter->second]->get_name() << "." << property << "=" <<
  //      m_boxes[expanding_box_without_constraint.begin()->second]->get_name() <<
  //      "." << property << " * " <<
  //      static_cast<double>(iter->first) / expanding_box_without_constraint.begin()->first;
  //    constraints.add_local_constraint(Constraint(sstr.str()), false);
  //  }
  //}
  sum_expression.pop_back();
  sum_expression += " = " + property;
  //auto start = std::chrono::high_resolution_clock::now();
  constraints.add_local_constraint(Constraint(sum_expression), true);
  //auto stop = std::chrono::high_resolution_clock::now();
  //auto duration = duration_cast<std::chrono::microseconds>(stop - start);
  //qDebug() << "add local constraint time:" << duration.count();
}

void Layout::adjust_horizontal_layout(
    const std::unordered_map<std::string, double>& boxes_widths,
    const std::vector<std::vector<int>>& rows, std::vector<QRect>& boxes_rects) {
  for(auto& box : boxes_widths) {
    auto name = box.first.substr(0, box.first.find_first_of('.'));
    if(!m_name_map.contains(name)) {
      continue;
    }
    boxes_rects[m_name_map[name]].setWidth(static_cast<int>(box.second));
  }
  if(!boxes_widths.empty()) {
    qDebug() << "adjust row:";
    for(auto& row : rows) {
      auto debug = qDebug();
      debug << row[0] << " " << boxes_rects[row[0]] << ";";
      for(auto i = 1; i < static_cast<int>(row.size()); ++i) {
        boxes_rects[row[i]].moveLeft(boxes_rects[row[i - 1]].right() + 1);
        debug << row[i] << " " << boxes_rects[row[i]] << ";";
      }
    }
  }
}

void Layout::adjust_vertical_layout(
    const std::unordered_map<std::string, double>& boxes_heights,
    const std::vector<std::vector<int>>& columns,
    std::vector<QRect>& boxes_rects) {
  for(auto& box : boxes_heights) {
    auto name = box.first.substr(0, box.first.find_first_of('.'));
    if(!m_name_map.contains(name)) {
      continue;
    }
    boxes_rects[m_name_map[name]].setHeight(static_cast<int>(box.second));
  }
  if(!boxes_heights.empty()) {
    qDebug() << "adjust column:";
    for(auto& column : columns) {
      auto debug = qDebug();
      debug << column[0] << " " << boxes_rects[column[0]] << ";";
      for(auto i = 1; i < static_cast<int>(column.size()); ++i) {
        boxes_rects[column[i]].moveTop(boxes_rects[column[i - 1]].bottom() + 1);
        debug << column[i] << " " << boxes_rects[column[i]] << ";";
      }
    }
  }
}

void Layout::permute_horizontal(const std::vector<std::vector<int>>::iterator& iter, int size,
  const std::vector<std::vector<int>>::iterator& end, int width, std::vector<int>& tmp, std::vector<std::vector<int>>& result) {
  if(tmp.size() == size || iter == end) {
    auto w = 0;
    for(auto i : tmp) {
      w += m_boxes[i]->get_rect().width();
    }
    if(w == width) {
      result.push_back(tmp);
    }
    return;
  }
  //for(auto i = 0; i < static_cast<int>(iter->size()); ++i) {
  //  if(tmp.empty()) {
  //    tmp.push_back((*iter)[i]);
  //  } else {
  //    if(m_boxes[(*iter)[i]]->get_rect().height() != 0) {
  //      tmp.push_back((*iter)[i]);
  //    }
  //  }
  //}
  for(auto i = 0; i < static_cast<int>(iter->size()); ++i) {
    if(!tmp.empty()) {
      if(m_boxes[tmp.back()]->get_rect().right() + 1 == m_boxes[(*iter)[i]]->get_rect().left()) {
        tmp.push_back((*iter)[i]);
      }
    } else {
      tmp.push_back((*iter)[i]);
    }
    permute_horizontal(std::next(iter), size, end, width, tmp, result);
    if(!tmp.empty()) {
      tmp.pop_back();
    }
  }
}

void Layout::permute_vertical(const std::vector<std::vector<int>>::iterator& iter, int size,
  const std::vector<std::vector<int>>::iterator& end, int height, std::vector<int>& tmp, std::vector<std::vector<int>>& result) {
  if(tmp.size() == size || iter == end) {
    auto h = 0;
    for(auto i : tmp) {
      h += m_boxes[i]->get_rect().height();
    }
    if(h == height) {
      result.push_back(tmp);
    }
    return;
  }
  for(auto i = 0; i < static_cast<int>(iter->size()); ++i) {
    if(!tmp.empty()) {
      if(m_boxes[tmp.back()]->get_rect().bottom() + 1 == m_boxes[(*iter)[i]]->get_rect().top()) {
        tmp.push_back((*iter)[i]);
      }
    } else {
      tmp.push_back((*iter)[i]);
    }
    permute_vertical(std::next(iter), size, end, height, tmp, result);
    if(!tmp.empty()) {
      tmp.pop_back();
    }
  }
}

std::vector<std::vector<int>> Layout::build_rows(
    const std::vector<QRect>& boxes_rects) {
  auto size = static_cast<int>(boxes_rects.size());
  auto top_map_row = std::map<int, std::vector<int>>();
  auto bottom_map_row = std::map<int, std::vector<int>>();
  auto total_rect = QRect();
  for(auto i = 0; i < size; ++i) {
    const auto& rect = boxes_rects[i];
    if(rect.width() != 0 && rect.height() != 0) {
      total_rect = total_rect.united(rect);
    }
    top_map_row[rect.top()].push_back(i);
    bottom_map_row[rect.bottom()].push_back(i);
  }
  auto map_rows = std::vector<std::map<int, std::vector<int>>>(top_map_row.size());
  for(auto i = 0; i < size; ++i) {
    const auto& rect = boxes_rects[i];
    auto index = 0;
    for(auto iter = top_map_row.begin(); iter != top_map_row.end(); ++iter, ++index) {
      auto y = iter->first;
      if((rect.height() == 0 && (y == rect.y() || y == rect.bottom())) || (y >= rect.y() && y <= rect.bottom())) {
        map_rows[index][rect.x()].push_back(i);
      } else if(y > rect.bottom()) {
        break;
      }
    }
    auto& last_row = *bottom_map_row.rbegin();
    for(auto iter = last_row.second.begin(); iter != last_row.second.end(); ++iter) {
      if(i == *iter) {
        map_rows.back()[rect.x()].push_back(i);
      }
    }

    //for(auto& bm : bottom_map_row) {
    //  auto y = bm.first;
    //  if((rect.height() == 0 && (y == rect.y() || y == rect.bottom())) || (y >= rect.y() && y <= rect.bottom())) {
    //    map_rows[y][rect.x()].push_back(i);
    //  } else if(y > rect.bottom()) {
    //    break;
    //  }
    //}
  }
  for(auto& v : map_rows.back()) {
    v.second.erase(std::unique(v.second.begin(), v.second.end()), v.second.end());
  }
  auto tmp_rows = std::vector<std::vector<std::vector<int>>>(map_rows.size());
  for(auto i = 0; i < static_cast<int>(map_rows.size()); ++i) {
    for(auto& m : map_rows[i]) {
      if(m.second.size() > 1) {
        auto sub_row = std::vector<int>();
        for(auto k : m.second) {
          if(boxes_rects[k].height() != 0 && boxes_rects[k].width() == 0) {
            tmp_rows[i].push_back({k});
          } else {
            sub_row.push_back(k);
          }
        }
        if(!sub_row.empty()) {
          tmp_rows[i].push_back(sub_row);
        }
      } else {
        tmp_rows[i].push_back(m.second);
      }
    }
  }
  auto rows = std::vector<std::vector<int>>();
  auto row = std::vector<int>();
  for(auto& r : tmp_rows) {
    permute_horizontal(r.begin(), r.size(), r.end(), total_rect.width(), row, rows);
  }
  return rows;

  /*
  auto size = static_cast<int>(boxes_rects.size());
  auto vertical_list = std::vector<int>();
  auto map_row = std::map<int, std::vector<int>>();
  auto rect = QRect();
  for(auto i = 0; i < size; ++i) {
    vertical_list.push_back(boxes_rects[i].y());
    if(boxes_rects[i].width() != 0 && boxes_rects[i].height() != 0) {
      rect = rect.united(boxes_rects[i]);
    }
    map_row[boxes_rects[i].bottom()].push_back(i);
  }
  std::sort(vertical_list.begin(), vertical_list.end());
  vertical_list.erase(std::unique(vertical_list.begin(), vertical_list.end()),
    vertical_list.end());
  auto map_rows = std::vector<std::map<int, std::vector<int>>>(vertical_list.size());
  for(auto i = 0; i < size; ++i) {
    const auto& rect = boxes_rects[i];
    for(auto j = 0; j != static_cast<int>(vertical_list.size()); ++j) {
      auto y = vertical_list[j];
      if((rect.height() == 0 && (y == rect.y() || y == rect.bottom())) || (y >= rect.y() && y <= rect.bottom())) {
        map_rows[j][rect.x()].push_back(i);
      } else if(y > rect.bottom()) {
        break;
      }
    }
  }
  auto tmp_rows = std::vector<std::vector<std::vector<int>>>(map_rows.size());
  for(auto i = 0; i < static_cast<int>(tmp_rows.size()); ++i) {
    for(auto& m : map_rows[i]) {
      if(m.second.size() > 1) {
        auto sub_row = std::vector<int>();
        for(auto k : m.second) {
          if(boxes_rects[k].height() != 0 && boxes_rects[k].width() == 0) {
            tmp_rows[i].push_back({k});
          } else {
            sub_row.push_back(k);
          }
        }
        if(!sub_row.empty()) {
          tmp_rows[i].push_back(sub_row);
        }
      } else {
        tmp_rows[i].push_back(m.second);
      }
    }
  }
  auto rows = std::vector<std::vector<int>>();
  auto row = std::vector<int>();
  for(auto& r : tmp_rows) {
    permute_horizontal(r.begin(), r.size(), r.end(), rect.width(), row, rows);
  }
  //for(auto& r : map_rows) {
  //  permute_horizontal(r.begin(), r.size(), r.end(), rect.width(), row, rows);
  //}
  //for(auto& row : rows) {
  //  if(!row.empty()) {
  //    std::sort(row.begin(), row.end(), [&] (int a, int b) {
  //      return boxes_rects[a].x() < boxes_rects[b].x();
  //    });
  //  }
  //}
  return rows;
  */
}

std::vector<std::vector<int>> Layout::build_columns(
    const std::vector<QRect>& boxes_rects) {
  auto size = static_cast<int>(boxes_rects.size());
  auto total_rect = QRect();
  auto left_map_column = std::map<int, std::vector<int>>();
  auto right_map_column = std::map<int, std::vector<int>>();
  for(auto i = 0; i < size; ++i) {
    const auto& rect = boxes_rects[i];
    if(rect.width() != 0 && rect.height() != 0) {
      total_rect = total_rect.united(rect);
    }
    left_map_column[rect.left()].push_back(i);
    right_map_column[rect.right()].push_back(i);
  }
  auto map_columns = std::vector<std::map<int, std::vector<int>>>(left_map_column.size());
  for(auto i = 0; i < size; ++i) {
    const auto& rect = boxes_rects[i];
    auto index = 0;
    for(auto iter = left_map_column.begin(); iter != left_map_column.end(); ++iter, ++index) {
      auto x = iter->first;
      if((rect.width() == 0 && (x == rect.x() || x == rect.right())) || (x >= rect.x() && x <= rect.right())) {
        map_columns[index][rect.y()].push_back(i);
      } else if(x > rect.right()) {
        break;
      }
    }
    auto& last_column = *right_map_column.rbegin();
    for(auto iter = last_column.second.begin(); iter != last_column.second.end(); ++iter) {
      if(i == *iter) {
        map_columns.back()[rect.y()].push_back(i);
      }
    }
  }
  for(auto& v : map_columns.back()) {
    v.second.erase(std::unique(v.second.begin(), v.second.end()), v.second.end());
  }
  auto tmp_columns = std::vector<std::vector<std::vector<int>>>(map_columns.size());
  for(auto i = 0; i < static_cast<int>(tmp_columns.size()); ++i) {
    for(auto& m : map_columns[i]) {
      if(m.second.size() > 1) {
        auto sub_column = std::vector<int>();
        for(auto k : m.second) {
          if(boxes_rects[k].height() == 0 && boxes_rects[k].width() != 0) {
            tmp_columns[i].push_back({k});
          } else {
            sub_column.push_back(k);
          }
        }
        if(!sub_column.empty()) {
          tmp_columns[i].push_back(sub_column);
        }
      } else {
        tmp_columns[i].push_back(m.second);
      }
    }
  }
  auto columns = std::vector<std::vector<int>>();
  auto column = std::vector<int>();
  for(auto& c : tmp_columns) {
    permute_vertical(c.begin(), c.size(), c.end(), total_rect.height(), column, columns);
  }
  return columns;
  //auto size = static_cast<int>(boxes_rects.size());
  //auto horizontal_list = std::vector<int>();
  //auto rect = QRect();
  //for(auto i = 0; i < size; ++i) {
  //  //if(boxes_rects[i].width() != 0) {
  //    horizontal_list.push_back(boxes_rects[i].x());
  //  //}
  //    if(boxes_rects[i].width() != 0 && boxes_rects[i].height() != 0) {
  //      rect = rect.united(boxes_rects[i]);
  //    }
  //}
  //std::sort(horizontal_list.begin(), horizontal_list.end());
  //horizontal_list.erase(
  //  std::unique(horizontal_list.begin(), horizontal_list.end()),
  //  horizontal_list.end());
  ////auto columns = std::vector<std::vector<int>>(horizontal_list.size());
  //auto map_columns = std::vector<std::map<int, std::vector<int>>>(horizontal_list.size());
  //for(auto i = 0; i < size; ++i) {
  //  const auto& rect = boxes_rects[i];
  //  for(auto j = 0; j != static_cast<int>(horizontal_list.size()); ++j) {
  //    auto x = horizontal_list[j];
  //    //if(rect.width() != 0 && x >= rect.x() && x <= rect.right()) {
  //    if((rect.width() == 0 && (x == rect.x() || x == rect.right())) || (x >= rect.x() && x <= rect.right())) {
  //      //columns[j].push_back(i);
  //      map_columns[j][rect.y()].push_back(i);
  //    } else if(x > rect.right()) {
  //      break;
  //    }
  //  }
  //}
  //auto tmp_columns = std::vector<std::vector<std::vector<int>>>(map_columns.size());
  //for(auto i = 0; i < static_cast<int>(tmp_columns.size()); ++i) {
  //  for(auto& m : map_columns[i]) {
  //    if(m.second.size() > 1) {
  //      auto sub_column = std::vector<int>();
  //      for(auto k : m.second) {
  //        if(boxes_rects[k].height() == 0 && boxes_rects[k].width() != 0) {
  //          tmp_columns[i].push_back({k});
  //        } else {
  //          sub_column.push_back(k);
  //        }
  //      }
  //      if(!sub_column.empty()) {
  //        tmp_columns[i].push_back(sub_column);
  //      }
  //    } else {
  //      tmp_columns[i].push_back(m.second);
  //    }
  //  }
  //}
  //auto columns = std::vector<std::vector<int>>();
  //auto column = std::vector<int>();
  //for(auto& c : tmp_columns) {
  //  permute_vertical(c.begin(), c.size(), c.end(), rect.height(), column, columns);
  //}
  ////for(auto& c : map_columns) {
  ////  permute_vertical(c.begin(), c.size(), c.end(), rect.height(), column, columns);
  ////}
  ////for(auto& column : columns) {
  ////  if(!column.empty()) {
  ////    std::sort(column.begin(), column.end(), [&] (int a, int b) {
  ////      return boxes_rects[a].y() < boxes_rects[b].y();
  ////    });
  ////  }
  ////}
  //return columns;
}

expr_vector Layout::build_horizontal_formulas(const std::vector<std::vector<int>>& rows) {
  auto formulas = expr_vector(m_horizontal_solver.get_context());
  for(auto& row : rows) {
    if(row.empty()) {
      continue;
    }
    auto formula = m_horizontal_solver.declare_variable(m_boxes[row[0]]->get_name() + ".width");
    for(auto i = 1; i < static_cast<int>(row.size()); ++i) {
      formula = formula + m_horizontal_solver.declare_variable(m_boxes[row[i]]->get_name() + ".width");
    }
    formulas.push_back(formula == m_horizontal_solver.declare_variable(LAYOUT_NAME));
  }
  return formulas;
}

z3::expr_vector Layout::build_vertical_formulas(const std::vector<std::vector<int>>& columns) {
  auto formulas = expr_vector(m_vertical_solver.get_context());
  for(auto& column : columns) {
    if(column.empty()) {
      continue;
    }
    auto formula = m_vertical_solver.declare_variable(m_boxes[column[0]]->get_name() + ".height");
    for(auto i = 1; i < static_cast<int>(column.size()); ++i) {
      formula = formula + m_vertical_solver.declare_variable(m_boxes[column[i]]->get_name() + ".height");
    }
    formulas.push_back(formula == m_vertical_solver.declare_variable(LAYOUT_NAME));
  }
  return formulas;
}

void Layout::calculate_min_max_size() {
  auto boxes_rects = std::vector<QRect>();
  auto horizontal_unchanged_variables = std::unordered_set<std::string>();
  auto vertical_unchanged_variables = std::unordered_set<std::string>();
  auto horizontal_additional_formulas = expr_vector(m_horizontal_solver.get_context());
  auto vertical_additional_formulas = expr_vector(m_vertical_solver.get_context());
  for(auto box : m_boxes) {
    boxes_rects.push_back(box->get_rect());
    if(box->get_horizontal_size_policy() == SizePolicy::Fixed) {
      horizontal_unchanged_variables.insert(box->get_name());
    } else {
      horizontal_additional_formulas.push_back(m_horizontal_solver.declare_variable(box->get_name() + ".width") % m_min_fixed_box_width == 0);
      horizontal_additional_formulas.push_back(m_horizontal_solver.declare_variable(box->get_name() + ".width") <= m_horizontal_solver.declare_variable(LAYOUT_NAME));
    }
    if(box->get_vertical_size_policy() == SizePolicy::Fixed) {
      vertical_unchanged_variables.insert(box->get_name());
    } else {
      vertical_additional_formulas.push_back(m_vertical_solver.declare_variable(box->get_name() + ".height") % m_min_fixed_box_height == 0);
      vertical_additional_formulas.push_back(m_vertical_solver.declare_variable(box->get_name() + ".height") <= m_vertical_solver.declare_variable(LAYOUT_NAME));
    }
  }
  //horizontal_unchanged_variables.insert(LAYOUT_NAME);
  //vertical_unchanged_variables.insert(LAYOUT_NAME);
  {
    auto rows = build_rows(boxes_rects);
    auto horizontal_formulas = build_horizontal_formulas(rows);
    for(auto iter = horizontal_additional_formulas.begin(); iter != horizontal_additional_formulas.end(); ++iter) {
      horizontal_formulas.push_back(*iter);
    }
    auto columns = build_columns(boxes_rects);
    auto vertical_formulas = build_vertical_formulas(columns);
    for(auto iter = vertical_additional_formulas.begin(); iter != vertical_additional_formulas.end(); ++iter) {
      vertical_formulas.push_back(*iter);
    }
    //auto min_width = m_horizontal_solver.solve_minimum(horizontal_formulas);
    //auto min_height = m_vertical_solver.solve_minimum(vertical_formulas);
    auto min_width = MAX_LAYOUT_SIZE;
    auto max_width = -1;
    auto min_height = MAX_LAYOUT_SIZE;
    auto max_height = -1;
    auto new_boxes_rects = boxes_rects;
    for(auto height = m_min_fixed_box_height; height <= m_total_fixed_box_height; height += m_min_fixed_box_height) {
      //auto solution = m_vertical_solver.solve(vertical_formulas, height);
      //if(solution.empty()) {
      //  continue;
      //}
      auto solutions = m_vertical_solver.solve(vertical_formulas, vertical_unchanged_variables, height);
      if(solutions.empty()) {
        continue;
      }
      for(auto& solution : solutions) {
        adjust_vertical_layout(solution, columns, new_boxes_rects);
        auto new_horizontal_formulas = build_horizontal_formulas(build_rows(new_boxes_rects));
        auto new_max_width = static_cast<int>(m_horizontal_solver.solve_maximum(new_horizontal_formulas, m_total_fixed_box_width));
        for(auto iter = horizontal_additional_formulas.begin(); iter != horizontal_additional_formulas.end(); ++iter) {
          new_horizontal_formulas.push_back(*iter);
        }
        new_horizontal_formulas.push_back(m_horizontal_solver.declare_variable(LAYOUT_NAME) <= m_total_fixed_box_width);
        auto new_min_width = static_cast<int>(m_horizontal_solver.solve_minimum(new_horizontal_formulas, horizontal_unchanged_variables));
        min_height = std::min(min_height, height);
        min_width = std::min(min_width, new_min_width);
        max_width = std::max(max_width, new_max_width);
        //columns = build_columns(new_boxes_rects);
        //vertical_formulas = build_vertical_formulas(columns);
        if(min_width == m_min_fixed_box_width && max_width == MAX_LAYOUT_SIZE) {
          break;
        }
      }
      if(min_width == m_min_fixed_box_width && max_width == MAX_LAYOUT_SIZE) {
        break;
      }
    }
    new_boxes_rects = boxes_rects;
    for(auto width = m_min_fixed_box_width; width <= m_total_fixed_box_width; width += m_min_fixed_box_width) {
      //auto solution = m_horizontal_solver.solve(horizontal_formulas, width);
      //if(solution.empty()) {
      //  continue;
      //}
      auto solutions = m_horizontal_solver.solve(horizontal_formulas, horizontal_unchanged_variables, width);
      if(solutions.empty()) {
        continue;
      }
      for(auto& solution : solutions) {
        adjust_horizontal_layout(solution, rows, new_boxes_rects);
        auto new_vertical_formulas = build_vertical_formulas(build_columns(new_boxes_rects));
        //auto new_max_height = static_cast<int>(m_vertical_solver.solve_maximum(new_vertical_formulas));
        auto new_max_height = static_cast<int>(m_vertical_solver.solve_maximum(new_vertical_formulas, m_total_fixed_box_height));
        for(auto iter = vertical_additional_formulas.begin(); iter != vertical_additional_formulas.end(); ++iter) {
          new_vertical_formulas.push_back(*iter);
        }
        new_vertical_formulas.push_back(m_vertical_solver.declare_variable(LAYOUT_NAME) <= m_total_fixed_box_height);
        auto new_min_height = static_cast<int>(m_vertical_solver.solve_minimum(new_vertical_formulas, vertical_unchanged_variables));
        //auto new_min_height = static_cast<int>(m_vertical_solver.solve_minimum(new_vertical_formulas));
        min_width = std::min(min_width, width);
        min_height = std::min(min_height, new_min_height);
        max_height = std::max(max_height, new_max_height);
        if(min_height == m_min_fixed_box_height && max_height == MAX_LAYOUT_SIZE) {
          break;
        }
        //rows = build_rows(new_boxes_rects);
        //horizontal_formulas = build_horizontal_formulas(rows);
      }
      if(min_height == m_min_fixed_box_height && max_height == MAX_LAYOUT_SIZE) {
        break;
      }
    }
    if(min_width == MAX_LAYOUT_SIZE) {
      m_min_size.setWidth(m_rect.width());
    } else {
      m_min_size.setWidth(min_width);
    }
    if(min_height == MAX_LAYOUT_SIZE) {
      m_min_size.setHeight(m_rect.height());
    } else {
      m_min_size.setHeight(min_height);
    }
    if(max_width == -1) {
      m_max_size.setWidth(m_min_size.width());
    } else {
      m_max_size.setWidth(max_width);
    }
    if(max_height == -1) {
      m_max_size.setHeight(m_min_size.height());
    } else {
      m_max_size.setHeight(max_height);
    }
  }

}

expr_vector Layout::set_position_variable_value(const std::vector<QRect>& boxes_rects, const std::vector<Constraint::Variable>& variables) {
  auto formulas = expr_vector(m_position_solver.get_context());
  for(auto& v : variables) {
    auto name = v.m_name;
    if(!m_name_map.contains(name)) {
      continue;
    }
    auto rect = boxes_rects[m_name_map[name]];
    auto value = [&] {
      if(v.m_property == Constraint::Property::LEFT) {
        return rect.x();
      } else if(v.m_property == Constraint::Property::TOP) {
        return rect.y();
      } else if(v.m_property == Constraint::Property::RIGHT) {
        return rect.right();
      } else if(v.m_property == Constraint::Property::BOTTOM) {
        return rect.bottom();
      }
      return -1;
    }();
    if(value >= 0) {
      formulas.push_back(m_position_solver.declare_variable(v.m_content) == value);
    }
  }
  return formulas;
}
