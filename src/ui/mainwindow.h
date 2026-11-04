#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWebEngineView>
#include <QWebEngineProfile>
#include <QWebEngineScript>
#include <QWebEngineScriptCollection>
#include <QWebEngineSettings>
#include <QTimer>
#include <QSettings>
#include <QSystemTrayIcon>
#include <memory>
#include "core/app_lock.h"

class LockOverlay;
class QStackedWidget;
class QWebEngineNotification;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(AppLock *lock, QWidget *parent = nullptr);
    ~MainWindow() override = default;

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onWebPageLoaded(bool ok);
    void checkInactivity();
    void triggerInactivityLock();
    void lockNow();
    void openSettingsDialog();
    void setPasscode();
    void removePasscode();
    void tryUnlock(const QString &passcode);
    void setTheme(const QString &themeName);
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);

private:
    void applyMyKOSPalette();
    void initTrayIcon();
    void handleWebNotification(std::unique_ptr<QWebEngineNotification> notification);
    void registerScript(const QString &name, const QString &resourcePath,
                        QWebEngineScript::InjectionPoint point = QWebEngineScript::DocumentReady);
    void injectScript(const QString &name, const QString &resourcePath);
    bool promptPasscode(AppLock *lock);
    void showAboutDialog();
    void refreshLockState();

    QWebEngineView *m_webView = nullptr;
    QWebEngineProfile *m_profile = nullptr;
    AppLock *m_lock = nullptr;
    int m_failedAttempts = 0;
    QTimer *m_inactivityTimer = nullptr;
    qint64 m_inactivityTimeoutMs = 5 * 60 * 1000LL;
    qint64 m_lastNativeActivity = 0;
    QStackedWidget *m_stack = nullptr;
    LockOverlay *m_overlay = nullptr;

    // Menús y acciones
    QMenu *m_themeMenu = nullptr;
    QActionGroup *m_themeGroup = nullptr;
    QAction *m_actThemeWaDark = nullptr;
    QAction *m_actThemeMyKOS = nullptr;
    QAction *m_actThemeWaLight = nullptr;

    QMenu *m_settingsMenu = nullptr;
    QAction *m_actSettings = nullptr;
    QAction *m_actSetPasscode = nullptr;
    QAction *m_actRemovePasscode = nullptr;
    QAction *m_actLockNow = nullptr;
    QAction *m_actAbout = nullptr;

    // Bandeja del sistema y notificaciones
    QSystemTrayIcon *m_trayIcon = nullptr;
    QMenu *m_trayMenu = nullptr;
    QAction *m_actTrayToggle = nullptr;
    QAction *m_actTrayLock = nullptr;
    std::shared_ptr<QWebEngineNotification> m_lastNotification;
};

#endif // MAINWINDOW_H
