#ifndef DALI_LAYOUT_H
#define DALI_LAYOUT_H
#include <set>
#include <map>
#include <vector>
#include <QString>
#include <QRect>
#include "Dali/Constraint.hpp"
#include "Dali/Constraints.hpp"
#include "Dali/Dali.hpp"
#include "Dali/Solver.hpp"

namespace Dali {

  class Layout {
    public:

      Layout();
      ~Layout();

      QRect get_rect() const;
      void set_rect(const QRect& rect);

      void add_box(LayoutBox* box);
      void add_width_constraint(const Constraint& constraint);
      void add_height_constraint(const Constraint& constraint);

      int get_box_count() const;

      LayoutBox* get_box(int index) const;

      QSize get_min_size() const;
      QSize get_max_size() const;

      void resize(const QSize& size);

      bool build();

      int get_index_by_name(const QString& name);

    private:
      std::vector<LayoutBox*> m_boxes;
      std::map<QString, int> m_name_map;
      std::vector<int> m_width_sorted_constraint;
      std::vector<int> m_height_sorted_constraint;
      std::vector<int> m_top_row;
      std::vector<int> m_left_column;
      std::vector<int> m_bottom_row;
      std::vector<int> m_right_column;
      QRect m_rect;
      QSize m_min_size;
      QSize m_max_size;
      QPoint m_min_pos;
      QPoint m_max_pos;
      Constraints m_width_constraints;
      Constraints m_height_constraints;
      Solver m_width_solver;
      Solver m_height_solver;

      bool is_horizontal_one_row() const;
      bool is_vertical_one_column() const;
      void calculate_min_max_size();
      void calculate_one_row_min_max_size();
      void calculate_one_column_min_max_size();
      //void calculate_fixed_box_size(int index);
      void resize_width(int width);
      void resize_height(int height);
  };
}

#endif
