#include "videolabel.h"

#include <QUrl>

VideoLabel::VideoLabel(QString path, QString filename, QWidget *parent) : QLabel(parent)
{
    m_path = path;
    m_filename = filename;
    setScaledContents(true);
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    setAttribute(Qt::WA_TransparentForMouseEvents);

    playlist.setPlaybackMode(QMediaPlaylist::PlaybackMode::Loop);
    playlist.addMedia(QUrl::fromLocalFile(m_path + m_filename));
    mediaPlayer.setPlaylist(&playlist);
    mediaPlayer.setVideoOutput(&vfp);
    mediaPlayer.play();

    connect(&vfp, &VideoFrameProcessor::newFrame, this, &VideoLabel::OnNewFrame);
}

bool VideoLabel::videoError() const
{
    if(mediaPlayer.error() != QMediaPlayer::FormatError && vfp.error() == QAbstractVideoSurface::NoError)
        return false;
    else
        return true;
}

void VideoLabel::OnNewFrame(QImage newVideoFrame)
{
    if(mutex.tryLock())
    {
        setPixmap(QPixmap::fromImage(newVideoFrame));
        if(!m_medialLoaded)
        {
            m_medialLoaded = true;
            emit medialLoaded();
        }
        mutex.unlock();
    }
}
