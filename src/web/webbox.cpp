#include "qspwebbox.h"

#include <QEventLoop>
#include <QWebChannel>
#include <QWebEnginePage>
#include <QWebEngineSettings>

#include "comtools.h"

QspWebBox::QspWebBox(QWidget *parent) : QWebEngineView(parent), m_font(font())
{
    settings()->setDefaultTextEncoding(QStringLiteral("utf-8"));
    setFocusPolicy(Qt::NoFocus);
    settings()->setAttribute(QWebEngineSettings::PlaybackRequiresUserGesture, false);
    settings()->setUnknownUrlSchemePolicy(QWebEngineSettings::AllowAllUnknownUrlSchemes);
    setContextMenuPolicy(Qt::NoContextMenu);
    setContentsMargins(0, 0, 0, 0);
    registerUrlSchemeHandlers();
    page()->triggerAction(QWebEnginePage::Stop);
    page()->deleteLater();
    setPage(createPage());
    connect(&qeweush, &QspExecWebEngineUrlSchemeHandler::qspLinkClicked, this, &QspWebBox::OnQspLinkClicked);
    page()->load(QUrl(QStringLiteral("qsp:/")));
}

void QspWebBox::registerUrlSchemeHandlers()
{
    profile.installUrlSchemeHandler(QByteArrayLiteral("qsp"), &qweush);
    profile.installUrlSchemeHandler(QByteArrayLiteral("exec"), &qeweush);
}

QWebEnginePage *QspWebBox::createPage()
{
    auto *newPage = new QWebEnginePage(&profile, this);
    auto *channel = new QWebChannel(newPage);
    channel->registerObject(QStringLiteral("qsp"), &qspJS);
    newPage->setWebChannel(channel);
    newPage->settings()->setDefaultTextEncoding(QStringLiteral("utf-8"));
    newPage->settings()->setAttribute(QWebEngineSettings::PlaybackRequiresUserGesture, false);
    newPage->settings()->setUnknownUrlSchemePolicy(QWebEngineSettings::AllowAllUnknownUrlSchemes);
    return newPage;
}

void QspWebBox::SetIsHtml(bool isHtml, bool refresh)
{
    if (m_isUseHtml != isHtml)
    {
        m_isUseHtml = isHtml;
        if (refresh)
            RefreshUI();
    }
}

void QspWebBox::RefreshUI(bool isScroll)
{
    if (m_isQuit)
        return;

    const QString str = m_text;
    QString text;
    if (m_videoFix)
    {
        int copypos = 0;
        int startIndex = str.indexOf("<video", 0, Qt::CaseInsensitive);
        while (startIndex >= 0)
        {
            int endIndex = str.indexOf(">", startIndex, Qt::CaseInsensitive);
            if (endIndex < 0)
                break;
            endIndex = endIndex + 1;
            text.append(str.mid(copypos, startIndex + 6 - copypos));
            if (!str.mid(startIndex, endIndex - startIndex).contains("autoplay", Qt::CaseInsensitive))
                text.append(" autoplay");
            if (!str.mid(startIndex, endIndex - startIndex).contains("loop", Qt::CaseInsensitive))
                text.append(" loop");
            text.append(str.mid(startIndex + 6, endIndex - startIndex - 6));
            copypos = endIndex;
            const int cloaseTegPos = str.indexOf("</video>", 0, Qt::CaseInsensitive);
            if (cloaseTegPos == -1)
                text.append("</video>");
            else
            {
                const int nextV = str.indexOf("<video", endIndex, Qt::CaseInsensitive);
                if (nextV == -1)
                    text.append("</video>");
                else if (cloaseTegPos > nextV)
                    text.append("</video>");
            }
            startIndex = str.indexOf("<video", endIndex, Qt::CaseInsensitive);
        }
        text.append(str.mid(copypos));
    }
    else
        text = str;

    text = QSPTools::HtmlizeWhitespaces(m_isUseHtml ? text : QSPTools::ProceedAsPlain(text));
    if (m_showPlainText)
        qweush.SetPlainText(text);
    else
        qweush.SetHtml(text);

    const QString url_str = QByteArray::fromPercentEncoding(url().toString().toUtf8());
    if (url_str.compare("qsp:", Qt::CaseInsensitive) != 0 && url_str.compare("qsp:/", Qt::CaseInsensitive) != 0)
    {
        page()->triggerAction(QWebEnginePage::Stop);
        page()->deleteLater();
        setPage(createPage());
        QEventLoop loop;
        connect(page(), &QWebEnginePage::loadFinished, &loop, &QEventLoop::quit);
        page()->load(QUrl(QStringLiteral("qsp:/")));
        loop.exec();
    }
    page()->triggerAction(QWebEnginePage::ReloadAndBypassCache);
}

