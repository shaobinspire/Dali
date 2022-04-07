#include "Dali/Constraints.hpp"

using namespace Dali;

void Constraints::add_local_constraint(const Constraint& constraint) {
  auto& variable_names = constraint.get_variable_names();
  auto is_contained = [&] {
    for(auto& name : variable_names) {
      if(m_global_variable_name.contains(name) &&
          constraint.get_comparsion_operator() == Constraint::ComparisonOperator::EQUAL_TO) {
        return true;
      }
    }
    return false;
  }();
  if(!is_contained) {
    m_constraints.push_back(constraint);
  }
}

void Constraints::add_global_constraint(const Constraint& constraint) {
  m_constraints.push_back(constraint);
  auto& variable_names = constraint.get_variable_names();
  for(auto& name : variable_names) {
    m_global_variable_name.insert(name);
  }
}

int Constraints::get_constraint_count() const {
  return static_cast<int>(m_constraints.size());
}

const Constraint& Constraints::get_constraint(int index) const {
  if(index < 0 || index >= get_constraint_count()) {
    throw std::out_of_range("The index is out of range.");
  }
  return m_constraints[index];
}

bool Constraints::has_varaible_name_in_global(const QString& name) {
  return m_global_variable_name.contains(name);
}
