#include <QTest>
#include <QSignalSpy>
#include <QSettings>
#include <QCryptographicHash>
#include <QPasswordDigestor>
#include "core/app_lock.h"

class TestAppLock : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testInitialEmptyPasscode();
    void testSetPasscodeAndVerify();
    void testRemovePasscode();
    void testLockAndUnlockSignals();
    void testLegacyMigration();
};

void TestAppLock::initTestCase()
{
    QCoreApplication::setOrganizationName("MyKOS");
    QCoreApplication::setApplicationName("MyKOS-Whats-Test");
    QSettings settings;
    settings.clear();
    settings.sync();
}

void TestAppLock::cleanupTestCase()
{
    QSettings settings;
    settings.clear();
    settings.sync();
}

void TestAppLock::init()
{
    // Limpiar configuraciones antes de cada test
    QSettings settings;
    settings.remove("security");
    settings.sync();
}

void TestAppLock::cleanup()
{
    QSettings settings;
    settings.remove("security");
    settings.sync();
}

void TestAppLock::testInitialEmptyPasscode()
{
    AppLock lock;
    QVERIFY(!lock.hasPasscode());
    QVERIFY(lock.isUnlocked());
    // Sin passcode configurado, verifyPasscode siempre devuelve true
    QVERIFY(lock.verifyPasscode("cualquiera"));
    QVERIFY(lock.verifyPasscode(""));
}

void TestAppLock::testSetPasscodeAndVerify()
{
    AppLock lock;
    lock.setPasscode("MiClaveSuperSegura2026!");

    QVERIFY(lock.hasPasscode());
    QVERIFY(lock.verifyPasscode("MiClaveSuperSegura2026!"));
    QVERIFY(!lock.verifyPasscode("ClaveErronea"));
    QVERIFY(!lock.verifyPasscode(""));

    // Comprobar que en QSettings se guardó con 100k iteraciones
    QSettings settings;
    settings.beginGroup("security");
    QCOMPARE(settings.value("iterations").toInt(), 100000);
    QVERIFY(!settings.value("hash").toByteArray().isEmpty());
    QCOMPARE(settings.value("salt").toByteArray().size(), 16);
    settings.endGroup();
}

void TestAppLock::testRemovePasscode()
{
    AppLock lock;
    lock.setPasscode("Temporal123");
    QVERIFY(lock.hasPasscode());

    // Vaciar passcode elimina la protección
    lock.setPasscode(QString());
    QVERIFY(!lock.hasPasscode());

    QSettings settings;
    settings.beginGroup("security");
    QVERIFY(settings.value("hash").toByteArray().isEmpty());
    QVERIFY(settings.value("salt").toByteArray().isEmpty());
    settings.endGroup();
}

void TestAppLock::testLockAndUnlockSignals()
{
    AppLock lock;
    lock.setPasscode("test1234");

    QSignalSpy spyUnlocked(&lock, &AppLock::unlocked);
    QSignalSpy spyLocked(&lock, &AppLock::locked);

    lock.lock();
    QVERIFY(!lock.isUnlocked());
    QCOMPARE(spyLocked.count(), 1);

    lock.unlock();
    QVERIFY(lock.isUnlocked());
    QCOMPARE(spyUnlocked.count(), 1);
}

void TestAppLock::testLegacyMigration()
{
    // Simular una configuración legacy de 10.000 iteraciones sin campo "iterations"
    const QString legacyPass = "AntiguaClave10k";
    const QByteArray salt(16, 'x');
    const QByteArray legacyHash = QPasswordDigestor::deriveKeyPbkdf2(
        QCryptographicHash::Sha256,
        legacyPass.toUtf8(),
        salt,
        10000,
        32
    );

    {
        QSettings settings;
        settings.beginGroup("security");
        settings.setValue("hash", legacyHash);
        settings.setValue("salt", salt);
        settings.remove("iterations"); // Ausente en versiones anteriores
        settings.endGroup();
        settings.sync();
    }

    // Al cargar, AppLock debe usar el fallback LEGACY_ITERATIONS (10.000)
    AppLock lock;
    QVERIFY(lock.hasPasscode());
    QVERIFY(lock.verifyPasscode(legacyPass));
    QVERIFY(!lock.verifyPasscode("ClaveIncorrecta"));

    // Al cambiar la contraseña, debe actualizarse a 100.000 iteraciones
    lock.setPasscode("NuevaClave100k");
    QVERIFY(lock.verifyPasscode("NuevaClave100k"));

    QSettings settings;
    settings.beginGroup("security");
    QCOMPARE(settings.value("iterations").toInt(), 100000);
    settings.endGroup();
}

QTEST_MAIN(TestAppLock)
#include "test_app_lock.moc"
