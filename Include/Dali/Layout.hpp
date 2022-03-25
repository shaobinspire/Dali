#ifndef DALI_LAYOUT_H
#define DALI_LAYOUT_H
#include <set>
#include <unordered_set>
#include <vector>
#include <QRect>
#include "Dali/LayoutItem.hpp"
#include "Dali/Dali.hpp"

namespace Dali {

  class Layout {
    public:

      Layout();

      QRect get_rect() const;
      void set_rect(const QRect& rect);

      void add_item(LayoutItem* item);

      int get_item_size() const;

      LayoutItem* get_item(int index) const;

      void calculate_min_max_size();

      QSize get_min_size() const;
      QSize get_max_size() const;

    private:
      enum class Direction {
        NONE,
        LEFT,
        RIGHT,
        DOWN,
        UP
      };
      struct GraphItem {
        int m_index;
        Direction m_direction;
      };
      std::vector<LayoutItem*> m_items;
      QSize m_min_size;
      QSize m_max_size;
      QRect m_rect;
      int m_first_fixed_item;
      int m_fixed_item_count;
      std::vector<std::vector<GraphItem>> m_graph;
      std::map<int, bool> m_visited;
      std::vector<std::vector<GraphItem>> m_paths;

      std::map<int, std::vector<int>> m_horizontal_map;
      std::map<int, std::vector<int>> m_vertical_map;
      std::map<int, std::vector<int>, std::greater<int>> m_reverse_horizontal_map;
      std::map<int, std::vector<int>, std::greater<int>> m_reverse_vertical_map;

      bool preprocess_base_cases();
      void build_graph();
      void get_paths();
      void dfs(int u, int d, Direction direction, std::vector<GraphItem>& path, int& fixed_item_count);
      void rebuild_layout();

      Direction flip_direction(Direction direction);
      void add_edge(int u, int v, Direction direction);
      bool check_valid_align_left(const std::map<int, QRect>& rects);
      bool check_valid_align_right(const std::map<int, QRect>& rects);
      bool check_valid_align_bottom(const std::map<int, QRect>& rects);
      bool is_opposite_direction(Direction direction1, Direction direction2);
  };
}

#endif
