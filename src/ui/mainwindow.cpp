#include "mainwindow.h"
#include "ui/lockoverlay.h"

#include <QActionGroup>
#include <QApplication>
#include <QBoxLayout>
#include <QCheckBox>
#include <QCloseEvent>
#include <QColor>
#include <QDateTime>
#include <QDebug>
#include <QDialogButtonBox>
#include <QDir>
#include <QEvent>
#include <QFile>
#include <QIcon>
#include <QInputDialog>
#include <QKeySequence>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QRegularExpression>
#include <QSettings>
#include <QSpinBox>
#include <QStackedWidget>
#include <QUrl>
#include <QVariant>
#include <QWebEngineCookieStore>
#include <QWebEngineNotification>
#include <QWebEnginePermission>
#include <QWebEngineScript>
#include <QWebEngineScriptCollection>
#include <QWebEngineSettings>

MainWindow::MainWindow(AppLock *lock, QWidget *parent)
    : QMainWindow(parent), m_lock(lock)
{
    setWindowTitle("MyKOS-Whats");
    setWindowIcon(QIcon(":/icons/app.png"));
    resize(1200, 800);
    applyMyKOSPalette();

    // Registrar también la actividad fuera de la página web (menús, diálogos)
    m_lastNativeActivity = QDateTime::currentMSecsSinceEpoch();
    qApp->installEventFilter(this);

    // Perfil web dedicado para persistir la sesión de WhatsApp
    m_profile = new QWebEngineProfile("mykos-whats-profile", this);
    m_profile->setPersistentCookiesPolicy(QWebEngineProfile::ForcePersistentCookies);

    // Integración de notificaciones nativas de escritorio
    m_profile->setNotificationPresenter([this](std::unique_ptr<QWebEngineNotification> notif) {
        handleWebNotification(std::move(notif));
    });

    // User-Agent: WhatsApp Web bloquea explícitamente cadenas que contengan "QtWebEngine"
    // o versiones de Chromium antiguas (< 110). Se elimina QtWebEngine y se garantiza una versión moderna.
    QString userAgent = m_profile->httpUserAgent();
    userAgent.replace(QRegularExpression(QStringLiteral("QtWebEngine/[^\\s]+\\s*")), QString());
    QRegularExpression chromeRegex(QStringLiteral("Chrome/(\\d+)\\.([\\d\\.]+)"));
    QRegularExpressionMatch chromeMatch = chromeRegex.match(userAgent);
    if (chromeMatch.hasMatch()) {
        int major = chromeMatch.captured(1).toInt();
        if (major < 130) {
            userAgent.replace(chromeMatch.captured(0), QStringLiteral("Chrome/130.0.0.0"));
        }
    } else {
        userAgent += QStringLiteral(" Chrome/130.0.0.0");
    }
    m_profile->setHttpUserAgent(userAgent);
    qDebug() << "[MainWindow] User-Agent configurado:" << userAgent;

    // Habilitar características necesarias en el motor web
    QWebEngineSettings *s = m_profile->settings();
    s->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    s->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);
    s->setAttribute(QWebEngineSettings::WebGLEnabled, true);
    s->setAttribute(QWebEngineSettings::DnsPrefetchEnabled, true);
    s->setAttribute(QWebEngineSettings::ScrollAnimatorEnabled, true);
    s->setAttribute(QWebEngineSettings::PluginsEnabled, true);

    // Inyección de scripts en el perfil antes de la carga de páginas
    const QString basePath = ":/web/scripts/";
    registerScript("sw-recovery", basePath + "sw-recovery.js", QWebEngineScript::DocumentCreation);
    registerScript("activity-tracker", basePath + "activity-tracker.js", QWebEngineScript::DocumentCreation);
    registerScript("theme-control", basePath + "theme-control.js", QWebEngineScript::DocumentReady);
    registerScript("file-drop", basePath + "file-drop.js", QWebEngineScript::DocumentReady);
    registerScript("connection-watchdog", basePath + "connection-watchdog.js", QWebEngineScript::DocumentReady);

    // WebView
    m_webView = new QWebEngineView(m_profile, this);

    // Conceder permisos necesarios para WhatsApp (micrófono para audios, notificaciones de escritorio)
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
    connect(m_webView->page(), &QWebEnginePage::permissionRequested,
            this, [](QWebEnginePermission permission) {
        const QString host = permission.origin().host();
        if (host == "web.whatsapp.com" || host.endsWith(".whatsapp.com")) {
            permission.grant();
        }
    });
