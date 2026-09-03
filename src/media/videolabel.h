#ifndef VIDEOLABEL_H
#define VIDEOLABEL_H

#include <QWidget>
#include <QString>
#include <QLabel>
#include <QImage>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QSize>
#include <QMutex>

#include "videoframeprocessor.h"

class VideoLabel : public QLabel
{
    Q_OBJECT

public:
    explicit VideoLabel(const QString &path, const QString &filename, QWidget *parent = nullptr);
    bool videoError() const;
    QSize resolution() const { return vfp.mediaResolution; }
    bool hasFrame() const { return m_medialLoaded; }

private:
    VideoFrameProcessor vfp;
    QMediaPlayer mediaPlayer;
    QMediaPlaylist playlist;
    bool m_medialLoaded = false;
    QMutex mutex;

private slots:
    void OnNewFrame(QImage newVideoFrame);
};

#endif // VIDEOLABEL_H
