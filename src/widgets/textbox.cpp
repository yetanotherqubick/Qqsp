#include "qsptextbox.h"

#include <QFileInfo>
#include <QPalette>
#include <QAbstractScrollArea>
#include <QScrollBar>
#include <QPainter>
#include <QTextBlock>
#include <QMovie>
#include <QLabel>
#include <QTextImageFormat>

#include "comtools.h"
#include "videolabel.h"

namespace
{
    bool setAnimationGeometry(QWidget *widget, const QRect &rect, const QTextImageFormat &format)
    {
        if (format.width() > 0 && format.height() > 0)
        {
            widget->setGeometry(rect.x(), rect.y(), format.width(), format.height());
            return true;
        }

        widget->move(rect.x(), rect.y());
        return false;
    }
}

QspTextBox::QspTextBox(QWidget *parent) : QTextBrowser(parent)
{
    //SetBorders(5);
    setFocusPolicy(Qt::NoFocus);
    setFrameStyle(QFrame::NoFrame);
    setFrameShadow(QFrame::Plain);
    setContentsMargins(0,0,0,0);
    m_isUseHtml = false;
    showPlainText = false;
    disableVideo = false;
    //m_linkColor = palette().color(QPalette::Link);
    //m_fontColor = palette().color(QPalette::Text);
    //m_backColor = QColor(224, 224, 224);
    m_font = font();
    setOpenLinks(false);
#ifndef _WEBBOX_COMMON
    connect(verticalScrollBar(), &QScrollBar::valueChanged, this, &QspTextBox::resizeAnimations);
    connect(horizontalScrollBar(), &QScrollBar::valueChanged, this, &QspTextBox::resizeAnimations);
#endif
}

QspTextBox::~QspTextBox()
{
#ifndef _WEBBOX_COMMON
    clearManualResources();
#endif
}

void QspTextBox::SetIsHtml(bool isHtml, bool refresh)
{
    if (m_isUseHtml != isHtml)
    {
        m_isUseHtml = isHtml;
        if (refresh)
            RefreshUI();
    }
}

void QspTextBox::RefreshUI(bool isScroll)
{
    QString str = m_text;
    QString text;
    if(m_isUseHtml)
    {
        if(str.endsWith("\r"))
            str.chop(1);
        if(str.endsWith("\n"))
            str.chop(1);
        str = str.replace("\r", "").replace("\n", "<br>").replace("<video ", "<img ", Qt::CaseInsensitive);
        text = str.replace("</center><br>", "</center>", Qt::CaseInsensitive).replace("</table><br>", "</table>", Qt::CaseInsensitive);
    }
    else
    {
        text = Qt::convertFromPlainText(str);
    }
    //TODO: set colour and font
    if(showPlainText)
        setPlainText(text);
    else
        setHtml(text);
    if (isScroll) verticalScrollBar()->setValue(verticalScrollBar()->maximum());
}

void QspTextBox::SetText(const QString& text, bool isScroll, bool refresh)
{
    if (m_text != text)
    {
#ifndef _WEBBOX_COMMON
        clearManualResources();
#endif
        if (isScroll)
        {
            if (m_text.isEmpty() || !text.startsWith(m_text))
                isScroll = false;
        }
        m_text = text;
        if (refresh)
            RefreshUI(isScroll);
#ifndef _WEBBOX_COMMON
        resizeAnimations();
#endif
    }
}

void QspTextBox::SetTextFont(const QFont& new_font)
{
    if (m_font != new_font)
    {
        m_font = new_font;
        setFont(new_font); //TODO: check which one of this to use
        setCurrentFont(new_font);
    }
}

bool QspTextBox::SetLinkColor(const QColor &color)
{
    //QPalette p = palette();
    //p.setBrush( QPalette::Link, clr);
    //setPalette( p );
    //NOTE: From Qt documentation:
    //Note that we do not use the Link and LinkVisited roles when rendering rich text in Qt, and that we recommend that you use CSS and the QTextDocument::setDefaultStyleSheet() function to alter the appearance of links.
    if(m_linkColor != color)
    {
        QString sheet = QString::fromLatin1("a { text-decoration: underline; color: %1 }").arg(color.name());
        document()->setDefaultStyleSheet(sheet);
        m_linkColor = color;
        RefreshUI();
        return true;
    }
    return false;
}

