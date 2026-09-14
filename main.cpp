#include "mainwindow.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QFileInfo>
#include <QLocale>
#include <QObject>
#include <QSettings>
#include <QString>
#include <QTranslator>
#ifdef _WEBBOX
#include "url_schemes.h"

#include <QtWebEngine>
#endif

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
    a.setApplicationName("Qqsp");
    a.setOrganizationName("Qqsp");
    a.setApplicationVersion("1.9");
    a.setDoubleClickInterval(1);

    QObject::tr("__LANGNAME__");
    QObject::tr("__LANGID__");

    QString langid;
    QFileInfo settingsFile(QApplication::applicationDirPath() + "/" + QSP_CUSTOM_CONFIG);
    if (settingsFile.exists() && settingsFile.isFile())
    {
        QSettings settings(QApplication::applicationDirPath() + "/" + QSP_CUSTOM_CONFIG, QSettings::IniFormat);
        langid = settings.value("application/language", QLocale::system().name()).toString();
    }
    else
    {
        QSettings settings(QSettings::IniFormat, QSettings::UserScope, QApplication::organizationName(), QApplication::applicationName());
        langid = settings.value("application/language", QLocale::system().name()).toString();
    }
    QTranslator qtTranslator;

    if (qtTranslator.load(QApplication::applicationName() + "." + langid, QApplication::applicationDirPath()))
        a.installTranslator(&qtTranslator);
    else if (qtTranslator.load(QApplication::applicationName() + "." + langid, ":/translations/"))
        a.installTranslator(&qtTranslator);

    QCommandLineParser parser;
    parser.setApplicationDescription("Qqsp");
    parser.addPositionalArgument("file", QCoreApplication::translate("main", "Game file to open."));
    parser.addHelpOption();
    parser.addVersionOption();
    parser.process(a);

#ifdef _WEBBOX
    register_url_schemes();
    QtWebEngine::initialize();
#endif

    MainWindow w;

    if (parser.positionalArguments().size() != 0)
    {
        QFileInfo file(parser.positionalArguments().at(0));
        w.OpenGameFile(file.filePath());
    }

    w.show();

    return a.exec();
}
