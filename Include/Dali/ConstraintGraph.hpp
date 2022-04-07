#ifndef DALI_CONSTRAINT_GRAPH_H
#define DALI_CONSTRAINT_GRAPH_H
#include <vector>
#include <QRect>
#include <QString>
#include "Dali/Dali.hpp"

namespace Dali {

  class ConstraintGraph {
    public:
      ConstraintGraph(int size);

      void add_edge(int u, int v);

      bool topological_sort();

      std::vector<int> get_sorted_list();

    private:
      std::vector<std::vector<int>> m_adjacency_list;
      std::vector<int> m_sorted_list;

      bool visit(int node, std::vector<bool>& permanent_visited,
        std::vector<bool>& temporary_visited);
  };
}
#endif
