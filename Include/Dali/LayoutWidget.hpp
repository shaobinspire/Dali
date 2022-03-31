#ifndef DALI_LAYOUT_WIDGET_H
#define DALI_LAYOUT_WIDGET_H
#include <QWidget>
#include "nlohmann/json.hpp"
#include "Dali/Dali.hpp"

namespace Dali {

  class LayoutWidget : public QWidget {
    public:
      explicit LayoutWidget(QWidget* parent = nullptr);

      bool parse_json_file(const QString& name);

      double get_scale() const;
      void set_scale(double scale);

      QSize get_min_size() const;
      QSize get_max_size() const;

      void resize(const QSize& size);

    protected:
      void paintEvent(QPaintEvent* event) override;

    private:
      std::unique_ptr<Layout> m_layout;
      nlohmann::json m_json;
      double m_scale;
  };
}

#endif
