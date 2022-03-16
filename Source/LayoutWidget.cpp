#include "LayoutWidget.h"
#include <fstream>
#include <QApplication>
#include <QDesktopWidget>
#include "LayoutBox.h"

using namespace Dali;
using namespace nlohmann;

static constexpr auto DPI = 96;

std::unique_ptr<Layout> parse(const json& json, QPoint& pos) {
  auto& children = json["children"];
  auto orignal_pos = pos;
  int width = 0; 
  int height = 0;
  auto direction = [&] {
    if(json["direction"] == "horizontal") {
      return Layout::Direction::HORIZONTAL;
    }
    return Layout::Direction::VERTICAL;
  }();
  auto layout = std::make_unique<Layout>(direction);
  layout->set_pos(pos);
  for(auto& child : children) {
    if(!child.contains("direction")) {
      auto box = std::make_unique<LayoutBox>();
      if(child.contains("name")) {
        box->set_name(QString::fromStdString(child["name"].get<std::string>()));
      }
      width = child["width"];
      height = child["height"];
      box->set_rect({pos.x(), pos.y(), width, height});
      if(child.contains("policy")) {
        auto size_policy = [&] {
          if(child["policy"] == "fixed") {
            return LayoutBase::SizePolicy::Fixed;
          } else {
            return LayoutBase::SizePolicy::Expanding;
          }
        }();
        box->set_horizontal_size_policy(size_policy);
        box->set_vertical_size_policy(size_policy);
      } else {
        if(child.contains("horizontal_policy")) {
          auto size_policy = [&] {
            if(child["horizontal_policy"] == "fixed") {
              return LayoutBase::SizePolicy::Fixed;
            } else {
              return LayoutBase::SizePolicy::Expanding;
            }
          }();
          box->set_horizontal_size_policy(size_policy);
        }
        if(child.contains("vertical_policy")) {
          auto size_policy = [&] {
            if(child["vertical_policy"] == "fixed") {
              return LayoutBase::SizePolicy::Fixed;
            } else {
              return LayoutBase::SizePolicy::Expanding;
            }
          }();
          box->set_vertical_size_policy(size_policy);
        }
      }
      if(layout->get_direction() == Layout::Direction::HORIZONTAL) {
        pos.setX(pos.x() + width);
      } else {
        pos.setY(pos.y() + height);
      }
      layout->add_child(std::move(box));
    } else {
      layout->add_child(parse(child, pos));
      if(layout->get_direction() == Layout::Direction::HORIZONTAL) {
        pos.setX(pos.x() + width);
        pos.setY(orignal_pos.y());
      } else {
        pos.setX(orignal_pos.x());
        pos.setY(pos.y() + height);
      }
    }
  }
  if(layout->get_direction() == Layout::Direction::HORIZONTAL) {
    pos.setY(pos.y() + height);
  } else {
    pos.setX(pos.x() + width);
  }
  return layout;
}

LayoutWidget::LayoutWidget(QWidget *parent)
  : QWidget(parent),
    m_layout(nullptr) {
  m_scale_x = QApplication::desktop()->logicalDpiX() / DPI;
  m_scale_y = QApplication::desktop()->logicalDpiY() / DPI;
}

void LayoutWidget::parse_json_file(const QString& name) {
  auto ifs = std::ifstream(name.toStdString());
  m_json = json::parse(ifs);
  auto pos = QPoint();
  m_layout = std::move(parse(m_json, pos));
}

void LayoutWidget::paintEvent(QPaintEvent* event) {
  auto painter = QPainter(this);
  //painter.setViewport(10, 10, size().width() - 20, size().height() - 20);
  painter.setViewport(10, 10, size().width(), size().height());
  painter.scale(m_scale_x, m_scale_y);
  if(m_layout) {
    m_layout->paint(painter);
  }
  QWidget::paintEvent(event);
}

