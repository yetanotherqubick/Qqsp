#include "rollbackmanager.h"

#include <algorithm>
#include <cstdlib>
#include <vector>

#include <qsp_default.h>

RollbackManager::RollbackManager(int maxSnapshots)
    : m_maxSnapshots(qMax(1, maxSnapshots))
    , m_cursor(-1)
{
}

bool RollbackManager::captureSnapshot(const QString &label, bool isChoice)
{
    if (m_maxSnapshots <= 0)
        return false;
    // The engine copies the serialized state into a caller-provided buffer and
    // reports the required size when it does not fit; grow until it does.
    int bufSize = 64 * 1024;
    QSP_CHAR *buf = static_cast<QSP_CHAR *>(std::malloc(bufSize * sizeof(QSP_CHAR)));
    if (!buf)
        return false;

    int realSize = 0;
    while (!QSPSaveGameAsData(buf, bufSize, &realSize, QSP_FALSE))
    {
        std::free(buf);
        if (realSize <= bufSize)
            return false;
        bufSize = realSize;
        buf = static_cast<QSP_CHAR *>(std::malloc(bufSize * sizeof(QSP_CHAR)));
        if (!buf)
            return false;
    }

    Snapshot snap;
    snap.data = QByteArray(reinterpret_cast<const char *>(buf), realSize * sizeof(QSP_CHAR));
    snap.timestamp = QDateTime::currentDateTime();
    snap.label = label;
    snap.choice = isChoice;

    std::free(buf);

    if (m_cursor >= 0 && m_cursor < static_cast<int>(m_snapshots.size()) - 1)
        m_snapshots.erase(m_snapshots.begin() + m_cursor + 1, m_snapshots.end());

    while (static_cast<int>(m_snapshots.size()) >= m_maxSnapshots)
    {
        m_snapshots.pop_front();
        if (m_cursor > 0)
            --m_cursor;
    }

    m_snapshots.push_back(std::move(snap));
    m_cursor = static_cast<int>(m_snapshots.size()) - 1;
    return true;
}

bool RollbackManager::restoreAt(int index)
{
    if (index < 0 || index >= static_cast<int>(m_snapshots.size()))
        return false;

    const Snapshot &snap = m_snapshots[index];
    // QSPOpenSavedGameFromData consumes a NUL-terminated UTF-16 string.
    std::vector<QSP_CHAR> text;
    text.reserve(snap.data.size() / static_cast<int>(sizeof(QSP_CHAR)) + 1);
    const char *raw = snap.data.constData();
    for (int i = 0; i + 1 < snap.data.size(); i += 2)
        text.push_back(static_cast<QSP_CHAR>(static_cast<unsigned char>(raw[i])
            | (static_cast<unsigned char>(raw[i + 1]) << 8)));
    text.push_back(0);

    if (QSPOpenSavedGameFromData(text.data(), QSP_FALSE))
    {
        m_cursor = index;
        return true;
    }
    return false;
}

bool RollbackManager::back()
{
    if (!canGoBack())
        return false;
    // Land on the nearest earlier choice snapshot; timer-driven transitions
    // in between are skipped so one step undoes one player decision.
    int target = m_cursor - 1;
    while (target > 0 && !m_snapshots[target].choice)
        --target;
    return restoreAt(target);
}

bool RollbackManager::forward()
{
    if (!canGoForward())
        return false;
    // Land on the nearest later choice snapshot; timer-driven transitions
    // in between are skipped so one step redoes one player decision.
    int target = m_cursor + 1;
    while (target < static_cast<int>(m_snapshots.size()) - 1 && !m_snapshots[target].choice)
        ++target;
    return restoreAt(target);
}

bool RollbackManager::hasSnapshots() const
{
    return !m_snapshots.empty();
}

int RollbackManager::snapshotCount() const
{
    return static_cast<int>(m_snapshots.size());
}

int RollbackManager::maxSnapshots() const
{
    return m_maxSnapshots;
}

int RollbackManager::currentIndex() const
{
    return m_cursor;
}

bool RollbackManager::canGoBack() const
{
    return m_cursor > 0;
}

bool RollbackManager::canGoForward() const
{
    return m_cursor >= 0 && m_cursor < static_cast<int>(m_snapshots.size()) - 1;
}

QString RollbackManager::currentLabel() const
{
    if (m_cursor < 0 || m_cursor >= static_cast<int>(m_snapshots.size()))
        return QString();
    return m_snapshots[m_cursor].label;
}

QString RollbackManager::labelAt(int index) const
{
    if (index < 0 || index >= static_cast<int>(m_snapshots.size()))
        return QString();
    return m_snapshots[index].label;
}

void RollbackManager::setMaxSnapshots(int n)
{
    m_maxSnapshots = qMax(0, n);
    if (m_maxSnapshots == 0)
    {
        clear();
        return;
    }
    while (static_cast<int>(m_snapshots.size()) > m_maxSnapshots)
    {
        m_snapshots.pop_front();
        if (m_cursor > 0)
            --m_cursor;
    }
}

void RollbackManager::clear()
{
    m_snapshots.clear();
    m_cursor = -1;
}
