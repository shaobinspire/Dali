#include "Dali/Parser.hpp"
#include "Dali/Layout.hpp"
#include "Dali/LayoutBox.hpp"

using namespace Dali;
using namespace nlohmann;

SizePolicy get_size_policy(const std::string& policy) {
  if(policy == "fixed") {
    return SizePolicy::Fixed;
  }
  return SizePolicy::Expanding;
}

std::pair<std::shared_ptr<Layout>, std::string> Parser::parse(
    const json& m_json) {
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
