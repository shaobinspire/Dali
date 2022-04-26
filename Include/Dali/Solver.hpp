#ifndef DALI_SOLVER_H
#define DALI_SOLVER_H
#include <c++/z3++.h>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "Dali/Dali.hpp"

namespace Dali {
namespace Details {
  struct ScopeExit {
    std::function<void()> m_f;
    ScopeExit(std::function<void()> f);
    ~ScopeExit();
  };
}

  class Solver {
    public:
      Solver();

      void add_const_formula(const z3::expr_vector& formulas);

      //void reset();

      //std::unordered_map<std::string, double> solve(const Constraints& sum_constraints, int value);
      //std::unordered_map<std::string, double> solve(const std::vector<z3::expr>& sum_constraints, int value);
      std::unordered_map<std::string, double> solve(const z3::expr_vector& formulas, int value);
      std::vector<std::unordered_map<std::string, double>> solve(const z3::expr_vector& formulas,
        const std::unordered_set<std::string>& unchanged_variables, int value);
      double solve_maximum(const z3::expr_vector& formulas);
      double solve_minimum(const z3::expr_vector& formulas);
      double solve_minimum(const z3::expr_vector& formulas, const std::unordered_set<std::string>& unchanged_variables);

      z3::expr declare_variable(const std::string& name);

      z3::context& get_context();

    private:
      z3::context m_context;
      z3::solver m_solver;
  };
}

#endif
