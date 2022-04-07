#ifndef DALI_SOLVER_H
#define DALI_SOLVER_H
#include <c++/z3++.h>
#include "Dali/Constraints.hpp"
#include "Dali/Dali.hpp"

namespace Dali {

  class Solver {
    public:
      explicit Solver();

      void add_constraints(const Constraints& constraints);

      std::vector<std::pair<QString, double>> solve(int value);

      int get_min_value();

    private:
      z3::context m_context;
      z3::solver m_solver;
  };
}

#endif
