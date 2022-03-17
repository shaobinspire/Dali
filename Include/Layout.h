#ifndef DALI_LAYOUT_H
#define DALI_LAYOUT_H
#include <vector>
#include "Dali.h"
#include "LayoutItem.h"

namespace Dali {

  class Layout : public LayoutItem {
    public:
      enum class Direction {
        HORIZONTAL,
        VERTICAL
      };

      Layout();
      explicit Layout(Direction direction);

      Direction get_direction() const;
      void set_direction(Direction direction);

      void add_item(std::unique_ptr<LayoutItem> item);

      int get_item_size() const;

      void draw(QPainter& painter) override;

    private:
      Direction m_direction;
      std::vector<std::unique_ptr<LayoutItem>> m_items;
  };
}

#endif
