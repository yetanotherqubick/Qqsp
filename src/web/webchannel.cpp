#include "qspwebchannel.h"

#include <QMessageBox>

#include <qsp_default.h>
#include "callbacks_gui.h"
#include "comtools.h"

QspWebChannel::QspWebChannel(QObject *parent) : QObject(parent)
{
}

void QspWebChannel::ExecString(const QString &string)
{
    if (QSPExecString(qspStringFromQString(string), QSP_TRUE))
        return;

    QSP_CHAR *loc;
    int code, actIndex, line;
    QSPGetLastErrorData(&code, &loc, &actIndex, &line);
    const QString desc = QSPTools::qspStrToQt(QSPGetErrorDesc(code));
    const QString errorMessage = loc
            ? QStringLiteral("Location: %1\nArea: %2\nLine: %3\nCode: %4\nDesc: %5")
                  .arg(QSPTools::qspStrToQt(loc))
                  .arg(actIndex < 0 ? QStringLiteral("on visit") : QStringLiteral("on action"))
                  .arg(line)
                  .arg(code)
                  .arg(desc)
            : QStringLiteral("Code: %1\nDesc: %2")
                  .arg(code)
                  .arg(desc);
    QMessageBox::critical(nullptr, tr("Error"), errorMessage);
    QSPCallBacks::RefreshInt(QSP_FALSE);
}
