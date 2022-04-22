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

void Solver::add_const_formula(const expr_vector& formulas) {
  for(unsigned i = 0; i < formulas.size(); ++i) {
    qDebug() << formulas[i].to_string().c_str();
  }
  m_solver.add(formulas);
  //for(auto i = -1; i < constraints.get_constraint_count(); ++i) {
  //  if(auto formula = constraints.get_constraint(i).convert_to_formula(m_context)) {
  //    m_solver.add(formula);
  //  }
  //}
}

//  m_solver.reset();
//}

//std::unordered_map<std::string, double> Solver::solve(const Constraints& sum_constraints, int value) {
//  m_solver.push();
//  for(auto i = 0; i < sum_constraints.get_constraint_count(); ++i) {
//    if(auto formula = sum_constraints.get_constraint(i).convert_to_formula(m_context)) {
//      m_solver.add(formula);
//    }
//  }
//  m_solver.add(m_context.real_const(layout_name) == m_context.int_val(value));
//  auto status = m_solver.check();
//  auto on_exit = Details::ScopeExit([&] { m_solver.pop(); });
//  if(status != check_result::sat) {
//    return std::unordered_map<std::string, double>();
//    //return std::vector<std::pair<std::string, double>>();
//  }
//  //auto result = std::vector<std::pair<std::string, double>>();
//  auto result = std::unordered_map<std::string, double>();
//  auto model = m_solver.get_model();
//  for(unsigned int i = 0; i < model.num_consts(); ++i) {
//    auto decl = model.get_const_decl(i);
//    auto value = model.get_const_interp(decl).as_double();
//    //if(value < 0) {
//    //  result.clear();
//    //  return result;
//    //}
//    result[decl.name().str()] = value;
//    //result.push_back({decl.name().str(),
//    //  model.get_const_interp(decl).as_double()});
//  }
//  return result;
//}
//
//std::unordered_map<std::string, double> Solver::solve(const std::vector<expr>& sum_constraints, int value) {
//  m_solver.push();
//  for(auto& e : sum_constraints) {
//    m_solver.add(e);
//  }
//  m_solver.add(m_context.int_const(layout_name) == m_context.int_val(value));
//  auto status = m_solver.check();
//  auto on_exit = Details::ScopeExit([&] { m_solver.pop(); });
//  if(status != check_result::sat) {
//    return std::unordered_map<std::string, double>();
//  }
//  auto result = std::unordered_map<std::string, double>();
//  auto model = m_solver.get_model();
//  for(unsigned int i = 0; i < model.num_consts(); ++i) {
//    auto decl = model.get_const_decl(i);
//    auto value = model.get_const_interp(decl).as_double();
//    if(value < 0) {
//      result.clear();
//      return result;
//    }
//    result[decl.name().str()] = value;
//  }
//  return result;
//}

std::unordered_map<std::string, double> Solver::solve(const z3::expr_vector& formulas, int value) {
  m_solver.push();
  for(unsigned i = 0; i < formulas.size(); ++i) {
    qDebug() << formulas[i].to_string().c_str();
  }
  m_solver.add(formulas);
  m_solver.add(m_context.real_const(LAYOUT_NAME) == m_context.int_val(value));
  auto status = m_solver.check();
  auto on_exit = Details::ScopeExit([&] { m_solver.pop(); });
  if(status != check_result::sat) {
    return std::unordered_map<std::string, double>();
  }
  auto result = std::unordered_map<std::string, double>();
  auto model = m_solver.get_model();
  for(unsigned int i = 0; i < model.num_consts(); ++i) {
    auto decl = model.get_const_decl(i);
    result[decl.name().str()] = model.get_const_interp(decl).as_double();
  }
  return result;
}

double Solver::solve_maximum(const z3::expr_vector& formulas) {
  auto result = solve(formulas, MAX_LAYOUT_SIZE);
  if(result.empty()) {
    return -1;
  }
  return result[LAYOUT_NAME];
}

double Solver::solve_minimum(const z3::expr_vector& formulas) {
  m_solver.push();
  for(unsigned i = 0; i < formulas.size(); ++i) {
    qDebug() << formulas[i].to_string().c_str();
  }
  m_solver.add(formulas);
  auto status = m_solver.check();
  auto on_exit = Details::ScopeExit([&] { m_solver.pop(); });
  if(status != check_result::sat) {
    return 0;
  }
  auto model = m_solver.get_model();
  for(unsigned int i = 0; i < model.num_consts(); ++i) {
    auto decl = model.get_const_decl(i);
    if(decl.name().str() == LAYOUT_NAME) {
      return model.get_const_interp(decl).as_double();
    }
  }
  return 0;
}

expr Solver::declare_variable(const std::string& name) {
  return m_context.real_const(name.c_str());
}

context& Solver::get_context() {
  return m_context;
}
