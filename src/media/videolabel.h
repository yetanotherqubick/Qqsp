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

signals:
    void medialLoaded();

public:
    explicit VideoLabel(QString path, QString filename, QWidget *parent = nullptr);
    ~VideoLabel() = default;
    bool videoError() const;
    QSize getResolution() const { return vfp.mediaResolution; }
    bool hasFrame() const { return m_medialLoaded; }
    bool resolution_set = false;

private:
    QString m_path;
    QString m_filename;
    VideoFrameProcessor vfp;
    QMediaPlayer mediaPlayer;
    QMediaPlaylist playlist;
    bool m_videoError = false;
    bool m_medialLoaded = false;
    QMutex mutex;

private slots:
    void OnNewFrame(QImage newVideoFrame);
};

#endif // VIDEOLABEL_H
