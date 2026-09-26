#include "callbacks_gui.h"

#include "comtools.h"
#include "qspinputdlg.h"
#include "qspmsgdlg.h"

#include <QCoreApplication>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QFileDialog>
#include <QFileInfo>
#include <QInputDialog>
#include <QLineEdit>
#include <QThread>
#include <QTimer>

#include <QAudio>
#include <QAudioOutput>
#include <QDebug>

#include <algorithm>
#include <cstring>
#ifdef _WEBBOX
#include "qspwebbox.h"
#endif

QString QSPCallBacks::m_gamePath;
MainWindow *QSPCallBacks::m_frame;
bool QSPCallBacks::m_isHtml;
QSPSounds QSPCallBacks::m_sounds;
float QSPCallBacks::m_volumeCoeff;
bool QSPCallBacks::m_isAllowHTML5Extras;
QString QSPCallBacks::m_gameFilePath;

namespace
{
float toLinearAmplitude(float coeff)
{
    return QAudio::convertVolume(coeff, QAudio::LogarithmicVolumeScale, QAudio::LinearVolumeScale);
}
} // namespace

void QSPCallBacks::Init(MainWindow *frame)
{
    m_frame = frame;
    m_volumeCoeff = 1.0F;

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
    QSPSetCallBack(QSP_CALL_SHOWMENU, (QSP_CALLBACK)&ShowMenu);
    QSPSetCallBack(QSP_CALL_INPUTBOX, (QSP_CALLBACK)&Input);
    QSPSetCallBack(QSP_CALL_SHOWIMAGE, (QSP_CALLBACK)&ShowImage);
    QSPSetCallBack(QSP_CALL_SHOWWINDOW, (QSP_CALLBACK)&ShowPane);
    // QSPSetCallBack(QSP_CALL_OPENGAME, (QSP_CALLBACK)&OpenGame); //replace
    QSPSetCallBack(QSP_CALL_OPENGAMESTATUS, (QSP_CALLBACK)&OpenGameStatus);
    QSPSetCallBack(QSP_CALL_SAVEGAMESTATUS, (QSP_CALLBACK)&SaveGameStatus);
    QSPSetCallBack(QSP_CALL_DEBUG, (QSP_CALLBACK)&Debug);
}

void QSPCallBacks::DeInit()
{
    CloseFile(nullptr);
}

void QSPCallBacks::SetTimer(int msecs)
{
    if (m_frame->IsQuit())
    {
        return;
    }
    if (msecs)
    {
        m_frame->GetTimer()->start(msecs);
    }
    else
    {
        m_frame->GetTimer()->stop();
    }
}

