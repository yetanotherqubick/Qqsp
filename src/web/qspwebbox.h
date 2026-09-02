#ifndef QSPWEBBOX_H
#define QSPWEBBOX_H

#include <QWidget>
#include <QWebEngineView>
#include <QString>
#include <QFont>
#include <QColor>
#include <QUrl>
#include <QWebChannel>
#include <QWebEngineProfile>

#include "qspwebengineurlschemehandler.h"
#include "qspexecwebengineurlschemehandler.h"
#include "qspwebchannel.h"

class QspWebBox : public QWebEngineView
{
    Q_OBJECT

public:
    explicit QspWebBox(QWidget *parent = nullptr);
    ~QspWebBox() = default;

    void RefreshUI(bool isScroll = false);
    void LoadBackImage(const QString& fileName);

    void SetIsHtml(bool isHtml);
    void SetText(const QString& text, bool isScroll = false);
    void SetTextFont(const QFont& new_font);
    QFont GetTextFont() const { return m_font; }
    QString GetText() const { return m_text; }
    bool SetLinkColor(const QColor &color);
    QColor GetLinkColor() const { return m_linkColor; }
    void SetGamePath(const QString& path);
    QColor GetBackgroundColor() const;
    QColor GetForegroundColor() const;
    bool SetBackgroundColor(const QColor& color);
    bool SetForegroundColor(const QColor& color);
    void SetShowPlainText(bool isPlain);
    void SetVideoFix(bool isFix);
    void SetHead(const QString &head);
    void SetFontType(const int fontType);
    void SetSizeType(const int sizeType);
    void SetCustomCSS(bool customCSS);
    void Quit();

private:
    bool m_isUseHtml;
    QString m_path;
    QFont m_font;
    QString m_text;
    QString m_head;
    QColor m_linkColor;
    QColor m_backColor;
    QColor m_fontColor;
    bool showPlainText;
    bool m_videoFix;
    int m_fontType;
    int m_sizeType;
    QspWebEngineUrlSchemeHandler qweush;
    QspExecWebEngineUrlSchemeHandler qeweush;
    QWebEngineProfile profile;
    QspWebChannel qspJS;
    bool m_isQuit;

signals:
    void qspLinkClicked(QUrl url);

private slots:
    void OnQspLinkClicked(QUrl url);
};

#endif // QSPWEBBOX_H
