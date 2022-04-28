#ifndef DALI_LAYOUT_WIDGET_H
#define DALI_LAYOUT_WIDGET_H
#include <QWidget>
#include "nlohmann/json.hpp"
#include "Dali/Dali.hpp"
#include "Dali/Layout.hpp"

namespace Dali {

  class LayoutWidget : public QWidget {
    public:
      explicit LayoutWidget(QWidget* parent = nullptr);

      void adjust_size();

      //void adjust_size_fit_layout();

      bool set_layout(std::shared_ptr<Layout> layout);

      QSize get_min_size() const;
      QSize get_max_size() const;

      //void update_size(const QSize& size);

      bool has_valid_layout() const;

      QRect get_layout_rect() const;
      
      Layout::Status get_layout_status() const;

      void show_original_layout(bool is_show_original);

      bool is_show_original_layout() const;

      QSize sizeHint() const override;

    protected:
      void resizeEvent(QResizeEvent* event) override;
      void paintEvent(QPaintEvent* event) override;

    private:
      std::shared_ptr<Layout> m_layout;
      bool m_is_show_original;
  };
}

#endif