#else
    connect(m_webView->page(), &QWebEnginePage::featurePermissionRequested,
            this, [this](const QUrl &securityOrigin, QWebEnginePage::Feature feature) {
        const QString host = securityOrigin.host();
        if (host == "web.whatsapp.com" || host.endsWith(".whatsapp.com")) {
            m_webView->page()->setFeaturePermission(securityOrigin, feature, QWebEnginePage::PermissionGrantedByUser);
        }
    });
#endif

    m_webView->setUrl(QUrl("https://web.whatsapp.com/"));

    // Pantalla de bloqueo: oculta el contenido mientras la app está bloqueada
    m_overlay = new LockOverlay(this);
    connect(m_overlay, &LockOverlay::unlockRequested, this, &MainWindow::tryUnlock);
    connect(m_overlay, &LockOverlay::resetRequested, this, [this]() {
        auto reply = QMessageBox::warning(this, "Restablecer contraseña",
            "¿Has olvidado tu contraseña?\n\n"
            "Por seguridad, restablecer la contraseña cerrará la sesión de WhatsApp y "
            "eliminará todos los datos locales (cookies, caché y almacenamiento local).\n\n"
            "Tendrás que volver a escanear el código QR para iniciar sesión.\n\n"
            "¿Deseas continuar?",
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            // Borrar cookies, historial, caché HTTP y almacenamiento persistente del perfil
            if (m_profile) {
                m_profile->clearAllVisitedLinks();
                m_profile->clearHttpCache();
                if (m_profile->cookieStore()) {
                    m_profile->cookieStore()->deleteAllCookies();
                }
                const QString storagePath = m_profile->persistentStoragePath();
                if (!storagePath.isEmpty() && QDir(storagePath).exists()) {
                    QDir(storagePath).removeRecursively();
                }
                const QString cachePath = m_profile->cachePath();
                if (!cachePath.isEmpty() && QDir(cachePath).exists()) {
                    QDir(cachePath).removeRecursively();
                }
            }
            m_lock->setPasscode(QString());
            m_lock->unlock();
            refreshLockState();
            m_actRemovePasscode->setEnabled(false);
            m_actLockNow->setEnabled(false);
            m_webView->setUrl(QUrl("https://web.whatsapp.com/"));
            QMessageBox::information(this, "Contraseña restablecida",
                "Se ha eliminado la contraseña y se ha cerrado la sesión de WhatsApp.\n"
                "Escanea de nuevo el código QR para acceder.");
        }
    });

    m_stack = new QStackedWidget(this);
    m_stack->addWidget(m_overlay);
    m_stack->addWidget(m_webView);
    setCentralWidget(m_stack);
    refreshLockState();

    connect(m_webView, &QWebEngineView::loadStarted, [this]() {
        qDebug() << "[MainWindow] Cargando WhatsApp Web...";
    });
    connect(m_webView, &QWebEngineView::loadFinished, this, &MainWindow::onWebPageLoaded);

    // Menú principal
    QMenuBar *menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    QMenu *fileMenu = menuBar->addMenu("&Archivo");
    QAction *actQuit = fileMenu->addAction("&Salir");
    actQuit->setShortcut(QKeySequence::Quit);
    connect(actQuit, &QAction::triggered, qApp, &QCoreApplication::quit);

    // Menú Tema
    m_themeMenu = menuBar->addMenu("&Tema");
    m_themeGroup = new QActionGroup(this);
    m_themeGroup->setExclusive(true);

    m_actThemeWaDark = m_themeMenu->addAction("Oscuro Oficial (WhatsApp)");
    m_actThemeWaDark->setCheckable(true);
    m_themeGroup->addAction(m_actThemeWaDark);

    m_actThemeMyKOS = m_themeMenu->addAction("Oscuro MyKOS (Personalizado)");
    m_actThemeMyKOS->setCheckable(true);
    m_themeGroup->addAction(m_actThemeMyKOS);

    m_actThemeWaLight = m_themeMenu->addAction("Claro Oficial");
    m_actThemeWaLight->setCheckable(true);
    m_themeGroup->addAction(m_actThemeWaLight);

    QSettings initSettings;
    QString savedTheme = initSettings.value("ui/theme", "whatsapp-dark").toString();
    if (savedTheme == "mykos") {
        m_actThemeMyKOS->setChecked(true);
    } else if (savedTheme == "whatsapp-light") {
        m_actThemeWaLight->setChecked(true);
    } else {
        m_actThemeWaDark->setChecked(true);
    }

    connect(m_actThemeWaDark, &QAction::triggered, this, [this]() { setTheme("whatsapp-dark"); });
    connect(m_actThemeMyKOS, &QAction::triggered, this, [this]() { setTheme("mykos"); });
    connect(m_actThemeWaLight, &QAction::triggered, this, [this]() { setTheme("whatsapp-light"); });

    m_settingsMenu = menuBar->addMenu("&Configuración");
    m_actSettings = m_settingsMenu->addAction("&Opciones...");
    connect(m_actSettings, &QAction::triggered, this, &MainWindow::openSettingsDialog);

    m_actSetPasscode = m_settingsMenu->addAction("&Cambiar contraseña");
    connect(m_actSetPasscode, &QAction::triggered, this, &MainWindow::setPasscode);

    m_actRemovePasscode = m_settingsMenu->addAction("R&emover contraseña");
    m_actRemovePasscode->setEnabled(lock->hasPasscode());
    connect(m_actRemovePasscode, &QAction::triggered, this, &MainWindow::removePasscode);

    m_actLockNow = m_settingsMenu->addAction("&Bloquear ahora");
    m_actLockNow->setShortcut(QKeySequence(QStringLiteral("Ctrl+L")));
    m_actLockNow->setEnabled(lock->hasPasscode());
    connect(m_actLockNow, &QAction::triggered, this, &MainWindow::lockNow);

    QMenu *helpMenu = menuBar->addMenu("&Ayuda");
    m_actAbout = helpMenu->addAction("&Acerca de...");
    connect(m_actAbout, &QAction::triggered, this, &MainWindow::showAboutDialog);

    // Timer de inactividad
    QSettings settings;
    m_inactivityTimeoutMs = settings.value("ui/inactivityTimeoutMin", 5).toInt() * 60000LL;

    m_inactivityTimer = new QTimer(this);
    m_inactivityTimer->setInterval(30000); // cada 30 segundos
    connect(m_inactivityTimer, &QTimer::timeout, this, &MainWindow::checkInactivity);
    m_inactivityTimer->start();

    // Conectar señales de AppLock para sincronizar estado de UI y bandeja
    connect(m_lock, &AppLock::unlocked, this, &MainWindow::refreshLockState);
    connect(m_lock, &AppLock::locked, this, &MainWindow::refreshLockState);
    connect(m_lock, &AppLock::passcodeSet, this, &MainWindow::refreshLockState);

    // Inicializar bandeja del sistema
    initTrayIcon();

    // Si no hay passcode configurado, preguntar de forma opcional una sola vez
    bool askedPasscode = settings.value("security/askedInitialPasscode", false).toBool();
    if (!lock->hasPasscode() && !askedPasscode) {
        settings.setValue("security/askedInitialPasscode", true);
        QTimer::singleShot(1000, this, [this]() {
            auto reply = QMessageBox::question(this, "Bloqueo de aplicación",
                "¿Deseas proteger la aplicación con una contraseña?",
                QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
            if (reply == QMessageBox::Yes) {
                setPasscode();
            }
        });
    }
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    Q_UNUSED(watched);
    switch (event->type()) {
    case QEvent::MouseMove: {
        const qint64 now = QDateTime::currentMSecsSinceEpoch();
        if (now - m_lastNativeActivity > 500) {
            m_lastNativeActivity = now;
        }
        break;
    }
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
    case QEvent::Wheel:
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
        m_lastNativeActivity = QDateTime::currentMSecsSinceEpoch();
        break;
    default:
        break;
    }
    return false;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings;
    bool closeToTray = settings.value("ui/closeToTray", true).toBool();
    if (closeToTray && m_trayIcon && m_trayIcon->isVisible()) {
        hide();
        event->ignore();
        static bool informed = false;
        if (!informed) {
            m_trayIcon->showMessage("MyKOS-Whats",
                                     "La aplicación sigue ejecutándose en segundo plano en la bandeja del sistema.",
                                     QSystemTrayIcon::Information, 3000);
            informed = true;
        }
    } else {
        event->accept();
    }
}

