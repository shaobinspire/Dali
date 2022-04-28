#include "Dali/Layout.hpp"
#include <map>
#include <fmt/core.h>
#include "Dali/LayoutBox.hpp"

using namespace Dali;
using namespace z3;

void adjust_horizontal_layout(
    const std::unordered_map<std::string, int>& name_map,
    const std::unordered_map<std::string, double>& boxes_widths,
    const std::vector<std::vector<int>>& rows, std::vector<QRect>& boxes_rects) {
  for(auto& box : boxes_widths) {
    auto name = box.first.substr(0, box.first.find_first_of('.'));
    if(!name_map.contains(name)) {
      continue;
    }
    boxes_rects[name_map.at(name)].setWidth(static_cast<int>(box.second));
  }
  if(!boxes_widths.empty()) {
    //qDebug() << "adjust row:";
    for(auto& row : rows) {
      //auto debug = qDebug();
      //debug << row[0] << " " << boxes_rects[row[0]] << ";";
      for(auto i = 1; i < static_cast<int>(row.size()); ++i) {
        boxes_rects[row[i]].moveLeft(boxes_rects[row[i - 1]].right() + 1);
        //debug << row[i] << " " << boxes_rects[row[i]] << ";";
      }
    }
  }
}

void adjust_vertical_layout(
    const std::unordered_map<std::string, int>& name_map,
    const std::unordered_map<std::string, double>& boxes_heights,
    const std::vector<std::vector<int>>& columns,
    std::vector<QRect>& boxes_rects) {
  for(auto& box : boxes_heights) {
    auto name = box.first.substr(0, box.first.find_first_of('.'));
    if(!name_map.contains(name)) {
      continue;
    }
    boxes_rects[name_map.at(name)].setHeight(static_cast<int>(box.second));
  }
  if(!boxes_heights.empty()) {
    //qDebug() << "adjust column:";
    for(auto& column : columns) {
      //auto debug = qDebug();
      //debug << column[0] << " " << boxes_rects[column[0]] << ";";
      for(auto i = 1; i < static_cast<int>(column.size()); ++i) {
        boxes_rects[column[i]].moveTop(boxes_rects[column[i - 1]].bottom() + 1);
        //debug << column[i] << " " << boxes_rects[column[i]] << ";";
      }
    }
  }
}

void permute_horizontal(
  const std::vector<QRect>& boxes_rects, const std::vector<std::vector<int>>::iterator& iter, int size,
  const std::vector<std::vector<int>>::iterator& end, int width, std::vector<int>& tmp, std::vector<std::vector<int>>& result) {
  if(tmp.size() == size || iter == end) {
    auto w = 0;
    for(auto i : tmp) {
      w += boxes_rects[i].width();
    }
    if(w == width) {
      result.push_back(tmp);
    }
    return;
  }
  for(auto i = 0; i < static_cast<int>(iter->size()); ++i) {
    if(!tmp.empty()) {
      if(boxes_rects[tmp.back()].right() + 1 == boxes_rects[(*iter)[i]].left()) {
        tmp.push_back((*iter)[i]);
      }
    } else {
      tmp.push_back((*iter)[i]);
    }
    permute_horizontal(boxes_rects, std::next(iter), size, end, width, tmp, result);
    if(!tmp.empty()) {
      tmp.pop_back();
    }
  }
}

void permute_vertical(const std::vector<QRect>& boxes_rects, const std::vector<std::vector<int>>::iterator& iter, int size,
  const std::vector<std::vector<int>>::iterator& end, int height, std::vector<int>& tmp, std::vector<std::vector<int>>& result) {
  if(tmp.size() == size || iter == end) {
    auto h = 0;
    for(auto i : tmp) {
      h += boxes_rects[i].height();
    }
    if(h == height) {
      result.push_back(tmp);
    }
    return;
  }
  for(auto i = 0; i < static_cast<int>(iter->size()); ++i) {
    if(!tmp.empty()) {
      if(boxes_rects[tmp.back()].bottom() + 1 == boxes_rects[(*iter)[i]].top()) {
        tmp.push_back((*iter)[i]);
      }
    } else {
      tmp.push_back((*iter)[i]);
    }
    permute_vertical(boxes_rects, std::next(iter), size, end, height, tmp, result);
    if(!tmp.empty()) {
      tmp.pop_back();
    }
  }
}

