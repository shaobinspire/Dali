#ifndef DALI_JSON_EDITOR_H
#define DALI_JSON_EDITOR_H
#include <QPlainTextEdit>
#include "nlohmann/json.hpp"
#include "valijson_nlohmann_bundled.hpp"
#include "Dali/Dali.hpp"

namespace Dali {

  class JsonEditor : public QPlainTextEdit {
    public:

      using ParseResultCallback = std::function<void (bool is_failed)>;

      explicit JsonEditor(QWidget* parent = nullptr);

      void load_json(const QString& file_name);

      const nlohmann::json& get_json() const;

      const std::string& get_errors() const;

      void paint_line_number_event(QPaintEvent* event);

      void connect_parse_result(const ParseResultCallback& parse_result_callback);

    protected:
      void resizeEvent(QResizeEvent *event) override;

    private:
      nlohmann::json m_json;
      valijson::Schema m_schema;
      QWidget* m_line_number_widget;
      std::string m_errors;
      ParseResultCallback m_parse_result_callback;

      void on_text_changed();
      void parse(const std::string& text);
      void update_line_number_widget(const QRect& rect, int dy);
      void update_line_number_widget_width();
  };
}

#endif
