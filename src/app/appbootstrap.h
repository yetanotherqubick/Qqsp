#ifndef APPBOOTSTRAP_H
#define APPBOOTSTRAP_H

class QApplication;

namespace QspApp
{
// Application identity and interaction attributes shared by the client and
// the smoke driver; call after the application object exists, before any
// window is created.
void configureApplication(QApplication *app);
}

#endif
