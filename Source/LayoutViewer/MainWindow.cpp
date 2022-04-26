#include "LayoutViewer/MainWindow.hpp"
#include <QApplication>
#include <QDockWidget>
#include <QEvent>
#include <QFileDialog>
#include <QLayout>
#include <QMenuBar>
#include <QMessageBox>
#include <QResizeEvent>
#include <QSaveFile>
#include <QScreen>
#include <QStatusBar>
#include <QTextStream>
#include <QToolBar>
#include "Dali/Layout.hpp"
#include "LayoutViewer/LayoutWidget.hpp"

using namespace Dali;

MainWindow::MainWindow() {
  auto central_widget = new QWidget(this);
  central_widget->setStyleSheet("background-color: #F1F1F1;");
  //central_widget->setStyleSheet("background-color: red;");
  central_widget->installEventFilter(this);
  auto layout = new QHBoxLayout(central_widget);
  m_layout_widget = new LayoutWidget();
  //m_layout_widget->setStyleSheet("background-color: green;");
  layout->addWidget(m_layout_widget);
  setCentralWidget(central_widget);
  create_menu();
  create_dock_windows();
  create_size_setting_tool_bar();
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

bool MainWindow::eventFilter(QObject* watched, QEvent* event) {
  if(watched == centralWidget() && event->type() == QEvent::Resize) {
    update_layout_size_message();
    if(auto status = m_layout_widget->get_layout_status();
      status != Layout::Status::NONE) {
      auto message = QString("The layout is %1.");
      if(status == Layout::Status::LAYOUT_OVERFLOW) {
        message = message.arg("overflow");
      } else {
        message = message.arg("underflow");
      }
      m_error_output->setText(message);
    } else if(!m_error_output->toPlainText().isEmpty()) {
      m_error_output->clear();
    }
  }
  return QWidget::eventFilter(watched, event);
}

void MainWindow::closeEvent(QCloseEvent* event) {
  if(maybe_save()) {
    event->accept();
  } else {
    event->ignore();
  }
}

void MainWindow::create_dock_windows() {
  auto json_dock = new QDockWidget(tr("Json Editor"), this);
  json_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  m_editor = new JsonEditor();
  m_editor->setMinimumWidth(300);
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
  auto file_tool_bar = addToolBar(tr("File"));
  auto open_icon = QIcon::fromTheme("document-open", QIcon(":/images/open.png"));
  auto open_action =
    file_menu->addAction(open_icon, tr("&Open..."), this, &MainWindow::open);
  open_action->setShortcuts(QKeySequence::Open);
  file_tool_bar->addAction(open_action);
  auto save_icon = QIcon::fromTheme("document-save", QIcon(":/images/save.png"));
  auto save_action =
    file_menu->addAction(save_icon, tr("&Save"), this, &MainWindow::save);
  save_action->setShortcuts(QKeySequence::Save);
  file_tool_bar->addAction(save_action);
  auto save_as_action =
    file_menu->addAction(tr("&Save &As..."), this, &MainWindow::save);
  save_as_action->setShortcuts(QKeySequence::SaveAs);
  auto refresh_icon =
    QIcon::fromTheme("document-refresh", QIcon(":/images/refresh.png"));
  m_refresh_action =
    file_menu->addAction(refresh_icon, tr("&Refresh"), this, &MainWindow::refresh);
  m_refresh_action->setShortcuts(QKeySequence::Refresh);
  file_tool_bar->addAction(m_refresh_action);
  m_refresh_action->setEnabled(false);
  m_view_menu = menuBar()->addMenu(tr("&View"));
  auto zoom_menu = menuBar()->addMenu(tr("&Zoom"));
  auto zoom_tool_bar = addToolBar(tr("Zoom"));
  auto reset_zoom_icon = QIcon(":/images/resetzoom.png");
  m_show_original_action =
    zoom_menu->addAction(reset_zoom_icon, tr("&Show Original"), this, &MainWindow::show_original);
  zoom_tool_bar->addAction(m_show_original_action);
  m_show_original_action->setEnabled(false);
}

void MainWindow::create_size_setting_tool_bar() {
  auto size_setting_tool_bar = addToolBar(tr("Size"));
  size_setting_tool_bar->setAllowedAreas(Qt::TopToolBarArea);
  size_setting_tool_bar->addWidget(new QLabel(tr("Width:")));
  m_width_spin_box = new QSpinBox();
  m_width_spin_box->setRange(0, MAX_LAYOUT_SIZE);
  connect(m_width_spin_box, QOverload<int>::of(&QSpinBox::valueChanged),
    [=] (auto value) {
      m_layout_widget->resize(QSize(value, m_layout_widget->size().height()));
    });
  size_setting_tool_bar->addWidget(m_width_spin_box);
  size_setting_tool_bar->addWidget(new QLabel(("  ")));
  size_setting_tool_bar->addWidget(new QLabel(tr("Height:")));
  m_height_spin_box = new QSpinBox();
  m_height_spin_box->setRange(0, MAX_LAYOUT_SIZE);
  connect(m_height_spin_box, QOverload<int>::of(&QSpinBox::valueChanged),
    [=] (auto value) {
      m_layout_widget->resize(QSize(m_layout_widget->size().width(), value));
    });
  size_setting_tool_bar->addWidget(m_height_spin_box);
}

void MainWindow::open() {
  if(!maybe_save()) {
    return;
  }
  auto file_name = QFileDialog::getOpenFileName(this);
  if(file_name.isEmpty()) {
    return;
  }
  m_file_name = file_name;
  m_editor->load_json(m_file_name);
  m_file_name_label->setText(m_file_name.split("/").back());
}

void MainWindow::refresh() {
  m_editor->load_json(m_file_name);
}

bool MainWindow::save() {
  if(m_file_name.isEmpty()) {
    return save_as();
  }
  return save_file(m_file_name);
}

bool MainWindow::save_as() {
  auto dialog = QFileDialog(this);
  dialog.setWindowModality(Qt::WindowModal);
  dialog.setAcceptMode(QFileDialog::AcceptSave);
  dialog.setNameFilter("*.json");
  if(dialog.exec() != QDialog::Accepted) {
    return false;
  }
  return save_file(dialog.selectedFiles().first());
}

void MainWindow::show_original() {
  if(!maybe_save()) {
    return;
  }
  m_error_output->setText("");
  auto layout = m_parser.parse(m_editor->get_json());
  m_layout_widget->set_layout(layout);
  m_layout_widget->show_original_layout(true);
  adjustSize();
  m_layout_widget->update();
  m_layout_widget->show_original_layout(false);
}

bool MainWindow::save_file(const QString& file_name) {
  auto error_message = QString();
  QGuiApplication::setOverrideCursor(Qt::WaitCursor);
  auto file = QSaveFile(file_name);
  if(file.open(QFile::WriteOnly | QFile::Text)) {
    auto out = QTextStream(&file);
    out << m_editor->toPlainText();
    if(!file.commit()) {
      error_message = tr("Cannot write file %1:\n%2.")
        .arg(QDir::toNativeSeparators(file_name), file.errorString());
    }
  } else {
    error_message = tr("Cannot open file %1 for writing:\n%2.")
      .arg(QDir::toNativeSeparators(file_name), file.errorString());
  }
  QGuiApplication::restoreOverrideCursor();
  if(!error_message.isEmpty()) {
    m_error_output->setText(error_message);
    return false;
  }
  m_file_name = file_name;
  m_editor->document()->setModified(false);
  statusBar()->showMessage(tr("File saved"), 2000);
  return true;
}

bool MainWindow::maybe_save() {
  if(!m_editor->document()->isModified()) {
    return true;
  }
  auto result = QMessageBox::warning(this, tr("Application"),
    tr("The Json has been modified.\n Do you want to save your changes?"),
    QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
  if(result == QMessageBox::Save) {
    return save();
  } else if(result == QMessageBox::Cancel) {
    return false;
  }
  return true;
}

void MainWindow::parse_result(bool is_failed) {
  if(is_failed) {
    m_refresh_action->setEnabled(false);
    m_show_original_action->setEnabled(true);
    m_error_output->setText(QString::fromStdString(m_editor->get_errors()));
    m_layout_widget->set_layout(nullptr);
    m_layout_size_label->setText("");
    m_size_label->setText("");
  } else {
    m_refresh_action->setEnabled(true);
    m_show_original_action->setEnabled(true);
    m_error_output->setText("");
    auto layout = m_parser.parse(m_editor->get_json());
    m_layout_widget->set_layout(layout);
    m_layout_widget->adjust_size();
    update_layout_size_message();
    auto min_size = m_layout_widget->get_min_size();
    auto max_size = m_layout_widget->get_max_size();
    m_size_label->setText(QString("Min: %1x%2  Max: %3x%4").arg(min_size.width()).
      arg(min_size.height()).arg(max_size.width()).arg(max_size.height()));
  }
  m_layout_widget->update();
}

void MainWindow::update_layout_size_message() {
  auto rect = m_layout_widget->get_layout_rect();
  m_layout_size_label->setText(QString("Layout Size: %1x%2    ")
    .arg(rect.width()).arg(rect.height()));
  m_width_spin_box->blockSignals(true);
  m_width_spin_box->setValue(rect.width());
  m_height_spin_box->blockSignals(true);
  m_height_spin_box->setValue(rect.height());
}
