#include "Dali/LayoutBox.hpp"

using namespace Dali;

QRect LayoutBox::get_rect() const {
  return m_rect;
}

void LayoutBox::set_rect(const QRect& rect) {
  m_rect = rect;
}

QPoint LayoutBox::get_pos() const {
  return m_rect.topLeft();
}

void LayoutBox::set_pos(const QPoint& pos) {
  m_rect.moveTopLeft(pos);
}

QSize LayoutBox::get_size() const {
  return m_rect.size();
}

void LayoutBox::set_size(const QSize& size) {
  m_rect.setSize(size);
}

SizePolicy LayoutBox::get_horizontal_size_policy() const {
  return m_horizontal_size_policy;
}

void LayoutBox::set_horizontal_size_policy(SizePolicy policy) {
  m_horizontal_size_policy = policy;
}

SizePolicy LayoutBox::get_vertical_size_policy() const {
  return m_vertical_size_policy;
}

void LayoutBox::set_vertical_size_policy(SizePolicy policy) {
  m_vertical_size_policy = policy;
}

QString LayoutBox::get_name() const {
  return m_name;
}

void LayoutBox::set_name(const QString& name) {
  m_name = name;
}

//const Constraint& LayoutBox::get_width_constraints() const {
//  return m_width_constraints;
//}
//
//Constraint& Dali::LayoutBox::get_width_constraints() {
//  return m_width_constraints;
//}
//
//void LayoutBox::set_width_constraints(const Constraint& constraints) {
//  m_width_constraints = constraints;
//}
//
//const Constraint& LayoutBox::get_height_constraints() const {
//  return m_height_constraints;
//}
//
//Constraint& Dali::LayoutBox::get_height_constraints() {
//  return m_height_constraints;
//}
//
//void LayoutBox::set_height_constraints(const Constraint& constraints) {
//  m_height_constraints = constraints;
//}