void MainWindow::initTrayIcon()
{
    if (!QSystemTrayIcon::isSystemTrayAvailable()) return;

    m_trayIcon = new QSystemTrayIcon(QIcon(":/icons/app.png"), this);
    m_trayIcon->setToolTip("MyKOS-Whats");

    m_trayMenu = new QMenu(this);
    m_actTrayToggle = m_trayMenu->addAction("Mostrar / Ocultar");
    connect(m_actTrayToggle, &QAction::triggered, this, [this]() {
        if (isVisible() && !isMinimized()) {
            hide();
        } else {
            showNormal();
            raise();
            activateWindow();
        }
    });

    m_actTrayLock = m_trayMenu->addAction("Bloquear ahora");
    connect(m_actTrayLock, &QAction::triggered, this, &MainWindow::lockNow);

    m_trayMenu->addSeparator();

    QAction *actTrayQuit = m_trayMenu->addAction("&Salir");
    connect(actTrayQuit, &QAction::triggered, qApp, &QCoreApplication::quit);

    m_trayIcon->setContextMenu(m_trayMenu);

    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::onTrayIconActivated);
    connect(m_trayIcon, &QSystemTrayIcon::messageClicked, this, [this]() {
        if (m_lastNotification) {
            m_lastNotification->click();
        }
        showNormal();
        raise();
        activateWindow();
    });

    m_trayIcon->show();
}

