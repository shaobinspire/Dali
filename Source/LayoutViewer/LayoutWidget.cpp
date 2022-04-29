#include "LayoutViewer/LayoutWidget.hpp"
#include <QApplication>
#include <QDesktopWidget>
#include <QPainter>
#include <QResizeEvent>
#include "Dali/Constraint.hpp"
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

auto get_semitransparent_color(SizePolicy policy) {
  if(policy == SizePolicy::Fixed) {
    static auto color = QColor(255, 187, 0, 200);
    return color;
  }
  static auto color = QColor(0, 136, 255, 200);
  return color;
}

auto get_color(Layout::Status status, SizePolicy policy) {
  if(status == Layout::Status::NONE) {
    return get_color(policy);
  }
  return get_semitransparent_color(policy);
}

void paint(QPainter& painter, std::shared_ptr<Layout>& layout) {
  for(auto i = 0; i < layout->get_box_count(); ++i) {
    auto box = layout->get_box(i);
    auto rect = QRect();
    auto status = layout->get_status();
    if(status == Layout::Status::NONE) {
      rect = box->get_rect();
    } else {
      rect = layout->get_temporary_box_rect(i);
    }
    if(box->get_horizontal_size_policy() == box->get_vertical_size_policy()) {
      painter.fillRect(
        rect, get_color(status, box->get_horizontal_size_policy()));
    } else {
      auto top_left = rect.topLeft() + QPoint(1, 1);
      auto top_right = rect.topRight() + QPoint(0, 1);
      auto bottom_left = rect.bottomLeft() + QPoint(1, 0);
      painter.setPen(QPen(QBrush(
        get_color(status, box->get_horizontal_size_policy())), 2,
        Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
      painter.drawLine(QLineF(top_left, bottom_left));
      painter.drawLine(QLineF(top_right, rect.bottomRight()));
      painter.setPen(QPen(QBrush(
        get_color(status, box->get_vertical_size_policy())), 2,
        Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
      painter.drawLine(QLineF(top_left, top_right));
      painter.drawLine(QLineF(bottom_left, rect.bottomRight()));
    }
    painter.setPen(Qt::black);
    auto position = QString("\n(%1, %2, %3, %4)").
      arg(rect.x()).arg(rect.y()).arg(rect.width()).arg(rect.height());
    auto name = QString();
    if(box->is_name_visible()) {
      name = QString::fromStdString(box->get_name());
    }
    painter.drawText(rect, Qt::AlignCenter | Qt::TextWordWrap, name + position);
  }
}

LayoutWidget::LayoutWidget(QWidget *parent)
  : QWidget(parent),
    m_layout(nullptr),
    m_is_show_original(false) {}

void LayoutWidget::adjust_size() {
  if(m_layout) {
    m_layout->resize(size());
  }
}

bool LayoutWidget::set_layout(std::shared_ptr<Layout> layout) {
  setMinimumSize(0, 0);
  setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
  m_layout = layout;
  if(!m_layout) {
    return false;
  }
  //if(m_is_show_original) {
  //  m_layout->build_constraints();
  //  return true;
  //}
  if(!m_layout->build()) {
    m_layout.reset();
    return false;
  }
  setMinimumSize(m_layout->get_min_size());
  return true;
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

bool LayoutWidget::has_valid_layout() const {
  return m_layout != nullptr;
}

QRect LayoutWidget::get_layout_rect() const {
  if(!m_layout) {
    return QRect();
  }
  return m_layout->get_rect();
}

Layout::Status LayoutWidget::get_layout_status() const {
  if(!m_layout) {
    return Layout::Status::NONE;
  }
  return m_layout->get_status();
}

void LayoutWidget::show_original_layout(bool is_show_original) {
  m_is_show_original = is_show_original;
}

bool LayoutWidget::is_show_original_layout() const {
  return m_is_show_original;
}

QSize LayoutWidget::sizeHint() const {
  if(!m_layout) {
    return {0, 0};
  }
  return m_layout->get_rect().size();
}

void LayoutWidget::resizeEvent(QResizeEvent* event) {
  if(m_layout && !m_is_show_original) {
    m_layout->resize(event->size());
  }
  QWidget::resizeEvent(event);
}

void LayoutWidget::paintEvent(QPaintEvent* event) {
  auto painter = QPainter(this);
  if(m_layout) {
    painter.save();
    paint(painter, m_layout);
    painter.restore();
  }
  QWidget::paintEvent(event);
}

