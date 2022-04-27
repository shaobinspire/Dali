#define JSON_DIAGNOSTICS 1

#include <QApplication>
#include <QtPlugin>
#include "LayoutViewer/MainWindow.hpp"

#ifdef _MSC_VER
  Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
  Q_IMPORT_PLUGIN(QWindowsVistaStylePlugin) 
#elif __linux__
  Q_IMPORT_PLUGIN(QXcbIntegrationPlugin)
#endif

using namespace Dali;

int main(int argc, char** argv) {
  Q_INIT_RESOURCE(Resources);
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
    Qt::HighDpiScaleFactorRoundingPolicy::Floor);
  QGuiApplication::setApplicationDisplayName(QObject::tr("Dali Layout Viewer"));
  auto application = new QApplication(argc, argv);
  application->setOrganizationName(QObject::tr("Spire Trading Inc"));
  application->setApplicationName(QObject::tr("Dali Layout Viewer"));
  MainWindow window;
  window.show();
  application->exec();
}
