#ifndef QSPMSGDLG_H
#define QSPMSGDLG_H

#include "qsptextbox.h"

#include <QColor>
#include <QDialog>
#include <QFont>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>

namespace Ui
{
class QspMsgDlg;
}

class QspMsgDlg : public QDialog
{
    Q_OBJECT

public:
    explicit QspMsgDlg(QWidget *parent = 0);
    QspMsgDlg(const QString &caption = "", const QString &text = "", QWidget *parent = 0);
    QspMsgDlg(const QColor &backColor, const QColor &fontColor, const QFont &new_font, const QString &caption, const QString &text,
              bool isHtml, const QString &gamePath, QWidget *parent = 0);
    ~QspMsgDlg();

private:
    // Events
    void OnLinkClicked(const QUrl &url);

    // Fields
    QspTextBox m_desc;
    QVBoxLayout layout;
    QPushButton okButton;
};

#endif // QSPMSGDLG_H
