#include "qspexecwebengineurlschemehandler.h"

#include <QWebEngineUrlRequestJob>
#include <QTimer>

QspExecWebEngineUrlSchemeHandler::QspExecWebEngineUrlSchemeHandler(QObject *parent) : QWebEngineUrlSchemeHandler(parent)
{
}

void QspExecWebEngineUrlSchemeHandler::requestStarted(QWebEngineUrlRequestJob *request)
{
    const QUrl requestedUrl = request->requestUrl();
    QTimer::singleShot(0, this, [this, requestedUrl]() {
        emit qspLinkClicked(requestedUrl);
    });
}
