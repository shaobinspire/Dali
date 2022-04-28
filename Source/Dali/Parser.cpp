#include "Dali/Parser.hpp"
#include "Dali/Layout.hpp"
#include "Dali/LayoutBox.hpp"

using namespace Dali;
using namespace nlohmann;
using namespace valijson;
using namespace valijson::adapters;

const auto layout_schema = R"({
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
          "left": {
            "type": "number"
          },
          "top": {
            "type": "number"
          },
          "width": {
            "type": "number"
          },
          "height": {
            "type": "number"
          },
          "right": {
            "type": "number"
          },
          "bottom": {
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
          "left",
          "top",
          "horizontal",
          "vertical"
        ],
        "oneOf": [
          {
            "required": [
              "width",
              "height"
            ]
          },
          {
            "required": [
              "right",
              "bottom"
            ]
          },
          {
            "required": [
              "right",
              "height"
            ]
          },
          {
            "required": [
              "width",
              "bottom"
            ]
          }
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

SizePolicy get_size_policy(const std::string& policy) {
  if(policy == "fixed") {
    return SizePolicy::Fixed;
  }
  return SizePolicy::Expanding;
}

Parser::Parser() {
  auto parser = SchemaParser();
  auto schemaDocumentAdapter = NlohmannJsonAdapter(layout_schema);
  parser.populateSchema(schemaDocumentAdapter, m_schema);
}

std::pair<std::shared_ptr<Layout>, std::string> Parser::parse() {
  auto layout = std::make_shared<Layout>();
  try {
    for(auto& item : m_json["layout"]) {
      auto box = new LayoutBox();
      if(item.contains("name")) {
        box->set_name(item["name"].get<std::string>());
      }
      auto top_left = QPoint();
      if(item.contains("left") && item.contains("top")) {
        top_left = {item["left"], item["top"]};
      }
      auto width = -1;
      if(item.contains("width")) {
        width = item["width"];
      }
      auto height = -1;
      if(item.contains("height")) {
        height = item["height"];
      }
      auto right = -1;
      if(item.contains("right")) {
        right = item["right"];
      }
      auto bottom = -1;
      if(item.contains("bottom")) {
        bottom = item["bottom"];
      }
      if(width < 0 && right >= 0) {
        width = right - top_left.x() + 1;
      }
      if(height < 0 && bottom >= 0) {
        height = bottom - top_left.y() + 1;
      }
      box->set_rect({top_left, QSize(width, height)});
      if(item.contains("policy")) {
        auto size_policy = get_size_policy(item["policy"]);
        box->set_horizontal_size_policy(size_policy);
        box->set_vertical_size_policy(size_policy);
      } else {
        if(item.contains("horizontal")) {
          box->set_horizontal_size_policy(
            get_size_policy(item["horizontal"]));
        }
        if(item.contains("vertical")) {
          box->set_vertical_size_policy(
            get_size_policy(item["vertical"]));
        }
      }
      layout->set_rect(layout->get_rect().united(box->get_rect()));
      layout->add_box(box);
    }
    if(m_json.contains("constraints")) {
      for(auto& expression : m_json["constraints"]) {
        auto constraint = Constraint(expression.get<std::string>());
        if(constraint.get_type_related() == Constraint::Type::WIDTH_RELATED) {
          layout->add_width_constraint(std::move(constraint));
        } else if(constraint.get_type_related() == Constraint::Type::HEIGHT_RELATED) {
          layout->add_height_constraint(std::move(constraint));
        } else if(constraint.get_type_related() != Constraint::Type::NONE) {
          layout->add_position_constraint(std::move(constraint));
        }
      }
    }
    return {layout, ""};
  } catch(json::exception& e) {
    return {layout, e.what()};
  }
}

std::pair<bool, std::string> Parser::verify(const std::string& text) {
  try {
    m_json = json::parse(text);
  } catch(json::exception& e) {
    return {false, e.what()};
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
    return {false, ss.str()};
  }
  return {true, ""};
}
