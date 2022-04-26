#ifndef DALI_CONSTRAINT_H
#define DALI_CONSTRAINT_H
#include <c++/z3++.h>
#include <unordered_set>
#include <variant>
#include "Dali/Dali.hpp"

namespace Dali {

  class Constraint {
    public:
      enum class Type {
        NONE,
        LEFT_RELATED,
        RIGHT_RELATED,
        TOP_RELATED,
        BOTTOM_RELATED,
        WIDTH_RELATED,
        HEIGHT_RELATED
      };

      enum class Property {
        NONE,
        LEFT,
        RIGHT,
        TOP,
        BOTTOM,
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
        std::string m_content;
        std::string m_name;
        Property m_property;
      };

      using Element = std::variant<double, Operator, Variable>;

      explicit Constraint(std::string expression);

      z3::expr convert_to_formula(z3::context& context) const;

      const std::unordered_set<std::string>& get_variable_names() const;
      const std::vector<Variable>& get_variables() const;

      Type get_type_related() const;

      ComparisonOperator get_comparsion_operator() const;

    private:
      std::string m_expression;
      std::vector<Element> m_lhs_elements;
      std::vector<Element> m_rhs_elements;
      std::unordered_set<std::string> m_variable_names;
      std::vector<Variable> m_variables;
      Type m_type_related;
      ComparisonOperator m_comparison_operator;

      void parse();
      std::vector<Constraint::Element> convert_to_rpn(const std::string& expression);
  };

  std::string to_string(Constraint::Property property);
}

#endif
