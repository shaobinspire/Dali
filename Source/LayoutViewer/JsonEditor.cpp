#include "LayoutViewer/JsonEditor.hpp"
#include <iomanip>
#include <QPainter>
#include <QTextBlock>
#include <fstream>
#include "LayoutViewer/LineNumberWidget.hpp"

using namespace Dali;
using namespace nlohmann;
using namespace valijson;
using namespace valijson::adapters;

json& get_layout_schema() {
  static auto layout_schema = R"({
    "type": "object",
    "properties": {
      "layout": {
        "type": "array",
        "items": {
          "type": "object",
          "properties": {
            "name": {
              "type": "string"
            },
            "x": {
              "type": "number"
            },
            "y": {
              "type": "number"
            },
            "width": {
              "type": "number"
            },
            "height": {
              "type": "number"
            },
            "horizontal": {
              "type": "string",
              "enum": ["fixed", "take_space"]
            },
            "vertical": {
              "type": "string",
              "enum": ["fixed", "take_space"]
            }
          },
          "required": [
            "x",
            "y",
            "width",
            "height",
            "horizontal",
            "vertical"
          ]
        }
      },
      "constraints": {
        "type": "array",
        "items": {
          "type": "string"
        }
      }
    },
    "required": ["layout"]
  })"_json;
  return layout_schema;
}

JsonEditor::JsonEditor(QWidget* parent)
    : QPlainTextEdit(parent) {
  auto parser = SchemaParser();
  auto schemaDocumentAdapter = NlohmannJsonAdapter(get_layout_schema());
  parser.populateSchema(schemaDocumentAdapter, m_schema);
  m_line_number_widget = new LineNumberWidget(*this);
  update_line_number_widget_width();
  connect(this, &JsonEditor::textChanged, this, &JsonEditor::on_text_changed);
  connect(this, &JsonEditor::updateRequest,
    this, &JsonEditor::update_line_number_widget);
  connect(this, &JsonEditor::blockCountChanged,
    this, &JsonEditor::update_line_number_widget_width);
}

void JsonEditor::load_json(const QString& file_name) {
  auto ifs = std::ifstream(file_name.toStdString());
  auto text = std::string(std::istreambuf_iterator<char>(ifs),
                 std::istreambuf_iterator<char>());
  setPlainText(QString::fromStdString(text));
}

const nlohmann::json& JsonEditor::get_json() const {
  return m_json;
}

const std::string& JsonEditor::get_errors() const {
  return m_errors;
}

void JsonEditor::paint_line_number_event(QPaintEvent* event) {
  auto painter = QPainter(m_line_number_widget);
  painter.setPen(Qt::black);
  painter.fillRect(event->rect(), Qt::lightGray);
  auto block = firstVisibleBlock();
  auto block_number = block.blockNumber();
  auto top =
    qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
  auto bottom = top + qRound(blockBoundingRect(block).height());
  while(block.isValid() && top <= event->rect().bottom()) {
    if(block.isVisible() && bottom >= event->rect().top()) {
      auto number = QString::number(block_number + 1);
      painter.drawText(0, top, m_line_number_widget->width(),
        fontMetrics().height(), Qt::AlignRight, number);
    }
    block = block.next();
    top = bottom;
    bottom = top + qRound(blockBoundingRect(block).height());
    ++block_number;
  }
}

void JsonEditor::connect_parse_result(
    const ParseResultCallback& parse_result_callback) {
  m_parse_result_callback = parse_result_callback;
}

void JsonEditor::resizeEvent(QResizeEvent* event) {
  QPlainTextEdit::resizeEvent(event);
  auto rect = contentsRect();
  m_line_number_widget->setGeometry(QRect(rect.left(), rect.top(),
    m_line_number_widget->sizeHint().width(), rect.height()));
}

void JsonEditor::on_text_changed() {
  parse(toPlainText().toStdString());
}

void JsonEditor::parse(const std::string& text) {
  m_errors.clear();
  try {
    m_json = json::parse(text);
  } catch(json::exception& e) {
    m_errors = e.what();
    m_parse_result_callback(true);
    return;
  }
  auto validator = Validator(Validator::kStrongTypes);
  auto results = ValidationResults();
  auto adapter = NlohmannJsonAdapter(m_json);
  if(!validator.validate(m_schema, adapter, &results)) {
    auto error = ValidationResults::Error();
    unsigned int errorNum = 1;
    auto ss = std::stringstream();
    while(results.popError(error)) {
      auto context = std::string();
      for(auto& itr : error.context) {
        context += itr;
      }
      ss << errorNum << ". " << context << ": " << error.description << std::endl;
      ++errorNum;
    }
    m_errors = ss.str();
    m_parse_result_callback(true);
    return;
  }
  m_parse_result_callback(false);
}

void JsonEditor::update_line_number_widget(const QRect& rect, int dy) {
  if(dy) {
    m_line_number_widget->scroll(0, dy);
  } else {
    m_line_number_widget->update(0, rect.y(), m_line_number_widget->width(),
      rect.height());
  }
}

void JsonEditor::update_line_number_widget_width() {
  setViewportMargins(m_line_number_widget->sizeHint().width(), 0, 0, 0);
}
