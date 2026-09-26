#include "videolabel.h"

#include <QAudioOutput>
#include <QVideoFrame>
#include <QUrl>

VideoLabel::VideoLabel(QString path, QString filename, QWidget *parent) : QLabel(parent)
{
    m_path = path;
    m_filename = filename;
    setScaledContents(true);
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    setAttribute(Qt::WA_TransparentForMouseEvents);
    resolution_set = false;
    m_medialLoaded = false;

    QAudioOutput *audio = new QAudioOutput(this);
    mediaPlayer.setAudioOutput(audio);
    mediaPlayer.setSource(QUrl::fromLocalFile(m_path + m_filename));
    mediaPlayer.setLoops(QMediaPlayer::Infinite);
    mediaPlayer.setVideoSink(&videoSink);
    connect(&videoSink, &QVideoSink::videoFrameChanged, this, &VideoLabel::OnNewFrame);
    mediaPlayer.play();
}

VideoLabel::~VideoLabel() = default;

void VideoLabel::OnNewFrame(const QVideoFrame &frame)
{
    if (!frame.isValid())
    {
        return;
    }
    if (!resolution_set)
    {
        m_resolution = frame.size();
        resolution_set = true;
    }
    QImage image = frame.toImage();
    if (!image.isNull())
    {
        setPixmap(QPixmap::fromImage(image));
        if (!m_medialLoaded)
        {
            m_medialLoaded = true;
            Q_EMIT medialLoaded();
        }
    }
}
