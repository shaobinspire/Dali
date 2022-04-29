#include "Dali/Constraints.hpp"
#include "Dali/Constraint.hpp"

using namespace Dali;
using namespace z3;

void Constraints::add_local_constraint(const Constraint& constraint,
    bool is_forced, bool is_fixed) {
  if(is_forced) {
    m_constraints.push_back(constraint);
    return;
  }
  auto& variable_names = constraint.get_variable_names();
  auto is_contained = [&] {
    for(auto& name : variable_names) {
      if(!is_fixed && m_global_variable_name.contains(name) ||
          is_fixed && m_assignment_variables.contains(name)) {
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
  if(variable_names.size() == 1 && constraint.get_comparsion_operator() ==
      Constraint::ComparisonOperator::EQUAL_TO) {
    m_assignment_variables.insert(*variable_names.begin());
  }
  for(auto& name : variable_names) {
    m_global_variable_name.insert(name);
  }
}

expr_vector Constraints::convert(context& context) const {
  auto formulas = expr_vector(context);
  for(auto& constraint : m_constraints) {
    if(auto formula = constraint.convert_to_formula(context)) {
      formulas.push_back(formula);
    }
  }
  return formulas;
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

bool Constraints::has_varaible_name_in_global(const std::string& name) const {
  return m_global_variable_name.contains(name);
}

void Constraints::clear() {
  m_constraints.clear();
  m_global_variable_name.clear();
  m_assignment_variables.clear();
}
