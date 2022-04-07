#ifndef DALI_LAYOUT_BOX_H
#define DALI_LAYOUT_BOX_H
#include <QRect>
#include <QString>
#include "Dali/Dali.hpp"

namespace Dali {

  class LayoutBox {
    public:
      LayoutBox() = default;

      QRect get_rect() const;
      void set_rect(const QRect& rect);

      QPoint get_pos() const;
      void set_pos(const QPoint& pos);

      QSize get_size() const;
      void set_size(const QSize& size);

      SizePolicy get_horizontal_size_policy() const;
      void set_horizontal_size_policy(SizePolicy policy);

      SizePolicy get_vertical_size_policy() const;
      void set_vertical_size_policy(SizePolicy policy);

      QString get_name() const;
      void set_name(const QString& name);

      //const Constraint& get_width_constraints() const;
      //Constraint& get_width_constraints();
      //void set_width_constraints(const Constraint& constraint);
      //const Constraint& get_height_constraints() const;
      //Constraint& get_height_constraints();
      //void set_height_constraints(const Constraint& constraint);

    private:
      QRect m_rect;
      QString m_name;
      SizePolicy m_horizontal_size_policy;
      SizePolicy m_vertical_size_policy;
      //Constraint m_width_constraints;
      //Constraint m_height_constraints;
  };
}
#endif