void QSPCallBacks::RefreshInt(QSP_BOOL isRedraw)
{
    static int oldFullRefreshCount = 0;
    int i;
    int numVal;
    bool isScroll;
    bool isCanSave;
    QSP_CHAR *strVal;
    if (m_frame->IsQuit())
    {
        return;
    }
    // -------------------------------
    UpdateGamePath(m_frame->gameFilePath());
    // -------------------------------
    const QSP_CHAR *mainDesc = QSPGetMainDesc();
    const QSP_CHAR *varsDesc = QSPGetVarsDesc();
    // -------------------------------
    isScroll = !(QSPGetVarValues(QSP_VAR("DISABLESCROLL"), 0, &numVal, &strVal) && numVal);
    isCanSave = !(QSPGetVarValues(QSP_VAR("NOSAVE"), 0, &numVal, &strVal) && numVal);
    m_isHtml = QSPGetVarValues(QSP_VAR("USEHTML"), 0, &numVal, &strVal) && numVal;
    // -------------------------------
    m_frame->GetVars()->SetIsHtml(m_isHtml);
    if (QSPIsVarsDescChanged())
    {
        m_frame->EnableControls(false, true);
        if (m_isAllowHTML5Extras)
        {
            if (QSPGetVarValues(QSP_VAR("SETSTATHEAD"), 0, &numVal, &strVal) && strVal)
            {
                m_frame->GetVars()->SetHead(QSPTools::qspStrToQt(strVal));
            }
            else
            {
                m_frame->GetVars()->SetHead(QString(""));
            }
        }
        m_frame->GetVars()->SetText(QSPTools::qspStrToQt(varsDesc), isScroll);
        m_frame->EnableControls(true, true);
    }
    // -------------------------------
    int fullRefreshCount = QSPGetFullRefreshCount();
    if (oldFullRefreshCount != fullRefreshCount)
    {
        isScroll = false;
        oldFullRefreshCount = fullRefreshCount;
    }
    m_frame->GetDesc()->SetIsHtml(m_isHtml);
    if (QSPIsMainDescChanged())
    {
        m_frame->EnableControls(false, true);
        if (m_isAllowHTML5Extras)
        {
            if (QSPGetVarValues(QSP_VAR("SETMAINDESCHEAD"), 0, &numVal, &strVal) && strVal)
            {
                m_frame->GetDesc()->SetHead(QSPTools::qspStrToQt(strVal));
            }
            else
            {
                m_frame->GetDesc()->SetHead(QString(""));
            }
        }
        m_frame->GetDesc()->SetText(QSPTools::qspStrToQt(mainDesc), isScroll);
        m_frame->EnableControls(true, true);
    }
    // -------------------------------
    m_frame->GetActions()->SetIsHtml(m_isHtml);
    m_frame->GetActions()->SetIsShowNums(m_frame->IsShowHotkeys());
    if (QSPIsActionsChanged())
    {
        int actionsCount = QSPGetActions(nullptr, 0);
        QSPListItem *actions = new QSPListItem[actionsCount];
        QSPGetActions(actions, actionsCount);
        m_frame->GetActions()->BeginItems();
        for (i = 0; i < actionsCount; ++i)
        {
            m_frame->GetActions()->AddItem(QSPTools::GetCaseInsensitiveFilePath(m_gamePath, QSPTools::qspStrToQt(actions[i].Image)),
                                           QSPTools::qspStrToQt(actions[i].Name));
        }
        m_frame->GetActions()->EndItems();
        delete[] actions;
    }
    m_frame->GetActions()->SetSelection(QSPGetSelActionIndex());
    m_frame->GetObjects()->SetIsHtml(m_isHtml);
    if (QSPIsObjectsChanged())
    {
        int objectsCount = QSPGetObjects(nullptr, 0);
        QSPListItem *objects = new QSPListItem[objectsCount];
        QSPGetObjects(objects, objectsCount);
        m_frame->GetObjects()->BeginItems();
        for (i = 0; i < objectsCount; ++i)
        {
            m_frame->GetObjects()->AddItem(QSPTools::GetCaseInsensitiveFilePath(m_gamePath, QSPTools::qspStrToQt(objects[i].Image)),
                                           QSPTools::qspStrToQt(objects[i].Name));
        }
        m_frame->GetObjects()->EndItems();
        delete[] objects;
    }
    m_frame->GetObjects()->SetSelection(QSPGetSelObjectIndex());
    // -------------------------------
    if (QSPGetVarValues(QSP_VAR("BACKIMAGE"), 0, &numVal, &strVal) && strVal)
    {
        m_frame->GetDesc()->LoadBackImage(QSPTools::GetCaseInsensitiveFilePath(m_gamePath, QSPTools::qspStrToQt(strVal)));
    }
    else
    {
        m_frame->GetDesc()->LoadBackImage(QString(""));
    }
    // -------------------------------
    m_frame->ApplyParams();
    if (isRedraw)
    {
        m_frame->EnableControls(false, true);
        // m_frame->Update();
        // QCoreApplication::processEvents();
        if (m_frame->IsQuit())
        {
            return;
        }
        m_frame->EnableControls(true, true);
    }
    m_frame->GetGameMenu()->setEnabled(isCanSave);
}

void QSPCallBacks::SetInputStrText(const QSP_CHAR *text)
{
    if (m_frame->IsQuit())
    {
        return;
    }
    m_frame->GetInput()->SetText(QSPTools::qspStrToQt(text));
}

