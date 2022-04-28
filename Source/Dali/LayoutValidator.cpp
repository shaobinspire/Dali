#include "Dali/LayoutValidator.hpp"

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


LayoutValidator::LayoutValidator() {
  auto parser = SchemaParser();
  auto schemaDocumentAdapter = NlohmannJsonAdapter(layout_schema);
  parser.populateSchema(schemaDocumentAdapter, m_schema);
}

std::pair<bool, std::string> LayoutValidator::validate(const nlohmann::json& json) {
  auto validator = Validator(Validator::kStrongTypes);
  auto results = ValidationResults();
  auto adapter = NlohmannJsonAdapter(json);
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
