#include <QtTest>

#include "qsptextbox.h"

class TestTextBox : public QObject
{
    Q_OBJECT

private slots:
    void defaults();
    void setText();
    void htmlMode();
    void showPlainText();
    void colors();
    void linkColor();
    void font();
    void gamePath();
};

void TestTextBox::defaults()
{
    QspTextBox box;

    QCOMPARE(box.GetText(), QString());
    QCOMPARE(box.GetTextFont(), box.font());
    QVERIFY(box.GetLinkColor() != QColor());
}

void TestTextBox::setText()
{
    QspTextBox box;
    const QString text = QStringLiteral("Hello\nWorld");

    box.SetText(text);

    QCOMPARE(box.GetText(), text);
    QCOMPARE(box.toPlainText(), text);
}

void TestTextBox::htmlMode()
{
    QspTextBox box;
    box.SetIsHtml(true);
    box.SetText(QStringLiteral("<b>Hello</b><br>World"));

    QCOMPARE(box.GetText(), QStringLiteral("<b>Hello</b><br>World"));
    QCOMPARE(box.toPlainText(), QStringLiteral("Hello\nWorld"));
    QVERIFY(box.toHtml().contains(QStringLiteral("<b>")));
}

void TestTextBox::showPlainText()
{
    QspTextBox box;
    box.SetIsHtml(true);
    box.SetShowPlainText(true);
    box.SetText(QStringLiteral("<b>Hello</b>"));

    QCOMPARE(box.toPlainText(), QStringLiteral("<b>Hello</b>"));
}

void TestTextBox::colors()
{
    QspTextBox box;
    const QColor background(10, 20, 30);
    const QColor foreground(200, 210, 220);

    QVERIFY(box.SetBackgroundColor(background));
    QCOMPARE(box.GetBackgroundColor(), background);
    QVERIFY(!box.SetBackgroundColor(background));

    QVERIFY(box.SetForegroundColor(foreground));
    QCOMPARE(box.GetForegroundColor(), foreground);
    QVERIFY(!box.SetForegroundColor(foreground));
}

void TestTextBox::linkColor()
{
    QspTextBox box;
    const QColor color(20, 40, 60);

    QVERIFY(box.SetLinkColor(color));
    QCOMPARE(box.GetLinkColor(), color);
    QVERIFY(!box.SetLinkColor(color));
    QVERIFY(box.document()->defaultStyleSheet().contains(color.name()));
}

void TestTextBox::font()
{
    QspTextBox box;
    QFont font = box.font();
    font.setPointSize(font.pointSize() + 2);

    box.SetTextFont(font);

    QCOMPARE(box.GetTextFont(), font);
    QCOMPARE(box.font(), font);
}

void TestTextBox::gamePath()
{
    QspTextBox box;
    const QString path = QStringLiteral("/tmp/qsp-game/");

    box.SetGamePath(path);

    QCOMPARE(box.searchPaths(), QStringList() << path);
}

QTEST_MAIN(TestTextBox)
#include "test_textbox.moc"