QSP_BOOL QSPCallBacks::IsPlay(const QSP_CHAR *file)
{
    QSP_BOOL playing = QSP_FALSE;
    QSPSounds::iterator elem = m_sounds.find(
        QFileInfo(m_gamePath + QSPTools::GetCaseInsensitiveFilePath(m_gamePath, QSPTools::qspStrToQt(file))).absoluteFilePath());
    if (elem != m_sounds.end())
    {
        // Qt 6 reaches PlayingState asynchronously; loading and buffering
        // count as playing so the engine does not restart a starting track.
        if (elem->second.player->playbackState() == QMediaPlayer::PlaybackState::PlayingState
            || elem->second.player->mediaStatus() == QMediaPlayer::LoadingMedia
            || elem->second.player->mediaStatus() == QMediaPlayer::BufferingMedia)
        {
            playing = QSP_TRUE;
        }
    }
    return playing;
}

void QSPCallBacks::CloseFile(const QSP_CHAR *file)
{
    if (file)
    {
        QSPSounds::iterator elem = m_sounds.find(
            QFileInfo(m_gamePath + QSPTools::GetCaseInsensitiveFilePath(m_gamePath, QSPTools::qspStrToQt(file))).absoluteFilePath());
        if (elem != m_sounds.end())
        {
            m_sounds.erase(elem);
        }
    }
    else
    {
        m_sounds.clear();
    }
}

void QSPCallBacks::PlayFile(const QSP_CHAR *file, int volume)
{
    if (SetVolume(file, volume))
    {
        return;
    }
    CloseFile(file);
    const QString strFile(
        QFileInfo(m_gamePath + QSPTools::GetCaseInsensitiveFilePath(m_gamePath, QSPTools::qspStrToQt(file))).absoluteFilePath());
    const QFileInfo fileInfo(strFile);
    if (!fileInfo.exists() || !fileInfo.isFile())
    {
        qWarning() << "Audio file not found:" << strFile;
        return;
    }
    if (!fileInfo.isReadable())
    {
        qWarning() << "Audio file not readable:" << strFile;
        return;
    }
    auto emplaced = m_sounds.emplace(strFile, QSPSound());
    QSPSound &snd = emplaced.first->second;
    snd.baseVolume = std::clamp(static_cast<float>(volume) / 100.0f, 0.0f, 1.0f);
    QObject::connect(snd.player, &QMediaPlayer::errorOccurred, [strFile](QMediaPlayer::Error, const QString &errorString) {
        qWarning() << "Audio playback error for" << strFile << ":" << errorString;
    });
    snd.player->setSource(QUrl::fromLocalFile(strFile));
    snd.output->setVolume(snd.baseVolume * toLinearAmplitude(m_volumeCoeff));
    snd.player->play();
    UpdateSounds();
}

void QSPCallBacks::Debug(const QSP_CHAR *str)
{
    if (m_frame->IsQuit())
    {
        return;
    }
    m_frame->appendDebugLine(QSPTools::qspStrToQt(str));
}

void QSPCallBacks::ShowPane(int type, QSP_BOOL isShow)
{
    if (m_frame->IsQuit())
    {
        return;
    }
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
    QObject::connect(&wtimer, &QTimer::timeout, &loop, &QEventLoop::quit);
    wtimer.start(50);
    loop.exec();
    // RefreshInt(QSP_TRUE);
    if (m_frame->IsQuit())
    {
        return;
    }
    bool isSave = m_frame->GetGameMenu()->isEnabled();
    bool isBreak = false;
    m_frame->EnableControls(false, true);
    int i;
    int count = msecs / 50;
    for (i = 0; i < count; ++i)
    {
        // QThread::msleep(50);
        wtimer.start(50);
        loop.exec();
        // qDebug() << QSPTools::qspStrToQt(QSPGetMainDesc());
        // m_frame->Update();
        // QCoreApplication::processEvents();
        if (m_frame->IsQuit() || m_frame->IsKeyPressedWhileDisabled()) // TODO: implement
        {
            isBreak = true;
            break;
        }
    }
    if (!isBreak) // NOTE: no check in old code
    {
        // QThread::msleep(msecs % 50);
        wtimer.start(msecs % 50);
        loop.exec();
        // m_frame->Update();
        // QCoreApplication::processEvents();
    }
    m_frame->EnableControls(true, true);
    m_frame->GetGameMenu()->setEnabled(isSave);
}

int QSPCallBacks::GetMSCount()
{
    static QElapsedTimer stopWatch;
    if (stopWatch.isValid() == false)
    {
        stopWatch.start();
    }
    int ret = stopWatch.restart();
    return ret;
}

