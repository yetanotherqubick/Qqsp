#ifndef ROLLBACKMANAGER_H
#define ROLLBACKMANAGER_H

#include <QByteArray>
#include <QDateTime>
#include <QString>
#include <deque>

// Maintains an in-memory ring buffer of serialized QSP game states captured
// via QSPSaveGameAsData. A cursor allows bidirectional navigation: step back
// through history and then forward again if no new action has been taken.
// Capturing while the cursor is not at the newest entry truncates all
// snapshots ahead of the cursor. Call only from the main thread.
class RollbackManager
{
public:
    struct Snapshot
    {
        QByteArray data;      // raw bytes from QSPSaveGameAsData
        QDateTime timestamp;  // wall-clock time of capture
        QString label;        // human-readable description
    };

    explicit RollbackManager(int maxSnapshots = 10);

    bool captureSnapshot(const QString &label = QString());
    bool restoreAt(int index);
    bool back();
    bool forward();

    bool hasSnapshots() const;
    int snapshotCount() const;
    int maxSnapshots() const;
    int currentIndex() const;
    bool canGoBack() const;
    bool canGoForward() const;
    QString currentLabel() const;
    QString labelAt(int index) const;

    void setMaxSnapshots(int n);
    void clear();

private:
    int m_maxSnapshots;
    int m_cursor;
    std::deque<Snapshot> m_snapshots;
};

#endif
