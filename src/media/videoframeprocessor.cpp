#include "videoframeprocessor.h"

#include <QVideoSurfaceFormat>

VideoFrameProcessor::VideoFrameProcessor(QObject *parent) : QAbstractVideoSurface(parent)
{
}

bool VideoFrameProcessor::present(const QVideoFrame &frame)
{
    if(frame.isValid())
    {
        QVideoFrame videoFrame(frame);
        if(videoFrame.map(QAbstractVideoBuffer::ReadOnly))
        {
            if(!hasFrame)
            {
                mediaResolution = videoFrame.size();
                hasFrame = true;
            }
            const QImage image(
                              videoFrame.bits(),
                              videoFrame.width(),
                              videoFrame.height(),
                              videoFrame.bytesPerLine(),
                              QVideoFrame::imageFormatFromPixelFormat(videoFrame.pixelFormat()));
            emit newFrame(image.copy());
        }
        videoFrame.unmap();
    }

    return true;
}

QList<QVideoFrame::PixelFormat> VideoFrameProcessor::supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const
{
    Q_UNUSED(handleType);
    return QList<QVideoFrame::PixelFormat>()
            << QVideoFrame::Format_RGB32
            << QVideoFrame::Format_ARGB32
            << QVideoFrame::Format_ARGB32_Premultiplied
            << QVideoFrame::Format_RGB565
            << QVideoFrame::Format_RGB555
            << QVideoFrame::Format_ARGB8565_Premultiplied
            << QVideoFrame::Format_RGB24;
}

bool VideoFrameProcessor::isFormatSupported(const QVideoSurfaceFormat &format) const
{
    const QImage::Format imageFormat = QVideoFrame::imageFormatFromPixelFormat(format.pixelFormat());
    const QSize size = format.frameSize();

    return imageFormat != QImage::Format_Invalid
            && !size.isEmpty()
            && format.handleType() == QAbstractVideoBuffer::NoHandle;
}