void QSPCallBacks::Msg(const QSP_CHAR *str)
{
    if (m_frame->IsQuit())
    {
        return;
    }
    RefreshInt(QSP_FALSE);
    QspMsgDlg dialog(m_frame->GetDesc()->GetBackgroundColor(), m_frame->GetDesc()->GetForegroundColor(), m_frame->GetDesc()->GetTextFont(),
                     MainWindow::tr("Info"), // caption
                     QSPTools::qspStrToQt(str), m_isHtml, m_gamePath, m_frame);
    m_frame->EnableControls(false);
    dialog.exec();
    m_frame->EnableControls(true);
}

void QSPCallBacks::DeleteMenu()
{
    if (m_frame->IsQuit())
    {
        return;
    }
    m_frame->DeleteMenu();
}

int QSPCallBacks::ShowMenu(QSPListItem *items, int count)
{
    if (m_frame->IsQuit())
    {
        return -1;
    }
    m_frame->EnableControls(false);
    int index = m_frame->ShowMenu(items, count);
    m_frame->EnableControls(true);
    return index;
}

void QSPCallBacks::Input(const QSP_CHAR *text, QSP_CHAR *buffer, int maxLen)
{
    if (m_frame->IsQuit())
    {
        return;
    }
    RefreshInt(QSP_FALSE);
    //  QSPInputDlg dialog(m_frame,
    //      wxID_ANY,
    //      m_frame->GetDesc()->GetBackgroundColor(),
    //      m_frame->GetDesc()->GetForegroundColor(),
    //      m_frame->GetDesc()->GetTextFont(),
    //      _("Input data"),
    //      wxString(text.Str, text.End),
    //      m_isHtml,
    //      m_gamePath
    //  );
    //  m_frame->EnableControls(false);
    //  dialog.ShowModal();
    //  m_frame->EnableControls(true);
    //  #ifdef _UNICODE
    //      wcsncpy(buffer, dialog.GetText().c_str(), maxLen);
    //  #else
    //      strncpy(buffer, dialog.GetText().c_str(), maxLen);
    //  #endif
    // QString inputText = QInputDialog::getMultiLineText(m_frame, MainWindow::tr("Input data"), QSPTools::qspStrToQt(text));
    QString inputText = QInputDialog::getText(m_frame, MainWindow::tr("Input data"), QSPTools::qspStrToQt(text), QLineEdit::Normal);
    // QSP_CHAR is uint16_t and QString::utf16() provides the same-width
    // buffer; copy like strncpy: at most maxLen - 1 units, zero-padded.
    const int copyLen = std::min(maxLen - 1, static_cast<int>(inputText.length()));
    if (copyLen > 0)
    {
        memcpy(buffer, inputText.utf16(), copyLen * sizeof(QSP_CHAR));
    }
    memset(buffer + copyLen, 0, (maxLen - copyLen) * sizeof(QSP_CHAR));
}

void QSPCallBacks::ShowImage(const QSP_CHAR *file)
{
    if (m_frame->IsQuit())
    {
        return;
    }
    m_frame->GetImgView()->OpenFile(
        QSPTools::GetCaseInsensitiveFilePath(m_gamePath, QSPTools::qspStrToQt(file))); // NOTE: will not display image if file is not found
    if (QSPTools::qspStrToQt(file) == "")
    {
        m_frame->GetImageDock()->setVisible(false);
    }
    else
    {
        m_frame->GetImageDock()->setVisible(true);
    }

    // m_frame->GetImgView()->setVisible(true);
}

// void QSPCallBacks::OpenGame(const QSP_CHAR *file, QSP_BOOL isNewGame)
//{
//   if (m_frame->IsQuit()) return;
//   if (QSPLoadGameWorld(file, isNewGame) && isNewGame)
//   {
//         QFileInfo fileName(QSPTools::qspStrToQt(file));
//         m_gamePath = fileName.canonicalPath();
//         if(!m_gamePath.endsWith('/')) m_gamePath+="/";
//       m_frame->UpdateGamePath(m_gamePath);
//   }
// }

