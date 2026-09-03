#include <QtTest>

#include "videoframeprocessor.h"

class TestVideoFrameProcessor : public QObject
{
    Q_OBJECT

private slots:
    void defaults();
    void supportedPixelFormats();
    void formatValidation();
    void present();
};

void TestVideoFrameProcessor::defaults()
{
    VideoFrameProcessor processor;

    QVERIFY(!processor.hasFrame);
    QCOMPARE(processor.mediaResolution, QSize());
}

void TestVideoFrameProcessor::supportedPixelFormats()
{
    VideoFrameProcessor processor;
    const QList<QVideoFrame::PixelFormat> formats =
        processor.supportedPixelFormats(QAbstractVideoBuffer::NoHandle);

    QVERIFY(formats.contains(QVideoFrame::Format_RGB32));
    QVERIFY(formats.contains(QVideoFrame::Format_ARGB32));
    QVERIFY(formats.contains(QVideoFrame::Format_ARGB32_Premultiplied));
    QVERIFY(formats.contains(QVideoFrame::Format_RGB565));
    QVERIFY(formats.contains(QVideoFrame::Format_RGB24));
}

void TestVideoFrameProcessor::formatValidation()
{
    VideoFrameProcessor processor;

    const QVideoSurfaceFormat valid(QSize(320, 240), QVideoFrame::Format_RGB32);
    QVERIFY(processor.isFormatSupported(valid));

    const QVideoSurfaceFormat empty(QSize(), QVideoFrame::Format_RGB32);
    QVERIFY(!processor.isFormatSupported(empty));

    const QVideoSurfaceFormat invalid(QSize(320, 240), QVideoFrame::Format_Invalid);
    QVERIFY(!processor.isFormatSupported(invalid));
}

void TestVideoFrameProcessor::present()
{
    VideoFrameProcessor processor;
    QSignalSpy spy(&processor, SIGNAL(newFrame(QImage)));

    QImage image(2, 3, QImage::Format_RGB32);
    image.fill(Qt::white);
    QVideoFrame frame(image);

    QVERIFY(processor.present(frame));
    QVERIFY(processor.hasFrame);
    QCOMPARE(processor.mediaResolution, QSize(2, 3));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).value<QImage>().size(), QSize(2, 3));
}

QTEST_MAIN(TestVideoFrameProcessor)
#include "test_videoframeprocessor.moc"
