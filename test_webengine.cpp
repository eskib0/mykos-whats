#include <QApplication>
#include <QWebEngineView>
#include <QWidget>
#include <QVBoxLayout>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_UseSoftwareOpenGL);
    
    QApplication app(argc, argv);
    
    QWidget window;
    QVBoxLayout *layout = new QVBoxLayout(&window);
    
    QWebEngineView *webView = new QWebEngineView(&window);
    webView->setUrl(QUrl("https://www.google.com"));
    
    qDebug() << "Cargando página de prueba...";
    layout->addWidget(webView);
    
    window.resize(800, 600);
    window.show();
    
    return app.exec();
}
