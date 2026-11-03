#include <QApplication>
#include <QMainWindow>
#include <QWebEngineView>
#include <QWebEngineProfile>
#include <QWebEnginePage>
#include <QWebEngineSettings>
#include <QWebEngineScript>
#include <QWebEngineScriptCollection>
#include <QMessageBox>
#include <QInputDialog>
#include <QLineEdit>
#include <QDir>
#include <QFile>
#include <QUrl>
#include <QDebug>
#include <QVBoxLayout>
#include <QWidget>
#include <QCloseEvent>
#include <QTimer>
#include <QPalette>
#include <QColor>
#include "core/app_lock.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(AppLock *lock, QWidget *parent = nullptr)
        : QMainWindow(parent), m_lock(lock)
    {
        setWindowTitle("MyKOS-Whast");
        resize(1200, 800);

        // Aplicar paleta MyKOS al contenedor Qt
        QPalette kosPalette;
        kosPalette.setColor(QPalette::Window, QColor("#02060c"));
        kosPalette.setColor(QPalette::WindowText, QColor("#f4f8ff"));
        kosPalette.setColor(QPalette::Base, QColor("#06101e"));
        kosPalette.setColor(QPalette::AlternateBase, QColor("#0a172a"));
        kosPalette.setColor(QPalette::Text, QColor("#f4f8ff"));
        kosPalette.setColor(QPalette::Button, QColor("#0a172a"));
        kosPalette.setColor(QPalette::ButtonText, QColor("#f4f8ff"));
        kosPalette.setColor(QPalette::Highlight, QColor("#4fa8ff"));
        kosPalette.setColor(QPalette::HighlightedText, QColor("#02060c"));
        kosPalette.setColor(QPalette::ToolTipBase, QColor("#06101e"));
        kosPalette.setColor(QPalette::ToolTipText, QColor("#f4f8ff"));
        qApp->setPalette(kosPalette);

        // Estilo global para diálogos nativos Qt
        qApp->setStyleSheet(R"(
            QDialog, QMessageBox, QInputDialog {
                background-color: #02060c;
                color: #f4f8ff;
            }
            QLineEdit, QComboBox, QTextEdit {
                background-color: #06101e;
                color: #f4f8ff;
                border: 1px solid #284c78;
                border-radius: 8px;
                padding: 8px;
                selection-background-color: #4fa8ff;
            }
            QPushButton {
                background-color: #0a172a;
                color: #f4f8ff;
                border: 1px solid #284c78;
                border-radius: 8px;
                padding: 8px 16px;
            }
            QPushButton:hover {
                background-color: #10243e;
                border-color: #4fa8ff;
            }
            QLabel {
                color: #f4f8ff;
            }
        )");

        auto *central = new QWidget(this);
        setCentralWidget(central);

        auto *layout = new QVBoxLayout(central);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);

        m_webView = new QWebEngineView(this);
        m_webView->setStyleSheet("background-color: #02060c; border: none;");
        layout->addWidget(m_webView);

        // Crear perfil personalizado y asignarlo a la página
        m_profile = new QWebEngineProfile("whatsie-profile", this);
        m_profile->setPersistentCookiesPolicy(QWebEngineProfile::ForcePersistentCookies);

        QString userAgent = "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36";
        m_profile->setHttpUserAgent(userAgent);

        auto *page = new QWebEnginePage(m_profile, m_webView);
        m_webView->setPage(page);

        // Configurar settings en el perfil para que afecten a toda la página
        QWebEngineSettings *s = m_profile->settings();
        s->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
        s->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);
        s->setAttribute(QWebEngineSettings::WebGLEnabled, true);
        s->setAttribute(QWebEngineSettings::DnsPrefetchEnabled, true);
        s->setAttribute(QWebEngineSettings::ScrollAnimatorEnabled, true);

        // Inyectar scripts de utilidad
        injectScript("connection-watchdog", ":/scripts/connection-watchdog.js");
        injectScript("file-drop", ":/scripts/file-drop.js");
        injectScript("sw-recovery", ":/scripts/sw-recovery.js");
        injectScript("theme-control", ":/scripts/theme-control.js");

        // Conectar señal de carga y aplicar tema
        connect(m_webView, &QWebEngineView::loadFinished, this, [this](bool ok) {
            qDebug() << "Página cargada:" << ok;
            if (ok) {
                m_webView->page()->runJavaScript("if (window.MyKOSTheme) { window.MyKOSTheme.forceTheme(); }");
            } else {
                qWarning() << "Error cargando la página";
            }
        });

        // Temporizador para reforzar el tema frente a re-renderizados SPA de WhatsApp Web
        auto *themeTimer = new QTimer(this);
        connect(themeTimer, &QTimer::timeout, this, [this]() {
            if (m_webView && m_webView->page()) {
                m_webView->page()->runJavaScript("if (window.MyKOSTheme) { window.MyKOSTheme.forceTheme(); }");
            }
        });
        themeTimer->start(2000);

        m_webView->load(QUrl("https://web.whatsapp.com/"));
    }

    void injectScript(const QString &name, const QString &resourcePath)
    {
        QFile f(resourcePath);
        if (!f.open(QIODevice::ReadOnly)) {
            qWarning() << "No se pudo cargar el script:" << resourcePath;
            return;
        }
        QString source = QString::fromUtf8(f.readAll());
        f.close();

        QWebEngineScript script;
        script.setName(name);
        script.setSourceCode(source);
        script.setInjectionPoint(QWebEngineScript::DocumentReady);
        script.setWorldId(QWebEngineScript::MainWorld);
        script.setRunsOnSubFrames(true);

        m_profile->scripts()->insert(script);
        qDebug() << "Script inyectado:" << name;
    }

