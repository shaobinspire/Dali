#ifndef DALI_MAIN_WINDOW_H
#define DALI_MAIN_WINDOW_H
#include <QMainWindow>
#include <QMenu>
#include "Dali.h"

namespace Dali {

  class MainWindow : public QMainWindow {
    public:
      MainWindow();

    protected:
      void wheelEvent(QWheelEvent* event) override;

    private:
      LayoutWidget* m_layout_widget;
      QAction* m_open_action;
      QAction* m_zoom_in_action;
      QAction* m_zoom_out_action;
      QAction* m_normal_size_action;
      double m_scale;

      void create_menu();
      void open();
      void zoom_in();
      void zoom_out();
      void normal_size();
      void update_size();
  };
}

#endif
