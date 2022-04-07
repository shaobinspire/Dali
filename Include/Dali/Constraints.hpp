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
      void add(const std::vector<QString>& boxes);
      //void set(const QString& name, int value);
      std::vector<std::pair<QString, double>> solve(int value);
      //bool contains(const QString& variable_name);
      const std::vector<Constraint>& get_constraints() const;
      int get_min_value();
    private:
      std::vector<Constraint> m_constraints;
      z3::context m_context;
      z3::solver m_solver;
  };
}

#endif
