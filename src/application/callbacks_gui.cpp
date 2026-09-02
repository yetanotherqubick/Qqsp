#include "callbacks_gui.h"

#include <QElapsedTimer>
#include <QFileInfo>
#include <QFileDialog>
#include <QInputDialog>
#include <QLineEdit>
#include <QTimer>
#include <QEventLoop>

#include "comtools.h"
#include "qspmsgdlg.h"

QString QSPCallBacks::m_gamePath;
MainWindow *QSPCallBacks::m_frame;
bool QSPCallBacks::m_isHtml;
QSPSounds QSPCallBacks::m_sounds;
float QSPCallBacks::m_volumeCoeff;
bool QSPCallBacks::m_isAllowHTML5Extras;

namespace
{
QString soundFilePath(const QString &gamePath, const QSP_CHAR *file)
{
    const QString path = QSPTools::GetCaseInsensitiveFilePath(gamePath, QSPTools::qspStrToQt(file));
    return QFileInfo(gamePath + path).absoluteFilePath();
}
}

void QSPCallBacks::Init(MainWindow *frame)
{
	m_frame = frame;
    m_volumeCoeff = 1.0f;

    m_isAllowHTML5Extras = false;

	QSPSetCallBack(QSP_CALL_SETTIMER, (QSP_CALLBACK)&SetTimer);
	QSPSetCallBack(QSP_CALL_REFRESHINT, (QSP_CALLBACK)&RefreshInt);
	QSPSetCallBack(QSP_CALL_SETINPUTSTRTEXT, (QSP_CALLBACK)&SetInputStrText);
	QSPSetCallBack(QSP_CALL_ISPLAYINGFILE, (QSP_CALLBACK)&IsPlay);
	QSPSetCallBack(QSP_CALL_PLAYFILE, (QSP_CALLBACK)&PlayFile);
	QSPSetCallBack(QSP_CALL_CLOSEFILE, (QSP_CALLBACK)&CloseFile);
	QSPSetCallBack(QSP_CALL_SHOWMSGSTR, (QSP_CALLBACK)&Msg);
	QSPSetCallBack(QSP_CALL_SLEEP, (QSP_CALLBACK)&Sleep);
	QSPSetCallBack(QSP_CALL_GETMSCOUNT, (QSP_CALLBACK)&GetMSCount);
    QSPSetCallBack(QSP_CALL_DELETEMENU, (QSP_CALLBACK)&DeleteMenu);
    QSPSetCallBack(QSP_CALL_ADDMENUITEM, (QSP_CALLBACK)&AddMenuItem);
	QSPSetCallBack(QSP_CALL_SHOWMENU, (QSP_CALLBACK)&ShowMenu);
	QSPSetCallBack(QSP_CALL_INPUTBOX, (QSP_CALLBACK)&Input);
	QSPSetCallBack(QSP_CALL_SHOWIMAGE, (QSP_CALLBACK)&ShowImage);
	QSPSetCallBack(QSP_CALL_SHOWWINDOW, (QSP_CALLBACK)&ShowPane);
	QSPSetCallBack(QSP_CALL_OPENGAMESTATUS, (QSP_CALLBACK)&OpenGameStatus);
	QSPSetCallBack(QSP_CALL_SAVEGAMESTATUS, (QSP_CALLBACK)&SaveGameStatus);
}

void QSPCallBacks::DeInit()
{
    CloseFile(0);
}

void QSPCallBacks::SetTimer(int msecs)
{
	if (m_frame->IsQuit()) return;
	if (msecs)
        m_frame->GetTimer()->start(msecs);
	else
        m_frame->GetTimer()->stop();
}

