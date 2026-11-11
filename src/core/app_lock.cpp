#include "app_lock.h"
#include <QCryptographicHash>
#include <QPasswordDigestor>
#include <QRandomGenerator>
#include <QStandardPaths>
#include <QSettings>
#include <QFile>
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
    // Configs antiguas no guardaban "iterations": se asume el valor legacy (10k)
    m_iterations = settings.value("iterations", LEGACY_ITERATIONS).toInt();
    settings.endGroup();

#if defined(Q_OS_UNIX)
    QString fileName = settings.fileName();
    if (!fileName.isEmpty() && QFile::exists(fileName)) {
        // Blindaje de permisos 0600 (solo lectura y escritura para el propietario)
        QFile::setPermissions(fileName, QFileDevice::ReadOwner | QFileDevice::WriteOwner);
    }
#endif

    if (m_hash.isEmpty() || m_salt.isEmpty()) {
        m_hash = QByteArray();
        m_salt = QByteArray();
        m_iterations = HASH_ITERATIONS;
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
    settings.setValue("iterations", m_iterations);
    settings.endGroup();
    settings.sync();

    bool ok = (settings.status() == QSettings::NoError);
    if (!ok) {
        qWarning() << "Error al guardar configuración de seguridad en QSettings:" << settings.status();
    } else {
        qDebug() << "Configuración guardada correctamente.";
#if defined(Q_OS_UNIX)
        QString fileName = settings.fileName();
        if (!fileName.isEmpty() && QFile::exists(fileName)) {
            // Blindaje estricto 0600 tras guardar
            QFile::setPermissions(fileName, QFileDevice::ReadOwner | QFileDevice::WriteOwner);
        }
#endif
    }
    return ok;
}

bool AppLock::isUnlocked() const
{
    return m_unlocked;
}

bool AppLock::hasPasscode() const
{
    return !m_hash.isEmpty();
}

QByteArray AppLock::deriveHash(const QString &passcode, const QByteArray &salt, int iterations) const
{
    // PBKDF2 real con QPasswordDigestor (Qt 5.10+)
    return QPasswordDigestor::deriveKeyPbkdf2(
        QCryptographicHash::Sha256,
        passcode.toUtf8(),
        salt,
        iterations,
        32 // longitud de clave = 32 bytes (SHA-256)
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
    m_iterations = HASH_ITERATIONS;
    m_hash = deriveHash(passcode, m_salt, m_iterations);

    saveConfig();
    emit passcodeSet();
}

bool AppLock::verifyPasscode(const QString &passcode)
{
    if (m_hash.isEmpty()) return true; // Sin passcode configurado
    QByteArray test = deriveHash(passcode, m_salt, m_iterations);
    // Comparación en tiempo constante para evitar ataques de temporización
    if (test.size() != m_hash.size()) {
        test.fill('\0');
        return false;
    }
    volatile quint8 diff = 0;
    const int n = test.size();
    for (int i = 0; i < n; ++i) {
        diff |= static_cast<quint8>(test[i] ^ m_hash[i]);
    }
    // Zeroing del buffer derivado en memoria
    test.fill('\0');
    return diff == 0;
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
