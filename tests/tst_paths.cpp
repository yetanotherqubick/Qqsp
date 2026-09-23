#include <QFile>
#include <QFileInfo>
#include <QTemporaryDir>
#include <QtTest>

#include "comtools.h"

// Regression coverage seeded by the engine-switch work (ROADMAP M5):
// - GameDirFromFilePath must derive the game DIRECTORY from the loaded .qsp
//   FILE path with FILE semantics. The previous implementation fed a directory
//   with trailing slash to QFileInfo::canonicalPath() (parent semantics), which
//   degraded the game path to the parent directory and caused slow loads,
//   missing images, and freezes.
// - The case-insensitive file cache must not thrash (rebuild) between calls
//   with the same directory, and must keep entries for previously used
//   directories.
class TestPaths : public QObject
{
    Q_OBJECT

private slots:
    void gameDirFromFile();
    void gameDirFromEmpty();
    void gameDirFromDirectoryInput();
    void gameDirFromNonExistent();
    void caseInsensitiveResolution();
    void caseInsensitiveCacheNoThrash();

private:
    static QString canonical(const QString &path);
};

QString TestPaths::canonical(const QString &path)
{
    return QFileInfo(path).canonicalFilePath();
}

void TestPaths::gameDirFromFile()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QVERIFY(QFile::copy("/etc/hostname", dir.path() + "/game.qsp"));
    QString file = canonical(dir.path()) + "/game.qsp";

    QCOMPARE(QSPTools::GameDirFromFilePath(file), canonical(dir.path()) + '/');
}

void TestPaths::gameDirFromEmpty()
{
    QCOMPARE(QSPTools::GameDirFromFilePath(QString()), QString());
}

void TestPaths::gameDirFromDirectoryInput()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    // Defensive contract: a directory input resolves to the directory itself.
    QCOMPARE(QSPTools::GameDirFromFilePath(dir.path()), canonical(dir.path()) + '/');
    QCOMPARE(QSPTools::GameDirFromFilePath(dir.path() + "/"), canonical(dir.path()) + '/');
}

void TestPaths::gameDirFromNonExistent()
{
    // A not-yet-existing file still yields its absolute parent directory.
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QString file = canonical(dir.path()) + "/missing/game.qsp";
    QCOMPARE(QSPTools::GameDirFromFilePath(file), canonical(dir.path()) + "/missing/");
}

void TestPaths::caseInsensitiveResolution()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QVERIFY(QDir().mkpath(dir.path() + "/Images"));
    QVERIFY(QFile::copy("/etc/hostname", dir.path() + "/Images/first.PNG"));

    QSPTools::useCaseInsensitiveFilePath = true;
    // Lookups are directory-relative; case-insensitive on the relative path.
    QCOMPARE(QSPTools::GetCaseInsensitiveFilePath(dir.path() + '/', "IMAGES/FIRST.PNG"),
             QString("Images/first.PNG"));
    QCOMPARE(QSPTools::GetCaseInsensitiveFilePath(dir.path() + '/', "nope.png"),
             QString("nope.png"));
}

void TestPaths::caseInsensitiveCacheNoThrash()
{
    QTemporaryDir dirA, dirB;
    QVERIFY(dirA.isValid());
    QVERIFY(dirB.isValid());
    QVERIFY(QFile::copy("/etc/hostname", dirA.path() + "/a.png"));
    QVERIFY(QFile::copy("/etc/hostname", dirB.path() + "/b.png"));

    QString gameA = canonical(dirA.path()) + '/';
    QString gameB = canonical(dirB.path()) + '/';

    QSPTools::useCaseInsensitiveFilePath = true;
    int before = QSPTools::file_cache_builds;

    QSPTools::GetCaseInsensitiveFilePath(gameA, "a.png");
    int afterA = QSPTools::file_cache_builds;
    QCOMPARE(afterA, before + 1); // one build for a new directory

    QSPTools::GetCaseInsensitiveFilePath(gameA, "a.png");
    QCOMPARE(QSPTools::file_cache_builds, afterA); // repeated use: no rebuild

    QSPTools::GetCaseInsensitiveFilePath(gameB, "b.png");
    int afterB = QSPTools::file_cache_builds;
    QCOMPARE(afterB, afterA + 1); // one build for the other directory

    // Returning to the first directory must reuse its cached entry instead of
    // rescanning (the pre-fix behavior rebuilt it on every alternation).
    QCOMPARE(QSPTools::GetCaseInsensitiveFilePath(gameA, "a.png"), QString("a.png"));
    QCOMPARE(QSPTools::GetCaseInsensitiveFilePath(gameB, "b.png"), QString("b.png"));
    QCOMPARE(QSPTools::file_cache_builds, afterB);
}

QTEST_MAIN(TestPaths)
#include "tst_paths.moc"