void QSPCallBacks::RefreshInt(QSP_BOOL isRedraw)
{
	static int oldFullRefreshCount = 0;
	int i, numVal;
	bool isScroll, isCanSave;
    QSP_CHAR *strVal, *imgPath;
	if (m_frame->IsQuit()) return;
    UpdateGamePath();
    const QSP_CHAR *mainDesc = QSPGetMainDesc();
    const QSP_CHAR *varsDesc = QSPGetVarsDesc();
    isScroll = !(QSPGetVarValues(QSP_FMT("DISABLESCROLL"), 0, &numVal, &strVal) && numVal);
    isCanSave = !(QSPGetVarValues(QSP_FMT("NOSAVE"), 0, &numVal, &strVal) && numVal);
    const bool isHtml = QSPGetVarValues(QSP_FMT("USEHTML"), 0, &numVal, &strVal) && numVal;
    const bool isHtmlChanged = m_isHtml != isHtml;
    m_isHtml = isHtml;
    const int fullRefreshCount = QSPGetFullRefreshCount();
    if (oldFullRefreshCount != fullRefreshCount)
    {
        isScroll = false;
        oldFullRefreshCount = fullRefreshCount;
    }
	m_frame->GetVars()->SetIsHtml(m_isHtml, false);
	const bool varsChanged = QSPIsVarsDescChanged();
	if (varsChanged)
	{
        m_frame->EnableControls(false, true);
        if(m_isAllowHTML5Extras)
        {
            if (QSPGetVarValues(QSP_FMT("SETSTATHEAD"), 0, &numVal, &strVal) && strVal)
                m_frame->GetVars()->SetHead(QSPTools::qspStrToQt(strVal), false);
            else
                 m_frame->GetVars()->SetHead(QString(), false);
        }
        m_frame->GetVars()->SetText(QSPTools::qspStrToQt(varsDesc), isScroll, false);
        m_frame->EnableControls(true, true);
	}
    if (isHtmlChanged || varsChanged)
        m_frame->GetVars()->RefreshUI(isScroll);
	m_frame->GetDesc()->SetIsHtml(m_isHtml, false);
    const bool mainDescChanged = QSPIsMainDescChanged();
    if (mainDescChanged)
    {
        m_frame->EnableControls(false, true);
        if(m_isAllowHTML5Extras)
        {
            if (QSPGetVarValues(QSP_FMT("SETMAINDESCHEAD"), 0, &numVal, &strVal) && strVal)
                m_frame->GetDesc()->SetHead(QSPTools::qspStrToQt(strVal), false);
            else
                 m_frame->GetDesc()->SetHead(QString(), false);
        }
        m_frame->GetDesc()->SetText(QSPTools::qspStrToQt(mainDesc), isScroll, false);
        m_frame->EnableControls(true, true);
	}
    if (isHtmlChanged || mainDescChanged)
        m_frame->GetDesc()->RefreshUI(isScroll);
	m_frame->GetActions()->SetIsHtml(m_isHtml);
	m_frame->GetActions()->SetIsShowNums(m_frame->IsShowHotkeys());
    if (QSPIsActionsChanged())
    {
        const int actionsCount = QSPGetActionsCount();
        m_frame->GetActions()->BeginItems();
        for (i = 0; i < actionsCount; ++i)
        {
            QSPGetActionData(i, &imgPath, &strVal);
            m_frame->GetActions()->AddItem(QSPTools::GetCaseInsensitiveFilePath(m_gamePath, QSPTools::qspStrToQt(imgPath)), QSPTools::qspStrToQt(strVal));
        }
        m_frame->GetActions()->EndItems();
    }
	m_frame->GetActions()->SetSelection(QSPGetSelActionIndex());
	m_frame->GetObjects()->SetIsHtml(m_isHtml);
    if (QSPIsObjectsChanged())
    {
        const int objectsCount = QSPGetObjectsCount();
        m_frame->GetObjects()->BeginItems();
        for (i = 0; i < objectsCount; ++i)
        {
            QSPGetObjectData(i, &imgPath, &strVal);
            m_frame->GetObjects()->AddItem(QSPTools::GetCaseInsensitiveFilePath(m_gamePath, QSPTools::qspStrToQt(imgPath)), QSPTools::qspStrToQt(strVal));
        }
        m_frame->GetObjects()->EndItems();
    }
	m_frame->GetObjects()->SetSelection(QSPGetSelObjectIndex());
    if (QSPGetVarValues(QSP_FMT("BACKIMAGE"), 0, &numVal, &strVal) && strVal)
        m_frame->GetDesc()->LoadBackImage(QSPTools::GetCaseInsensitiveFilePath(m_gamePath, QSPTools::qspStrToQt(strVal)));
	else
        m_frame->GetDesc()->LoadBackImage(QString());
    m_frame->ApplyParams();
	if (isRedraw)
	{
		m_frame->EnableControls(false, true);
		if (m_frame->IsQuit()) return;
		m_frame->EnableControls(true, true);
	}
    m_frame->GetGameMenu()->setEnabled(isCanSave);
}

void QSPCallBacks::SetInputStrText(const QSP_CHAR *text)
{
	if (m_frame->IsQuit()) return;
    m_frame->GetInput()->SetText(QSPTools::qspStrToQt(text));
}