void MainWindow::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick) {
        if (isVisible() && !isMinimized()) {
            hide();
        } else {
            showNormal();
            raise();
            activateWindow();
        }
    }
}

void MainWindow::handleWebNotification(std::unique_ptr<QWebEngineNotification> notification)
{
    if (!notification) return;

    std::shared_ptr<QWebEngineNotification> sharedNotif = std::move(notification);
    m_lastNotification = sharedNotif;

    connect(sharedNotif.get(), &QWebEngineNotification::closed, this, [this, sharedNotif]() {
        if (m_lastNotification == sharedNotif) {
            m_lastNotification.reset();
        }
    });

    QString title = sharedNotif->title();
    QString message = sharedNotif->message();

    // Privacidad: si la aplicación está bloqueada, ocultar el remitente y cuerpo del mensaje
    if (!m_lock->isUnlocked()) {
        title = "WhatsApp Web";
        message = "Nuevo mensaje recibido (contenido protegido por bloqueo)";
    }

    if (m_trayIcon && m_trayIcon->isVisible()) {
        m_trayIcon->showMessage(title, message, QSystemTrayIcon::Information, 5000);
    }
}

void MainWindow::refreshLockState()
{
    if (m_stack) {
        m_stack->setCurrentWidget(m_lock->isUnlocked() ? static_cast<QWidget *>(m_webView)
                                                       : static_cast<QWidget *>(m_overlay));
    }
    const bool canLock = m_lock && m_lock->hasPasscode() && m_lock->isUnlocked();
    if (m_actLockNow) {
        m_actLockNow->setEnabled(canLock);
    }
    if (m_actRemovePasscode) {
        m_actRemovePasscode->setEnabled(m_lock && m_lock->hasPasscode());
    }
    if (m_actTrayLock) {
        m_actTrayLock->setEnabled(canLock);
    }
}

void MainWindow::tryUnlock(const QString &passcode)
{
    if (m_overlay && m_overlay->isLockedOut()) return;

    if (m_lock->verifyPasscode(passcode)) {
        m_failedAttempts = 0;
        m_lock->unlock();
        refreshLockState();
    } else {
        m_failedAttempts++;
        int lockoutSec = 0;
        if (m_failedAttempts >= 6) {
            lockoutSec = 30;
        } else if (m_failedAttempts == 5) {
            lockoutSec = 15;
        } else if (m_failedAttempts == 4) {
            lockoutSec = 5;
        } else if (m_failedAttempts == 3) {
            lockoutSec = 2;
        }

        if (lockoutSec > 0) {
            m_overlay->setLockedOut(lockoutSec);
        } else {
            m_overlay->showError(QString("Contraseña incorrecta (intento %1).").arg(m_failedAttempts));
        }
    }
}

