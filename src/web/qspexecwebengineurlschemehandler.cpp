#include "qspexecwebengineurlschemehandler.h"

#include "callbacks_gui.h"
#include "comtools.h"

#include <QBuffer>
#include <QMessageBox>
#include <QString>
#include <QUrl>
#include <qsp_default.h>

QspExecWebEngineUrlSchemeHandler::QspExecWebEngineUrlSchemeHandler(QObject *parent) : QWebEngineUrlSchemeHandler(parent) {}

void QspExecWebEngineUrlSchemeHandler::requestStarted(QWebEngineUrlRequestJob *request)
{
    url = request->requestUrl();
    QTimer::singleShot(0, this, &QspExecWebEngineUrlSchemeHandler::QspLinkClicked);
}

void QspExecWebEngineUrlSchemeHandler::QspLinkClicked()
{
    Q_EMIT qspLinkClicked(url);
}

void QspExecWebEngineUrlSchemeHandler::legacyLinkClicked(QWebEngineUrlRequestJob *request)
{
    url = request->requestUrl();
    QString href;
    href = QByteArray::fromPercentEncoding(url.toString().toUtf8());
    QString string = href.mid(5);
    if (!QSPExecString(qspStringFromQString(string), QSP_TRUE))
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
    // request->redirect(QUrl("qsp:/"));
}
#include <QObject>
#include <QTimer>
