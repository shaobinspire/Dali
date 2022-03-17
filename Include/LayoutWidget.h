#ifndef DALI_LAYOUTWIDGET_H
#define DALI_LAYOUTWIDGET_H
#include <QWidget>
#include "nlohmann/json.hpp"
#include "Layout.h"

namespace Dali {

  class LayoutWidget : public QWidget {
    public:
      explicit LayoutWidget(QWidget* parent = nullptr);

      bool parse_json_file(const QString& name);

      double get_scale() const;
      void set_scale(double scale);

    protected:
      void paintEvent(QPaintEvent* event) override;

    private:
      std::unique_ptr<Layout> m_layout;
      nlohmann::json m_json;
      double m_scale;
  };
}

#endif
