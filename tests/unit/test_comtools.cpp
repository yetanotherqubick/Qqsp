#include <QtTest>

#include "comtools.h"

class TestComtools : public QObject
{
    Q_OBJECT

private slots:
    void getHexColor_data();
    void getHexColor();
    void proceedAsPlain_data();
    void proceedAsPlain();
    void htmlizeWhitespaces_data();
    void htmlizeWhitespaces();
    void caseInsensitiveFilePath_data();
    void caseInsensitiveFilePath();
    void caseInsensitiveAbsoluteFilePath();
    void qspStrToQt();
    void wxToQColor();
};

void TestComtools::getHexColor_data()
{
    QTest::addColumn<QColor>("color");
    QTest::addColumn<QString>("expected");

    QTest::newRow("black") << QColor(0, 0, 0) << QStringLiteral("000000");
    QTest::newRow("white") << QColor(255, 255, 255) << QStringLiteral("ffffff");
    QTest::newRow("mixed") << QColor(0x12, 0xab, 0x34) << QStringLiteral("12ab34");
}

void TestComtools::getHexColor()
{
    QFETCH(QColor, color);
    QFETCH(QString, expected);
    QCOMPARE(QSPTools::GetHexColor(color), expected);
}

void TestComtools::proceedAsPlain_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("expected");

    QTest::newRow("plain") << QStringLiteral("hello world") << QStringLiteral("hello world");
    QTest::newRow("angle brackets") << QStringLiteral("<tag>") << QStringLiteral("&lt;tag&gt;");
    QTest::newRow("ampersand") << QStringLiteral("a & b") << QStringLiteral("a &amp; b");
    QTest::newRow("html characters") << QStringLiteral("<&>") << QStringLiteral("&lt;&amp;&gt;");
}

void TestComtools::proceedAsPlain()
{
    QFETCH(QString, input);
    QFETCH(QString, expected);
    QCOMPARE(QSPTools::ProceedAsPlain(input), expected);
}

void TestComtools::htmlizeWhitespaces_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("expected");

    QTest::newRow("leading spaces") << QStringLiteral("  text") << QStringLiteral("&ensp; text");
    QTest::newRow("trailing spaces") << QStringLiteral("text  ") << QStringLiteral("text &ensp;");
    QTest::newRow("newline") << QStringLiteral("one\ntwo") << QStringLiteral("one<br>two");
    QTest::newRow("carriage return") << QStringLiteral("one\r\ntwo") << QStringLiteral("one<br>two");
    QTest::newRow("tab at column one") << QStringLiteral("a\tb") << QStringLiteral("a &emsp;  b");
    QTest::newRow("tab at column zero") << QStringLiteral("\tb") << QStringLiteral("&emsp;&emsp;&emsp;&emsp;b");
    QTest::newRow("whitespace after tag") << QStringLiteral("a <b> b") << QStringLiteral("a <b>&ensp; b");
    QTest::newRow("quoted tag characters")
        << QStringLiteral("<a title=\"a>b\">x</a>")
        << QStringLiteral("<a title=\"a&gt;b\">x</a>");
    QTest::newRow("ampersand in quoted attribute")
        << QStringLiteral("<a title=\"a&b\">x</a>")
        << QStringLiteral("<a title=\"a&amp;b\">x</a>");
}

void TestComtools::htmlizeWhitespaces()
{
    QFETCH(QString, input);
    QFETCH(QString, expected);
    QCOMPARE(QSPTools::HtmlizeWhitespaces(input), expected);
}

void TestComtools::caseInsensitiveFilePath_data()
{
    QTest::addColumn<QString>("originalPath");

    QTest::newRow("normalizes backslashes") << QStringLiteral("dir\\file.txt");
    QTest::newRow("removes leading slash") << QStringLiteral("/dir/file.txt");
}

void TestComtools::caseInsensitiveFilePath()
{
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    QVERIFY(QDir(tempDir.path()).mkpath(QStringLiteral("Dir")));
    QFile file(tempDir.path() + QStringLiteral("/Dir/File.TXT"));
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.close();

    const bool oldSetting = QSPTools::useCaseInsensitiveFilePath;
    QSPTools::useCaseInsensitiveFilePath = true;

    QFETCH(QString, originalPath);
    QString expected;
    if (originalPath == QStringLiteral("dir\\file.txt"))
        expected = QStringLiteral("Dir/File.TXT");
    else
        expected = QStringLiteral("Dir/File.TXT");

    QCOMPARE(QSPTools::GetCaseInsensitiveFilePath(tempDir.path(), originalPath), expected);
    QSPTools::useCaseInsensitiveFilePath = oldSetting;
}

void TestComtools::caseInsensitiveAbsoluteFilePath()
{
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    QVERIFY(QDir(tempDir.path()).mkpath(QStringLiteral("Dir")));
    QFile file(tempDir.path() + QStringLiteral("/Dir/File.TXT"));
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.close();

    const bool oldSetting = QSPTools::useCaseInsensitiveFilePath;
    QSPTools::useCaseInsensitiveFilePath = true;

    const QString requested = tempDir.path() + QStringLiteral("/dir/file.txt");
    QCOMPARE(QSPTools::GetCaseInsensitiveAbsoluteFilePath(tempDir.path(), requested),
             tempDir.path() + QStringLiteral("/Dir/File.TXT"));

    QSPTools::useCaseInsensitiveFilePath = oldSetting;
}

void TestComtools::qspStrToQt()
{
    const QSP_CHAR text[] = u"QSP text";
    QCOMPARE(QSPTools::qspStrToQt(text), QStringLiteral("QSP text"));
    QCOMPARE(QSPTools::qspStrToQt(nullptr), QString());
}

void TestComtools::wxToQColor()
{
    QCOMPARE(QSPTools::wxtoQColor(0), QColor(Qt::black));

    const QColor converted = QSPTools::wxtoQColor(0x00112233);
    QCOMPARE(converted.red(), 0x33);
    QCOMPARE(converted.green(), 0x22);
    QCOMPARE(converted.blue(), 0x11);
}

QTEST_MAIN(TestComtools)
#include "test_comtools.moc"
