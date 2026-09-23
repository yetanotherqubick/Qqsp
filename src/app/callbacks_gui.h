#ifndef CALLBACKS_GUI_H
#define CALLBACKS_GUI_H

#include "mainwindow.h"

#include <QAudioOutput>
#include <QMap>
#include <QAudioDevice>
#include <QMediaDevices>
#include <QUrl>
#include <QMediaPlayer>
#include <map>
#include <QString>
#include <qsp_default.h>

// A sound in playback: the player and its audio output travel together, the
// engine's volume is kept as the per-sound base volume (0..1, linear) and
// combined with the overall coefficient on every output volume change.
struct QSPSound
{
    QMediaPlayer *player;
    QAudioOutput *output;
    float baseVolume;

    QSPSound()
        : player(new QMediaPlayer())
        , output(new QAudioOutput())
        , baseVolume(1.0f)
    {
        player->setAudioOutput(output);
        output->setDevice(QMediaDevices::defaultAudioOutput());
    }
    ~QSPSound()
    {
        player->stop();
        delete output;
        delete player;
    }
    QSPSound(const QSPSound &) = delete;
    QSPSound &operator=(const QSPSound &) = delete;
};

typedef std::map<QString, QSPSound> QSPSounds;

// static QSPString qspStringFromPair(const QSP_CHAR *start, const QSP_CHAR *end)
//{
//     QSPString string;
//     string.Str = (QSP_CHAR *)start;
//     string.End = (QSP_CHAR *)end;
//     return string;
// }

// static QSPString qspStringFromLen(const QSP_CHAR *s, int len)
//{
//     QSPString string;
//     string.Str = (QSP_CHAR *)s;
//     string.End = (QSP_CHAR *)s + len;
//     return string;
// }

static const QSP_CHAR *qspStringFromQString(const QString &s)
{
    // QSPString string;
    // string.Str = (QSP_CHAR *)s.utf16();
    // string.End = (QSP_CHAR *)s.utf16() + s.length();
    return (QSP_CHAR *)s.utf16();
}

// Legacy QSP_CHAR is uint16_t while QSP_FMT() yields char16_t literals; the
// two are representation-identical but distinct C++ types, so bridge them.
#define QSP_VAR(name) reinterpret_cast<const QSP_CHAR *>(QSP_FMT(name))

class QSPCallBacks
{
public:
    // Methods
    static void Init(MainWindow *frame);
    static void DeInit();
    static void SetOverallVolume(float coeff);
    static void SetAllowHTML5Extras(bool HTML5Extras);

    // CallBacks
    static void RefreshInt(QSP_BOOL isRedraw);
    static void SetTimer(int msecs);
    static void SetInputStrText(const QSP_CHAR *text);
    static QSP_BOOL IsPlay(const QSP_CHAR *file);
    static void CloseFile(const QSP_CHAR *file);
    static void PlayFile(const QSP_CHAR *file, int volume);
    static void ShowPane(int type, QSP_BOOL isShow);
    static void Sleep(int msecs);
    static int GetMSCount();
    static void Msg(const QSP_CHAR *str);
    static void DeleteMenu();
    static int ShowMenu(QSPListItem *items, int count);
    static void Input(const QSP_CHAR *text, QSP_CHAR *buffer, int maxLen);
    static void ShowImage(const QSP_CHAR *file);
    // static void OpenGame(const QSP_CHAR *file, QSP_BOOL isNewGame);
    static void OpenGameStatus(const QSP_CHAR *file);
    static void SaveGameStatus(const QSP_CHAR *file);

    // Game path ownership: the loaded .qsp file path lives in MainWindow
    // (gameFilePath()); QSPCallBacks derives the game directory from it once
    // per change and propagates it to the widgets.
    static void SetGameFilePath(const QString &filePath);
    static void UpdateGamePath(const QString &filePath);

    static QString m_gamePath;

private:
    // Internal methods
    static void UpdateGamePath();
    static bool SetVolume(const QSP_CHAR *file, int volume);
    static void UpdateSounds();

    // Fields
    static MainWindow *m_frame;
    static bool m_isHtml;
    static QSPSounds m_sounds;
    static float m_volumeCoeff;
    static bool m_isAllowHTML5Extras;
    static QString m_gameFilePath;
};

#endif
