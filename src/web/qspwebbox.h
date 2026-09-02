#ifndef QSPWEBBOX_H
#define QSPWEBBOX_H

#include <QWebEngineView>
#include <QString>
#include <QFont>
#include <QColor>
#include <QUrl>
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

    void SetIsHtml(bool isHtml, bool refresh = true);
    void SetText(const QString& text, bool isScroll = false, bool refresh = true);
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
    void SetHead(const QString &head, bool refresh = true);
    void SetFontType(const int fontType);
    void SetSizeType(const int sizeType);
    void SetCustomCSS(bool customCSS);
    void Quit();

private:
    QWebEnginePage *createPage();
    void registerUrlSchemeHandlers();

    bool m_isUseHtml = false;
    QFont m_font;
    QString m_text;
    QString m_head;
    QColor m_linkColor;
    QColor m_backColor;
    QColor m_fontColor;
    bool m_showPlainText = false;
    bool m_videoFix = true;
    int m_fontType = 0;
    int m_sizeType = 0;
    QspWebEngineUrlSchemeHandler qweush;
    QspExecWebEngineUrlSchemeHandler qeweush;
    QWebEngineProfile profile;
    QspWebChannel qspJS;
    bool m_isQuit = false;

signals:
    void qspLinkClicked(QUrl url);

private slots:
    void OnQspLinkClicked(QUrl url);
};

#endif // QSPWEBBOX_H
