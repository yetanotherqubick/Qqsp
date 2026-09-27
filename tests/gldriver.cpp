#include <QApplication>
#include <QMetaObject>
#include <QTimer>
#include <cstdio>
#include "appbootstrap.h"
#include "mainwindow.h"
#ifdef _WEBBOX
#include "url_schemes.h"
#endif

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QspApp::configureApplication(&app);
#ifdef _WEBBOX
    register_url_schemes();
#endif
    if (argc < 2)
    {
        printf("usage: gldriver <game.qsp>\n");
        return 2;
    }

    MainWindow w;
    w.show();

    int openMs = qEnvironmentVariableIntValue("GLDRIVER_OPEN_MS");
    if (openMs <= 0) openMs = 2000;
    int actMs = qEnvironmentVariableIntValue("GLDRIVER_ACTION_MS");
    if (actMs <= 0) actMs = 15000;
    int quitMs = qEnvironmentVariableIntValue("GLDRIVER_QUIT_MS");
    if (quitMs <= 0) quitMs = 40000;

    QTimer::singleShot(openMs, [&] {
        printf("DIAG: opening game\n"); fflush(stdout);
        w.OpenGameFile(QString::fromUtf8(argv[1]));
        printf("DIAG: game opened=%d\n", (int)w.isGameOpened()); fflush(stdout);
    });
    QTimer::singleShot(actMs, [&] {
        printf("DIAG: executing first action\n"); fflush(stdout);
        QSPSetSelActionIndex(0, QSP_TRUE);
        QSPExecuteSelActionCode(QSP_TRUE);
    });
    QTimer::singleShot(quitMs, [&] {
        printf("DIAG: quit\n"); fflush(stdout);
        app.quit();
    });
    int rbMs = qEnvironmentVariableIntValue("GLDRIVER_ROLLBACK_MS");
    if (rbMs > 0)
    {
        QTimer::singleShot(rbMs, [&] {
            printf("DIAG: rollback step back\n"); fflush(stdout);
            QMetaObject::invokeMethod(&w, "OnRollbackStepBack");
        });
    }
    return app.exec();
}
