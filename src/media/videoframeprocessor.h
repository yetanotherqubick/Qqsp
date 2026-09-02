#ifndef VIDEOFRAMEPROCESSOR_H
#define VIDEOFRAMEPROCESSOR_H

#include <QObject>
#include <QAbstractVideoSurface>
#include <QAbstractVideoBuffer>
#include <QVideoFrame>
#include <QList>
#include <QSize>
#include <QImage>

class VideoFrameProcessor : public QAbstractVideoSurface
{
    Q_OBJECT

signals:
    void newFrame(QImage);

public:
    explicit VideoFrameProcessor(QObject *parent = nullptr);
    ~VideoFrameProcessor() = default;
    bool present(const QVideoFrame& frame);
    QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const;
    bool isFormatSupported(const QVideoSurfaceFormat &format) const;
    QSize mediaResolution;
    bool hasFrame = false;
};

#endif // VIDEOFRAMEPROCESSOR_H
