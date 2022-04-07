#include "Dali/Constraints.hpp"

using namespace Dali;
using namespace z3;

Constraints::Constraints()
    : m_solver(m_context) {}

void Constraints::add(const Constraint& constraint) {
  m_constraints.push_back(constraint);
  if(auto formula = constraint.get_formula(m_context)) {
    m_solver.add(formula);
  }
}

std::vector<std::pair<QString, double>> Constraints::solve(int value, const std::vector<QString>& boxes) {
  qDebug() << "width:" << value;
  if(boxes.empty()) {
    return std::vector<std::pair<QString, double>>();
  }
  m_solver.push();
  auto expression = m_context.int_const(boxes.begin()->toLocal8Bit().data());
  for(auto iter = boxes.begin() + 1; iter != boxes.end(); ++iter) {
    expression = expression + m_context.int_const(iter->toLocal8Bit().data());
  }
  m_solver.add(expression == m_context.real_val(std::to_string(value).c_str()));
  auto status = m_solver.check();
  if(status != check_result::sat) {
    m_solver.pop();
    qDebug() << "failed";
    return std::vector<std::pair<QString, double>>();
  }
  auto result = std::vector<std::pair<QString, double>>();
  auto model = m_solver.get_model();
  for(unsigned int i = 0; i < model.num_consts(); ++i) {
    auto decl = model.get_const_decl(i);
    result.push_back({QString::fromStdString(decl.name().str()), model.get_const_interp(decl).as_double()});
  }
  m_solver.pop();
  qDebug() << "successful";
  return result;
}

const std::vector<Constraint>& Constraints::get_constraints() const {
  return m_constraints;
}

//bool Constraints::contains(const QString& variable_name) {
//  m_context.
//  auto model = m_solver.get_model();
//  for(unsigned int i = 0; i < model.num_consts(); ++i) {
//    if(QString::fromStdString(model.get_const_decl(i).name().str()) == variable_name) {
//      return true;
//    }
//  }
//  return false;
//}
