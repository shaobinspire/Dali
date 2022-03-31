#ifndef DALI_MAIN_WINDOW_H
#define DALI_MAIN_WINDOW_H
#include <QLabel>
#include <QMainWindow>
#include "Dali/Dali.hpp"

namespace Dali {

  class MainWindow : public QMainWindow {
    public:
      MainWindow();

    protected:
      //void wheelEvent(QWheelEvent* event) override;
      void resizeEvent(QResizeEvent* event) override;

    private:
      LayoutWidget* m_layout_widget;
      QLabel* m_size_label;
      QAction* m_open_action;
      QAction* m_refresh_action;
      //QAction* m_zoom_in_action;
      //QAction* m_zoom_out_action;
      //QAction* m_normal_size_action;
      QString m_file_name;
      //double m_scale;

      void create_menu();
      void open();
      void refresh();
      //void zoom_in();
      //void zoom_out();
      //void normal_size();
      //void update_size();
  };
}

#endif
