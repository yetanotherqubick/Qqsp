#ifndef QSPIMGCANVAS_H
#define QSPIMGCANVAS_H

#include <QWidget>
#include <QLabel>
#include <QString>
#include <QColor>
#include <QPixmap>
#include <QVBoxLayout>
#include <QMovie>

namespace Ui {
class QspImgCanvas;
}

class QspImgCanvas : public QWidget
{
    Q_OBJECT

public:
    explicit QspImgCanvas(QWidget *parent = nullptr);

    // Methods
    bool OpenFile(const QString& fileName);
    void RefreshUI();
    void SetGamePath(const QString& path);

    // Overloaded methods
    bool SetBackgroundColor(const QColor& color) override;

private:
    void resizeEvent(QResizeEvent *event) override;
    void updateImage();
    // Fields
    bool m_isAnim;
    QPixmap m_image;
    QMovie m_movie;
    QLabel label_image;
    QVBoxLayout layout;
    QString m_path;
    int m_posX;
    int m_posY;
private slots:
    void OnNewFrame(int frameNumber);
};

#endif // QSPIMGCANVAS_H
