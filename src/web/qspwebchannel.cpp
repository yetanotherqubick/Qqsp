#include "qspwebchannel.h"

#include "callbacks_gui.h"
#include "comtools.h"

#include <QMessageBox>
#include <qsp_default.h>

QspWebChannel::QspWebChannel(QObject *parent) : QObject(parent) {}

void QspWebChannel::ExecString(const QString &string)
{
    if (!QSPExecString(qspStringFromQString(string), QSP_TRUE))
    {
        ShowError();
    }
}

void QspWebChannel::ShowError()
{
    QString errorMessage;
    QSPErrorInfo errorInfo = QSPGetLastErrorData();
    int code = errorInfo.ErrorNum;
    QString desc = QSPTools::qspStrToQt(QSPGetErrorDesc(code));
    if (errorInfo.LocName)
    {
        errorMessage = QString("Location: %1\nArea: %2\nLine: %3\nCode: %4\nDesc: %5")
                           .arg(QSPTools::qspStrToQt(errorInfo.LocName))
                           .arg(errorInfo.ActIndex < 0 ? QString("on visit") : QString("on action"))
                           .arg(errorInfo.TopLineNum)
                           .arg(code)
                           .arg(desc);
    }
    else
    {
        errorMessage = QString("Code: %1\nDesc: %2").arg(code).arg(desc);
    }
    QMessageBox dialog(QMessageBox::Critical, tr("Error"), errorMessage, QMessageBox::Ok);
    dialog.exec();
    QSPCallBacks::RefreshInt(QSP_FALSE);
}
#include <QObject>
