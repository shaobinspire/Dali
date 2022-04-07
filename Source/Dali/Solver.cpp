#include "Dali/Solver.hpp"

using namespace Dali;
using namespace z3;

Solver::Solver()
  : m_solver(m_context) {}

void Solver::add_constraints(const Constraints& constraints) {
  for(auto i = 0; i < constraints.get_constraint_count(); ++i) {
    if(auto formula = constraints.get_constraint(i).convert_to_formula(m_context)) {
      m_solver.add(formula);
    }
  }
}

std::vector<std::pair<QString, double>> Solver::solve(int value) {
  m_solver.push();
  m_solver.add(m_context.int_const(layout_name) == m_context.real_val(std::to_string(value).c_str()));
  auto status = m_solver.check();
  if(status != check_result::sat) {
    m_solver.pop();
    qDebug() << "failed";
    return std::vector<std::pair<QString, double>>();
  }
  auto result = std::vector<std::pair<QString, double>>();
  auto model = m_solver.get_model();
  for(unsigned int i = 0; i < model.num_consts(); ++i) {
    auto decl = model.get_const_decl(i);
    result.push_back({QString::fromStdString(decl.name().str()), model.get_const_interp(decl).as_double()});
  }
  m_solver.pop();
  qDebug() << "successful";
  return result;
}

int Solver::get_min_value() {
  auto status = m_solver.check();
  if(status != check_result::sat) {
    return 0;
  }
  auto model = m_solver.get_model();
  for(unsigned int i = 0; i < model.num_consts(); ++i) {
    auto decl = model.get_const_decl(i);
    if(decl.name().str() == "layout") {
      return model.get_const_interp(decl).as_int64();
    }
  }
  return 0;
}
