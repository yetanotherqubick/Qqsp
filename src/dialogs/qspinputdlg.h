#ifndef QSPINPUTDLG_H
#define QSPINPUTDLG_H

#include "qsptextbox.h"

#include <QColor>
#include <QFont>
#include <QString>
#include <QWidget>

// TODO: implement this to replace generic QInputDialog

namespace Ui
{
class QspInputDlg;
}

class QspInputDlg : public QWidget
{
    Q_OBJECT

public:
    explicit QspInputDlg(QWidget *parent = 0);
    ~QspInputDlg();
    QspInputDlg(const QColor &backColor, const QColor &fontColor, const QFont &font, const QString &caption, const QString &text,
                bool isHtml, const QString &gamePath, QWidget *parent = 0);

private:
    QString GetText() const
    {
        return m_text;
    }
    QspTextBox m_desc;
    QString m_text;
};

#endif // QSPINPUTDLG_H
