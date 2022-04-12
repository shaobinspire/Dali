#ifndef DALI_MAIN_WINDOW_H
#define DALI_MAIN_WINDOW_H
#include <QLabel>
#include <QPlainTextEdit>
#include <QMainWindow>
#include "Dali/Parser.hpp"

namespace Dali {

  class MainWindow : public QMainWindow {
    public:
      MainWindow();

    protected:
      void resizeEvent(QResizeEvent* event) override;

    private:
      LayoutWidget* m_layout_widget;
      QPlainTextEdit* m_text_edit;
      QMenu* m_view_menu;
      QLabel* m_file_name_label;
      QLabel* m_layout_size_label;
      QLabel* m_size_label;
      QAction* m_open_action;
      QAction* m_refresh_action;
      QString m_file_name;
      Parser m_parser;

      void create_dock_window();
      void create_menu();
      void open();
      void refresh();
      void update_layout_size_message();
  };
}

#endif
