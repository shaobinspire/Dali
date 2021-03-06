#include "Dali/Constraint.hpp"
#include <regex>
#include <stack>

using namespace Dali;
using namespace z3;

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

struct ExpressionToken {
  std::string m_token;
  bool m_is_operator;
};

void ltrim(std::string& s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [] (auto ch) {
    return !std::isspace(ch);
  }));
}

void rtrim(std::string& s) {
  s.erase(std::find_if(s.rbegin(), s.rend(), [] (auto ch) {
    return !std::isspace(ch);
  }).base(), s.end());
}

void trim(std::string& s) {
  ltrim(s);
  rtrim(s);
}

bool is_number(const std::string &s) {
  return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

auto to_property(const std::string& value) {
  if(value == "height" || value == "height()") {
    return Constraint::Property::HEIGHT;
  } else if(value == "width" || value == "width()") {
    return Constraint::Property::WIDTH;
  } else if(value == "left") {
    return Constraint::Property::LEFT;
  } else if(value == "top") {
    return Constraint::Property::TOP;
  } else if(value == "right") {
    return Constraint::Property::RIGHT;
  } else if(value == "bottom") {
    return Constraint::Property::BOTTOM;
  }
  return Constraint::Property::NONE;
}

auto to_operator(const std::string& value) {
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

int get_precedence(const std::string& op) {
  if(op == "+" || op == "-") {
    return 1;
  } else if(op == "*" || op == "/") {
    return 2;
  }
  return -1;
}

std::vector<ExpressionToken> split(const std::string& expression) {
  auto list = std::vector<ExpressionToken>();
  auto token_start = expression.begin();
  for(auto iter = token_start; iter != expression.end(); ++iter) {
    if(*iter == '(' || *iter == ')' || *iter == '+' || *iter == '-' ||
        *iter == '*' || *iter == '/') {
      if(token_start != iter) {
        auto str = std::string(token_start, iter);
        trim(str);
        list.push_back({str, false});
      } else if(*iter == '+' || *iter == '-' || *iter == '*' || *iter == '/') {
        return list;
      }
      list.push_back({std::string(1, *iter), true});
      token_start = iter + 1;
    }
  }
  if(token_start != expression.end()) {
    auto str = std::string(token_start, expression.end());
    trim(str);
    list.push_back({str, false});
  }
  return list;
}

Constraint::Variable parse_variable(const std::string& term) {
  auto variable = Constraint::Variable();
  variable.m_content = term;
  if(auto p = term.find("."); p != std::string::npos) {
    variable.m_name = term.substr(0, p);
    variable.m_property = to_property(term.substr(p + 1));
  } else {
    variable.m_property = to_property(term);
  }
  return variable;
}

Constraint::Element parse_element(const std::string& term) {
  auto element = Constraint::Element();
  if(term.find_first_not_of(".0123456789") == std::string::npos) {
    element = std::stod(term);
  } else {
    element = parse_variable(term);
  }
  return element;
}

Constraint::Type parse_type(Constraint::Property property) {
  switch(property) {
    case Constraint::Property::LEFT:
      return Constraint::Type::LEFT_RELATED;
    case Constraint::Property::RIGHT:
      return Constraint::Type::RIGHT_RELATED;
    case Constraint::Property::TOP:
      return Constraint::Type::TOP_RELATED;
    case Constraint::Property::BOTTOM:
      return Constraint::Type::BOTTOM_RELATED;
    case Constraint::Property::WIDTH:
      return Constraint::Type::WIDTH_RELATED;
    case Constraint::Property::HEIGHT:
      return Constraint::Type::HEIGHT_RELATED;
  }
  return Constraint::Type::NONE;
}

expr get_formula(context& context,
    const std::vector<Constraint::Element>& elements) {
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
        if(variable.m_name.empty()) {
          stack.push(context.int_const(LAYOUT_NAME));
        } else {
          stack.push(context.int_const(variable.m_content.c_str()));
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

Constraint::Constraint(std::string expression)
    : m_expression(std::move(expression)),
      m_type_related(Type::NONE) {
  parse();
}

expr Constraint::convert_to_formula(context& context) const {
  if(m_lhs_elements.empty() || m_rhs_elements.empty()) {
    return expr(context);
  }
  if(m_comparison_operator == ComparisonOperator::EQUAL_TO) {
    return get_formula(context, m_lhs_elements) ==
      get_formula(context, m_rhs_elements);
  } else if(m_comparison_operator == ComparisonOperator::LESS_THAN) {
    return get_formula(context, m_lhs_elements) <
      get_formula(context, m_rhs_elements);
  } else if(m_comparison_operator == ComparisonOperator::LESS_THAN_OR_EQUAL_TO) {
    return get_formula(context, m_lhs_elements) <=
      get_formula(context, m_rhs_elements);
  } else if(m_comparison_operator == ComparisonOperator::GREATER_THAN) {
    return get_formula(context, m_lhs_elements) >
      get_formula(context, m_rhs_elements);
  } else if(m_comparison_operator == ComparisonOperator::GREATER_THAN_OR_EQUAL_TO) {
    return get_formula(context, m_lhs_elements) >=
      get_formula(context, m_rhs_elements);
  }
  return expr(context);
}

const std::unordered_set<std::string>& Constraint::get_variable_names() const {
  return m_variable_names;
}

const std::vector<Constraint::Variable>& Constraint::get_variables() const {
  return m_variables;
}

Constraint::Type Constraint::get_type_related() const {
  return m_type_related;
}

Constraint::ComparisonOperator Constraint::get_comparsion_operator() const {
  return m_comparison_operator;
}

void Constraint::parse() {
  auto reg_exp = std::regex("(=+|[<>!]=?)");
  auto match = std::smatch();
  if(!std::regex_search(m_expression, match, reg_exp)) {
    return;
  }
  if(match.str().front() == '=') {
    m_comparison_operator = ComparisonOperator::EQUAL_TO;
  } else if(match.str() == "<") {
    m_comparison_operator = ComparisonOperator::LESS_THAN;
  } else if(match.str() == "<=") {
    m_comparison_operator = ComparisonOperator::LESS_THAN_OR_EQUAL_TO;
  } else if(match.str() == ">") {
    m_comparison_operator = ComparisonOperator::GREATER_THAN;
  } else if(match.str() == ">=") {
    m_comparison_operator = ComparisonOperator::GREATER_THAN_OR_EQUAL_TO;
  } else {
    m_comparison_operator = ComparisonOperator::NONE;
    return;
  }
  m_lhs_elements = convert_to_rpn(match.prefix());
  m_rhs_elements = convert_to_rpn(match.suffix());
}

std::vector<Constraint::Element> Constraint::convert_to_rpn(
    const std::string& expression) {
  auto elements = std::vector<Constraint::Element>();
  auto operator_stack = std::stack<ExpressionToken>();
  auto list = split(expression);
  for(auto& token : list) {
    if(token.m_token.empty()) {
      return elements;
    }
    if(!token.m_is_operator) {
      elements.push_back(parse_element(token.m_token));
      if(std::holds_alternative<Variable>(elements.back())) {
        auto& variable = std::get<Variable>(elements.back());
        m_variable_names.insert(variable.m_name);
        auto type_related = parse_type(variable.m_property);
        if(m_type_related == Type::NONE) {
          m_type_related = type_related;
        } else if(m_type_related != type_related) {
          m_type_related = Type::INVALID;
        }
        m_variables.push_back(variable);
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
          get_precedence(token.m_token) <=
            get_precedence(operator_stack.top().m_token)) {
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
