#ifndef DALI_LAYOUTBOX_H
#define DALI_LAYOUTBOX_H
#include <QString>
#include "Dali.h"
#include "LayoutBase.h"

namespace Dali {

  class LayoutBox : public LayoutBase {
    public:
      LayoutBox();

      QString get_name() const;
      void set_name(const QString& name);

      void paint(QPainter& painter) override;

    private:
      QString m_name;
  };
}

#endif
