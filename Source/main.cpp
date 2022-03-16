#include <QApplication>
#include <QtPlugin>
#include "LayoutWidget.h"

#ifdef _MSC_VER
  Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
#elif __linux__
Q_IMPORT_PLUGIN(QXcbIntegrationPlugin)
#endif

using namespace Dali;

int main(int argc, char** argv) {
  auto application = new QApplication(argc, argv);
  application->setOrganizationName(QObject::tr("Spire Trading Inc"));
  application->setApplicationName(QObject::tr("Dali"));
  auto widget = LayoutWidget();
  widget.parse_json_file("layout.json");
  widget.show();
  application->exec();
}