void QspWebBox::LoadBackImage(const QString &fileName)
{
    qweush.SetBackgroundImage(fileName);
}

void QspWebBox::SetText(const QString &text, bool isScroll, bool refresh)
{
    if (m_text != text)
    {
        if (isScroll)
        {
            if (m_text.isEmpty() || !text.startsWith(m_text))
                isScroll = false;
        }
        m_text = text;
        if (refresh)
            RefreshUI(isScroll);
    }
}

void QspWebBox::SetTextFont(const QFont &new_font)
{
    if (m_font != new_font)
    {
        m_font = new_font;
        qweush.SetTextFont(new_font);
    }
}

bool QspWebBox::SetLinkColor(const QColor &color)
{
    if (m_linkColor != color)
    {
        m_linkColor = color;
        qweush.SetLinkColor(color);
        RefreshUI();
        return true;
    }
    return false;
}

void QspWebBox::SetGamePath(const QString& path)
{
    qweush.SetGamePath(path);
}

QColor QspWebBox::GetBackgroundColor() const
{
    return m_backColor;
}

QColor QspWebBox::GetForegroundColor() const
{
    return m_fontColor;
}

bool QspWebBox::SetBackgroundColor(const QColor& color)
{
    if (m_backColor != color)
    {
        m_backColor = color;
        qweush.SetBackgroundColor(color);
        RefreshUI();
        return true;
    }
    return false;
}

bool QspWebBox::SetForegroundColor(const QColor& color)
{
    if (m_fontColor != color)
    {
        m_fontColor = color;
        qweush.SetForegroundColor(color);
        RefreshUI();
        return true;
    }
    return false;
}

void QspWebBox::SetShowPlainText(bool isPlain)
{
    m_showPlainText = isPlain;
    RefreshUI();
}

void QspWebBox::SetVideoFix(bool isFix)
{
    m_videoFix = isFix;
}

void QspWebBox::SetHead(const QString &head, bool refresh)
{
    if (m_head != head)
    {
        m_head = head;
        qweush.SetHead(head);
        if (refresh)
            RefreshUI();
    }
}

void QspWebBox::SetFontType(int fontType)
{
    if (m_fontType != fontType)
    {
        m_fontType = fontType;
        qweush.SetFontType(fontType);
        RefreshUI();
    }
}

void QspWebBox::SetSizeType(int sizeType)
{
    if (m_sizeType != sizeType)
    {
        m_sizeType = sizeType;
        qweush.SetSizeType(sizeType);
        RefreshUI();
    }
}

void QspWebBox::SetCustomCSS(bool customCSS)
{
    qweush.SetCustomCSS(customCSS);
}

void QspWebBox::Quit()
{
    m_isQuit = true;
    page()->triggerAction(QWebEnginePage::Stop);
    page()->deleteLater();
    QWebEnginePage *newpage = new QWebEnginePage(this);
    setPage(newpage);
    QEventLoop loop;
    connect(page(), &QWebEnginePage::loadFinished, &loop, &QEventLoop::quit);
    page()->load(QUrl(QStringLiteral("about:blank")));
    loop.exec();
}

void QspWebBox::OnQspLinkClicked(QUrl url)
{
    emit qspLinkClicked(url);
}
