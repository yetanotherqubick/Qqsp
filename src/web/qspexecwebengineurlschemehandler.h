#ifndef QSPEXECWEBENGINEURLSCHEMEHANDLER_H
#define QSPEXECWEBENGINEURLSCHEMEHANDLER_H

#include <QObject>
#include <QUrl>
#include <QWebEngineUrlRequestJob>
#include <QWebEngineUrlSchemeHandler>

class QspExecWebEngineUrlSchemeHandler : public QWebEngineUrlSchemeHandler
{
    Q_OBJECT

public:
    explicit QspExecWebEngineUrlSchemeHandler(QObject *parent = nullptr);
    void requestStarted(QWebEngineUrlRequestJob *request) override;

signals:
    void qspLinkClicked(QUrl url);

public slots:
    void QspLinkClicked();

private:
    void legacyLinkClicked(QWebEngineUrlRequestJob *request);
    QUrl url;
};

#endif // QSPEXECWEBENGINEURLSCHEMEHANDLER_H
