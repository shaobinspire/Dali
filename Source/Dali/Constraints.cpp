#include "Dali/Constraints.hpp"
#include "Dali/Constraint.hpp"

using namespace Dali;
using namespace z3;

void Constraints::add_local_constraint(const Constraint& constraint, bool forced) {
  if(forced) {
    m_constraints.push_back(constraint);
    return;
  }
  auto& variable_names = constraint.get_variable_names();
  auto is_contained = [&] {
    for(auto& name : variable_names) {
      if(m_global_variable_name.contains(name)) {
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

bool Constraints::has_varaible_name_in_global(const std::string& name) {
  return m_global_variable_name.contains(name);
}

void Constraints::clear() {
  m_constraints.clear();
}
