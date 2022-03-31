#include "LayoutViewer/MainWindow.hpp"
#include <QFileDialog>
#include <QLayout>
#include <QMenuBar>
#include <QMessageBox>
#include <QResizeEvent>
#include <QScreen>
#include <QStatusBar>
#include "Dali/LayoutWidget.hpp"

using namespace Dali;

const auto MIN_SCALE_FACTOR = 0.4;
const auto MAX_SCALE_FACTOR = 8;
const auto SCALE_FACTOR_STEP = 0.2;

MainWindow::MainWindow()
    : m_scale(1.0) {
  auto central_widget = new QWidget(this);
  auto layout = new QHBoxLayout(central_widget);
  m_layout_widget = new LayoutWidget();
  layout->addWidget(m_layout_widget);
  setCentralWidget(central_widget);
  create_menu();
  m_size_label = new QLabel();
  //m_size_label->setWindowFlag(Qt::FramelessWindowHint);
  //m_size_label->setAttribute(Qt::WA_NoSystemBackground);
  //m_size_label->setAttribute(Qt::WA_TranslucentBackground);
  statusBar()->addPermanentWidget(m_size_label);
  statusBar()->showMessage(tr("Ready"));
  auto availableGeometry = screen()->availableGeometry();
  resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
  move((availableGeometry.width() - width()) / 2,
    (availableGeometry.height() - height()) / 2);
}

void MainWindow::wheelEvent(QWheelEvent* event) {
  if(event->angleDelta().y() > 0) {
    zoom_in();
  } else {
    zoom_out();
  }
  QWidget::wheelEvent(event);
}

void MainWindow::resizeEvent(QResizeEvent* event) {
  m_layout_widget->resize(event->size());
  QWidget::resizeEvent(event);
}

void MainWindow::create_menu() {
  auto file_menu = menuBar()->addMenu(tr("&File"));
  m_open_action = file_menu->addAction(tr("&Open..."), this, &MainWindow::open);
  m_open_action->setShortcuts(QKeySequence::Open);
  m_refresh_action =
    file_menu->addAction(tr("&Refresh"), this, &MainWindow::refresh);
  m_refresh_action->setShortcuts(QKeySequence::Refresh);
  m_refresh_action->setEnabled(false);
  auto view_menu = menuBar()->addMenu(tr("&View"));
  m_zoom_in_action =
    view_menu->addAction(tr("Zoom &In"), this, &MainWindow::zoom_in);
  m_zoom_in_action->setShortcut(QKeySequence::ZoomIn);
  m_zoom_in_action->setEnabled(false);
  m_zoom_out_action =
    view_menu->addAction(tr("Zoom &Out"), this, &MainWindow::zoom_out);
  m_zoom_out_action->setShortcut(QKeySequence::ZoomOut);
  m_zoom_out_action->setEnabled(false);
  m_normal_size_action =
    view_menu->addAction(tr("&Normal Size"), this, &MainWindow::normal_size);
  m_normal_size_action->setShortcut(tr("Ctrl+S"));
  m_normal_size_action->setEnabled(false);
}

void MainWindow::open() {
  m_file_name = QFileDialog::getOpenFileName(this);
  refresh();
}

void MainWindow::refresh() {
  if(m_file_name.isEmpty()) {
    m_refresh_action->setEnabled(false);
    return;
  }
  if(!m_layout_widget->parse_json_file(m_file_name)) {
    m_refresh_action->setEnabled(false);
    auto message_box = QMessageBox();
    message_box.setIcon(QMessageBox::Warning);
    message_box.setText("Invalid json file.");
    message_box.exec();
  } else {
    m_refresh_action->setEnabled(true);
    normal_size();
    centralWidget()->adjustSize();
    adjustSize();
    statusBar()->showMessage(m_file_name.split("/").back());
    auto min_size = m_layout_widget->get_min_size();
    auto max_size = m_layout_widget->get_max_size();
    m_size_label->setText(QString("min: %1x%2 max: %3x%4").arg(min_size.width()).
      arg(min_size.height()).arg(max_size.width()).arg(max_size.height()));
  }
}

void MainWindow::zoom_in() {
  if(m_scale < MAX_SCALE_FACTOR) {
    m_scale += SCALE_FACTOR_STEP;
  }
  update_size();
}

void MainWindow::zoom_out() {
  if(m_scale > MIN_SCALE_FACTOR) {
    m_scale -= SCALE_FACTOR_STEP;
  }
  update_size();
}

void MainWindow::normal_size() {
  m_scale = 1.0;
  update_size();
}

void MainWindow::update_size() {
  if(m_scale != m_layout_widget->get_scale()) {
    m_layout_widget->set_scale(m_scale);
    centralWidget()->adjustSize();
    adjustSize();
  }
  m_zoom_in_action->setEnabled(m_scale < MAX_SCALE_FACTOR);
  m_zoom_out_action->setEnabled(m_scale > MIN_SCALE_FACTOR);
  m_normal_size_action->setEnabled(true);
}
