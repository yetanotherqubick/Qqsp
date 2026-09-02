#include "qspimgcanvas.h"

#include <QFileInfo>

QspImgCanvas::QspImgCanvas(QWidget *parent) : QWidget(parent)
{
    m_posX = m_posY = 0;
    m_isAnim = false;
    setWindowTitle(tr("Image"));
    setContentsMargins(0,0,0,0);
    label_image.setFrameStyle(QFrame::NoFrame);
    label_image.setFrameShadow(QFrame::Plain);
    label_image.setContentsMargins(0,0,0,0);
    layout.setContentsMargins(0,0,0,0);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    label_image.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout.addWidget(&label_image);
    setLayout(&layout);
    label_image.setMinimumSize(50, 50);
    setMinimumSize(50, 50);
    connect(&m_movie, SIGNAL(frameChanged(int)), this, SLOT(OnNewFrame(int)) );
}

bool QspImgCanvas::OpenFile(const QString &fileName)
{
    bool ret = false;
    QFileInfo file(m_path + fileName);
    QString path(file.absoluteFilePath());
    setWindowTitle(path);
    m_isAnim = false;
    if (!path.isEmpty())
    {
        if (file.exists() && file.isFile())
        {
            if(path.endsWith(".gif", Qt::CaseInsensitive) || path.endsWith(".mng", Qt::CaseInsensitive))
            {
                m_movie.stop();
                m_movie.setFileName(path);
                m_movie.start();
                if(m_movie.isValid())
                    m_isAnim = true;
            }
            if (m_isAnim)
            {
                ret = true;
            }
            else
            {
                ret = m_image.load(path);
                if(ret)
                    label_image.setPixmap(m_image);
            }
            if (ret)
            {
                updateImage();
            }
            return ret;
        }
        return false;
    }
    return true;
}

void QspImgCanvas::RefreshUI()
{
}

void QspImgCanvas::SetGamePath(const QString &path)
{
    m_path = path;
}

bool QspImgCanvas::SetBackgroundColor(const QColor &color)
{
    QString sheet = QString::fromLatin1("QLabel { background-color : %1 }").arg(color.name());
    label_image.setStyleSheet(sheet);
    return true;
}

void QspImgCanvas::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    updateImage();
}

void QspImgCanvas::updateImage()
{
    if (!m_image.isNull())
    {
        label_image.setPixmap(m_image.scaled(label_image.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

void QspImgCanvas::OnNewFrame(int frameNumber)
{
    if (m_isAnim)
    {
        if(m_movie.isValid())
        {
            label_image.setPixmap(m_movie.currentPixmap().scaled(label_image.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }
}
