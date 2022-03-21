#ifndef DALI_LAYOUT_H
#define DALI_LAYOUT_H
#include <vector>
#include <QRect>
#include "Dali/LayoutItem.hpp"

namespace Dali {

  class Layout {
    public:

      Layout() = default;

      QRect get_rect() const;
      void set_rect(const QRect& rect);

      void add_item(const LayoutItem& item);

      int get_item_size() const;

      const LayoutItem& get_item(int index) const;

    private:
      QRect m_rect;
      std::vector<LayoutItem> m_items;
  };
}

#endif
