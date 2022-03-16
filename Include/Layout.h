#ifndef DALI_LAYOUT_H
#define DALI_LAYOUT_H
#include <vector>
#include "Dali.h"
#include "LayoutBase.h"

namespace Dali {

  class Layout : public LayoutBase {
    public:
      enum class Direction {
        HORIZONTAL,
        VERTICAL
      };

      Layout();
      explicit Layout(Direction direction);

      Direction get_direction() const;
      void set_direction(Direction direction);

      void add_child(std::unique_ptr<LayoutBase> layout);

      void paint(QPainter& painter) override;

    private:
      Direction m_direction;
      std::vector<std::unique_ptr<LayoutBase>> m_children;
  };
}

#endif
