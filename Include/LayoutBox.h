#ifndef DALI_LAYOUTBOX_H
#define DALI_LAYOUTBOX_H
#include <QString>
#include "Dali.h"
#include "LayoutBase.h"

namespace Dali {

  class LayoutBox : public LayoutBase {
    public:

      enum class SizePolicy {
        Expanding,
        Fixed
      };

      LayoutBox();

      SizePolicy get_horizontal_size_policy() const;
      void set_horizontal_size_policy(SizePolicy policy);
      SizePolicy get_vertical_size_policy() const;
      void set_vertical_size_policy(SizePolicy policy);

      QString get_name() const;
      void set_name(const QString& name);

      void draw(QPainter& painter) override;

    private:
      QString m_name;
      SizePolicy m_horizontal_size_policy;
      SizePolicy m_vertical_size_policy;
  };
}

#endif
