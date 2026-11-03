#include "lockoverlay.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QShowEvent>
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
        "QPushButton { background-color: #0a172a; color: #f4f8ff; border: 1px solid #284c78;"
        "              border-radius: 6px; padding: 8px 16px; }"
        "QPushButton:hover { background-color: #10243e; }"
        "QPushButton:pressed { background-color: #162f50; }"
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

    QPushButton *button = new QPushButton("Desbloquear", this);
    button->setMaximumWidth(320);
    button->setCursor(Qt::PointingHandCursor);
    layout->addWidget(button, 0, Qt::AlignCenter);

    QPushButton *btnReset = new QPushButton("¿Olvidaste tu contraseña?", this);
    btnReset->setCursor(Qt::PointingHandCursor);
    btnReset->setStyleSheet("QPushButton { background: transparent; border: none; color: #8da6c2; font-size: 12px; text-decoration: underline; padding: 6px; }"
                           "QPushButton:hover { color: #4fa8ff; }");
    layout->addWidget(btnReset, 0, Qt::AlignCenter);

    m_error = new QLabel(this);
    m_error->setObjectName("lockError");
    m_error->setAlignment(Qt::AlignCenter);
    m_error->hide();
    layout->addWidget(m_error);

    layout->addStretch();

    connect(button, &QPushButton::clicked, this, &LockOverlay::emitUnlock);
    connect(btnReset, &QPushButton::clicked, this, &LockOverlay::resetRequested);
    connect(m_input, &QLineEdit::returnPressed, this, &LockOverlay::emitUnlock);
}

void LockOverlay::showError(const QString &message)
{
    m_error->setText(message);
    m_error->show();
    m_input->selectAll();
    m_input->setFocus();
}

void LockOverlay::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    m_input->clear();
    m_error->hide();
    m_input->setFocus();
}

void LockOverlay::emitUnlock()
{
    const QString passcode = m_input->text();
    if (passcode.isEmpty()) {
        showError("Introduce tu contraseña.");
        return;
    }
    emit unlockRequested(passcode);
}
