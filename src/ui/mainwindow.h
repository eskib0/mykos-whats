#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>
#include <QUrl>
#include "core/app_lock.h"

class MainWindow : public QObject
{
    Q_OBJECT
public:
    explicit MainWindow(AppLock *lock, QObject *parent = nullptr)
        : QObject(parent), m_lock(lock) {}

    Q_PROPERTY(QUrl webViewUrl READ webViewUrl WRITE setWebViewUrl NOTIFY webViewUrlChanged)

public slots:
    void unlockApp() {
        if (m_lock) m_lock->unlock();
        emit appUnlocked();
    }

    bool isLocked() const {
        return m_lock && !m_lock->isUnlocked();
    }

    QUrl webViewUrl() const { return m_webViewUrl; }

public slots:
    void setWebViewUrl(const QUrl &url) {
        if (m_webViewUrl == url) return;
        m_webViewUrl = url;
        emit webViewUrlChanged();
    }

signals:
    void appUnlocked();
    void webViewUrlChanged();

private:
    AppLock *m_lock = nullptr;
    QUrl m_webViewUrl = QUrl("https://web.whatsapp.com/");
};

#endif // MAINWINDOW_H
