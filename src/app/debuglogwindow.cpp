#include "debuglogwindow.h"

#include <QApplication>
#include <QClipboard>
#include <QToolBar>

DebugLogWindow::DebugLogWindow(QWidget *parent) : QMainWindow(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("QSP debug log"));
    setAttribute(Qt::WA_DeleteOnClose);
    resize(500, 400);

    logText = new QPlainTextEdit(this);
    logText->setReadOnly(true);
    logText->setMaximumBlockCount(5000);
    setCentralWidget(logText);

    QToolBar *toolbar = addToolBar(tr("Debug"));
    toolbar->setMovable(false);
    clearAction = toolbar->addAction(tr("Clear"));
    connect(clearAction, &QAction::triggered, logText, &QPlainTextEdit::clear);
    copyAction = toolbar->addAction(tr("Copy all"));
    connect(copyAction, &QAction::triggered, this, [this] {
        QGuiApplication::clipboard()->setText(logText->toPlainText());
    });
}

void DebugLogWindow::appendLine(const QString &line)
{
    logText->appendPlainText(line);
}