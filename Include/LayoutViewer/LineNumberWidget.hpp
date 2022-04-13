#ifndef DALI_LINE_NUMBER_WIDGET_H
#define DALI_LINE_NUMBER_WIDGET_H
#include <QWidget>
#include "LayoutViewer/JsonEditor.hpp"

namespace Dali {

  class LineNumberWidget : public QWidget {
    public:
      explicit LineNumberWidget(JsonEditor& editor);

      QSize sizeHint() const override;

    protected:
      void paintEvent(QPaintEvent* event) override;

    private:
      JsonEditor* m_editor;
  };
}

#endif
