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

Layout* parse(const json& json, QPoint& pos) {
  auto& children = json["children"];
  auto orignal_pos = pos;
  auto direction = get_direction(json["direction"]);
  auto layout = new Layout(direction);
  layout->set_pos(pos);
  auto update_layout_rect = [&] (LayoutBase* item) {
    if(layout->get_direction() == Layout::Direction::HORIZONTAL) {
      pos = {pos.x() + item->get_width(), orignal_pos.y()};
      layout->set_size({layout->get_width() + item->get_width(),
        item->get_height()});
    } else {
      pos = {orignal_pos.x(), pos.y() + item->get_height()};
      layout->set_size({item->get_width(),
        layout->get_height() + item->get_height()});
    }
  };
  for(auto& child : children) {
    if(!child.contains("direction")) {
      auto box = new LayoutBox();
      if(child.contains("name")) {
        box->set_name(QString::fromStdString(child["name"].get<std::string>()));
      }
      box->set_rect({pos.x(), pos.y(), child["width"], child["height"]});
      if(child.contains("policy")) {
        auto size_policy = get_size_policy(child["policy"]);
        box->set_horizontal_size_policy(size_policy);
        box->set_vertical_size_policy(size_policy);
      } else {
        if(child.contains("horizontal_policy")) {
          box->set_horizontal_size_policy(
            get_size_policy(child["horizontal_policy"]));
        }
        if(child.contains("vertical_policy")) {
          box->set_vertical_size_policy(
            get_size_policy(child["vertical_policy"]));
        }
      }
      update_layout_rect(box);
      layout->add_child(box);
    } else {
      auto child_layout = parse(child, pos);
      update_layout_rect(child_layout);
      layout->add_child(child_layout);
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
  m_layout = std::unique_ptr<Layout>(parse(m_json, pos));
  setFixedSize(m_layout->get_rect().size());
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
  setFixedSize({static_cast<int>(m_layout->get_width() * m_scale),
    static_cast<int>(m_layout->get_height() * m_scale)});
}

void LayoutWidget::paintEvent(QPaintEvent* event) {
  auto painter = QPainter(this);
  painter.scale(m_scale, m_scale);
  if(m_layout) {
    m_layout->draw(painter);
  }
  QWidget::paintEvent(event);
}

