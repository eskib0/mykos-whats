#ifndef APP_LOCK_H
#define APP_LOCK_H

#include <QObject>
#include <QByteArray>
#include <QString>

class AppLock : public QObject
{
    Q_OBJECT
public:
    explicit AppLock(QObject *parent = nullptr);

    bool isUnlocked() const;
    bool hasPasscode() const;

public slots:
    bool verifyPasscode(const QString &passcode);
    void unlock();
    void lock();
    void setPasscode(const QString &passcode);

signals:
    void passcodeSet();
    void unlocked();
    void locked();

private:
    bool loadConfig();
    bool saveConfig();
    QByteArray deriveHash(const QString &passcode, const QByteArray &salt, int iterations) const;

    bool m_unlocked = false;
    QByteArray m_salt;
    QByteArray m_hash;
    int m_iterations = HASH_ITERATIONS;
    // Iteraciones para passcodes nuevos (100k proporciona gran seguridad local sin congelar la UI)
    static const int HASH_ITERATIONS = 100000;
    // Iteraciones de builds antiguos; se usa al verificar configs que no guardaban el valor
    static const int LEGACY_ITERATIONS = 10000;
};

#endif // APP_LOCK_H
