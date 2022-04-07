#include "Dali/Constraint.hpp"
#include <stack>
#include <QStringList>

using namespace Dali;
using namespace z3;

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

struct ExpressionToken {
  QString m_token;
  bool m_is_operator;
};

auto to_property(const QString& value) {
  if(value == "height" || value == "height()") {
    return Constraint::Property::HEIGHT;
  } else if(value == "width" || value == "width()") {
    return Constraint::Property::WIDTH;
  }
  return Constraint::Property::NONE;
}

auto to_operator(const QString& value) {
  if(value == "+") {
    return Constraint::Operator::ADDITION;
  } else if(value == "-") {
    return Constraint::Operator::SUBTRACTION;
  } else if(value == "*") {
    return Constraint::Operator::MULTIPLICATION;
  } else if(value == "/") {
    return Constraint::Operator::DIVISION;
  }
  return Constraint::Operator::NONE;
}

auto evaluate(Constraint::Operator o, const expr& a, const expr& b) {
  if(o == Constraint::Operator::ADDITION) {
    return a + b;
  } else if(o == Constraint::Operator::SUBTRACTION) {
    return a - b;
  } else if(o == Constraint::Operator::MULTIPLICATION) {
    return a * b;
  } else if(o == Constraint::Operator::DIVISION) {
    return a / b;
  }
  return a;
}

int get_precedence(const QString& op) {
  if(op == "+" || op == "-") {
    return 1;
  } else if(op == "*" || op == "/") {
    return 2;
  }
  return -1;
}

std::vector<ExpressionToken> split(const QString& expression) {
  auto separators = QRegExp("[()+\\-*\\/]");
  auto index = 0;
  auto list = std::vector<ExpressionToken>();
  for(auto i = 0; i < expression.length(); ++i) {
    auto k = expression.indexOf(separators, i);
    if(k < 0) {
      list.push_back({expression.mid(i).trimmed(), false});
      break;
    } else {
      if(k - index > 0) {
        auto token = expression.mid(index, k - index).trimmed();
        if(!token.isEmpty()) {
          list.push_back({token, false});
        }
      }
      list.push_back({expression.mid(k, 1), true});
      index = k + 1;
      i = k;
    }
  }
  return list;
}

Constraint::Variable parse_variable(const QString& term) {
  auto variable = Constraint::Variable();
  if(term.contains(".")) {
    variable.m_name = term.section('.', 0, 0);
    variable.m_property = to_property(term.section('.', 1, 1));
  } else {
    variable.m_property = to_property(term);
  }
  return variable;
}

Constraint::Element parse_element(const QString& term) {
  auto element = Constraint::Element();
  if(term.contains(QRegExp("[a-zA-Z]"))) {
    element = parse_variable(term);
  } else {
    element = term.toDouble();
  }
  return element;
}

//double evaluate(const std::vector<Constraint::Element>& expression,
//  const std::function<double (double number)>& get_number,
//  const std::function<double (const QString& name)>& get_variable_value) {
//  auto stack = std::stack<double>();
//  for(auto& element : expression) {
//    std::visit(overloaded{
//      [&](double number) { stack.push(number); },
//      [&] (const Constraint::Variable& variable) {
//        stack.push(get_variable_value(variable.m_name));
//      },
//      [&] (const Constraint::Operator o) {
//        auto a = stack.top();
//        stack.pop();
//        auto b = stack.top();
//        stack.pop();
//        stack.push(::evaluate(o, b, a));
//      }}, element);
//  }
//  return stack.top();
//}

//std::unordered_map<QString, Variable> parse_coefficient(const std::vector<Element>& expression) {
//  auto coefficients = std::unordered_map<QString, Variable>();
//  for(auto& element : expression) {
//    if(std::holds_alternative<Variable>(element)) {
//      auto variable = std::get<Variable>(element);
//      if(variable.m_name.isEmpty()) {
//        variable.m_name = "layout";
//      }
//      variable.m_coefficient = 1;
//      coefficients[variable.m_name] = variable;
//    }
//  }
//  for(auto& coefficient : coefficients) {
//    coefficients[coefficient.first].m_coefficient = evaluate(expression,
//      [] (double number) { return 0; },
//      [&] (const QString& name) {
//        if(name == coefficient.first) {
//          return 1;
//        }
//        return 0;
//      });
//  }
//  coefficients[""] = {-1, "", Property::NONE,
//    evaluate(expression,
//    [] (double number) { return number; },
//    [] (const QString&) { return 0; })};
//  return coefficients;
//}
//Constraint::Constraint(std::vector<Element> expression) {}
//  //: m_constraints(std::move(expression)) {}
//

