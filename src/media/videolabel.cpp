#include "videolabel.h"

#include <QUrl>

VideoLabel::VideoLabel(const QString &path, const QString &filename, QWidget *parent) : QLabel(parent)
{
    setScaledContents(true);
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    setAttribute(Qt::WA_TransparentForMouseEvents);

    playlist.setPlaybackMode(QMediaPlaylist::PlaybackMode::Loop);
    playlist.addMedia(QUrl::fromLocalFile(path + filename));
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
        }
        mutex.unlock();
    }
}
