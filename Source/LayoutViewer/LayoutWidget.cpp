#include "LayoutViewer/LayoutWidget.hpp"
#include <QApplication>
#include <QDesktopWidget>
#include <QPainter>
#include "Dali/Constraint.hpp"
#include "Dali/Layout.hpp"
#include "Dali/LayoutBox.hpp"

using namespace Dali;
using namespace nlohmann;

auto get_color(SizePolicy policy) {
  if(policy == SizePolicy::Fixed) {
    static auto color = QColor(0xFFBB00);
    return color;
  }
  static auto color = QColor(0x0066FF);
  return color;
}

LayoutWidget::LayoutWidget(QWidget *parent)
  : QWidget(parent),
    m_layout(nullptr) {}

void LayoutWidget::set_layout(std::shared_ptr<Layout> layout) {
  setMinimumSize(0, 0);
  setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
  m_layout = layout;
  if(!m_layout) {
    return;
  }
  m_layout->build();
  setMinimumSize(m_layout->get_min_size());
  setMaximumSize(m_layout->get_max_size());
}

QSize LayoutWidget::get_min_size() const {
  if(!m_layout) {
    return QSize();
  }
  return m_layout->get_min_size();
}

QSize LayoutWidget::get_max_size() const {
  if(!m_layout) {
    return QSize();
  }
  return m_layout->get_max_size();
}

void LayoutWidget::update_size(const QSize& size) {
  if(m_layout) {
    m_layout->resize(size);
  }
}

bool LayoutWidget::has_valid_layout() const {
  return m_layout != nullptr;
}

void LayoutWidget::paintEvent(QPaintEvent* event) {
  auto painter = QPainter(this);
  if(m_layout) {
    painter.save();
    for(auto i = 0; i < m_layout->get_box_count(); ++i) {
      auto box = m_layout->get_box(i);
      auto rect = box->get_rect();
      if(box->get_horizontal_size_policy() == box->get_vertical_size_policy()) {
        painter.fillRect(rect, get_color(box->get_horizontal_size_policy()));
      } else {
        auto top_left = rect.topLeft() + QPoint(1, 1);
        auto top_right = rect.topRight() + QPoint(0, 1);
        auto bottom_left = rect.bottomLeft() + QPoint(1, 0);
        painter.setPen(QPen(QBrush(get_color(box->get_horizontal_size_policy())), 2,
          Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
        painter.drawLine(QLineF(top_left, bottom_left));
        painter.drawLine(QLineF(top_right, rect.bottomRight()));
        painter.setPen(QPen(QBrush(get_color(box->get_vertical_size_policy())), 2,
          Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
        painter.drawLine(QLineF(top_left, top_right));
        painter.drawLine(QLineF(bottom_left, rect.bottomRight()));
      }
      painter.setPen(Qt::black);
      auto position = QString("\n(%1, %2, %3, %4)").
        arg(rect.x()).arg(rect.y()).arg(rect.width()).arg(rect.height());
      auto name = QString();
      if(box->is_name_visible()) {
        name = box->get_name();
      }
      painter.drawText(rect, Qt::AlignCenter | Qt::TextWordWrap, name + position);
    }
    painter.restore();
  }
  QWidget::paintEvent(event);
}