//std::map<int, double> ConstraintExpression::get_coefficient() {
//  auto coefficient = std::map<int, double>();
//  coefficient[-1] = evaluate(
//    [] (auto i) { return 0; },
//    [] { return 0; });
//  auto variables = std::vector<Variable>();
//  for(auto& element : m_expression) {
//    if(std::holds_alternative<Variable>(element)) {
//      variables.push_back(std::get<Variable>(element));
//    }
//  }
//  for(auto& v : variables) {
//    auto index = v.m_index;
//    auto stack = std::stack<std::tuple<double, bool>>();
//    for(auto& element : m_expression) {
//      std::visit(overloaded{
//        [&](double number) { stack.push({number, true}); },
//        [&] (const Variable& variable) {
//          if(variable.m_index == index) {
//            stack.push({1, false});
//          } else {
//            stack.push({0, false});
//          }
//        },
//        [&] (const Operator o) {
//          auto a = stack.top();
//          stack.pop();
//          auto b = stack.top();
//          stack.pop();
//          auto value = [&] () -> std::tuple<double, bool> {
//            if(o == ConstraintExpression::Operator::ADDITION) {
//              if(std::get<1>(a) && std::get<1>(b)) {
//                return {0.0, true};
//              } else if(std::get<1>(a) && !std::get<1>(b)) {
//                return b;
//              } else if(!std::get<1>(a) && std::get<1>(b)) {
//                return a;
//              }
//              return {std::get<0>(a) + std::get<0>(b), false};
//            } else if(o == ConstraintExpression::Operator::SUBTRACTION) {
//              if(std::get<1>(a) && std::get<1>(b)) {
//                return {0.0, true};
//              } else if(std::get<1>(a) && !std::get<1>(b)) {
//                return b;
//              } else if(!std::get<1>(a) && std::get<1>(b)) {
//                return {-std::get<0>(a), true};
//              }
//              return {std::get<0>(a) + std::get<0>(b), false};
//            } else if(o == ConstraintExpression::Operator::MULTIPLICATION) {
//              return {std::get<0>(a) * std::get<0>(b), false};
//            } else if(o == ConstraintExpression::Operator::DIVISION) {
//              return {std::get<0>(b) / std::get<0>(a), false};
//            }
//          }();
//          //stack.push(::evaluate(o, a, b));
//        }}, element);
//    }
//  }
//  return coefficient;
//}
expr get_formula(context& context, const std::vector<Constraint::Element>& elements) {
  if(elements.empty()) {
    return expr(context);
  }
  auto stack = std::stack<expr>();
  for(auto& element : elements) {
    std::visit(overloaded{
      [&](double number) {
        stack.push(context.real_val(std::to_string(number).c_str()));
      },
      [&] (const Constraint::Variable& variable) {
        if(variable.m_name.isEmpty()) {
          stack.push(context.int_const("layout"));
        } else {
          stack.push(context.int_const(variable.m_name.toStdString().c_str()));
        }
      },
      [&] (const Constraint::Operator o) {
        auto a = stack.top();
        stack.pop();
        auto b = stack.top();
        stack.pop();
        stack.push(evaluate(o, b, a));
      }}, element);
  }
  return stack.top();
}

Constraint::Constraint(QString expression)
    : m_expression(std::move(expression)),
      m_is_width_related(true) {
  parse();
}

expr Constraint::convert_to_formula(context& context) const {
  if(m_lhs_elements.empty() || m_rhs_elements.empty()) {
    return expr(context);
  }
  if(m_comparison_operator == ComparisonOperator::EQUAL_TO) {
    return get_formula(context, m_lhs_elements) == get_formula(context, m_rhs_elements);
  } else if(m_comparison_operator == ComparisonOperator::LESS_THAN) {
    return get_formula(context, m_lhs_elements) < get_formula(context, m_rhs_elements);
  } else if(m_comparison_operator == ComparisonOperator::LESS_THAN_OR_EQUAL_TO) {
    return get_formula(context, m_lhs_elements) <= get_formula(context, m_rhs_elements);
  } else if(m_comparison_operator == ComparisonOperator::GREATER_THAN) {
    return get_formula(context, m_lhs_elements) > get_formula(context, m_rhs_elements);
  } else if(m_comparison_operator == ComparisonOperator::GREATER_THAN_OR_EQUAL_TO) {
    return get_formula(context, m_lhs_elements) >= get_formula(context, m_rhs_elements);
  }
  return expr(context);
}

