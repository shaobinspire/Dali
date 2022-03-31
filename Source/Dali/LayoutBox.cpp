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

const ConstraintExpression& LayoutBox::get_width_constraint() const {
  return m_width_constraint;
}

ConstraintExpression& Dali::LayoutBox::get_width_constraint() {
  return m_width_constraint;
}

void LayoutBox::set_width_constraint(const ConstraintExpression& constraint) {
  m_width_constraint = constraint;
}

const ConstraintExpression& LayoutBox::get_height_constraint() const {
  return m_height_constraint;
}

ConstraintExpression& Dali::LayoutBox::get_height_constraint() {
  return m_height_constraint;
}

void LayoutBox::set_height_constraint(const ConstraintExpression& constraint) {
  m_height_constraint = constraint;
}
