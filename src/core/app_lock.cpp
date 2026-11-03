#include "app_lock.h"
#include <QCryptographicHash>
#include <QPasswordDigestor>
#include <QRandomGenerator>
#include <QStandardPaths>
#include <QSettings>
#include <QDebug>
#include <QDir>

AppLock::AppLock(QObject *parent) : QObject(parent), m_unlocked(false)
{
    // Asegurar que el directorio de datos existe
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataDir);

    loadConfig();
}

bool AppLock::loadConfig()
{
    QSettings settings;
    settings.beginGroup("security");

    m_hash = settings.value("hash").toByteArray();
    m_salt = settings.value("salt").toByteArray();

    if (m_hash.isEmpty() || m_salt.isEmpty()) {
        m_hash = QByteArray();
        m_salt = QByteArray();
        m_unlocked = true;

        qDebug() << "Sin configuración previa. Modo desbloqueado por defecto.";
        return false;
    }

    m_unlocked = false;
    qDebug() << "Configuración cargada. Bloqueado al inicio.";
    return true;
}

bool AppLock::saveConfig()
{
    QSettings settings;
    settings.beginGroup("security");
    settings.setValue("hash", m_hash);
    settings.setValue("salt", m_salt);
    settings.endGroup();
    settings.sync();

    qDebug() << "Configuración guardada.";
    return true;
}

bool AppLock::isUnlocked() const
{
    return m_unlocked;
}

bool AppLock::hasPasscode() const
{
    return !m_hash.isEmpty();
}

QByteArray AppLock::deriveHash(const QString &passcode, const QByteArray &salt) const
{
    // PBKDF2 real con QPasswordDigestor (Qt 5.10+)
    return QPasswordDigestor::deriveKeyPbkdf2(
        QCryptographicHash::Sha256,
        passcode.toUtf8(),
        salt,
        HASH_ITERATIONS,
        salt.size() // longitud de clave = longitud del salt (16 bytes)
    );
}

void AppLock::setPasscode(const QString &passcode)
{
    if (passcode.isEmpty()) {
        m_hash = QByteArray();
        m_salt = QByteArray();
        saveConfig();
        emit passcodeSet();
        return;
    }

    // Generar salt aleatorio de 16 bytes
    QByteArray salt(16, '\0');
    QRandomGenerator::global()->fillRange(
        reinterpret_cast<quint32*>(salt.data()), salt.size() / sizeof(quint32));

    m_salt = salt;
    m_hash = deriveHash(passcode, m_salt);

    saveConfig();
    emit passcodeSet();
}

bool AppLock::verifyPasscode(const QString &passcode)
{
    if (m_hash.isEmpty()) return true; // Sin passcode configurado
    QByteArray test = deriveHash(passcode, m_salt);
    return test == m_hash;
}

void AppLock::unlock()
{
    m_unlocked = true;
    emit unlocked();
}

void AppLock::lock()
{
    m_unlocked = false;
    emit locked();
}
