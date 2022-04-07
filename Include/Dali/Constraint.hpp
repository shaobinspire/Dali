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

      struct Variable {
        QString m_name;
        Property m_property;
      };

      using Element = std::variant<double, Operator, Variable>;

      explicit Constraint(QString expression);

      z3::expr get_formula(z3::context& context) const;

      const std::unordered_set<QString>& get_variable_names() const;

      bool is_width_related() const;

      //void build_linear_equation(const std::vector<Element>& expression);

      //double evaluate(const std::function<int (int index)>& get_box_size,
      //  const std::function<int ()>& get_layout_size);

      //int get_element_count() const;

      //const Element& get_element(int index) const;

      //Element& get_element(int index);

      //bool is_valid();
      //z3::expr get_express(std::unordered_map<QString, int>&);

    private:
      QString m_expression;
      std::vector<Element> m_lhs_elements;
      std::vector<Element> m_rhs_elements;
      std::unordered_set<QString> m_variable_names;
      bool m_is_width_related;

      void parse();
      std::vector<Constraint::Element> convert_to_rpn(const QString& expression);
      //void convert_to_rpn();
      //z3::expr m_expression;
    //  std::unordered_map<QString, std::vector<Variable>> m_variables;
      //std::vector<Element> m_constraints;
      //double evaluate(const std::vector<Element>& expression, double value);
  };

  //using Constraint = std::unordered_map<QString, Variable>;
  //Constraint parse_constraint_expression(const QString& expression);
  //ConstraintExpression parse_constraints(const std::vector<>& constrains);
}

#endif
