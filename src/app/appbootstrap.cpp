#include "appbootstrap.h"

#include <QApplication>

void QspApp::configureApplication(QApplication *app)
{
    app->setApplicationName("Qqsp");
    app->setOrganizationName("Qqsp");
    app->setApplicationVersion("1.9");
    app->setDoubleClickInterval(1);
}