std::vector<std::vector<int>> build_rows(
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
  set_param("parallel.enable", true);
      }
    }
    auto& last_row = *bottom_map_row.rbegin();
    for(auto iter = last_row.second.begin(); iter != last_row.second.end(); ++iter) {
      if(i == *iter) {
        map_rows.back()[rect.x()].push_back(i);
      }
    }
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
    permute_horizontal(boxes_rects, r.begin(), r.size(), r.end(), total_rect.width(), row, rows);
  }
  return rows;
}

std::vector<std::vector<int>> build_columns(
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
    permute_vertical(boxes_rects, c.begin(), c.size(), c.end(), total_rect.height(), column, columns);
  }
  return columns;
}

expr_vector build_horizontal_formulas(Solver& solver, const std::vector<std::string>& names, const std::vector<std::vector<int>>& rows) {
  auto formulas = expr_vector(solver.get_context());
  for(auto& row : rows) {
    if(row.empty()) {
      continue;
    }
    auto formula = solver.create_variable(names[row[0]] + ".width");
    for(auto i = 1; i < static_cast<int>(row.size()); ++i) {
      formula = formula + solver.create_variable(names[row[i]] + ".width");
    }
    formulas.push_back(formula == solver.create_variable(LAYOUT_NAME));
  }
  return formulas;
}

expr_vector build_vertical_formulas(Solver& solver, const std::vector<std::string>& names, const std::vector<std::vector<int>>& columns) {
  auto formulas = expr_vector(solver.get_context());
  for(auto& column : columns) {
    if(column.empty()) {
      continue;
    }
    auto formula = solver.create_variable(names[column[0]] + ".height");
    for(auto i = 1; i < static_cast<int>(column.size()); ++i) {
      formula = formula + solver.create_variable(names[column[i]] + ".height");
    }
    formulas.push_back(formula == solver.create_variable(LAYOUT_NAME));
  }
  return formulas;
}

expr_vector build_position_variable_value(Solver& solver,
    const std::unordered_map<std::string, int>& name_map,
    const std::vector<QRect>& boxes_rects,
    const std::vector<Constraint::Variable>& variables) {
  auto formulas = expr_vector(solver.get_context());
  for(auto& v : variables) {
    auto name = v.m_name;
    if(!name_map.contains(name)) {
      continue;
    }
    auto rect = boxes_rects[name_map.at(name)];
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
      formulas.push_back(solver.create_variable(v.m_content) == value);
    }
  }
  return formulas;
}

Layout::Layout()
  : m_status(Status::NONE),
    m_min_fixed_box_width(MAX_LAYOUT_SIZE),
    m_min_fixed_box_height(MAX_LAYOUT_SIZE),
    m_total_fixed_box_width(0),
    m_total_fixed_box_height(0),
    m_area(0) {
}

Layout::~Layout() {
  for(auto box : m_boxes) {
    delete box;
  }
  //m_thread.join();
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
  m_area += box->get_rect().width() * box->get_rect().height();
  m_rect = m_rect.united(box->get_rect());
  if(box->get_name().empty()) {
    box->set_name("DALI_LAYOUT_BOX" + m_boxes.size());
    box->set_name_visible(false);
  }
  m_boxes.push_back(box);
  m_name_map[box->get_name()] = static_cast<int>(m_boxes.size()) - 1;
  m_boxes_names.push_back(box->get_name());
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
  return m_min_size;
  //return m_min_size.get_future().get();
}

QSize Layout::get_max_size() {
  return m_max_size;
  //return m_max_size.get_future().get();
}

