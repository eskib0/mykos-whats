#include <QApplication>
#include <QIcon>
#include "core/app_lock.h"
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
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
