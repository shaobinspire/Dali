#ifndef DALI_SOLVER_H
#define DALI_SOLVER_H
#include <c++/z3++.h>
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

      void add_constraints(const Constraints& constraints);

      std::vector<std::pair<std::string, double>> solve(int value);

      int get_min_value();
      int get_max_value();

    private:
      z3::context m_context;
      z3::solver m_solver;
  };
}

#endif
