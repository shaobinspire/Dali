#ifndef DALI_JSON_EDITOR_H
#define DALI_JSON_EDITOR_H
#include <QPlainTextEdit>

namespace Dali {

  class JsonEditor : public QPlainTextEdit {
    public:

      explicit JsonEditor(QWidget* parent = nullptr);

      void load(const QString& file_name);

      void paint_line_number_event(QPaintEvent* event);

    protected:
      void resizeEvent(QResizeEvent *event) override;

    private:
      QWidget* m_line_number_widget;

      void update_line_number_widget(const QRect& rect, int dy);
      void update_line_number_widget_width();
  };
}

#endif
