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

Solver::Solver(const Solver& solver)
  : m_solver(m_context,
      Z3_solver_translate(solver.m_context, solver.m_solver, m_context)) {}

void Solver::add_const_formula(const expr_vector& formulas) {
  for(unsigned i = 0; i < formulas.size(); ++i) {
    qDebug() << formulas[i].to_string().c_str();
  }
  m_solver.add(formulas);
}

Solver::SolveResult Solver::solve(const expr_vector& formulas, int value) {
  //for(unsigned i = 0; i < formulas.size(); ++i) {
  //  qDebug() << formulas[i].to_string().c_str();
  //}
  m_solver.push();
  auto on_exit = Details::ScopeExit([&] { m_solver.pop(); });
  m_solver.add(formulas);
  m_solver.add(m_context.int_const(LAYOUT_NAME) == value);
  auto result = Solver::SolveResult();
  if(m_solver.check() != check_result::sat) {
    return result;
  }
  auto model = m_solver.get_model();
  for(auto i = static_cast<unsigned>(0); i < model.num_consts(); ++i) {
    auto decl = model.get_const_decl(i);
    result[decl.name().str()] = model.get_const_interp(decl).as_double();
  }
  return result;
}

std::vector<Solver::SolveResult> Solver::solve(const expr_vector& formulas,
    const std::unordered_set<std::string>& unchanged_variables, int value) {
  //for(unsigned i = 0; i < formulas.size(); ++i) {
  //  qDebug() << formulas[i].to_string().c_str();
  //}
  m_solver.push();
  auto on_exit = Details::ScopeExit([&] { m_solver.pop(); });
  m_solver.add(formulas);
  m_solver.add(m_context.int_const(LAYOUT_NAME) == value);
  auto solutions = std::vector<Solver::SolveResult>();
  while(true) {
    if(m_solver.check() != check_result::sat) {
      break;
    }
    auto model = m_solver.get_model();
    auto solution = Solver::SolveResult();
    auto changed_expr = expr_vector(m_context);
    for(auto i = static_cast<unsigned>(0); i < model.num_consts(); ++i) {
      auto decl = model.get_const_decl(i);
      auto name = decl.name().str();
      solution[name] = model.get_const_interp(decl).as_double();
      if(!unchanged_variables.contains(name)) {
        changed_expr.push_back(decl() != model.eval(decl()));
      }
    }
    m_solver.add(mk_or(changed_expr));
    solutions.push_back(solution);
  }
  return solutions;
}

double Solver::solve_maximum(const expr_vector& formulas, int lower_bound) {
  auto result = solve(formulas, MAX_LAYOUT_SIZE);
  if(!result.empty()) {
    return result[LAYOUT_NAME];
  }
  m_solver.push();
  auto on_exit = Details::ScopeExit([&] { m_solver.pop(); });
  m_solver.add(create_variable(LAYOUT_NAME) > lower_bound);
  m_solver.add(formulas);
  if(m_solver.check() == check_result::sat) {
    return MAX_LAYOUT_SIZE;
  }
  return -1;
}

double Solver::solve_minimum(const expr_vector& formulas) {
  m_solver.push();
  auto on_exit = Details::ScopeExit([&] { m_solver.pop(); });
  //for(unsigned i = 0; i < formulas.size(); ++i) {
  //  qDebug() << formulas[i].to_string().c_str();
  //}
  m_solver.add(formulas);
  if(m_solver.check() != check_result::sat) {
    return 0;
  }
  auto model = m_solver.get_model();
  for(auto i = static_cast<unsigned>(0); i < model.num_consts(); ++i) {
    auto decl = model.get_const_decl(i);
    if(decl.name().str() == LAYOUT_NAME) {
      return model.get_const_interp(decl).as_double();
    }
  }
  return 0;
}

double Solver::solve_minimum(const expr_vector& formulas,
    const std::unordered_set<std::string>& unchanged_variables,
    int lower_bound, int upper_bound) {
  for(unsigned i = 0; i < formulas.size(); ++i) {
    qDebug() << formulas[i].to_string().c_str();
  }
  m_solver.push();
  auto on_exit = Details::ScopeExit([&] { m_solver.pop(); });
  m_solver.add(create_variable(LAYOUT_NAME) <= upper_bound);
  m_solver.add(formulas);
  auto min = static_cast<double>(MAX_LAYOUT_SIZE);
  auto count = 0;
  while(true) {
    if(m_solver.check() != check_result::sat) {
      break;
    }
    ++count;
    auto model = m_solver.get_model();
    auto changed_expr = expr_vector(m_context);
    auto debug = qDebug();
    debug << "count: " << count << ": ";
    for(auto i = static_cast<unsigned>(0); i < model.num_consts(); ++i) {
      auto decl = model.get_const_decl(i);
      auto name = decl.name().str();
      debug << decl.name().str().c_str() << ":" << model.get_const_interp(decl).as_double() << " ";
      if(!unchanged_variables.contains(name)) {
        changed_expr.push_back(decl() != model.eval(decl()));
      }
      if(name == LAYOUT_NAME) {
        min = std::min(min, model.get_const_interp(decl).as_double());
      }
    }
    if(min == lower_bound) {
      break;
    }
    m_solver.add(mk_or(changed_expr));
  }
  return min;
}

bool Solver::check(const expr_vector& formulas) {
  m_solver.push();
  auto on_exit = Details::ScopeExit([&] { m_solver.pop(); });
  m_solver.add(formulas);
  return m_solver.check() == check_result::sat;
}

expr Solver::create_variable(const std::string& name) {
  return m_context.int_const(name.c_str());
}

//expr_vector Solver::create_expr_vector() {
//  return expr_vector(m_context);
//}

context& Solver::get_context() {
  return m_context;
}

expr_vector& Dali::concatenate_to(const expr_vector& src, expr_vector& dst) {
  for(auto iter = src.begin(); iter != src.end(); ++iter) {
    dst.push_back(*iter);
  }
  return dst;
}
