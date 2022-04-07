#ifndef DALI_CONSTRAINT_H
#define DALI_CONSTRAINT_H
#include <c++/z3++.h>
#include <unordered_set>
#include <variant>
#include <QString>
#include "Dali/Dali.hpp"

namespace Dali {

  class Constraint {
    public:

      enum class Property {
        NONE,
        WIDTH,
        HEIGHT
      };

      enum class Operator {
        NONE,
        ADDITION,
        DIVISION,
        MULTIPLICATION,
        SUBTRACTION
      };

      enum class ComparisonOperator {
        NONE,
        EQUAL_TO,
        LESS_THAN,
        LESS_THAN_OR_EQUAL_TO,
        GREATER_THAN,
        GREATER_THAN_OR_EQUAL_TO
      };

      struct Variable {
        QString m_name;
        Property m_property;
      };

      using Element = std::variant<double, Operator, Variable>;

      explicit Constraint(QString expression);

      z3::expr convert_to_formula(z3::context& context) const;

      const std::unordered_set<QString>& get_variable_names() const;

      bool is_width_related() const;

      ComparisonOperator get_comparsion_operator() const;

    private:
      QString m_expression;
      std::vector<Element> m_lhs_elements;
      std::vector<Element> m_rhs_elements;
      std::unordered_set<QString> m_variable_names;
      bool m_is_width_related;
      ComparisonOperator m_comparison_operator;

      void parse();
      std::vector<Constraint::Element> convert_to_rpn(const QString& expression);
  };
}

#endif