QSP_BOOL QSPCallBacks::IsPlay(const QSP_CHAR *file)
{
    const auto elem = m_sounds.find(soundFilePath(m_gamePath, file));
    return elem != m_sounds.end() && elem.value()->state() == QMediaPlayer::PlayingState
            ? QSP_TRUE : QSP_FALSE;
}

void QSPCallBacks::CloseFile(const QSP_CHAR *file)
{
    if (file)
    {
        const auto elem = m_sounds.find(soundFilePath(m_gamePath, file));
        if (elem != m_sounds.end())
        {
            delete elem.value();
            m_sounds.erase(elem);
        }
        return;
    }

    for (auto *sound : m_sounds)
        delete sound;
    m_sounds.clear();
}

void QSPCallBacks::UpdateSounds()
{
    for (auto it = m_sounds.begin(); it != m_sounds.end();)
    {
        if (it.value()->state() != QMediaPlayer::PlayingState)
        {
            delete it.value();
            it = m_sounds.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void QSPCallBacks::PlayFile(const QSP_CHAR *file, int volume)
{
    if (SetVolume(file, volume)) return;
    CloseFile(file);
    const QString strFile = soundFilePath(m_gamePath, file);
    auto *snd = new QMediaPlayer;
    snd->setMedia(QUrl::fromLocalFile(strFile));
    snd->setVolume(volume*m_volumeCoeff);
    snd->play();
    m_sounds.insert(strFile, snd);
    UpdateSounds();
}

void QSPCallBacks::ShowPane(int type, QSP_BOOL isShow)
{
	if (m_frame->IsQuit()) return;
	switch (type)
	{
	case QSP_WIN_ACTS:
        m_frame->GetActionsDock()->setVisible(isShow != QSP_FALSE);
		break;
	case QSP_WIN_OBJS:
        m_frame->GetObjectsDock()->setVisible(isShow != QSP_FALSE);
		break;
	case QSP_WIN_VARS:
        m_frame->GetVarsDock()->setVisible(isShow != QSP_FALSE);
		break;
	case QSP_WIN_INPUT:
        m_frame->GetInputDock()->setVisible(isShow != QSP_FALSE);
		break;
	}
}

void QSPCallBacks::Sleep(int msecs)
{
    QTimer wtimer;
    wtimer.setSingleShot(true);
    QEventLoop loop;
    QObject::connect(&wtimer, SIGNAL(timeout()), &loop, SLOT(quit()));
    wtimer.start(50);
    loop.exec();
	if (m_frame->IsQuit()) return;
    bool isSave = m_frame->GetGameMenu()->isEnabled();
	bool isBreak = false;
    m_frame->EnableControls(false, true);
	int i, count = msecs / 50;
    for (i = 0; i < count; ++i)
    {
        wtimer.start(50);
        loop.exec();
		if (m_frame->IsQuit() ||
            m_frame->IsKeyPressedWhileDisabled()) //TODO: implement
		{
			isBreak = true;
			break;
		}
	}
    if (!isBreak)
    {
        wtimer.start(msecs % 50);
        loop.exec();
    }
	m_frame->EnableControls(true, true);
    m_frame->GetGameMenu()->setEnabled(isSave);
}

int QSPCallBacks::GetMSCount()
{
    static QElapsedTimer stopWatch;
    if(stopWatch.isValid() == false)
        stopWatch.start();
    int ret = stopWatch.restart();
	return ret;
}

void QSPCallBacks::Msg(const QSP_CHAR *str)
{
	if (m_frame->IsQuit()) return;
	RefreshInt(QSP_FALSE);
    QspMsgDlg dialog(m_frame->GetDesc()->GetBackgroundColor(),
        m_frame->GetDesc()->GetForegroundColor(),
		m_frame->GetDesc()->GetTextFont(),
        MainWindow::tr("Info"),
        QSPTools::qspStrToQt(str),
		m_isHtml,
        m_gamePath,
        m_frame
	);
	m_frame->EnableControls(false);
    dialog.exec();
	m_frame->EnableControls(true);
}

void QSPCallBacks::DeleteMenu()
{
    if (m_frame->IsQuit()) return;
    m_frame->DeleteMenu();
}

void QSPCallBacks::AddMenuItem(const QSP_CHAR *name, const QSP_CHAR *imgPath)
{
    if (m_frame->IsQuit()) return;
    m_frame->AddMenuItem(QSPTools::qspStrToQt(name), QSPTools::GetCaseInsensitiveFilePath(m_gamePath, QSPTools::qspStrToQt(imgPath)));
}

int QSPCallBacks::ShowMenu()
{
    if (m_frame->IsQuit()) return -1;
    m_frame->EnableControls(false);
    int index = m_frame->ShowMenu();
    m_frame->EnableControls(true);
    return index;
}

void QSPCallBacks::Input(const QSP_CHAR *text, QSP_CHAR *buffer, int maxLen)
{
	if (m_frame->IsQuit()) return;
    RefreshInt(QSP_FALSE);
    const QString inputText = QInputDialog::getText(m_frame, MainWindow::tr("Input data"), QSPTools::qspStrToQt(text), QLineEdit::Normal);
    c16sncpy(buffer, (QSP_CHAR *)(inputText.utf16()), maxLen);
}

void QSPCallBacks::ShowImage(const QSP_CHAR *file)
{
	if (m_frame->IsQuit()) return;
    const QString path = QSPTools::qspStrToQt(file);
    m_frame->GetImgView()->OpenFile(QSPTools::GetCaseInsensitiveFilePath(m_gamePath, path));
    m_frame->GetImageDock()->setVisible(!path.isEmpty());
}

void QSPCallBacks::OpenGameStatus(const QSP_CHAR *file)
{
	if (m_frame->IsQuit()) return;
    if (file)
	{
        const QFileInfo fileInfo(QSPTools::qspStrToQt(file));
        if ( fileInfo.exists() && fileInfo.isFile() ) QSPOpenSavedGame(file, QSP_FALSE);
	}
	else
	{
        m_frame->EnableControls(false);
        const QString path = QFileDialog::getOpenFileName(m_frame, MainWindow::tr("Select saved game file"), m_frame->GetLastPath(), MainWindow::tr("Saved game files (*.sav)"));
        m_frame->EnableControls(true);
        if (!path.isEmpty())
		{
            m_frame->SetLastPath(QFileInfo(path).canonicalPath());
            QSPOpenSavedGame(qspStringFromQString(path), QSP_FALSE);
		}
	}
}

void QSPCallBacks::SaveGameStatus(const QSP_CHAR *file)
{
	if (m_frame->IsQuit()) return;
    if (file)
		QSPSaveGame(file, QSP_FALSE);
	else
	{
        m_frame->EnableControls(false);
        const QString path = QFileDialog::getSaveFileName(m_frame, MainWindow::tr("Select file to save"), m_frame->GetLastPath(), MainWindow::tr("Saved game files (*.sav)"));
		m_frame->EnableControls(true);
        if (!path.isEmpty())
		{
            m_frame->SetLastPath(QFileInfo(path).canonicalPath());
            QSPSaveGame(qspStringFromQString(path), QSP_FALSE);
		}
	}
}

void QSPCallBacks::UpdateGamePath()
{
    const QFileInfo fileName(QSPTools::qspStrToQt(QSPGetQstFullPath()));
    m_gamePath = fileName.canonicalPath();
    if(!m_gamePath.endsWith("/")) m_gamePath+="/";
    m_frame->GetDesc()->SetGamePath(m_gamePath);
    m_frame->GetObjects()->SetGamePath(m_gamePath);
    m_frame->GetActions()->SetGamePath(m_gamePath);
    m_frame->GetVars()->SetGamePath(m_gamePath);
    m_frame->GetImgView()->SetGamePath(m_gamePath);
}

bool QSPCallBacks::SetVolume(const QSP_CHAR *file, int volume)
{
    const auto elem = m_sounds.find(soundFilePath(m_gamePath, file));
    if (elem == m_sounds.end() || elem.value()->state() != QMediaPlayer::PlayingState)
        return false;

    elem.value()->setVolume(volume*m_volumeCoeff);
	return true;
}

void QSPCallBacks::SetOverallVolume(float coeff)
{
    if (coeff < 0.0)
        coeff = 0.0;
    else if (coeff > 1.0)
        coeff = 1.0;
    m_volumeCoeff = coeff;
    for (auto *snd : m_sounds)
    {
        if (snd->state() == QMediaPlayer::PlayingState)
            snd->setVolume(snd->volume()*m_volumeCoeff);
    }
}

void QSPCallBacks::SetAllowHTML5Extras(bool HTML5Extras)
{
    m_isAllowHTML5Extras = HTML5Extras;
}
