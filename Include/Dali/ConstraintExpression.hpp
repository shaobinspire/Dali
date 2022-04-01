#ifndef DALI_CONSTRAINT_EXPRESSION_H
#define DALI_CONSTRAINT_EXPRESSION_H
#include <variant>
#include "Dali/Dali.hpp"

namespace Dali {

  class ConstraintExpression {
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

      struct Variable {
        int m_index;
        QString m_name;
        Property m_property;
      };

      using Element = std::variant<double, Operator, Variable>;

      ConstraintExpression() = default;
      explicit ConstraintExpression(std::vector<Element> expression);

      double evaluate(const std::function<int (int index)>& get_box_size,
        const std::function<int ()>& get_layout_size);

      //std::map<int, double> get_coefficient();

      int get_element_count() const;

      const Element& get_element(int index) const;

      Element& get_element(int index);

      bool is_valid();

    private:
      std::vector<Element> m_expression;

      friend ConstraintExpression parse_expression(const QString& expression);
  };

  ConstraintExpression parse_expression(const QString& expression);
}

#endif
