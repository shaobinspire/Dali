#include "LayoutViewer/MainWindow.hpp"
#include <QApplication>
#include <QDockWidget>
#include <QFileDialog>
#include <QLayout>
#include <QMenuBar>
#include <QMessageBox>
#include <QResizeEvent>
#include <QScreen>
#include <QStatusBar>
#include "LayoutViewer/LayoutWidget.hpp"

using namespace Dali;

MainWindow::MainWindow() {
  auto central_widget = new QWidget(this);
  central_widget->setStyleSheet("background-color: #F1F1F1;");
  auto layout = new QHBoxLayout(central_widget);
  m_layout_widget = new LayoutWidget();
  layout->addWidget(m_layout_widget);
  setCentralWidget(central_widget);
  create_menu();
  create_dock_windows();
  m_file_name_label = new QLabel();
  statusBar()->addWidget(m_file_name_label);
  m_layout_size_label = new QLabel();
  statusBar()->addPermanentWidget(m_layout_size_label);
  m_size_label = new QLabel();
  statusBar()->addPermanentWidget(m_size_label);
  statusBar()->showMessage(tr("Ready"));
  statusBar()->setStyleSheet(R"(
    QStatusBar {
      background-color: #D8D8D8;
    }
    QStatusBar::item {
      background-color: #D8D8D8;
      border: none;
    })");
  auto availableGeometry = screen()->availableGeometry();
  resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
  move((availableGeometry.width() - width()) / 2,
    (availableGeometry.height() - height()) / 2);
}

void MainWindow::resizeEvent(QResizeEvent* event) {
  if(m_layout_widget->has_valid_layout()) {
    m_layout_widget->update_size(m_layout_widget->size());
    update_layout_size_message();
  }
  QWidget::resizeEvent(event);
}

void MainWindow::create_dock_windows() {
  auto json_dock = new QDockWidget(tr("Json"), this);
  json_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  m_editor = new JsonEditor();
  m_editor->connect_parse_result(std::bind_front(&MainWindow::parse_result, this));
  json_dock->setWidget(m_editor);
  addDockWidget(Qt::RightDockWidgetArea, json_dock);
  m_view_menu->addAction(json_dock->toggleViewAction());
  auto error_dock = new QDockWidget(tr("Error Output"), this);
  error_dock->setAllowedAreas(Qt::BottomDockWidgetArea);
  m_error_output = new QTextEdit();
  m_error_output->setReadOnly(true);
  error_dock->setWidget(m_error_output);
  addDockWidget(Qt::BottomDockWidgetArea, error_dock);
  m_view_menu->addAction(error_dock->toggleViewAction());
}

void MainWindow::create_menu() {
  auto file_menu = menuBar()->addMenu(tr("&File"));
  m_open_action = file_menu->addAction(tr("&Open..."), this, &MainWindow::open);
  m_open_action->setShortcuts(QKeySequence::Open);
  //m_refresh_action =
  //  file_menu->addAction(tr("&Refresh"), this, &MainWindow::refresh);
  //m_refresh_action->setShortcuts(QKeySequence::Refresh);
  //m_refresh_action->setEnabled(false);
  m_view_menu = menuBar()->addMenu(tr("&View"));
}

void MainWindow::open() {
  m_file_name = QFileDialog::getOpenFileName(this);
  m_editor->load_json(m_file_name);
  m_file_name_label->setText(m_file_name.split("/").back());
}

void MainWindow::parse_result(bool is_failed) {
  if(is_failed) {
    m_error_output->setText(QString::fromStdString(m_editor->get_errors()));
  } else {
    m_error_output->setText("");
    auto layout = m_parser.parse(m_editor->get_json());
    m_layout_widget->set_layout(layout);
    m_layout_widget->setGeometry(centralWidget()->geometry().marginsRemoved(
      centralWidget()->layout()->contentsMargins()));
    m_layout_widget->update_size(m_layout_widget->size());
    update_layout_size_message();
    auto min_size = m_layout_widget->get_min_size();
    auto max_size = m_layout_widget->get_max_size();
    m_size_label->setText(QString("Min: %1x%2  Max: %3x%4").arg(min_size.width()).
      arg(min_size.height()).arg(max_size.width()).arg(max_size.height()));
  }
}

void MainWindow::update_layout_size_message() {
  m_layout_size_label->setText(QString("Layout Size: %1x%2    ")
    .arg(m_layout_widget->width()).arg(m_layout_widget->height()));
}
