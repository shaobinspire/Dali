#ifndef DALI_CONSTRAINTS_H
#define DALI_CONSTRAINTS_H
#include <c++/z3++.h>
#include "Dali/Constraint.hpp"
#include "Dali/Dali.hpp"

namespace Dali {

  class Constraints {
    public:

      Constraints();
      void add(const Constraint& constraint);
      std::vector<std::pair<QString, double>> solve(int value, const std::vector<QString>& boxes);
      //bool contains(const QString& variable_name);
      const std::vector<Constraint>& get_constraints() const;

    private:
      std::vector<Constraint> m_constraints;
      z3::context m_context;
      z3::solver m_solver;
  };
}

#endif
