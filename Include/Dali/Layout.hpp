#ifndef DALI_LAYOUT_H
#define DALI_LAYOUT_H
//#include <future>
#include <set>
//#include <thread>
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

      void build_constraints();

      bool build();

      int get_index_by_name(const std::string& name);

      const std::vector<QRect>& get_box_rect() const;

      Status get_status() const;

    private:
      std::vector<LayoutBox*> m_boxes;
      std::vector<QRect> m_boxes_rects;
      std::unordered_map<std::string, int> m_name_map;
      std::vector<std::string> m_boxes_names;
      QRect m_rect;
      Constraints m_horizontal_constraints;
      Constraints m_vertical_constraints;
      Constraints m_position_constraints;
      Solver m_horizontal_solver;
      Solver m_vertical_solver;
      Solver m_position_solver;
      Status m_status;
      //std::promise<QSize> m_min_size;
      //std::promise<QSize> m_max_size;
      QSize m_min_size;
      QSize m_max_size;
      int m_min_fixed_box_width;
      int m_min_fixed_box_height;
      int m_total_fixed_box_width;
      int m_total_fixed_box_height;
      int m_area;
      //std::thread m_thread;

      void calculate_min_max_size();
  };
}

#endif
