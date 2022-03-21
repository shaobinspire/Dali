#include "Dali/LayoutWidget.hpp"
#include <fstream>
#include <QApplication>
#include <QDesktopWidget>
#include "Dali/LayoutItem.hpp"

using namespace Dali;
using namespace nlohmann;

auto get_color(LayoutItem::SizePolicy policy) {
  if(policy == LayoutItem::SizePolicy::Fixed) {
    static auto color = QColor(0xFFBB00);
    return color;
  }
  static auto color = QColor(0x0066FF);
  return color;
}

LayoutItem::SizePolicy get_size_policy(const std::string& policy) {
  if(policy == "fixed") {
    return LayoutItem::SizePolicy::Fixed;
  }
  return LayoutItem::SizePolicy::Expanding;
}

std::unique_ptr<Layout> parse(const json& json) {
  auto layout = std::make_unique<Layout>();
  for(auto& item : json) {
    auto layout_item = LayoutItem();
    if(item.contains("name")) {
      layout_item.set_name(QString::fromStdString(item["name"].get<std::string>()));
    }
    layout_item.set_rect({item["x"], item["y"], item["width"], item["height"]});
    if(item.contains("policy")) {
      auto size_policy = get_size_policy(item["policy"]);
      layout_item.set_horizontal_size_policy(size_policy);
      layout_item.set_vertical_size_policy(size_policy);
    } else {
      if(item.contains("horizontal_policy")) {
        layout_item.set_horizontal_size_policy(
          get_size_policy(item["horizontal_policy"]));
      }
      if(item.contains("vertical_policy")) {
        layout_item.set_vertical_size_policy(
          get_size_policy(item["vertical_policy"]));
      }
    }
    layout->set_rect(layout->get_rect().united(layout_item.get_rect()));
    layout->add_item(layout_item);
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
  m_layout = std::move(parse(m_json));
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
  setFixedSize(m_layout->get_rect().size() * m_scale);
}

void LayoutWidget::paintEvent(QPaintEvent* event) {
  auto painter = QPainter(this);
  painter.scale(m_scale, m_scale);
  if(m_layout) {
    painter.save();
    for(auto i = 0; i < m_layout->get_item_size(); ++i) {
      auto& item = m_layout->get_item(i);
      auto rect = item.get_rect();
      if(item.get_horizontal_size_policy() == item.get_vertical_size_policy()) {
        painter.fillRect(rect, get_color(item.get_horizontal_size_policy()));
      } else {
        auto top_left = rect.topLeft() + QPoint(1, 1);
        auto top_right = rect.topRight() + QPoint(0, 1);
        auto bottom_left = rect.bottomLeft() + QPoint(1, 0);
        painter.setPen(QPen(QBrush(get_color(item.get_horizontal_size_policy())), 2,
          Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
        painter.drawLine(QLineF(top_left, bottom_left));
        painter.drawLine(QLineF(top_right, rect.bottomRight()));
        painter.setPen(QPen(QBrush(get_color(item.get_vertical_size_policy())), 2,
          Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
        painter.drawLine(QLineF(top_left, top_right));
        painter.drawLine(QLineF(bottom_left, rect.bottomRight()));
      }
      painter.setPen(Qt::black);
      auto position = QString("\n(%1, %2, %3, %4)").
        arg(rect.x()).arg(rect.y()).arg(rect.width()).arg(rect.height());
      painter.drawText(rect, Qt::AlignCenter | Qt::TextWordWrap, item.get_name() + position);
    }
    painter.restore();
  }
  QWidget::paintEvent(event);
}

