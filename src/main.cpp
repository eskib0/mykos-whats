#include <QApplication>
#include <QIcon>
#include "core/app_lock.h"
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    // Optimizar rendimiento de Chromium en Linux con aceleración por hardware
    if (qEnvironmentVariableIsEmpty("QTWEBENGINE_CHROMIUM_FLAGS")) {
        qputenv("QTWEBENGINE_CHROMIUM_FLAGS",
                "--enable-gpu-rasterization --enable-zero-copy --disable-gpu-driver-bug-workarounds");
    }

    QApplication app(argc, argv);
    app.setApplicationName("MyKOS-Whats");
    app.setOrganizationName("MyKOS");
    app.setApplicationVersion("1.0.0");
    app.setWindowIcon(QIcon(":/icons/app.png"));

    // El desbloqueo inicial lo gestiona MainWindow con su pantalla de bloqueo
    AppLock lock;
    MainWindow window(&lock);
    window.show();

    return app.exec();
}
