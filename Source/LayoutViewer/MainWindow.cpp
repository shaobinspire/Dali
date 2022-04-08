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

MainWindow::MainWindow() {
  auto central_widget = new QWidget(this);
  auto layout = new QHBoxLayout(central_widget);
  m_layout_widget = new LayoutWidget();
  layout->addWidget(m_layout_widget);
  setCentralWidget(central_widget);
  create_menu();
  m_file_name_label = new QLabel();
  statusBar()->addWidget(m_file_name_label);
  m_layout_size_label = new QLabel();
  statusBar()->addPermanentWidget(m_layout_size_label);
  m_size_label = new QLabel();
  statusBar()->addPermanentWidget(m_size_label);
  statusBar()->showMessage(tr("Ready"));
  statusBar()->setStyleSheet(R"(
    QStatusBar::item {
      border: none;
    })");
  auto availableGeometry = screen()->availableGeometry();
  resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
  move((availableGeometry.width() - width()) / 2,
    (availableGeometry.height() - height()) / 2);
  setMinimumSize(0, 0);
}

void MainWindow::resizeEvent(QResizeEvent* event) {
  if(m_layout_widget->has_valid_layout()) {
    m_layout_widget->update_size(m_layout_widget->size());
    m_layout_size_label->setText(QString("Layout: %1x%2    ")
      .arg(m_layout_widget->width()).arg(m_layout_widget->height()));
  }
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
}

void MainWindow::open() {
  m_file_name = QFileDialog::getOpenFileName(this);
  refresh();
}

void MainWindow::refresh() {
  if(m_file_name.isEmpty()) {
    return;
  }
  if(!m_layout_widget->parse_json_file(m_file_name)) {
    m_refresh_action->setEnabled(false);
    auto message_box = QMessageBox();
    message_box.setIcon(QMessageBox::Warning);
    message_box.setText("Invalid json file.");
    message_box.exec();
  } else {
    m_layout_widget->setGeometry(centralWidget()->geometry());
    m_layout_widget->update_size(m_layout_widget->size());
    m_refresh_action->setEnabled(true);
    m_file_name_label->setText(m_file_name.split("/").back());
    auto min_size = m_layout_widget->get_min_size();
    auto max_size = m_layout_widget->get_max_size();
    m_layout_size_label->setText(QString("Layout Size: %1x%2    ")
      .arg(m_layout_widget->width()).arg(m_layout_widget->height()));
    m_size_label->setText(QString("Min: %1x%2  Max: %3x%4").arg(min_size.width()).
      arg(min_size.height()).arg(max_size.width()).arg(max_size.height()));
  }
}