protected:
    void closeEvent(QCloseEvent *event) override
    {
        if (m_lock) {
            m_lock->lock();
        }
        event->accept();
    }

private:
    QWebEngineView *m_webView = nullptr;
    QWebEngineProfile *m_profile = nullptr;
    AppLock *m_lock = nullptr;
};

bool promptPasscode(AppLock *lock)
{
    if (lock->isUnlocked()) {
        return true;
    }

    bool ok;
    QString pass = QInputDialog::getText(nullptr, "Bloqueo de aplicación",
                                         "Introduce la contraseña:",
                                         QLineEdit::Password, QString(), &ok);
    if (!ok || pass.isEmpty()) {
        return false;
    }

    if (!lock->verifyPasscode(pass)) {
        QMessageBox::warning(nullptr, "Error", "Contraseña incorrecta.");
        return false;
    }

    lock->unlock();
    return true;
}

int main(int argc, char *argv[])
{
    // No se fuerza software OpenGL: se deja que Qt elija el backend GPU por defecto.

    QApplication app(argc, argv);
    app.setApplicationName("MyKOS-Whast");
    app.setOrganizationName("whatsie");

    AppLock lock;

    // Si no hay contraseña configurada, preguntar si quiere crear una
    if (!lock.hasPasscode()) {
        auto reply = QMessageBox::question(nullptr, "Bloqueo de aplicación",
            "¿Quieres establecer una contraseña para proteger la aplicación?");
        if (reply == QMessageBox::Yes) {
            bool ok;
            QString pass1 = QInputDialog::getText(nullptr, "Nueva contraseña",
                                                  "Contraseña:", QLineEdit::Password,
                                                  QString(), &ok);
            if (ok && !pass1.isEmpty()) {
                QString pass2 = QInputDialog::getText(nullptr, "Confirmar contraseña",
                                                      "Repite la contraseña:", QLineEdit::Password,
                                                      QString(), &ok);
                if (ok && pass1 == pass2) {
                    lock.setPasscode(pass1);
                    QMessageBox::information(nullptr, "Listo", "Contraseña guardada.");
                } else {
                    QMessageBox::warning(nullptr, "Error", "Las contraseñas no coinciden.");
                }
            }
        }
    }

    // Pedir contraseña al inicio si está configurada
    if (!promptPasscode(&lock)) {
        return 1;
    }

    MainWindow w(&lock);
    w.show();

    return app.exec();
}

#include "main.moc"
