#include <QApplication>
#include <QIcon>
#include "core/app_lock.h"
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    // En Linux con NVIDIA y memoria VRAM limitada/compartida, Chromium colapsa en un bucle
    // infinito de Skia OOM al intentar compositar por GPU. '--disable-gpu-compositing'
    // previene el bucle de saturación al 100% de CPU manteniendo fluidez total.
    if (qEnvironmentVariableIsEmpty("QTWEBENGINE_CHROMIUM_FLAGS")) {
        qputenv("QTWEBENGINE_CHROMIUM_FLAGS", "--disable-gpu-compositing");
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
