#ifndef QSPWEBENGINEURLREQUESTINTERCEPTOR_H
#define QSPWEBENGINEURLREQUESTINTERCEPTOR_H

#include <QObject>
#include <QWebEngineUrlRequestInterceptor>

class QspWebEngineUrlRequestInterceptor : public QWebEngineUrlRequestInterceptor
{
    Q_OBJECT

public:
    explicit QspWebEngineUrlRequestInterceptor(QObject *parent = nullptr);
    void interceptRequest(QWebEngineUrlRequestInfo &info) override;

private:
};

#endif // QSPWEBENGINEURLREQUESTINTERCEPTOR_H
