#include "lockoverlay.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QShowEvent>
#include <QTimer>
#include <QVBoxLayout>

LockOverlay::LockOverlay(QWidget *parent) : QWidget(parent)
{
    setStyleSheet(
        "LockOverlay { background-color: #02060c; }"
        "QLabel#lockTitle { color: #f4f8ff; font-size: 22px; font-weight: 600; }"
        "QLabel#lockHint { color: #8da6c2; font-size: 14px; }"
        "QLabel#lockError { color: #ef4444; font-size: 13px; }"
        "QLineEdit { background-color: #06101e; color: #f4f8ff; border: 1px solid #284c78;"
        "            border-radius: 6px; padding: 8px 12px; font-size: 14px; }"
        "QLineEdit:focus { border: 1px solid #4fa8ff; }"
        "QLineEdit:disabled { background-color: #030810; color: #475569; border: 1px solid #1e293b; }"
        "QPushButton { background-color: #0a172a; color: #f4f8ff; border: 1px solid #284c78;"
        "              border-radius: 6px; padding: 8px 16px; }"
        "QPushButton:hover { background-color: #10243e; }"
        "QPushButton:pressed { background-color: #162f50; }"
        "QPushButton:disabled { background-color: #030810; color: #475569; border: 1px solid #1e293b; }"
    );

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addStretch();

    QLabel *icon = new QLabel("🔒", this);
    icon->setObjectName("lockTitle");
    icon->setAlignment(Qt::AlignCenter);
    icon->setStyleSheet("font-size: 42px;");
    layout->addWidget(icon);

    QLabel *title = new QLabel("MyKOS-Whats bloqueado", this);
    title->setObjectName("lockTitle");
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);

    QLabel *hint = new QLabel("Introduce tu contraseña para continuar", this);
    hint->setObjectName("lockHint");
    hint->setAlignment(Qt::AlignCenter);
    layout->addWidget(hint);

    m_input = new QLineEdit(this);
    m_input->setEchoMode(QLineEdit::Password);
    m_input->setPlaceholderText("Contraseña");
    m_input->setMaximumWidth(320);
    m_input->setMinimumHeight(36);
    layout->addWidget(m_input, 0, Qt::AlignCenter);

    m_button = new QPushButton("Desbloquear", this);
    m_button->setMaximumWidth(320);
    m_button->setCursor(Qt::PointingHandCursor);
    layout->addWidget(m_button, 0, Qt::AlignCenter);

    m_btnReset = new QPushButton("¿Olvidaste tu contraseña?", this);
    m_btnReset->setCursor(Qt::PointingHandCursor);
    m_btnReset->setStyleSheet("QPushButton { background: transparent; border: none; color: #8da6c2; font-size: 12px; text-decoration: underline; padding: 6px; }"
                           "QPushButton:hover { color: #4fa8ff; }");
    layout->addWidget(m_btnReset, 0, Qt::AlignCenter);

    m_error = new QLabel(this);
    m_error->setObjectName("lockError");
    m_error->setAlignment(Qt::AlignCenter);
    m_error->hide();
    layout->addWidget(m_error);

    layout->addStretch();

    m_lockoutTimer = new QTimer(this);
    connect(m_lockoutTimer, &QTimer::timeout, this, &LockOverlay::updateLockoutCountdown);

    connect(m_button, &QPushButton::clicked, this, &LockOverlay::emitUnlock);
    connect(m_btnReset, &QPushButton::clicked, this, &LockOverlay::resetRequested);
    connect(m_input, &QLineEdit::returnPressed, this, &LockOverlay::emitUnlock);
}

void LockOverlay::showError(const QString &message)
{
    m_error->setText(message);
    m_error->show();
    m_input->selectAll();
    m_input->setFocus();
}

void LockOverlay::setLockedOut(int seconds)
{
    if (seconds <= 0) return;
    m_lockoutSecondsRemaining = seconds;
    m_input->setEnabled(false);
    m_button->setEnabled(false);
    m_error->setText(QString("Demasiados intentos fallidos. Espera %1 s...").arg(m_lockoutSecondsRemaining));
    m_error->show();
    m_lockoutTimer->start(1000);
}

bool LockOverlay::isLockedOut() const
{
    return m_lockoutSecondsRemaining > 0;
}

void LockOverlay::updateLockoutCountdown()
{
    m_lockoutSecondsRemaining--;
    if (m_lockoutSecondsRemaining <= 0) {
        m_lockoutTimer->stop();
        m_lockoutSecondsRemaining = 0;
        m_input->setEnabled(true);
        m_button->setEnabled(true);
        m_error->hide();
        m_input->clear();
        m_input->setFocus();
    } else {
        m_error->setText(QString("Demasiados intentos fallidos. Espera %1 s...").arg(m_lockoutSecondsRemaining));
    }
}

void LockOverlay::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    if (m_lockoutSecondsRemaining <= 0) {
        m_input->setEnabled(true);
        m_button->setEnabled(true);
        m_input->clear();
        m_error->hide();
        m_input->setFocus();
    }
}

void LockOverlay::emitUnlock()
{
    if (m_lockoutSecondsRemaining > 0) return;
    const QString passcode = m_input->text();
    if (passcode.isEmpty()) {
        showError("Introduce tu contraseña.");
        return;
    }
    emit unlockRequested(passcode);
}
