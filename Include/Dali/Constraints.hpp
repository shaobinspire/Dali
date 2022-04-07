#ifndef DALI_CONSTRAINTS_H
#define DALI_CONSTRAINTS_H
#include "Dali/Constraint.hpp"
#include "Dali/Dali.hpp"

namespace Dali {

  class Constraints {
    public:
      void add_local_constraint(const Constraint& constraint);
      void add_global_constraint(const Constraint& constraint);
      int get_constraint_count() const;
      const Constraint& get_constraint(int index) const;
      bool has_varaible_name_in_global(const QString& name);
    private:
      std::vector<Constraint> m_constraints;
      std::unordered_set<QString> m_global_variable_name;
  };
}

#endif