const std::unordered_set<QString>& Constraint::get_variable_names() const {
  return m_variable_names;
}

bool Constraint::is_width_related() const {
  return m_is_width_related;
}

Constraint::ComparisonOperator Constraint::get_comparsion_operator() const {
  return m_comparison_operator;
}

//int Constraint::get_element_count() const {
//  return static_cast<int>(m_elements.size());
//}
//
//const Constraint::Element& Constraint::get_element(int index) const {
//  if(index < 0 || index >= get_element_count()) {
//    throw std::out_of_range("The index is out of range.");
//  }
//  return m_elements[index];
//}
//
//Constraint::Element& Constraint::get_element(int index) {
//  if(index < 0 || index >= get_element_count()) {
//    throw std::out_of_range("The index is out of range.");
//  }
//  return m_expression[index];
//}

//bool Constraint::is_valid() {
//  return get_element_count() > 0;
//}

std::vector<Constraint::Element> Constraint::convert_to_rpn(const QString& expression) {
  auto elements = std::vector<Constraint::Element>();
  auto operator_stack = std::stack<ExpressionToken>();
  auto list = split(expression);
  for(auto& token : list) {
    if(!token.m_is_operator) {
      elements.push_back(parse_element(token.m_token));
      if(std::holds_alternative<Variable>(elements.back())) {
        auto& variable = std::get<Variable>(elements.back());
        m_variable_names.insert(variable.m_name);
        if(variable.m_property == Property::HEIGHT) {
          m_is_width_related = false;
        } else if(variable.m_property == Property::WIDTH) {
          m_is_width_related = true;
        }
      }
    } else if(token.m_token == "(") {
      operator_stack.push(token);
    } else if(token.m_token == ")") {
      while(!operator_stack.empty() && operator_stack.top().m_token != "(") {
        elements.push_back(to_operator(operator_stack.top().m_token));
        operator_stack.pop();
      }
      if(operator_stack.top().m_token == "(") {
        operator_stack.pop();
      }
    } else {
      while(!operator_stack.empty() &&
          get_precedence(token.m_token) <= get_precedence(operator_stack.top().m_token)) {
        elements.push_back(to_operator(operator_stack.top().m_token));
        operator_stack.pop();
      }
      operator_stack.push(token);
    }
  }
  while(!operator_stack.empty()) {
    elements.push_back(to_operator(operator_stack.top().m_token));
    operator_stack.pop();
  }
  return elements;
}

void Constraint::parse() {
  auto regexp = QRegExp("(=|>|<|>=|<=)");
  auto pos = m_expression.indexOf(regexp);
  if(pos == -1) {
    return;
  }
  auto match = regexp.cap();
  if(match == "=") {
    m_comparison_operator = ComparisonOperator::EQUAL_TO;
  } else if(match == "<") {
    m_comparison_operator = ComparisonOperator::LESS_THAN;
  } else if(match == "<=") {
    m_comparison_operator = ComparisonOperator::LESS_THAN_OR_EQUAL_TO;
  } else if(match == ">") {
    m_comparison_operator = ComparisonOperator::GREATER_THAN;
  } else if(match == ">=") {
    m_comparison_operator = ComparisonOperator::GREATER_THAN_OR_EQUAL_TO;
  } else {
    m_comparison_operator = ComparisonOperator::NONE;
  }
  m_lhs_elements = convert_to_rpn(m_expression.mid(0, pos));
  m_rhs_elements = convert_to_rpn(m_expression.mid(pos + regexp.matchedLength()));
  //auto list = m_expression.split(QRegExp("="));
  //if(list.size() == 2) {
    //m_lhs_elements = convert_to_rpn(list[0]);
    //m_rhs_elements = convert_to_rpn(list[1]);
  //}
}

//const expr Constraint::get_express() const {
//  return m_expression;
//}

//Constraint Dali::parse_constraint_expression(const QString& expression) {
//  return Constraint(convert_to_rpn(expression));
//}