void QspTextBox::SetGamePath(const QString &path)
{
    m_path = path;
    setSearchPaths(QStringList(path));
}

QColor QspTextBox::GetBackgroundColor() const
{
    return m_backColor;
}

QColor QspTextBox::GetForegroundColor() const
{
    return m_fontColor;
}

bool QspTextBox::SetBackgroundColor(const QColor &color)
{
    if(m_backColor != color)
    {
        QPalette p = palette();
        p.setColor(QPalette::Base, color);
        setPalette(p);
        m_backColor = color;
        setTextBackgroundColor(color);
        RefreshUI();
        return true;
    }
    return false;
}

bool QspTextBox::SetForegroundColor(const QColor &color)
{
    if(m_fontColor != color)
    {
        m_fontColor = color;
        setTextColor(color);
        QPalette p = palette();
        p.setColor(QPalette::Text, color);
        p.setColor(QPalette::WindowText, color);
        setPalette(p);
        RefreshUI();
        return true;
    }
    return false;
}

void QspTextBox::SetShowPlainText(bool isPlain)
{
    showPlainText = isPlain;
    RefreshUI();
}

void QspTextBox::wheelEvent(QWheelEvent *e)
{
    if( e->modifiers() == Qt::ControlModifier )
        return;
    QTextBrowser::wheelEvent(e);
}

void QspTextBox::keyPressEvent(QKeyEvent *event)
{
    QTextBrowser::keyPressEvent(event);
}

#ifndef _WEBBOX_COMMON
void QspTextBox::SetBackgroundImage(const QImage& bmpBg)
{
    m_bmpBg = bmpBg;
    CalcImageSize();
}

void QspTextBox::LoadBackImage(const QString& fileName)
{
    QFileInfo file(m_path + fileName);
    QString path(file.absoluteFilePath());
    if (m_imagePath != path)
    {
        m_imagePath = path;
        if (file.exists() && file.isFile())
        {
            QImage image;
            if (image.load(path))
            {
                SetBackgroundImage(image);
                return;
            }
        }
        SetBackgroundImage(QImage());
    }
}

void QspTextBox::CalcImageSize()
{
    if (!m_bmpBg.isNull())
    {
        int w, h;
        w = this->childrenRect().width();
        h = this->childrenRect().height();
        if (w < 1) w = 1;
        if (h < 1) h = 1;
        int srcW = m_bmpBg.width(), srcH = m_bmpBg.height();
        int destW = srcW * h / srcH, destH = srcH * w / srcW;
        if (destW > w)
            destW = w;
        else
            destH = h;
        m_posX = (w - destW) / 2;
        m_posY = (h - destH) / 2;
        if (destW > 0 && destH > 0)
            m_bmpRealBg = m_bmpBg.scaled(destW, destH);
    }
}

void QspTextBox::paintEvent(QPaintEvent *e)
{
    QPainter painter(viewport());
    if (!m_bmpBg.isNull())
        painter.drawImage(m_posX, m_posY, m_bmpRealBg);
    QTextBrowser::paintEvent(e);
}

void QspTextBox::resizeEvent(QResizeEvent *e)
{
    if (!m_bmpBg.isNull())
        CalcImageSize();
    resizeAnimations();
    QTextBrowser::resizeEvent(e);
}

void QspTextBox::clearManualResources()
{
    for (const auto &animation : animations_gif)
    {
        delete animation.movie;
        delete animation.movieLabel;
    }
    animations_gif.clear();

    for (const auto &animation : animations_video)
        delete animation.videoLabel;
    animations_video.clear();
}

