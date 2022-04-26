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
    : m_solver(m_context) {
  m_context.set_enable_exceptions(false);
}

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

std::unordered_map<std::string, double> Solver::solve(const expr_vector& formulas, int value) {
  m_solver.push();
  for(unsigned i = 0; i < formulas.size(); ++i) {
    qDebug() << formulas[i].to_string().c_str();
  }
  m_solver.add(formulas);
  m_solver.add(m_context.int_const(LAYOUT_NAME) == m_context.int_val(value));
  auto status = m_solver.check();
  auto on_exit = Details::ScopeExit([&] { m_solver.pop(); });
  auto result = std::unordered_map<std::string, double>();
  if(status != check_result::sat) {
    return result;
  }
  auto model = m_solver.get_model();
  for(unsigned int i = 0; i < model.num_consts(); ++i) {
    //auto decl = model.get_const_decl(i);
    auto v = model[i];
    result[v.name().str()] = model.get_const_interp(v).as_double();
  }
  return result;
}

std::vector<std::unordered_map<std::string, double>> Solver::solve(const expr_vector& formulas,
  const std::unordered_set<std::string>& unchanged_variables, int value) {
  m_solver.push();
  for(unsigned i = 0; i < formulas.size(); ++i) {
    qDebug() << formulas[i].to_string().c_str();
  }
  m_solver.add(formulas);
  m_solver.add(m_context.int_const(LAYOUT_NAME) == m_context.int_val(value));
  auto solutions = std::vector<std::unordered_map<std::string, double>>();
  auto on_exit = Details::ScopeExit([&] { m_solver.pop(); });
  while(true) {
    if(m_solver.check() != check_result::sat) {
      return solutions;
    }
    auto model = m_solver.get_model();
    auto solution = std::unordered_map<std::string, double>();
    auto changed_expr = expr_vector(m_context);
    for(unsigned int i = 0; i < model.num_consts(); ++i) {
      auto decl = model.get_const_decl(i);
      solution[decl.name().str()] = model.get_const_interp(decl).as_double();
      if(!unchanged_variables.contains(decl.name().str())) {
        changed_expr.push_back(decl() != model.eval(decl()));
      }
    }
    m_solver.add(mk_or(changed_expr));
    solutions.push_back(solution);
  }
  return solutions;
}

double Solver::solve_maximum(const expr_vector& formulas) {
  auto result = solve(formulas, MAX_LAYOUT_SIZE);
  if(result.empty()) {
    return -1;
  }
  return result[LAYOUT_NAME];
}

double Solver::solve_minimum(const expr_vector& formulas) {
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
    auto v = model[i];
    if(v.name().str() == LAYOUT_NAME) {
      return model.get_const_interp(v).as_double();
    }
  }
  return 0;
}

double Solver::solve_minimum(const expr_vector& formulas, const std::unordered_set<std::string>& unchanged_variables) {
  for(unsigned i = 0; i < formulas.size(); ++i) {
    qDebug() << formulas[i].to_string().c_str();
  }
  m_solver.push();
  m_solver.add(formulas);
  auto on_exit = Details::ScopeExit([&] { m_solver.pop(); });
  auto min = static_cast<double>(MAX_LAYOUT_SIZE);
  while(true) {
    if(m_solver.check() != check_result::sat) {
      break;
    }
    auto model = m_solver.get_model();
    auto solution = std::unordered_map<std::string, double>();
    auto changed_expr = expr_vector(m_context);
    for(unsigned int i = 0; i < model.num_consts(); ++i) {
      auto decl = model.get_const_decl(i);
      solution[decl.name().str()] = model.get_const_interp(decl).as_double();
      if(!unchanged_variables.contains(decl.name().str())) {
        changed_expr.push_back(decl() != model.eval(decl()));
      }
      if(decl.name().str() == LAYOUT_NAME) {
        min = std::min(min, model.get_const_interp(decl).as_double());
      }
    }
    m_solver.add(mk_or(changed_expr));
  }
  if(min == MAX_LAYOUT_SIZE) {
    return 0;
  }
  return min;
}

bool Solver::check(const expr_vector& formulas) {
  m_solver.push();
  m_solver.add(formulas);
  auto on_exit = Details::ScopeExit([&] { m_solver.pop(); });
  return m_solver.check() == check_result::sat;
}

expr Solver::declare_variable(const std::string& name) {
  return m_context.int_const(name.c_str());
}

context& Solver::get_context() {
  return m_context;
}
