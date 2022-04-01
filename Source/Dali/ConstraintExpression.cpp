#include "Dali/ConstraintExpression.hpp"
#include <stack>

using namespace Dali;

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

struct ExpressionToken {
  QString m_token;
  bool m_is_operator;
};

auto to_property(const QString& value) {
  if(value == "height" || value == "height()") {
    return ConstraintExpression::Property::HEIGHT;
  } else if(value == "width" || value == "width()") {
    return ConstraintExpression::Property::WIDTH;
  }
  return ConstraintExpression::Property::NONE;
}

auto to_operator(const QString& value) {
  if(value == "+") {
    return ConstraintExpression::Operator::ADDITION;
  } else if(value == "-") {
    return ConstraintExpression::Operator::SUBTRACTION;
  } else if(value == "*") {
    return ConstraintExpression::Operator::MULTIPLICATION;
  } else if(value == "/") {
    return ConstraintExpression::Operator::DIVISION;
  }
  return ConstraintExpression::Operator::NONE;
}

auto evaluate(ConstraintExpression::Operator o, double a, double b) {
  if(o == ConstraintExpression::Operator::ADDITION) {
    return a + b;
  } else if(o == ConstraintExpression::Operator::SUBTRACTION) {
    return b - a;
  } else if(o == ConstraintExpression::Operator::MULTIPLICATION) {
    return a * b;
  } else if(o == ConstraintExpression::Operator::DIVISION) {
    return b / a;
  }
  return 0.0;
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
          list.push_back({std::move(token), false});
        }
      }
      list.push_back({expression.mid(k, 1), true});
      index = k + 1;
      i = k;
    }
  }
  return list;
}

ConstraintExpression::Element parse_element(const QString& term) {
  auto element = ConstraintExpression::Element();
  if(term.contains(QRegExp("[a-zA-Z]"))) {
    auto variable = ConstraintExpression::Variable();
    if(term.contains(".")) {
      variable.m_name = term.section('.', 0, 0);
      variable.m_property = to_property(term.section('.', 1, 1));
    } else {
      variable.m_property = to_property(term);
    }
    element = variable;
  } else {
    element = term.toDouble();
  }
  return element;
}

ConstraintExpression::ConstraintExpression(std::vector<Element> expression)
  : m_expression(std::move(expression)) {}

double ConstraintExpression::evaluate(
    const std::function<int (int index)>& get_box_size,
    const std::function<int ()>& get_layout_size) {
  auto stack = std::stack<double>();
  for(auto& element : m_expression) {
    std::visit(overloaded{
      [&](double number) { stack.push(number); },
      [&] (const Variable& variable) {
        if(variable.m_name.isEmpty()) {
          stack.push(get_layout_size());
        } else {
          stack.push(get_box_size(variable.m_index));
        }
      },
      [&] (const Operator o) {
        auto a = stack.top();
        stack.pop();
        auto b = stack.top();
        stack.pop();
        stack.push(::evaluate(o, a, b));
      }}, element);
  }
  return stack.top();
}

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

int ConstraintExpression::get_element_count() const {
  return static_cast<int>(m_expression.size());
}

const ConstraintExpression::Element& ConstraintExpression::get_element(int index) const {
  if(index < 0 || index >= get_element_count()) {
    throw std::out_of_range("The index is out of range.");
  }
  return m_expression[index];
}

ConstraintExpression::Element& ConstraintExpression::get_element(int index) {
  if(index < 0 || index >= get_element_count()) {
    throw std::out_of_range("The index is out of range.");
  }
  return m_expression[index];
}

bool ConstraintExpression::is_valid() {
  return get_element_count() > 0;
}

ConstraintExpression Dali::parse_expression(const QString& expression) {
  auto constraint_express = ConstraintExpression();
  auto operator_stack = std::stack<ExpressionToken>();
  auto list = split(expression);
  for(auto& token : list) {
    if(!token.m_is_operator) {
      constraint_express.m_expression.push_back(parse_element(token.m_token));
    } else if(token.m_token == "(") {
      operator_stack.push(token);
    } else if(token.m_token == ")") {
      while(!operator_stack.empty() && operator_stack.top().m_token != "(") {
        constraint_express.m_expression.push_back(to_operator(operator_stack.top().m_token));
        operator_stack.pop();
      }
      if(operator_stack.top().m_token == "(") {
        operator_stack.pop();
      }
    } else {
      while(!operator_stack.empty() &&
          get_precedence(token.m_token) <= get_precedence(operator_stack.top().m_token)) {
        constraint_express.m_expression.push_back(to_operator(operator_stack.top().m_token));
        operator_stack.pop();
        //auto o = to_operator(operator_stack.top().m_token);
        //operator_stack.pop();
        //auto a = constraint_express.m_expression.rbegin();
        //auto b = constraint_express.m_expression.rbegin() + 1;
        //if(std::holds_alternative<double>(*a) && std::holds_alternative<double>(*b)) {
        //  auto value = ::evaluate(o, std::get<double>(*a), std::get<double>(*b));
        //  constraint_express.m_expression.pop_back();
        //  constraint_express.m_expression.pop_back();
        //  constraint_express.m_expression.push_back(value);
        //} else {
        //  constraint_express.m_expression.push_back(o);
        //}
      }
      operator_stack.push(token);
    }
  }
  while(!operator_stack.empty()) {
    constraint_express.m_expression.push_back(to_operator(operator_stack.top().m_token));
    operator_stack.pop();
  }
  return constraint_express;
}
