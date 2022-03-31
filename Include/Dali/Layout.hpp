#ifndef DALI_LAYOUT_H
#define DALI_LAYOUT_H
#include <set>
#include <unordered_set>
#include <vector>
#include <QRect>
#include "Dali/ConstraintExpression.hpp"
#include "Dali/Dali.hpp"

namespace Dali {

  class Layout {
    public:

      Layout();
      ~Layout();

      QRect get_rect() const;
      void set_rect(const QRect& rect);

      void add_box(LayoutBox* box);

      int get_box_count() const;

      LayoutBox* get_box(int index) const;

      QSize get_min_size() const;
      QSize get_max_size() const;

      void resize(const QSize& size);

      bool build();

    private:
      std::vector<LayoutBox*> m_boxes;
      std::unordered_map<QString, int> m_name_map;
      std::vector<int> m_width_sorted_constraint;
      std::vector<int> m_height_sorted_constraint;
      QRect m_rect;
      QSize m_min_size;
      QSize m_max_size;

      bool build_constraints();
      void build_constraint_graph(ConstraintGraph& graph, int box_index,
        ConstraintExpression& constraint_expression);
      void calculate_min_max_size();
      void resize_width(int width);
      void resize_height(int height);
  };
}

#endif