void QSPCallBacks::OpenGameStatus(const QSP_CHAR *file)
{
    if (m_frame->IsQuit())
    {
        return;
    }
    if (file)
    {
        QFileInfo fileInfo(QSPTools::qspStrToQt(file));
        if (fileInfo.exists() && fileInfo.isFile())
        {
            QSPOpenSavedGameFromFile(file, QSP_FALSE);
        }
    }
    else
    {
        m_frame->EnableControls(false);
        QString path = QFileDialog::getOpenFileName(m_frame, MainWindow::tr("Select saved game file"), m_frame->GetLastPath(),
                                                    MainWindow::tr("Saved game files (*.sav)"));
        m_frame->EnableControls(true);
        if (!path.isEmpty())
        {
            m_frame->SetLastPath(QFileInfo(path).canonicalPath());
            QSPOpenSavedGameFromFile(qspStringFromQString(path), QSP_FALSE);
        }
    }
}

void QSPCallBacks::SaveGameStatus(const QSP_CHAR *file)
{
    if (m_frame->IsQuit())
    {
        return;
    }
    if (file)
    {
        QSPSaveGameAsFile(file, QSP_FALSE);
    }
    else
    {
        m_frame->EnableControls(false);
        QString path = QFileDialog::getSaveFileName(m_frame, MainWindow::tr("Select file to save"), m_frame->GetLastPath(),
                                                    MainWindow::tr("Saved game files (*.sav)"));
        m_frame->EnableControls(true);
        if (!path.isEmpty())
        {
            m_frame->SetLastPath(QFileInfo(path).canonicalPath());
            QSPSaveGameAsFile(qspStringFromQString(path), QSP_FALSE);
        }
    }
}

void QSPCallBacks::SetGameFilePath(const QString &filePath)
{
    m_gamePath = QSPTools::GameDirFromFilePath(filePath);
    m_frame->GetDesc()->SetGamePath(m_gamePath);
    m_frame->GetObjects()->SetGamePath(m_gamePath);
    m_frame->GetActions()->SetGamePath(m_gamePath);
    m_frame->GetVars()->SetGamePath(m_gamePath);
    m_frame->GetImgView()->SetGamePath(m_gamePath);
}

void QSPCallBacks::UpdateGamePath(const QString &filePath)
{
    if (QFileInfo(filePath).absoluteFilePath() == m_gameFilePath)
    {
        return;
    }
    SetGameFilePath(filePath);
}

bool QSPCallBacks::SetVolume(const QSP_CHAR *file, int volume)
{
    if (!IsPlay(file))
    {
        return false;
    }
    QSPSounds::iterator elem = m_sounds.find(
        QString(QFileInfo(m_gamePath + QSPTools::GetCaseInsensitiveFilePath(m_gamePath, QSPTools::qspStrToQt(file))).absoluteFilePath()));
    QSPSound &snd = elem->second;
    if (snd.player->playbackState() != QMediaPlayer::PlaybackState::StoppedState)
    {
        snd.baseVolume = static_cast<float>(volume) / 100.0f;
        snd.output->setVolume(snd.baseVolume * toLinearAmplitude(m_volumeCoeff));
        return true;
    }
    return false;
}

void QSPCallBacks::SetOverallVolume(float coeff)
{
    if (coeff < 0.0)
    {
        coeff = 0.0;
    }
    else if (coeff > 1.0)
    {
        coeff = 1.0;
    }
    m_volumeCoeff = coeff;
    const float amp = toLinearAmplitude(m_volumeCoeff);
    for (auto &entry : m_sounds)
    {
        entry.second.output->setVolume(entry.second.baseVolume * amp);
    }
}

void QSPCallBacks::SetAllowHTML5Extras(bool HTML5Extras)
{
    m_isAllowHTML5Extras = HTML5Extras;
}

void QSPCallBacks::UpdateSounds()
{
    for (auto i = m_sounds.begin(); i != m_sounds.end();)
    {
        if (i->second.player->playbackState() != QMediaPlayer::PlaybackState::StoppedState
            || i->second.player->mediaStatus() == QMediaPlayer::LoadingMedia
            || i->second.player->mediaStatus() == QMediaPlayer::BufferingMedia)
        {
            ++i;
        }
        else
        {
            i = m_sounds.erase(i);
        }
    }
}
#include <QMediaPlayer>
#include <QObject>
#include <QUrl>