void MainWindow::lockNow()
{
    if (!m_lock->hasPasscode()) return;
    m_lock->lock();
    refreshLockState();
}

void MainWindow::setTheme(const QString &themeName)
{
    QSettings settings;
    settings.setValue("ui/theme", themeName);
    if (m_webView && m_webView->page()) {
        m_webView->page()->runJavaScript(
            QString("if (window.MyKOSTheme) { window.MyKOSTheme.setTheme('%1'); }").arg(themeName));
    }
}

void MainWindow::onWebPageLoaded(bool ok)
{
    if (!ok) {
        qDebug() << "[MainWindow] Error al cargar WhatsApp Web";
        return;
    }

    QSettings settings;
    QString currentTheme = settings.value("ui/theme", "whatsapp-dark").toString();
    setTheme(currentTheme);
    qDebug() << "[MainWindow] WhatsApp Web cargado con tema:" << currentTheme;
}

void MainWindow::checkInactivity()
{
    if (!m_lock->isUnlocked()) return;

    const qint64 nativeActivity = m_lastNativeActivity;
    m_webView->page()->runJavaScript(
        "window.MyKOSActivity ? window.MyKOSActivity.lastActivity : 0",
        [this, nativeActivity](const QVariant &v) {
            qint64 lastActivity = qMax(nativeActivity, v.toLongLong());
            if (lastActivity <= 0) return;
            qint64 elapsed = QDateTime::currentMSecsSinceEpoch() - lastActivity;
            if (elapsed >= m_inactivityTimeoutMs) {
                QTimer::singleShot(0, this, &MainWindow::triggerInactivityLock);
            }
        });
}

void MainWindow::triggerInactivityLock()
{
    if (!m_lock->isUnlocked()) return;
    qDebug() << "[MainWindow] Inactividad detectada, bloqueando...";
    m_lock->lock();
    refreshLockState();
}

void MainWindow::openSettingsDialog()
{
    // Diálogo de opciones básico con timeout configurable
    QDialog dialog(this);
    dialog.setWindowTitle("Opciones");
    dialog.resize(400, 200);

    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QLabel lbl("Tiempo de bloqueo por inactividad (minutos):", &dialog);
    layout->addWidget(&lbl);

    QSpinBox *spin = new QSpinBox(&dialog);
    spin->setRange(1, 60);
    spin->setValue(static_cast<int>(m_inactivityTimeoutMs / 60000));
    spin->setSuffix(" min");
    layout->addWidget(spin);

    QSettings initSettings;
    QCheckBox *chkCloseToTray = new QCheckBox("Minimizar a la bandeja del sistema al cerrar", &dialog);
    chkCloseToTray->setChecked(initSettings.value("ui/closeToTray", true).toBool());
    layout->addWidget(chkCloseToTray);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    layout->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        m_inactivityTimeoutMs = spin->value() * 60000LL;
        QSettings settings;
        settings.setValue("ui/inactivityTimeoutMin", spin->value());
        settings.setValue("ui/closeToTray", chkCloseToTray->isChecked());
        qDebug() << "[MainWindow] Opciones guardadas: timeout=" << spin->value() << "min, closeToTray=" << chkCloseToTray->isChecked();
    }
}

void MainWindow::setPasscode()
{
    bool ok1 = false;
    QString pass1 = QInputDialog::getText(this, "Establecer contraseña",
                                           "Introduce una nueva contraseña:",
                                           QLineEdit::Password, QString(), &ok1);
    if (!ok1 || pass1.isEmpty()) return;

    bool ok2 = false;
    QString pass2 = QInputDialog::getText(this, "Confirmar contraseña",
                                           "Repite la contraseña:",
                                           QLineEdit::Password, QString(), &ok2);
    if (!ok2 || pass2.isEmpty()) return;

    if (pass1 != pass2) {
        QMessageBox::warning(this, "Error", "Las contraseñas no coinciden.");
        return;
    }

    m_lock->setPasscode(pass1);
    m_actRemovePasscode->setEnabled(true);
    m_actLockNow->setEnabled(true);
    QMessageBox::information(this, "Éxito", "Contraseña establecida correctamente.");
}

