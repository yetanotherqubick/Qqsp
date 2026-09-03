#include "qspwebengineurlschemehandler.h"

#include <QBuffer>
#include <QFile>
#include <QMimeDatabase>
#include <QMimeType>
#include <QUrl>

#include "comtools.h"

QspWebEngineUrlSchemeHandler::QspWebEngineUrlSchemeHandler(QObject *parent) : QWebEngineUrlSchemeHandler(parent)
{
}

void QspWebEngineUrlSchemeHandler::requestStarted(QWebEngineUrlRequestJob *request)
{
    const QUrl url = request->requestUrl();
    const QString url_str = url.toString();
    auto *buffer = new QBuffer(request);

    if (url_str.compare("qsp:", Qt::CaseInsensitive) == 0 || url_str.compare("qsp:/", Qt::CaseInsensitive) == 0)
    {
        buffer->open(QIODevice::WriteOnly);
        if (m_isUseHtml)
            buffer->write(createHtml().toUtf8());
        else
            buffer->write(m_text.toUtf8());
        buffer->close();
        request->reply(m_isUseHtml ? QByteArrayLiteral("text/html") : QByteArrayLiteral("text/plain; charset=utf-8"), buffer);
        return;
    }

    QString tmpurl = url_str.mid(5);
    if (tmpurl.contains('#') && !tmpurl.isEmpty())
        tmpurl = tmpurl.split('#').at(0);
    tmpurl = QByteArray::fromPercentEncoding(tmpurl.toUtf8());

    const QString path = QSPTools::GetCaseInsensitiveFilePath(m_path, tmpurl);
    const QString filePath = m_path + path;
    QMimeDatabase db;
    const QMimeType type = db.mimeTypeForFile(filePath);
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        request->fail(QWebEngineUrlRequestJob::UrlNotFound);
        return;
    }

    buffer->open(QIODevice::WriteOnly);
    buffer->write(file.readAll());
    buffer->close();
    request->reply(type.name().toUtf8(), buffer);
}

QString QspWebEngineUrlSchemeHandler::createHtml() const
{
    QString html;
    html.append("<!DOCTYPE html>\n<html>\n<head>\n<meta charset=\"UTF-8\">\n<style>\nbody {\n");
    if (m_backColor.isValid())
        html.append(QStringLiteral("background-color: %1;\n").arg(m_backColor.name()));
    if (m_fontColor.isValid())
        html.append(QStringLiteral("color: %1;\n").arg(m_fontColor.name()));
    if (m_fontType == 1)
        html.append(QStringLiteral("font-family: %1;\n").arg(m_font.family()));
    if (m_fontType == 2)
        html.append(QStringLiteral("font-family: %1 !important;\n").arg(m_font.family()));
    if (m_sizeType == 1)
        html.append(QStringLiteral("font-size: %1pt;\n").arg(m_font.pointSize()));
    if (m_sizeType == 2)
        html.append(QStringLiteral("font-size: %1pt !important;\n").arg(m_font.pointSize()));
    if (!m_bmpBg.isEmpty())
        html.append(QStringLiteral("background: url(%1) no-repeat center center fixed;\nbackground-size: cover;\n").arg(m_bmpBg));
    if (m_linkColor.isValid())
        html.append(QStringLiteral("}\na:link {\ncolor: %1;\n").arg(m_linkColor.name()));
    html.append("}\n</style>\n");
    if (m_isCustomCSS)
        html.append("<link rel=\"stylesheet\" type=\"text/css\" href=\"custom.css\">\n");
    html.append("<script src=\"qrc:///qtwebchannel/qwebchannel.js\"></script>\n");
    html.append("<script type=\"text/javascript\"> \nvar qsp; \ndocument.addEventListener(\"DOMContentLoaded\", function () { \nnew QWebChannel(qt.webChannelTransport, function(channel) { \nqsp = channel.objects.qsp; \n}); \n});\n</script>\n");
    html.append(m_head);
    html.append("</head>\n<body>\n");
    html.append(m_text);
    html.append("</body>\n</html>");
    return html;
}

void QspWebEngineUrlSchemeHandler::SetPlainText(const QString &text)
{
    m_isUseHtml = false;
    m_text = text;
}

void QspWebEngineUrlSchemeHandler::SetHtml(const QString &text)
{
    m_isUseHtml = true;
    m_text = text;
}

void QspWebEngineUrlSchemeHandler::SetGamePath(const QString &path)
{
    m_path = path;
}

void QspWebEngineUrlSchemeHandler::SetLinkColor(const QColor &color)
{
    m_linkColor = color;
}

void QspWebEngineUrlSchemeHandler::SetBackgroundColor(const QColor &color)
{
    m_backColor = color;
}

void QspWebEngineUrlSchemeHandler::SetForegroundColor(const QColor &color)
{
    m_fontColor = color;
}

void QspWebEngineUrlSchemeHandler::SetBackgroundImage(const QString &path)
{
    m_bmpBg = path;
}

void QspWebEngineUrlSchemeHandler::SetTextFont(const QFont &new_font)
{
    m_font = new_font;
}

void QspWebEngineUrlSchemeHandler::SetHead(const QString &head)
{
    m_head = head;
}

void QspWebEngineUrlSchemeHandler::SetFontType(int fontType)
{
    m_fontType = fontType;
}

void QspWebEngineUrlSchemeHandler::SetSizeType(int sizeType)
{
    m_sizeType = sizeType;
}

void QspWebEngineUrlSchemeHandler::SetCustomCSS(bool customCSS)
{
    m_isCustomCSS = customCSS;
}
