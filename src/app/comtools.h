#ifndef TOOLS_H
#define TOOLS_H

#include <QColor>
#include <QHash>
#include <QString>
#include <qsp_default.h>

class QSPTools
{
public:
    static QString GetHexColor(const QColor color);
    static QString HtmlizeWhitespaces(const QString &str);
    static QString ProceedAsPlain(const QString &str);
    static QString GetAppPath();
    static QString GetCaseInsensitiveFilePath(QString searchDir, QString originalPath);
    static QString GetCaseInsensitiveAbsoluteFilePath(QString searchDir, QString originalPath);
    static QString GameDirFromFilePath(const QString &filePath);
    static QString qspStrToQt(const QSP_CHAR *str);
    static QColor wxtoQColor(int wxColor);

    static bool useCaseInsensitiveFilePath;

    // Test instrumentation: number of times a directory file list was built.
    static int file_cache_builds;

private:
    // Case-insensitive lookup tables, keyed by the scanned directory. Entries
    // are evicted (all at once) when more than a few directories are seen, so
    // memory stays bounded without thrashing between the game directory and
    // its parent.
    static QHash<QString, QHash<QString, QString>> file_lists;
};

#endif
