#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QColor>
#include <QFont>
#include <QPalette>
#include <QAction>
#include <QDockWidget>
#include <QMenu>
#include <QMenuBar>
#include <QTimer>
#include <QToolBar>

#include "qsptextbox.h"
#include "qsplistbox.h"
#include "qspinputbox.h"
#include "qspimgcanvas.h"
#ifdef _WEBBOX
#include <QWebEngineProfile>
#include "qspwebbox.h"
#include "qspwebengineurlrequestinterceptor.h"
#include "qspwebengineurlschemehandler.h"
#endif
#ifdef _WEBBOX_WEBKIT
#include "qspwebbox_webkit.h"
#endif

#include <qsp_default.h>

#define QSP_LOGO "Qt Quest Soft Player"
#define QSP_APPNAME "Qqsp"
#define QSP_CONFIG "qqsp.ini"
#define QSP_CUSTOM_CONFIG "custom.ini"
#define QSP_QUICKSAVE "quicksave.sav"
#define QSP_GAME "game.qsp"

class QCloseEvent;
class QDragEnterEvent;
class QDropEvent;
class QKeyEvent;
class QListWidgetItem;
class QUrl;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // Methods
    void EnableControls(bool status, bool isExtended = false);
    void ApplyParams();
    void DeleteMenu();
    void AddMenuItem(const QString &name, const QString &imgPath);
    int ShowMenu();
    void UpdateGamePath(const QString &path);
    void ShowError();

    // Accessors
    QTimer *GetTimer() const { return m_timer; }
#ifndef _WEBBOX_COMMON
    QspTextBox *GetDesc() const { return _mainDescTextBox; }
    QspTextBox *GetVars() const { return _descTextBox; }
#else
    QspWebBox *GetDesc() const { return _mainDescTextBox; }
    QspWebBox *GetVars() const { return _descTextBox; }
#endif
    QspInputBox *GetInput() const { return _inputTextBox; }
    QspListBox *GetActions() const { return _actionsListBox; }
    QspListBox *GetObjects() const { return _objectsListBox; }

    QDockWidget *GetVarsDock() const { return _descWidget; }
    QDockWidget *GetInputDock() const { return _inputWidget; }
    QDockWidget *GetActionsDock() const { return _actionsWidget; }
    QDockWidget *GetObjectsDock() const { return _objectsWidget; }
    QDockWidget *GetImageDock() const { return _imgViewWidget; }

    QString GetLastPath() const { return lastPath; }
    void SetLastPath(const QString &path) { lastPath = path; }

    QspImgCanvas *GetImgView() const { return m_imgView; }
    QMenu *GetGameMenu() const { return _gameMenu; }
    bool IsShowHotkeys() const { return m_isShowHotkeys; }
    bool IsQuit() const { return m_isQuit; }
    bool IsKeyPressedWhileDisabled() const { return m_keyPressedWhileDisabled; }

    QColor GetLinkColor() const { return m_linkColor; }
    QColor GetBackgroundColor() const { return m_backColor; }
    QColor GetForegroundColor() const { return m_fontColor; }
    void SetLinkColor(const QColor &new_color) { m_settingsLinkColor = new_color; if (m_isUseLinkColor) ApplyLinkColor(new_color); }
    void SetBackgroundColor(const QColor &new_color) { m_settingsBackColor = new_color; if (m_isUseBackColor) ApplyBackColor(new_color); }
    void SetForegroundColor(const QColor &new_color) { m_settingsFontColor = new_color; if (m_isUseFontColor) ApplyFontColor(new_color); }

    void SetShowPlainText(bool isPlain);

    bool GetUseFontSize() const { return m_isUseFontSize; }
    void SetUseFontSize(bool isUseFontS) { m_isUseFontSize = isUseFontS; }
    int GetFontSize() const { return m_fontSize; }
    void SetFontSize(int fontS) { m_fontSize = fontS; }
    bool GetUseFont() const { return m_isUseFont; }
    void SetUseFont(bool isUseFont) { m_isUseFont = isUseFont; }
    QFont GetFont() const { return m_font; }
    void SetFont(const QFont &new_font) { m_font = new_font; if (m_isUseFont) ApplyFont(new_font, 2, 2); }
    bool GetAutostart() const { return autostartLastGame; }
    void SetAutostart(bool isAutostart) { autostartLastGame = isAutostart; }
    bool GetPerGameConfig() const { return perGameConfig; }
    void SetPerGameConfig(bool isPerGameConfig) { perGameConfig = isPerGameConfig; }
    bool GetUseBackColor() const { return m_isUseBackColor; }
    void SetUseBackColor(bool isUseBackColor) { m_isUseBackColor = isUseBackColor; }
    bool GetUseLinkColor() const { return m_isUseLinkColor; }
    void SetUseLinkColor(bool isUseLinkColor) { m_isUseLinkColor = isUseLinkColor; }
    bool GetUseFontColor() const { return m_isUseFontColor; }
    void SetUseFontColor(bool isUseFontColor) { m_isUseFontColor = isUseFontColor; }
    QString GetLangID() const { return langid; }
    void SetLangID(const QString &new_langid) { langid = new_langid; }
    float GetOverallVolume() const { return m_volume; }
    void SetOverallVolume(float new_volume);
    bool GetDisableVideo() const { return disableVideo; }
    void SetDisableVideo(bool isDisableVideo);
    bool GetVideoFix() const { return m_videoFix; }
    void SetVideoFix(bool isFix);
    void SetAllowHTML5Extras(bool HTML5Extras);
    bool GetAllowHTML5Extras() const { return m_isAllowHTML5Extras; }
    void SetUseCaseInsensitiveFilePath(bool CaseInsensitiveFilePath);
    bool GetUseCaseInsensitiveFilePath();

