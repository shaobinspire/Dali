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
      void resizeEvent(QResizeEvent* event) override;

    private:
      LayoutWidget* m_layout_widget;
      QLabel* m_size_label;
      QLabel* m_layout_size_label;
      QAction* m_open_action;
      QAction* m_refresh_action;
      QString m_file_name;

      void create_menu();
      void open();
      void refresh();
  };
}

#endif
