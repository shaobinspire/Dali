#ifndef DALI_LAYOUTITEM_H
#define DALI_LAYOUTITEM_H
#include <QPainter>
#include <QRect>

namespace Dali {
  class LayoutBase {
    public:
      enum class SizePolicy {
        Expanding,
        Fixed
      };

      virtual ~LayoutBase() = default;

      const QRect& get_rect() const;
      void set_rect(const QRect& rect);
      void set_pos(const QPoint& pos);
      void set_size(const QSize& size);

      SizePolicy get_horizontal_size_policy() const;
      SizePolicy get_vertical_size_policy() const;
      void set_horizontal_size_policy(SizePolicy policy);
      void set_vertical_size_policy(SizePolicy policy);

      virtual void paint(QPainter& painter);

    protected:
      LayoutBase() = default;

    private:
      QRect m_rect;
      SizePolicy m_horizontal_size_policy;
      SizePolicy m_vertical_size_policy;
  };
}
#endif