private:
    void CreateMenuBar();
    void CreateDockWindows();
    void LoadSettings(QString filePath = QString());
    void SaveSettings(QString filePath = QString());
    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void ActionsListBoxDoAction(int action);
    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);

    // Internal methods
    void UpdateTitle();
    void ReCreateGUI();
    void RefreshUI();
    void ApplyFont(const QFont &new_font, int fontType, int sizeType);
    bool ApplyFontColor(const QColor &color);
    bool ApplyBackColor(const QColor &color);
    bool ApplyLinkColor(const QColor &color);

    QMenuBar *mainMenuBar;
    QToolBar *mainToolBar;
    QMenu *_fileMenu;
    QMenu *_gameMenu;
    QMenu *_settingsMenu;
    QMenu *_showHideMenu;
#ifndef _WEBBOX_COMMON
    QspTextBox *_mainDescTextBox;
    QspTextBox *_descTextBox;
#else
    QspWebBox *_mainDescTextBox;
    QspWebBox *_descTextBox;
#endif
    QspListBox *_objectsListBox;
    QspListBox *_actionsListBox;

    QspInputBox *_inputTextBox;
    QDockWidget *_objectsWidget;
    QDockWidget *_actionsWidget;
    QDockWidget *_descWidget;
    QDockWidget *_inputWidget;
    QDockWidget *_mainDescWidget;
    QString lastPath;
    QString lastGame;

    // Fields
    bool m_isGameOpened;
    QString m_savedGamePath;
    QString m_configPath;
    bool perGameConfig;
    QString m_path;
    QTimer *m_timer;
    QspImgCanvas *m_imgView;
    QDockWidget *_imgViewWidget;
    int m_menuItemId;
    QMenu *m_menu;
    QColor m_backColor;
    QColor m_linkColor;
    QColor m_fontColor;
    QColor m_defaultBackColor;
    QColor m_defaultLinkColor;
    QColor m_defaultFontColor;
    QColor m_settingsBackColor;
    QColor m_settingsLinkColor;
    QColor m_settingsFontColor;
    bool m_isUseBackColor;
    bool m_isUseLinkColor;
    bool m_isUseFontColor;
    QPalette m_palette;
    int m_fontSize;
    QFont m_font;
    QFont m_defaultFont;
    bool m_isUseFontSize;
    bool m_isUseFont;
    bool m_isProcessEvents;
    bool m_isQuit;
    bool m_keyPressedWhileDisabled;
    bool m_isShowHotkeys;
    float m_volume;
    int m_menuIndex;
    bool showPlainText;
    bool showCaptions;
    bool autostartLastGame;
    QString langid;
    bool disableVideo;
    bool m_videoFix;
    bool m_isAllowHTML5Extras;

public slots:
    void OpenGameFile(const QString &path);

private slots:
    void OnOpenGame();
    void OnRestartGame();
    void OnOpenSavedGame();
    void OnSaveGame();
    void OnOpenQuickSavedGame();
    void OnQuickSaveGame();
    void OnOptions();
    void OnAbout();
    void OnToggleCaptions(bool checked);
    void OnToggleMenuBar(bool checked);
    void OnToggleHotkeys(bool checked);
    void OnToggleWinMode();
    void OnToggleShowPlainText(bool checked);
    void OnNewGame();
    void OnTimer();
    void OnLinkClicked(const QUrl &url);
    void OnObjectListBoxItemClicked(QListWidgetItem *itemClicked);
    void OnActionsListBoxItemClicked(QListWidgetItem *itemClicked);
    void OnObjectChange(int currentRow);
    void OnActionChange(int currentRow);
    void OnMenu(QAction *action);
    void OnInputTextChange();
    void OnInputTextEnter();
};

#endif // MAINWINDOW_H