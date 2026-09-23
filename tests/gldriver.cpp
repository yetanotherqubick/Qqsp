#include <QApplication>
#include <QTimer>
#include <cstdio>
#include "mainwindow.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    MainWindow w;
    w.show();
    QTimer::singleShot(2000, [&] {
        printf("DIAG: opening game\n"); fflush(stdout);
        w.OpenGameFile(QString::fromUtf8(argv[1]));
    });
    QTimer::singleShot(15000, [&] {
        printf("DIAG: executing first action\n"); fflush(stdout);
        QSPSetSelActionIndex(0, QSP_TRUE);
        QSPExecuteSelActionCode(QSP_TRUE);
    });
    QTimer::singleShot(40000, [&] {
        printf("DIAG: quit\n"); fflush(stdout);
        app.quit();
    });
    return app.exec();
}
