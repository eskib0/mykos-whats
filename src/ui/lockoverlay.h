#ifndef LOCKOVERLAY_H
#define LOCKOVERLAY_H

#include <QWidget>

class QLineEdit;
class QLabel;

// Pantalla de bloqueo mostrada sobre el WebView mientras la app está bloqueada.
// Oculta el contenido de WhatsApp hasta que se introduce el passcode.
class LockOverlay : public QWidget
{
    Q_OBJECT
public:
    explicit LockOverlay(QWidget *parent = nullptr);

    void showError(const QString &message);

signals:
    void unlockRequested(const QString &passcode);
    void resetRequested();

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void emitUnlock();

private:
    QLineEdit *m_input = nullptr;
    QLabel *m_error = nullptr;
};

#endif // LOCKOVERLAY_H
