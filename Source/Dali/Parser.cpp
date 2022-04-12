#include "Dali/Parser.hpp"
#include <fstream>
#include <QWidget>
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

std::shared_ptr<Layout> Parser::parse() {
  auto layout = std::make_shared<Layout>();
  for(auto& item : m_json["layout"]) {
    auto box = new LayoutBox();
    if(item.contains("name")) {
      box->set_name(QString::fromStdString(item["name"].get<std::string>()));
    }
    box->set_rect({item["x"], item["y"], item["width"], item["height"]});
    if(item.contains("policy")) {
      auto size_policy = get_size_policy(item["policy"]);
      box->set_horizontal_size_policy(size_policy);
      box->set_vertical_size_policy(size_policy);
    } else {
      if(item.contains("horizontal_policy")) {
        box->set_horizontal_size_policy(
          get_size_policy(item["horizontal_policy"]));
      }
      if(item.contains("vertical_policy")) {
        box->set_vertical_size_policy(
          get_size_policy(item["vertical_policy"]));
      }
    }
    layout->set_rect(layout->get_rect().united(box->get_rect()));
    layout->add_box(box);
  }
  if(m_json.contains("constraints")) {
    for(auto& expression : m_json["constraints"]) {
      auto constraint = Constraint(
        QString::fromStdString(expression.get<std::string>()));
      if(constraint.is_width_related()) {
        layout->add_width_constraint(std::move(constraint));
      } else {
        layout->add_height_constraint(std::move(constraint));
      }
    }
  }
  return layout;
}

std::string Parser::get_content() {
  return m_json.dump(2);
}

std::shared_ptr<Layout> Parser::parse(const std::string& name) {
  auto ifs = std::ifstream(name);
  try {
    m_json = json::parse(ifs);
    return parse();
  } catch(json::exception&) {
    return nullptr;
  }
}
