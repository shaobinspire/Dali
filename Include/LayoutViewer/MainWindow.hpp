#ifndef DALI_MAIN_WINDOW_H
#define DALI_MAIN_WINDOW_H
#include <QLabel>
#include <QMainWindow>
#include "Dali/Parser.hpp"
#include "LayoutViewer/JsonEditor.hpp"

namespace Dali {

  class MainWindow : public QMainWindow {
    public:
      MainWindow();

    protected:
      bool eventFilter(QObject* watched, QEvent* event) override;
      void closeEvent(QCloseEvent *event) override;

    private:
      LayoutWidget* m_layout_widget;
      JsonEditor* m_editor;
      QTextEdit* m_error_output;
      QMenu* m_view_menu;
      QLabel* m_file_name_label;
      QLabel* m_layout_size_label;
      QLabel* m_size_label;
      QAction* m_refresh_action;
      QString m_file_name;
      Parser m_parser;

      void create_dock_windows();
      void create_menu();
      void open();
      void refresh();
      bool save();
      bool save_as();
      bool save_file(const QString& file_name);
      bool maybe_save();
      void parse_result(bool is_failed);
      void update_layout_size_message();
  };
}

#endif
