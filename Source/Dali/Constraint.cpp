#include "Dali/Constraint.hpp"
#include <QRegExp>
#include <stack>

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
          stack.push(context.real_const(layout_name));
        } else {
          stack.push(context.real_const(variable.m_name.toStdString().c_str()));
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

const std::set<QString>& Constraint::get_variable_names() const {
  return m_variable_names;
}

bool Constraint::is_width_related() const {
  return m_is_width_related;
}

Constraint::ComparisonOperator Constraint::get_comparsion_operator() const {
  return m_comparison_operator;
}

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
}
