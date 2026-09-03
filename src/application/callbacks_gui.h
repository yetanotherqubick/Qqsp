#ifndef CALLBACKS_GUI_H
#define CALLBACKS_GUI_H

#include <QString>
#include <qsp_default.h>
#include <QMap>
#include <QMediaPlayer>
#include "mainwindow.h"

using QSPSounds = QMap<QString, QMediaPlayer *>;

static const QSP_CHAR *qspStringFromQString(const QString &s)
{
    return (QSP_CHAR *)s.utf16();
}

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
    static void AddMenuItem(const QSP_CHAR *name, const QSP_CHAR *imgPath);
    static int ShowMenu();
    static void Input(const QSP_CHAR *text, QSP_CHAR *buffer, int maxLen);
    static void ShowImage(const QSP_CHAR *file);
    static void OpenGameStatus(const QSP_CHAR *file);
    static void SaveGameStatus(const QSP_CHAR *file);

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
};

#endif
