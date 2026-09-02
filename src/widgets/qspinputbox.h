#ifndef QSPINPUTBOX_H
#define QSPINPUTBOX_H

#include <QWidget>
#include <QPlainTextEdit>
#include <QString>
#include <QStringList>

namespace Ui {
class QspInputBox;
}

class QspInputBox : public QPlainTextEdit
{
    Q_OBJECT

signals:
   void InputTextEnter();

public:
    explicit QspInputBox(QWidget *parent = nullptr);
    // Accessors
    void SetText(const QString& text);
    QString GetText() const;

private:
    void keyPressEvent(QKeyEvent *event) override;
    // Fields
    QStringList m_strings;
    int m_selIndex;
};

#endif // QSPINPUTBOX_H
