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
      ~Layout();

      Direction get_direction() const;
      void set_direction(Direction direction);

      void add_child(LayoutBase* layout);

      int get_size() const;

      LayoutBase* get_child(int index);

      void draw(QPainter& painter) override;

    private:
      Direction m_direction;
      std::vector<LayoutBase*> m_children;
  };
}

#endif
