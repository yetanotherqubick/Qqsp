#ifndef DEBUGLOGWINDOW_H
#define DEBUGLOGWINDOW_H

#include <QMainWindow>
#include <QPlainTextEdit>

class QAction;

class DebugLogWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit DebugLogWindow(QWidget *parent = nullptr);

    void appendLine(const QString &line);

private:
    QPlainTextEdit *logText;
    QAction *clearAction;
    QAction *copyAction;
};

#endif
