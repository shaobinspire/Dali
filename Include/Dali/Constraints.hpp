#ifndef DALI_CONSTRAINTS_H
#define DALI_CONSTRAINTS_H
#include <unordered_set>
#include <vector>
#include <c++/z3++.h>
#include "Dali/Dali.hpp"

namespace Dali {

  class Constraints {
    public:
      void add_local_constraint(const Constraint& constraint, bool forced);
      void add_global_constraint(const Constraint& constraint);
      z3::expr_vector convert(z3::context& context);
      int get_constraint_count() const;
      const Constraint& get_constraint(int index) const;
      bool has_varaible_name_in_global(const std::string& name);
      void clear();
    private:
      std::vector<Constraint> m_constraints;
      std::unordered_set<std::string> m_global_variable_name;
  };
}

#endif