QVariant QspTextBox::loadResource(int type, const QUrl &name)
{
    QString new_name = QSPTools::GetCaseInsensitiveFilePath(m_path, QString(QByteArray::fromPercentEncoding(name.toString().toUtf8())));
    if(new_name.endsWith(".gif", Qt::CaseInsensitive) || new_name.endsWith(".mng", Qt::CaseInsensitive))
    {
        QMovie *movie = new QMovie(m_path + new_name, QByteArray(), this);
        QLabel *videoL = new QLabel(this);
        if(movie->isValid())
        {
            videoL->setScaledContents(true);
            videoL->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
            videoL->setAttribute(Qt::WA_TransparentForMouseEvents);
            videoL->setMovie(movie);
            connect(movie, &QMovie::started, this, &QspTextBox::resizeAnimations);
            connect(movie, &QMovie::finished, this, &QspTextBox::resizeAnimations);
            movie->start();
            videoL->raise();
            videoL->show();
            videoL->setGeometry(0,0, movie->frameRect().size().width(), movie->frameRect().size().height());
            animations_gif.insert(QString(QByteArray::fromPercentEncoding(name.toString().toUtf8())), {movie, videoL});

            QImage image(movie->frameRect().size(), QImage::Format_ARGB32);
            image.fill(qRgba(0,0,0,0));
            return QVariant(image);
        }
        delete movie;
        delete videoL;
        QImage image(1, 1, QImage::Format_ARGB32);
        image.fill(qRgba(0,0,0,0));
        return QVariant(image);
    }
    if(new_name.endsWith(".mp4", Qt::CaseInsensitive) || new_name.endsWith(".avi", Qt::CaseInsensitive) || new_name.endsWith(".wmv", Qt::CaseInsensitive) || new_name.endsWith(".mkv", Qt::CaseInsensitive) || new_name.endsWith(".webm", Qt::CaseInsensitive) || new_name.endsWith(".m4v", Qt::CaseInsensitive) || new_name.endsWith(".ogv", Qt::CaseInsensitive))
    {
        if(!disableVideo)
        {
            VideoLabel *videoL = new VideoLabel(m_path, new_name, this);
            animations_video.insert(QString(QByteArray::fromPercentEncoding(name.toString().toUtf8())), { videoL });
            videoL->setGeometry(0,0, 0, 0);
            videoL->raise();
            videoL->show();
            QImage image(1,1, QImage::Format_ARGB32);
            image.fill(qRgba(0,0,0,0));
            return QVariant(image);
        }
        QImage image(1,1, QImage::Format_ARGB32);
        image.fill(qRgba(0,0,0,0));
        return QVariant(image);
    }
    if(type == QTextDocument::ImageResource)
        return QVariant(QImage(m_path + new_name));
    else
        return QTextBrowser::loadResource(type, QUrl(new_name));
}

void QspTextBox::resizeAnimations()
{
    if(mutex.tryLock())
    {
        if(animations_gif.count() > 0 || animations_video.count() > 0)
        {
            QTextBlock bl = document()->begin();
            QTextCursor cursor(document());
            while(bl.isValid())
            {
                QTextBlock::iterator it;
                for(it = bl.begin(); !(it.atEnd()); ++it)
                {
                    if(it.fragment().isValid() && it.fragment().charFormat().isImageFormat())
                    {
                        cursor.setPosition(it.fragment().position());
                        const QTextImageFormat imageFormat = it.fragment().charFormat().toImageFormat();
                        const QString imageName = imageFormat.name();
                        if (animations_gif.contains(imageName))
                            setAnimationGeometry(animations_gif[imageName].movieLabel, cursorRect(cursor), imageFormat);
                        else if (animations_video.contains(imageName))
                        {
                            VideoLabel *videoLabel = animations_video[imageName].videoLabel;
                            if(videoLabel != nullptr && videoLabel->hasFrame())
                            {
                                QRect curRect = cursorRect(cursor);
                                if(!setAnimationGeometry(videoLabel, curRect, imageFormat))
                                {
                                    const QSize resolution = videoLabel->resolution();
                                    if (resolution.isValid())
                                    {
                                        QTextImageFormat newImageFormat = imageFormat;
                                        newImageFormat.setWidth(resolution.width());
                                        newImageFormat.setHeight(resolution.height());
                                        if (newImageFormat.isValid())
                                        {
                                            QTextCursor cur(document());
                                            cur.setPosition(it.fragment().position());
                                            cur.setPosition(it.fragment().position() + it.fragment().length(), QTextCursor::KeepAnchor);
                                            cur.setCharFormat(newImageFormat);
                                            cur.setPosition(it.fragment().position());
                                            curRect = cursorRect(cur);
                                        }
                                        videoLabel->setGeometry(curRect.x(), curRect.y(), resolution.width(), resolution.height());
                                    }
                                }
                            }
                        }
                    }
                }
                bl = bl.next();
            }
        }
        mutex.unlock();
    }
}

void QspTextBox::repaintAnimation()
{
    viewport()->update();
}
#endif
