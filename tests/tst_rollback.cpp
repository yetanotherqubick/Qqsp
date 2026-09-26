#include <QtTest>

#include <qsp_default.h>

#include "rollbackmanager.h"

// RollbackManager coverage (ROADMAP M10): capture buffer growth against the
// QSPSaveGameAsData size contract, cursor/divergence semantics, ring-buffer
// eviction, and real save/restore roundtrips on the minimal fixture.

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

int varNum(const QString &name)
{
    int num = 0;
    QSP_CHAR *str = nullptr;
    QSPGetVarValues(qspStringFromQString(name), 0, &num, &str);
    return num;
}

QString varStr(const QString &name)
{
    int num = 0;
    QSP_CHAR *str = nullptr;
    if (!QSPGetVarValues(qspStringFromQString(name), 0, &num, &str))
        return QString();
    return strToQt(str);
}
} // namespace

class TestRollback : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();
    void captureGrowsBuffer();
    void backForwardCursor();
    void divergenceTruncatesForwardHistory();
    void depthEviction();
    void clearResetsHistory();
    void saveLoadRoundtrip();

private:
    RollbackManager m_rollback;
};

void TestRollback::init()
{
    m_rollback = RollbackManager(10);
    QSPInit();
    QString fixture = QSP_TEST_FIXTURE;
    QVERIFY2(QSPLoadGameWorldFromFile(qspStringFromQString(fixture), QSP_FALSE), "fixture failed to load");
    QVERIFY2(QSPRestartGame(QSP_TRUE), "restart failed");
}

void TestRollback::cleanup()
{
    QSPTerminate();
}

void TestRollback::captureGrowsBuffer()
{
    // The initial 64 KiB buffer is far below what the engine reports needing;
    // captureSnapshot must grow and still produce a non-empty snapshot.
    QVERIFY(m_rollback.captureSnapshot("first"));
    QCOMPARE(m_rollback.snapshotCount(), 1);
    QVERIFY(!m_rollback.currentLabel().isEmpty());
}

void TestRollback::backForwardCursor()
{
    QVERIFY(QSPExecString(qspStringFromQString("step = 1"), QSP_FALSE));
    QVERIFY(m_rollback.captureSnapshot("after step 1"));
    QVERIFY(QSPExecString(qspStringFromQString("step = 2"), QSP_FALSE));
    QVERIFY(m_rollback.captureSnapshot("after step 2"));

    QCOMPARE(m_rollback.snapshotCount(), 2);
    QCOMPARE(m_rollback.currentIndex(), 1);
    QVERIFY(m_rollback.canGoBack());
    QVERIFY(!m_rollback.canGoForward());

    QVERIFY(m_rollback.back());
    QCOMPARE(m_rollback.currentIndex(), 0);
    QVERIFY(m_rollback.canGoForward());

    QVERIFY(m_rollback.forward());
    QCOMPARE(m_rollback.currentIndex(), 1);
    QVERIFY(!m_rollback.canGoForward());
}

void TestRollback::divergenceTruncatesForwardHistory()
{
    QVERIFY(m_rollback.captureSnapshot("one"));
    QVERIFY(m_rollback.captureSnapshot("two"));
    QVERIFY(m_rollback.back());
    QCOMPARE(m_rollback.currentIndex(), 0);

    // Acting on a restored state truncates the forward branch.
    QVERIFY(m_rollback.captureSnapshot("branch"));
    QCOMPARE(m_rollback.snapshotCount(), 2);
    QCOMPARE(m_rollback.currentLabel(), QString("branch"));
    QVERIFY(!m_rollback.canGoForward());
}

void TestRollback::depthEviction()
{
    m_rollback.setMaxSnapshots(2);
    QVERIFY(m_rollback.captureSnapshot("one"));
    QVERIFY(m_rollback.captureSnapshot("two"));
    QCOMPARE(m_rollback.snapshotCount(), 2);
    QVERIFY(m_rollback.captureSnapshot("three"));
    QCOMPARE(m_rollback.snapshotCount(), 2);
    QCOMPARE(m_rollback.currentLabel(), QString("three"));
}

void TestRollback::clearResetsHistory()
{
    QVERIFY(m_rollback.captureSnapshot("only"));
    m_rollback.clear();
    QCOMPARE(m_rollback.snapshotCount(), 0);
    QVERIFY(!m_rollback.hasSnapshots());
    QCOMPARE(m_rollback.currentIndex(), -1);
}

void TestRollback::saveLoadRoundtrip()
{
    QVERIFY(QSPExecString(qspStringFromQString("$text = 'hello'"), QSP_FALSE));
    QVERIFY(QSPExecString(qspStringFromQString("counter = 42"), QSP_FALSE));

    int bufSize = 64 * 1024;
    int realSize = 0;
    QSP_CHAR *buf = static_cast<QSP_CHAR *>(std::malloc(bufSize * sizeof(QSP_CHAR)));
    while (!QSPSaveGameAsData(buf, bufSize, &realSize, QSP_FALSE))
    {
        std::free(buf);
        if (realSize <= bufSize)
        {
            std::free(buf);
            QFAIL("save serialization failed");
        }
        bufSize = realSize;
        buf = static_cast<QSP_CHAR *>(std::malloc(bufSize * sizeof(QSP_CHAR)));
    }
    const QByteArray saved(reinterpret_cast<const char *>(buf), realSize * sizeof(QSP_CHAR));
    std::free(buf);

    // Mutate the state, then restore and verify the pre-save values return.
    QVERIFY(QSPExecString(qspStringFromQString("$text = 'changed'"), QSP_FALSE));
    QVERIFY(QSPExecString(qspStringFromQString("counter = 0"), QSP_FALSE));
    QCOMPARE(varNum("counter"), 0);

    std::vector<QSP_CHAR> text;
    text.reserve(saved.size() / static_cast<int>(sizeof(QSP_CHAR)) + 1);
    for (int i = 0; i + 1 < saved.size(); i += 2)
        text.push_back(static_cast<QSP_CHAR>(static_cast<unsigned char>(saved[i])
            | (static_cast<unsigned char>(saved[i + 1]) << 8)));
    text.push_back(0);
    QVERIFY2(QSPOpenSavedGameFromData(text.data(), QSP_FALSE), "restore failed");

    QCOMPARE(varNum("counter"), 42);
    QCOMPARE(varStr("$text"), QString("hello"));
}

QTEST_MAIN(TestRollback)
#include "tst_rollback.moc"
