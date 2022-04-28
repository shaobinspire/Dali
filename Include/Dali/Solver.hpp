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
      using SolveResult = std::unordered_map<std::string, double>;

      Solver();

      void add_const_formula(const z3::expr_vector& formulas);

      SolveResult solve(const z3::expr_vector& formulas, int value);

      std::vector<SolveResult> solve(const z3::expr_vector& formulas,
        const std::unordered_set<std::string>& unchanged_variables, int value);

      double solve_maximum(const z3::expr_vector& formulas, int lower_bound);

      double solve_minimum(const z3::expr_vector& formulas);

      double solve_minimum(const z3::expr_vector& formulas,
        const std::unordered_set<std::string>& unchanged_variables,
        int lower_bound, int upper_bound);

      bool check(const z3::expr_vector& formulas);

      z3::expr create_variable(const std::string& name);
      z3::expr_vector create_expr_vector();

      z3::context& get_context();

    private:
      z3::context m_context;
      z3::solver m_solver;
  };

  z3::expr_vector& concatenate_to(const z3::expr_vector& src, z3::expr_vector& dst);
}

#endif
