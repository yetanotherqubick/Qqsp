#ifndef QSPNETWORKACCESSMANAGER_H
#define QSPNETWORKACCESSMANAGER_H

#include <QNetworkAccessManager>
#include <QString>
#include <QColor>
#include <QFont>

class QspNetworkAccessManager : public QNetworkAccessManager
{
    Q_OBJECT

public:
    QspNetworkAccessManager(QNetworkAccessManager *oldManager, QObject *parent = nullptr);
    ~QspNetworkAccessManager() = default;
    void SetPlainText(const QString& text);
    void SetHtml(const QString& text);
    void SetGamePath(const QString &path);
    void SetLinkColor(const QColor &color);
    void SetBackgroundColor(const QColor& color);
    void SetForegroundColor(const QColor& color);
    void SetBackgroundImage(const QString &path);
    void SetTextFont(const QFont& new_font);

protected:
    QNetworkReply *createRequest(Operation operation, const QNetworkRequest &request, QIODevice *device) override;

private:
    QString  m_text;
    QString m_path;
    QString m_bmpBg;
    QColor m_linkColor;
    QColor m_backColor;
    QColor m_fontColor;
    QFont m_font;
    bool m_isUseHtml = true;
};

#endif // QSPNETWORKACCESSMANAGER_H
