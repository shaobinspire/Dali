#include "Dali/ConstraintGraph.hpp"
using namespace Dali;

ConstraintGraph::ConstraintGraph(int size) {
  m_adjacency_list.resize(size);
}

void ConstraintGraph::add_edge(int u, int v) {
  m_adjacency_list[u].push_back(v);
}

bool ConstraintGraph::topological_sort() {
  auto permanent_visited = std::vector<bool>();
  permanent_visited.resize(m_adjacency_list.size());
  auto temporary_visited = std::vector<bool>();
  temporary_visited.resize(m_adjacency_list.size());
  for(auto i = 0; i < static_cast<int>(m_adjacency_list.size()); ++i) {
    if(!permanent_visited[i]) {
      if(!visit(i, permanent_visited, temporary_visited)) {
        return false;
      }
    }
  }
  return true;
}

std::vector<int> ConstraintGraph::get_sorted_list() {
  return m_sorted_list;
}

bool ConstraintGraph::visit(int node, std::vector<bool>& permanent_visited,
    std::vector<bool>& temporary_visited) {
  if(permanent_visited[node]) {
    return true;
  }
  if(temporary_visited[node]) {
    return false;
  }
  temporary_visited[node] = true;
  for(auto i = 0; i < static_cast<int>(m_adjacency_list[node].size()); ++i) {
    if(!visit(m_adjacency_list[node][i], permanent_visited, temporary_visited)) {
      return false;
    }
  }
  temporary_visited[node] = false;
  permanent_visited[node] = true;
  m_sorted_list.push_back(node);
  return true;
}
