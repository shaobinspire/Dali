#ifndef DALI_LAYOUTITEM_H
#define DALI_LAYOUTITEM_H
#include <QPainter>
#include <QPoint>
#include <QSize>

namespace Dali {
  class LayoutBase {
    public:
      virtual ~LayoutBase() = default;

      QRect get_rect() const;
      void set_rect(const QRect& rect);

      QPoint get_pos() const;
      void set_pos(const QPoint& pos);

      QSize get_size() const;
      void set_size(const QSize& size);

      int get_x() const;
      void set_x(int x);

      int get_y() const;
      void set_y(int y);

      int get_width() const;
      void set_width(int width);

      int get_height() const;
      void set_height(int height);

      virtual void draw(QPainter& painter) = 0;

    protected:
      LayoutBase();

    private:
      QPoint m_pos;
      QSize m_size;
  };
}
#endif
