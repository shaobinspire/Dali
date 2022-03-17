#ifndef DALI_LAYOUTITEM_H
#define DALI_LAYOUTITEM_H
#include <QPainter>
#include <QRect>

namespace Dali {

  class LayoutItem {
    public:
      virtual ~LayoutItem() = default;

      QRect get_rect() const;
      void set_rect(const QRect& rect);

      QPoint get_pos() const;
      void set_pos(const QPoint& pos);

      QSize get_size() const;
      void set_size(const QSize& size);

      virtual void draw(QPainter& painter) = 0;

    protected:
      LayoutItem() = default;

    private:
      QRect m_rect;
  };
}
#endif
