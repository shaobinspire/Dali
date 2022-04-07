#include "Dali/ConstraintGraph.hpp"

using namespace Dali;

ConstraintGraph::ConstraintGraph() {
  //m_adjacency_list.resize(size);
}

void ConstraintGraph::add_edge(const QString& u, const QString& v) {
  m_adjacency_list[u].push_back(v);
}

bool ConstraintGraph::topological_sort() {
  auto permanent_visited = std::unordered_map<QString,bool>();
  //permanent_visited.resize(m_adjacency_list.size());
  auto temporary_visited = std::unordered_map<QString, bool>();
  //temporary_visited.resize(m_adjacency_list.size());
  //for(auto i = 0; i < static_cast<int>(m_adjacency_list.size()); ++i) {
  for(auto& item : m_adjacency_list) {
    if(!permanent_visited[item.first]) {
      if(!visit(item.first, permanent_visited, temporary_visited)) {
        return false;
      }
    }
  }
  return true;
}

std::vector<QString> ConstraintGraph::get_sorted_list() {
  return m_sorted_list;
}

bool ConstraintGraph::visit(const QString& node, std::unordered_map<QString,bool>& permanent_visited,
    std::unordered_map<QString,bool>& temporary_visited) {
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