void MainWindow::removePasscode()
{
    // Verificar con la contraseña actual antes de eliminar
    bool ok = promptPasscode(m_lock);
    if (!ok) return;

    auto confirm = QMessageBox::warning(this, "Confirmar",
                                        "¿Seguro que quieres eliminar la contraseña? La app quedará sin protección.",
                                        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (confirm == QMessageBox::Yes) {
        m_lock->setPasscode(QString()); // Elimina el passcode
        m_actRemovePasscode->setEnabled(false);
        m_actLockNow->setEnabled(false);
        QMessageBox::information(this, "Éxito", "Contraseña eliminada.");
    }
}

bool MainWindow::promptPasscode(AppLock *lock)
{
    bool ok = false;
    QString passcode = QInputDialog::getText(this, "Verificar contraseña",
                                              "Introduce tu contraseña actual:",
                                              QLineEdit::Password, QString(), &ok);
    if (!ok || passcode.isEmpty()) return false;

    bool valid = lock->verifyPasscode(passcode);
    if (valid) {
        lock->unlock();
        return true;
    }
    QMessageBox::warning(this, "Error", "Contraseña incorrecta.");
    return false;
}

void MainWindow::showAboutDialog()
{
    QMessageBox::about(this, "Acerca de MyKOS-Whats",
                       "<h3>MyKOS-Whats v1.0.0</h3>"
                       "<p>Cliente de escritorio de WhatsApp Web para Linux.</p>"
                       "<p>Construido con Qt 6 y QWebEngine.</p>"
                       "<p>Licencia MIT.</p>");
}

void MainWindow::applyMyKOSPalette()
{
    qApp->setStyle("Fusion");
    QColor bg("#02060c");
    QColor panel("#0a172a");
    QColor accent("#4fa8ff");
    QColor text("#f4f8ff");
    QColor textDim("#8da6c2");

    QPalette pal;
    pal.setColor(QPalette::Window, bg);
    pal.setColor(QPalette::WindowText, text);
    pal.setColor(QPalette::Base, panel);
    pal.setColor(QPalette::AlternateBase, panel.darker(105));
    pal.setColor(QPalette::Text, text);
    pal.setColor(QPalette::ButtonText, text);
    pal.setColor(QPalette::Button, panel);
    pal.setColor(QPalette::Highlight, accent);
    pal.setColor(QPalette::HighlightedText, bg);
    qApp->setPalette(pal);

    // Estilo global para widgets Qt
    qApp->setStyleSheet(
        "QMenuBar { background-color: #0a172a; color: #f4f8ff; }"
        "QMenuBar::item:selected { background-color: #162f50; }"
        "QMenu { background-color: #0a172a; color: #f4f8ff; border: 1px solid #284c78; }"
        "QMenu::item:selected { background-color: #162f50; }"
        "QLabel { color: #f4f8ff; }"
        "QSpinBox { background-color: #06101e; color: #f4f8ff; border: 1px solid #284c78; padding: 4px; }"
        "QDialog { background-color: #0a172a; }"
    );
}

void MainWindow::registerScript(const QString &name, const QString &resourcePath, QWebEngineScript::InjectionPoint point)
{
    QFile file(resourcePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "[MainWindow] No se pudo cargar script:" << resourcePath;
        return;
    }
    QString code = QString::fromUtf8(file.readAll());
    file.close();

    QWebEngineScript script;
    script.setName(name);
    script.setSourceCode(code);
    script.setInjectionPoint(point);
    script.setWorldId(QWebEngineScript::MainWorld);
    script.setRunsOnSubFrames(true);

    m_profile->scripts()->insert(script);
    qDebug() << "[MainWindow] Script registrado en perfil:" << name;
}

void MainWindow::injectScript(const QString &name, const QString &resourcePath)
{
    QFile file(resourcePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "[MainWindow] No se pudo cargar script:" << resourcePath;
        return;
    }
    QString code = QString(file.readAll());
    file.close();

    m_webView->page()->runJavaScript(code);
    qDebug() << "[MainWindow] Script inyectado:" << name;
}
