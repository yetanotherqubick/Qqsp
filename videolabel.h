#ifndef VIDEOLABEL_H
#define VIDEOLABEL_H

#include "videoframeprocessor.h"

#include <QImage>
#include <QLabel>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QMutex>
#include <QSize>
#include <QString>
#include <QWidget>

class VideoLabel : public QLabel
{
    Q_OBJECT

signals:
    void medialLoaded();

public:
    explicit VideoLabel(QString path, QString filename, QWidget *parent = 0);
    ~VideoLabel();
    bool videoError();
    QSize getResolution()
    {
        return vfp.mediaResolution;
    }
    bool hasFrame()
    {
        return m_medialLoaded;
    }
    bool resolution_set;

private:
    QString m_path;
    QString m_filename;
    VideoFrameProcessor vfp;
    QMediaPlayer mediaPlayer;
    QMediaPlaylist playlist;
    bool m_videoError;
    bool m_medialLoaded;
    QMutex mutex;

private slots:
    void OnNewFrame(QImage newVideoFrame);
};

#endif // VIDEOLABEL_H