void Layout::resize(const QSize& size) {
  auto boxes_rects = std::vector<QRect>();
  for(auto box : m_boxes) {
    boxes_rects.push_back(box->get_rect());
  }
  auto rows = build_rows(boxes_rects);
  auto horizontal_formulas = build_horizontal_formulas(m_horizontal_solver, m_boxes_names, rows);
  qDebug() << "horizontal:";
  auto widths = m_horizontal_solver.solve(horizontal_formulas, size.width());
  adjust_horizontal_layout(m_name_map, widths, rows, boxes_rects);
  auto columns = build_columns(boxes_rects);
  auto vertical_formulas = build_vertical_formulas(m_vertical_solver, m_boxes_names, columns);
  qDebug() << "vertical:";
  auto heights = m_vertical_solver.solve(vertical_formulas, size.height());
  adjust_vertical_layout(m_name_map, heights, columns, boxes_rects);
  auto formulas = expr_vector(m_position_solver.get_context());
  for(auto i = 0; i < m_position_constraints.get_constraint_count(); ++i) {
    auto formula = build_position_variable_value(m_position_solver, m_name_map,
      boxes_rects, m_position_constraints.get_constraint(i).get_variables());
    concatenate_to(formula, formulas);
  }
  if(!m_position_solver.check(formulas)) {
    m_status = Status::NONE;
    return;
  }
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
}

