#include "LayoutWidget.h"
#include <fstream>
#include <QApplication>
#include <QDesktopWidget>
#include "LayoutBox.h"

using namespace Dali;
using namespace nlohmann;

Layout::Direction get_direction(const std::string& direction) {
  if(direction == "horizontal") {
    return Layout::Direction::HORIZONTAL;
  }
  return Layout::Direction::VERTICAL;
}

LayoutBox::SizePolicy get_size_policy(const std::string& policy) {
  if(policy == "fixed") {
    return LayoutBox::SizePolicy::Fixed;
  }
  return LayoutBox::SizePolicy::Expanding;
}

std::unique_ptr<Layout> parse(const json& json, QPoint& pos) {
  if(!json.contains("direction") || !json.contains("items")) {
    return nullptr;
  }
  auto direction = get_direction(json["direction"]);
  auto orignal_pos = pos;
  auto layout = std::make_unique<Layout>(direction);
  layout->set_pos(pos);
  auto update_pos = [&] (LayoutItem& item) {
    if(layout->get_direction() == Layout::Direction::HORIZONTAL) {
      pos = {pos.x() + item.get_size().width(), orignal_pos.y()};
    } else {
      pos = {orignal_pos.x(), pos.y() + item.get_size().height()};
    }
  };
  auto& items = json["items"];
  for(auto& item : items) {
    if(!item.contains("direction")) {
      auto box = std::make_unique<LayoutBox>();
      if(item.contains("name")) {
        box->set_name(QString::fromStdString(item["name"].get<std::string>()));
      }
      box->set_rect({pos.x(), pos.y(), item["width"], item["height"]});
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
      update_pos(*box);
      layout->add_item(std::move(box));
    } else {
      auto child_layout = parse(item, pos);
      update_pos(*child_layout);
      layout->add_item(std::move(child_layout));
    }
  }
  return layout;
}

LayoutWidget::LayoutWidget(QWidget *parent)
  : QWidget(parent),
    m_layout(nullptr),
    m_scale(1.0) {}

bool LayoutWidget::parse_json_file(const QString& name) {
  auto ifs = std::ifstream(name.toStdString());
  try {
    m_json = json::parse(ifs);
  } catch(json::exception&) {
    return false;
  }
  auto pos = QPoint();
  m_layout = std::move(parse(m_json, pos));
  if(m_layout) {
    setFixedSize(m_layout->get_rect().size());
  }
  return true;
}

double LayoutWidget::get_scale() const {
  return m_scale;
}

void LayoutWidget::set_scale(double scale) {
  if(!m_layout) {
    return;
  }
  m_scale = scale;
  setFixedSize(m_layout->get_size() * m_scale);
}

void LayoutWidget::paintEvent(QPaintEvent* event) {
  auto painter = QPainter(this);
  painter.scale(m_scale, m_scale);
  if(m_layout) {
    m_layout->draw(painter);
  }
  QWidget::paintEvent(event);
}

