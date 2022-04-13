#include "Dali/Solver.hpp"
#include "Dali/Constraint.hpp"
#include "Dali/Constraints.hpp"

using namespace Dali;
using namespace Dali::Details;
using namespace z3;

ScopeExit::ScopeExit(std::function<void()> f)
  : m_f(std::move(f)) {}

ScopeExit::~ScopeExit() {
  m_f();
}

Solver::Solver()
  : m_solver(m_context) {}

void Solver::add_constraints(const Constraints& constraints) {
  for(auto i = 0; i < constraints.get_constraint_count(); ++i) {
    if(auto formula = constraints.get_constraint(i).convert_to_formula(m_context)) {
      m_solver.add(formula);
    }
  }
}

std::vector<std::pair<std::string, double>> Solver::solve(int value) {
  m_solver.push();
  m_solver.add(m_context.real_const(layout_name) == m_context.int_val(value));
  auto status = m_solver.check();
  auto on_exit = Details::ScopeExit([&] { m_solver.pop(); });
  if(status != check_result::sat) {
    return std::vector<std::pair<std::string, double>>();
  }
  auto result = std::vector<std::pair<std::string, double>>();
  auto model = m_solver.get_model();
  for(unsigned int i = 0; i < model.num_consts(); ++i) {
    auto decl = model.get_const_decl(i);
    result.push_back({decl.name().str(),
      model.get_const_interp(decl).as_double()});
  }
  return result;
}

int Solver::get_min_value() {
  m_solver.push();
  auto status = m_solver.check();
  auto on_exit = Details::ScopeExit([&] { m_solver.pop(); });
  if(status != check_result::sat) {
    return 0;
  }
  auto model = m_solver.get_model();
  for(unsigned int i = 0; i < model.num_consts(); ++i) {
    auto decl = model.get_const_decl(i);
    if(decl.name().str() == layout_name) {
      return model.get_const_interp(decl).as_double();
    }
  }
  return 0;
}

int Solver::get_max_value() {
  m_solver.push();
  m_solver.add(m_context.real_const(layout_name) ==
    m_context.int_val(MAX_LAYOUT_SIZE));
  auto status = m_solver.check();
  auto on_exit = Details::ScopeExit([&] { m_solver.pop(); });
  if(status != check_result::sat) {
    return -1;
  }
  return MAX_LAYOUT_SIZE;
}