bool Layout::build() {
  if(m_area != m_rect.width() * m_rect.height()) {
    return false;
  }
  m_total_fixed_box_width = std::max(m_total_fixed_box_width, m_rect.width());
  m_total_fixed_box_height = std::max(m_total_fixed_box_height, m_rect.height());
  m_position_solver.add_const_formula(m_position_constraints.convert(m_position_solver.get_context()));
  auto horizontal_expanding_box = std::vector<std::pair<int, int>>();
  auto vertical_expanding_box = std::vector<std::pair<int, int>>();
  //auto horizontal_fixed_boxes = std::unordered_set<std::string>();
  //auto vertical_fixed_boxes = std::unordered_set<std::string>();
  //auto horizontal_additional_formulas = m_horizontal_solver.create_expr_vector();
  //auto vertical_additional_formulas = m_vertical_solver.create_expr_vector();
  //auto boxes_rects = std::vector<QRect>();
  for(auto i = 0; i < get_box_count(); ++i) {
    auto box = m_boxes[i];
    //boxes_rects.push_back(box->get_rect());
    if(box->get_horizontal_size_policy() == SizePolicy::Fixed) {
      //horizontal_fixed_boxes.insert(box->get_name());
      m_horizontal_constraints.add_local_constraint(Constraint(
        fmt::format("{}.width = {}", box->get_name(), box->get_rect().width())),
          true);
    } else {
      //horizontal_additional_formulas.push_back(
      //  m_horizontal_solver.create_variable(box->get_name() + ".width") %
      //    m_min_fixed_box_width == 0);
      //horizontal_additional_formulas.push_back(
      //  m_horizontal_solver.create_variable(box->get_name() + ".width") <=
      //    m_horizontal_solver.create_variable(LAYOUT_NAME));
      m_horizontal_constraints.add_local_constraint(Constraint(
        fmt::format("{}.width >= 0", box->get_name())), true);
      if(!m_horizontal_constraints.has_varaible_name_in_global(box->get_name())) {
        horizontal_expanding_box.push_back({box->get_rect().width(), i});
      }
    }
    if(box->get_vertical_size_policy() == SizePolicy::Fixed) {
      //vertical_fixed_boxes.insert(box->get_name());
      m_vertical_constraints.add_local_constraint(Constraint(
        fmt::format("{}.height = {}", box->get_name(), box->get_rect().height())),
          true);
    } else {
      //vertical_additional_formulas.push_back(
      //  m_vertical_solver.create_variable(box->get_name() + ".height") %
      //    m_min_fixed_box_height == -1);
      //vertical_additional_formulas.push_back(
      //  m_vertical_solver.create_variable(box->get_name() + ".height") <=
      //    m_vertical_solver.create_variable(LAYOUT_NAME));
      m_vertical_constraints.add_local_constraint(Constraint(
       fmt::format("{}.height >= 0", box->get_name())), true);
      if(!m_vertical_constraints.has_varaible_name_in_global(box->get_name())) {
        vertical_expanding_box.push_back({box->get_rect().height(), i});
      }
    }
  }
  if(!horizontal_expanding_box.empty()) {
    std::sort(horizontal_expanding_box.begin(), horizontal_expanding_box.end());
    for(auto iter = horizontal_expanding_box.begin() + 1;
        iter != horizontal_expanding_box.end(); ++iter) {
      m_horizontal_constraints.add_local_constraint(Constraint(
        fmt::format("{}.width = {}.width * {}", m_boxes[iter->second]->get_name(),
        m_boxes[horizontal_expanding_box.begin()->second]->get_name(),
        static_cast<double>(iter->first) / horizontal_expanding_box.begin()->first)), false);
    }
  }
  if(!vertical_expanding_box.empty()) {
    std::sort(vertical_expanding_box.begin(), vertical_expanding_box.end());
    for(auto iter = vertical_expanding_box.begin() + 1;
        iter != vertical_expanding_box.end(); ++iter) {
      m_vertical_constraints.add_local_constraint(Constraint(
        fmt::format("{}.height = {}.height * {}", m_boxes[iter->second]->get_name(),
        m_boxes[vertical_expanding_box.begin()->second]->get_name(),
        static_cast<double>(iter->first) / vertical_expanding_box.begin()->first)), false);
    }
  }
  m_horizontal_solver.add_const_formula(
    m_horizontal_constraints.convert(m_horizontal_solver.get_context()));
  m_vertical_solver.add_const_formula(
    m_vertical_constraints.convert(m_vertical_solver.get_context()));
  //m_thread = std::thread(&Layout::calculate_min_max_size, this,
  //  horizontal_solver, vertical_solver, boxes_rects,
  //  horizontal_fixed_boxes, vertical_fixed_boxes,
  //  horizontal_additional_formulas, vertical_additional_formulas);
  //m_thread = std::thread([&] {
  //  auto horizontal_solver = Solver();
  //  auto vertical_solver = Solver();
  //  horizontal_solver.add_const_formula(
  //    m_horizontal_constraints.convert(horizontal_solver.get_context()));
  //  vertical_solver.add_const_formula(
  //    m_vertical_constraints.convert(vertical_solver.get_context()));
  //  calculate_min_max_size(
  //    horizontal_solver, vertical_solver, boxes_rects,
  //    horizontal_fixed_boxes, vertical_fixed_boxes,
  //    horizontal_additional_formulas, vertical_additional_formulas);
  //  });
  //m_thread = std::thread(&Layout::calculate_min_max_size, this);
  //m_thread = std::thread([=] {
    //calculate_min_max_size(m_boxes, m_name_map, m_horizontal_constraints,
    //  m_vertical_constraints, m_rect, m_min_fixed_box_width, m_min_fixed_box_height,
    //  m_total_fixed_box_width, m_total_fixed_box_height);
    //});
    //calculate_min_max_size(m_boxes, m_name_map, m_horizontal_solver,
    //  m_vertical_solver, m_rect, m_min_fixed_box_width, m_min_fixed_box_height,
    //  m_total_fixed_box_width, m_total_fixed_box_height);
    //});
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



void Layout::calculate_min_max_size() {
  //auto horizontal_solver = Solver(source_horizontal_solver);
  //auto vertical_solver = Solver(source_vertical_solver);
  //auto horizontal_solver = Solver();
  //auto vertical_solver = Solver();
  //horizontal_solver.add_const_formula(
  //  horizontal_constraints.convert(horizontal_solver.get_context()));
  //vertical_solver.add_const_formula(
  //  vertical_constraints.convert(vertical_solver.get_context()));
  auto horizontal_fixed_boxes = std::unordered_set<std::string>();
  auto vertical_fixed_boxes = std::unordered_set<std::string>();
  auto horizontal_additional_formulas = expr_vector(m_horizontal_solver.get_context());
  auto vertical_additional_formulas = expr_vector(m_vertical_solver.get_context());
  auto boxes_rects = std::vector<QRect>(m_boxes.size());
  //auto names = std::vector<std::string>(m_boxes.size());
  for(auto i = 0; i < static_cast<int>(m_boxes.size()); ++i) {
    auto box = m_boxes[i];
    //names[i] = box->get_name();
    boxes_rects[i] = box->get_rect();
    if(box->get_horizontal_size_policy() == SizePolicy::Fixed) {
      horizontal_fixed_boxes.insert(box->get_name());
    } else {
      horizontal_additional_formulas.push_back(
        m_horizontal_solver.create_variable(box->get_name() + ".width") %
        m_min_fixed_box_width == 0);
      horizontal_additional_formulas.push_back(
        m_horizontal_solver.create_variable(box->get_name() + ".width") <=
        m_horizontal_solver.create_variable(LAYOUT_NAME));
    }
    if(box->get_vertical_size_policy() == SizePolicy::Fixed) {
      vertical_fixed_boxes.insert(box->get_name());
    } else {
      vertical_additional_formulas.push_back(
        m_vertical_solver.create_variable(box->get_name() + ".height") %
        m_min_fixed_box_height == 0);
      vertical_additional_formulas.push_back(
        m_vertical_solver.create_variable(box->get_name() + ".height") <=
        m_vertical_solver.create_variable(LAYOUT_NAME));
    }
  }
  //horizontal_solver.add_const_formula(horizontal_additional_formulas);
  //vertical_solver.add_const_formula(vertical_additional_formulas);
  auto rows = build_rows(boxes_rects);
  auto horizontal_formulas = build_horizontal_formulas(m_horizontal_solver, m_boxes_names, rows);
  concatenate_to(horizontal_additional_formulas, horizontal_formulas);
  auto columns = build_columns(boxes_rects);
  auto vertical_formulas = build_vertical_formulas(m_vertical_solver, m_boxes_names, columns);
  concatenate_to(vertical_additional_formulas, vertical_formulas);
  auto min_width = m_total_fixed_box_width;
  auto min_height = m_total_fixed_box_height;
  auto max_width = -1;
  auto max_height = -1;
  for(auto height = m_min_fixed_box_height; height <= m_total_fixed_box_height; height += m_min_fixed_box_height) {
    auto tmp_boxes_rects = boxes_rects;
    auto solutions = m_vertical_solver.solve(vertical_formulas, vertical_fixed_boxes, height);
    if(solutions.empty()) {
      continue;
    }
    for(auto& solution : solutions) {
      adjust_vertical_layout(m_name_map, solution, columns, tmp_boxes_rects);
      auto new_horizontal_formulas = build_horizontal_formulas(m_horizontal_solver, m_boxes_names, build_rows(tmp_boxes_rects));
      auto new_max_width = static_cast<int>(m_horizontal_solver.solve_maximum(new_horizontal_formulas, m_total_fixed_box_width));
      concatenate_to(horizontal_additional_formulas, new_horizontal_formulas);
      auto new_min_width = static_cast<int>(m_horizontal_solver.solve_minimum(
        new_horizontal_formulas, horizontal_fixed_boxes,
        m_min_fixed_box_width, min_width));
      min_height = std::min(min_height, height);
      min_width = std::min(min_width, new_min_width);
      max_width = std::max(max_width, new_max_width);
      if(min_width <= m_min_fixed_box_width && max_width >= m_total_fixed_box_width) {
        break;
      }
    }
    if(min_width <= m_min_fixed_box_width && max_width >= m_total_fixed_box_width) {
      break;
    }
  }
  for(auto width = m_min_fixed_box_width; width <= m_total_fixed_box_width; width += m_min_fixed_box_width) {
    auto tmp_boxes_rects = boxes_rects;
    auto solutions = m_horizontal_solver.solve(horizontal_formulas, horizontal_fixed_boxes, width);
    if(solutions.empty()) {
      continue;
    }
    for(auto& solution : solutions) {
      adjust_horizontal_layout(m_name_map, solution, rows, tmp_boxes_rects);
      auto new_vertical_formulas = build_vertical_formulas(m_vertical_solver, m_boxes_names, build_columns(tmp_boxes_rects));
      auto new_max_height = static_cast<int>(m_vertical_solver.solve_maximum(new_vertical_formulas, m_total_fixed_box_height));
      concatenate_to(vertical_additional_formulas, new_vertical_formulas);
      auto new_min_height = static_cast<int>(m_vertical_solver.solve_minimum(
        new_vertical_formulas, vertical_fixed_boxes, m_min_fixed_box_height, min_height));
      min_width = std::min(min_width, width);
      min_height = std::min(min_height, new_min_height);
      max_height = std::max(max_height, new_max_height);
      if(min_height <= m_min_fixed_box_height && max_height >= m_total_fixed_box_height) {
        break;
      }
    }
    if(min_height <= m_min_fixed_box_height && max_height >= m_total_fixed_box_height) {
      break;
    }
  }
  if(min_width == m_total_fixed_box_width) {
    m_min_size.setWidth(m_rect.width());
  } else {
    m_min_size.setWidth(min_width);
  }
  if(min_height == m_total_fixed_box_height) {
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
