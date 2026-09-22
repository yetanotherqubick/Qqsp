#ifndef VIDEOFRAMEPROCESSOR_H
#define VIDEOFRAMEPROCESSOR_H

#include <QAbstractVideoBuffer>
#include <QAbstractVideoSurface>
#include <QImage>
#include <QList>
#include <QObject>
#include <QSize>
#include <QString>
#include <QVideoFrame>

class VideoFrameProcessor : public QAbstractVideoSurface
{
    Q_OBJECT

signals:
    void medialLoaded();
    void newFrame(QImage);

public:
    explicit VideoFrameProcessor(QObject *parent = 0);
    ~VideoFrameProcessor();
    bool present(const QVideoFrame &frame);
    QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const;
    bool isFormatSupported(const QVideoSurfaceFormat &format) const;
    // QImage curFrame;
    QSize mediaResolution;
    bool hasFrame;
};

#endif // VIDEOFRAMEPROCESSOR_H
