#ifndef DALI_CONSTRAINT_GRAPH_H
#define DALI_CONSTRAINT_GRAPH_H
#include <vector>
#include <QRect>
#include <QString>
#include "Dali/Dali.hpp"

namespace Dali {

  class ConstraintGraph {
    public:
      ConstraintGraph();

      void add_edge(const QString& u, const QString& v);

      bool topological_sort();

      std::vector<QString> get_sorted_list();

    private:
      std::map<QString, std::vector<QString>> m_adjacency_list;
      std::vector<QString> m_sorted_list;

      bool visit(const QString& node, std::unordered_map<QString,bool>& permanent_visited,
        std::unordered_map<QString,bool>& temporary_visited);
  };
}
#endif
