#ifndef DALI_LAYOUT_WIDGET_H
#define DALI_LAYOUT_WIDGET_H
#include <QWidget>
#include "nlohmann/json.hpp"
#include "Dali/Dali.hpp"

namespace Dali {

  class LayoutWidget : public QWidget {
    public:
      explicit LayoutWidget(QWidget* parent = nullptr);

      void set_layout(std::shared_ptr<Layout> layout);

      QSize get_min_size() const;
      QSize get_max_size() const;

      //void update_size(const QSize& size);

      bool has_valid_layout() const;

    protected:
      void resizeEvent(QResizeEvent* event) override;
      void paintEvent(QPaintEvent* event) override;

    private:
      std::shared_ptr<Layout> m_layout;
  };
}

#endif
