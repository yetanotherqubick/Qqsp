#ifndef VIDEOLABEL_H
#define VIDEOLABEL_H

#include <QImage>
#include <QLabel>
#include <QMediaPlayer>
#include <QSize>
#include <QString>
#include <QVideoSink>
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
        return m_resolution;
    }
    bool hasFrame()
    {
        return m_medialLoaded;
    }
    bool resolution_set;

private:
    QString m_path;
    QString m_filename;
    QMediaPlayer mediaPlayer;
    QVideoSink videoSink;
    QSize m_resolution;
    bool m_videoError;
    bool m_medialLoaded;

private slots:
    void OnNewFrame(const QVideoFrame &frame);
};

#endif // VIDEOLABEL_H
