#ifndef DALI_LAYOUT_BOX_H
#define DALI_LAYOUT_BOX_H
#include <QRect>
#include <QString>
#include "Dali/Dali.hpp"

namespace Dali {

  class LayoutBox {
    public:
      LayoutBox();

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

      bool is_name_visible() const;
      void set_name_visible(bool is_visible);

    private:
      QRect m_rect;
      QString m_name;
      SizePolicy m_horizontal_size_policy;
      SizePolicy m_vertical_size_policy;
      bool m_is_name_visible;
  };
}
#endif
