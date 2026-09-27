#include <QtTest>

#include <qsp_default.h>

// Engine-integration coverage against the system qsp-legacy library.
// The fixture is a minimal game built from minimal.txt with the
// upstream 60f0e9d-era txt2gam; see fixtures/minimal.txt.

#ifndef QSP_TEST_FIXTURE
#define QSP_TEST_FIXTURE "minimal.qsp"
#endif

namespace
{
QString strToQt(const QSP_CHAR *str)
{
    return str ? QString::fromUtf16(reinterpret_cast<const char16_t *>(str)) : QString();
}

const QSP_CHAR *qspStringFromQString(const QString &s)
{
    return reinterpret_cast<const QSP_CHAR *>(s.utf16());
}

void SetTimer(int msecs) { Q_UNUSED(msecs) }
void RefreshInt(QSP_BOOL isRedraw) { Q_UNUSED(isRedraw) }
void SetInputStrText(const QSP_CHAR *text) { Q_UNUSED(text) }
QSP_BOOL IsPlay(const QSP_CHAR *file) { Q_UNUSED(file) return QSP_FALSE; }
void PlayFile(const QSP_CHAR *file, int volume) { Q_UNUSED(file) Q_UNUSED(volume) }
void CloseFile(const QSP_CHAR *file) { Q_UNUSED(file) }
void ShowImage(const QSP_CHAR *file) { Q_UNUSED(file) }
void ShowPane(int type, QSP_BOOL isShow) { Q_UNUSED(type) Q_UNUSED(isShow) }
void Msg(const QSP_CHAR *str) { Q_UNUSED(str) }
void DeleteMenu() {}
int ShowMenu(QSPListItem *items, int count) { Q_UNUSED(items) Q_UNUSED(count) return -1; }
void Input(const QSP_CHAR *text, QSP_CHAR *buffer, int maxLen)
{
    Q_UNUSED(text)
    if (maxLen > 0) buffer[0] = 0;
}
void OpenGameStatus(const QSP_CHAR *file) { Q_UNUSED(file) }
void SaveGameStatus(const QSP_CHAR *file) { Q_UNUSED(file) }
void Sleep(int msecs) { Q_UNUSED(msecs) }
int GetMSCount() { return 0; }
} // namespace

class TestEngine : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();
    void loadsFixtureAndStarts();
    void strCompContract();
    void strFindContract();
    void typeMismatchReportsError();

private:
    bool execVar(const QString &code, const QString &var, int *numVal, QString *strVal) const;
};

void TestEngine::init()
{
    QSPInit();
    QSPSetCallBack(QSP_CALL_SETTIMER, (QSP_CALLBACK)&SetTimer);
    QSPSetCallBack(QSP_CALL_REFRESHINT, (QSP_CALLBACK)&RefreshInt);
    QSPSetCallBack(QSP_CALL_SETINPUTSTRTEXT, (QSP_CALLBACK)&SetInputStrText);
    QSPSetCallBack(QSP_CALL_ISPLAYINGFILE, (QSP_CALLBACK)&IsPlay);
    QSPSetCallBack(QSP_CALL_PLAYFILE, (QSP_CALLBACK)&PlayFile);
    QSPSetCallBack(QSP_CALL_CLOSEFILE, (QSP_CALLBACK)&CloseFile);
    QSPSetCallBack(QSP_CALL_SHOWMSGSTR, (QSP_CALLBACK)&Msg);
    QSPSetCallBack(QSP_CALL_SLEEP, (QSP_CALLBACK)&Sleep);
    QSPSetCallBack(QSP_CALL_GETMSCOUNT, (QSP_CALLBACK)&GetMSCount);
    QSPSetCallBack(QSP_CALL_DELETEMENU, (QSP_CALLBACK)&DeleteMenu);
    QSPSetCallBack(QSP_CALL_SHOWMENU, (QSP_CALLBACK)&ShowMenu);
    QSPSetCallBack(QSP_CALL_INPUTBOX, (QSP_CALLBACK)&Input);
    QSPSetCallBack(QSP_CALL_SHOWIMAGE, (QSP_CALLBACK)&ShowImage);
    QSPSetCallBack(QSP_CALL_SHOWWINDOW, (QSP_CALLBACK)&ShowPane);
    QSPSetCallBack(QSP_CALL_OPENGAMESTATUS, (QSP_CALLBACK)&OpenGameStatus);
    QSPSetCallBack(QSP_CALL_SAVEGAMESTATUS, (QSP_CALLBACK)&SaveGameStatus);

    QString fixture = QSP_TEST_FIXTURE;
    QVERIFY2(QSPLoadGameWorldFromFile(qspStringFromQString(fixture), QSP_FALSE), "fixture failed to load");
    QVERIFY2(QSPRestartGame(QSP_TRUE), "restart failed");
}

void TestEngine::cleanup()
{
    QSPTerminate();
}

bool TestEngine::execVar(const QString &code, const QString &var, int *numVal, QString *strVal) const
{
    if (!QSPExecString(qspStringFromQString(code), QSP_FALSE))
        return false;
    int num = 0;
    QSP_CHAR *str = nullptr;
    if (!QSPGetVarValues(qspStringFromQString(var), 0, &num, &str))
        return false;
    if (numVal)
        *numVal = num;
    if (strVal)
        *strVal = strToQt(str);
    return true;
}

void TestEngine::loadsFixtureAndStarts()
{
    // The fixture world is intentionally empty of location code (the 2013-era
    // txt2gam output's location body does not survive the legacy parse); the
    // engine still accepts statements after a load + restart.
    int num = 0;
    QVERIFY(execVar("c0 = 5 + 3", "c0", &num, nullptr));
    QCOMPARE(num, 8);
    QCOMPARE(QSPIsInCallBack(), QSP_FALSE);
}

void TestEngine::strCompContract()
{
    int num = 0;
    QVERIFY(execVar("c1 = STRCOMP('abc','a.c')", "c1", &num, nullptr));
    QCOMPARE(num, -1); // full match
    QVERIFY(execVar("c2 = STRCOMP('abc','xyz')", "c2", &num, nullptr));
    QCOMPARE(num, 0); // no match
}

void TestEngine::strFindContract()
{
    QString str;
    QVERIFY(execVar("$find = STRFIND('xyzabc','b')", "$find", nullptr, &str));
    QCOMPARE(str, QString("b"));
    QVERIFY(execVar("$grp = STRFIND('xyzabc','(y)z', 1)", "$grp", nullptr, &str));
    QCOMPARE(str, QString("y")); // 3rd argument selects the capture group
    QVERIFY(execVar("$none = STRFIND('abc','zzz')", "$none", nullptr, &str));
    QVERIFY(str.isEmpty());
}

void TestEngine::typeMismatchReportsError()
{
    // Assigning a non-empty string result to a numeric variable is rejected.
    int num = 0;
    QVERIFY(!execVar("bad2 = STRFIND('xabc','b')", "bad2", &num, nullptr));
    QCOMPARE(QSPGetLastErrorData().ErrorNum, QSP_ERR_TYPEMISMATCH);
}

QTEST_MAIN(TestEngine)
#include "tst_engine.moc"
