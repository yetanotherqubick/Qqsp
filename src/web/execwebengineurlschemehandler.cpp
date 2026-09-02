#include "qspexecwebengineurlschemehandler.h"

#include <QWebEngineUrlRequestJob>
#include <QTimer>

QspExecWebEngineUrlSchemeHandler::QspExecWebEngineUrlSchemeHandler(QObject *parent) : QWebEngineUrlSchemeHandler(parent)
{

}

void QspExecWebEngineUrlSchemeHandler::requestStarted(QWebEngineUrlRequestJob *request)
{
    url = request->requestUrl();
    QTimer::singleShot(0, this, &QspExecWebEngineUrlSchemeHandler::QspLinkClicked);
}

void QspExecWebEngineUrlSchemeHandler::QspLinkClicked()
{
    emit qspLinkClicked(url);
}
