#ifndef DALI_LAYOUT_H
#define DALI_LAYOUT_H
#include <set>
#include <unordered_map>
#include <vector>
#include <QRect>
#include "Dali/Constraint.hpp"
#include "Dali/Constraints.hpp"
#include "Dali/Dali.hpp"
#include "Dali/Solver.hpp"

namespace Dali {

  class Layout {
    public:
      enum class Status {
        NONE,
        LAYOUT_OVERFLOW,
        LAYOUT_UNDERFLOW
      };

      Layout();
      ~Layout();

      QRect get_rect() const;
      void set_rect(const QRect& rect);

      void add_box(LayoutBox* box);
      void add_width_constraint(const Constraint& constraint);
      void add_height_constraint(const Constraint& constraint);
      void add_position_constraint(const Constraint& constraint);

      int get_box_count() const;

      LayoutBox* get_box(int index) const;

      QSize get_min_size();
      QSize get_max_size();

      void resize(const QSize& size);

      bool build();

      int get_index_by_name(const std::string& name);

      const std::vector<QRect>& get_box_rect() const;

      Status get_status() const;

    private:
      //struct Comparator {
      //  bool operator() (const std::pair<int, int>& a, const std::pair<int, int>& b) const {
      //    return b.first >= a.first && b.first < a.second || b.second > a.first && b.second <= a.second;
      //  }
      //};
      std::vector<LayoutBox*> m_boxes;
      std::vector<QRect> m_boxes_rects;
      std::unordered_map<std::string, int> m_name_map;
      std::unordered_set<std::string> m_horizontal_fixed_boxes;
      std::unordered_set<std::string> m_vertical_fixed_boxes;
      //std::vector<std::vector<int>> m_rows;
      //std::vector<std::vector<int>> m_columns;
      //std::map<int, std::vector<int>, std::greater<int>> m_reverse_horizontal_map;
      //std::map<int, std::vector<int>, std::greater<int>> m_reverse_vertical_map;
      //std::vector<int> m_top_row;
      //std::vector<int> m_left_column;
      //std::vector<int> m_bottom_row;
      //std::vector<int> m_right_column;
      QRect m_rect;
      QSize m_min_size;
      QSize m_max_size;
      QPoint m_min_pos;
      QPoint m_max_pos;
      Constraints m_horizontal_constraints;
      Constraints m_vertical_constraints;
      Constraints m_position_constraints;
      //Constraints m_width_sum_constraints;
      //Constraints m_height_sum_constraints;
      Solver m_horizontal_solver;
      Solver m_vertical_solver;
      Solver m_position_solver;
      Status m_status;
      QSize m_last_size;
      int m_min_fixed_box_width;
      int m_min_fixed_box_height;
      int m_total_fixed_box_width;
      int m_total_fixed_box_height;

      //bool is_horizontal_one_row() const;
      //bool is_vertical_one_column() const;
      //void calculate_one_row_min_max_size();
      //void calculate_one_column_min_max_size();
      //void rebuild_constraints();
      //void rebuild_constraints(bool is_horizontal, Solver& solver, std::vector<std::vector<int>>& row_or_column);
      //void rebuild_horizontal_constraints();
      //void rebuild_vertical_constraints();
      //void resize_width(int width);
      //void resize_height(int height);
      void build_constraints(const std::vector<int>& range, const std::string& property, Constraints& constraints);
      void adjust_horizontal_layout(const std::unordered_map<std::string, double>& boxes_widths,
        const std::vector<std::vector<int>>& rows, std::vector<QRect>& boxes_rects);
      void adjust_vertical_layout(const std::unordered_map<std::string, double>& boxes_heights,
        const std::vector<std::vector<int>>& columns, std::vector<QRect>& boxes_rects);
      std::vector<std::vector<int>> build_rows(const std::vector<QRect>& boxes_rects);
      std::vector<std::vector<int>> build_columns(const std::vector<QRect>& boxes_rects);
      z3::expr_vector build_horizontal_formulas(const std::vector<std::vector<int>>& rows);
      z3::expr_vector build_vertical_formulas(const std::vector<std::vector<int>>& columns);
      void calculate_min_max_size();
      void permute_horizontal(const std::vector<std::vector<int>>::iterator& iter, int size,
        const std::vector<std::vector<int>>::iterator& end, int width, std::vector<int>& tmp, std::vector<std::vector<int>>& result);
      void permute_vertical(const std::vector<std::vector<int>>::iterator& iter, int size,
        const std::vector<std::vector<int>>::iterator& end, int height, std::vector<int>& tmp, std::vector<std::vector<int>>& result);
      z3::expr_vector set_position_variable_value(const std::vector<QRect>& boxes_rects, const std::vector<Constraint::Variable>& variables);
  };
}

#endif
