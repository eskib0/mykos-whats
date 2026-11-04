#ifndef LOCKOVERLAY_H
#define LOCKOVERLAY_H

#include <QWidget>

class QLineEdit;
class QLabel;
class QPushButton;
class QTimer;

// Pantalla de bloqueo mostrada sobre el WebView mientras la app está bloqueada.
// Oculta el contenido de WhatsApp hasta que se introduce el passcode.
class LockOverlay : public QWidget
{
    Q_OBJECT
public:
    explicit LockOverlay(QWidget *parent = nullptr);

    void showError(const QString &message);
    void setLockedOut(int seconds);
    bool isLockedOut() const;

signals:
    void unlockRequested(const QString &passcode);
    void resetRequested();

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void emitUnlock();
    void updateLockoutCountdown();

private:
    QLineEdit *m_input = nullptr;
    QPushButton *m_button = nullptr;
    QPushButton *m_btnReset = nullptr;
    QLabel *m_error = nullptr;
    QTimer *m_lockoutTimer = nullptr;
    int m_lockoutSecondsRemaining = 0;
};

#endif // LOCKOVERLAY_H
