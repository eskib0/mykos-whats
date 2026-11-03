// theme-control.js - Control de tema MyKOS para WhatsApp Web
(function() {
    'use strict';

    const MYKOS_COLORS = {
        bg: '#02060c',
        bgDeeper: '#010408',
        bgSecondary: '#06101e',
        panel: '#0a172a',
        panelHover: '#10243e',
        panelActive: '#162f50',
        accent: '#4fa8ff',
        accentHover: '#71b8ff',
        text: '#f4f8ff',
        textDim: '#8da6c2',
        line: '#284c78',
        lineStrong: '#365b87',
        userBubble: '#0f3460',
        userBubbleDeeper: '#0a2444',
        contactBubble: '#06101e',
        success: '#4ade80',
        danger: '#ef4444',
        warning: '#f59e0b'
    };

    const MYKOS_CSS = `
        /* ============================================================ */
        /* MyKOS Theme Override for WhatsApp Web                         */
        /* ============================================================ */

        /* 1. Inyección de variables CSS nativas de WhatsApp Web */
        :root, html, html.dark, body, body.dark, #app, [data-color-mode="dark"], [data-color-mode="light"] {
            /* Fondos principales */
            --app-background: ${MYKOS_COLORS.bg} !important;
            --app-background-deeper: ${MYKOS_COLORS.bgDeeper} !important;
            --app-background-stripe: ${MYKOS_COLORS.panel} !important;
            --conversation-panel-background: ${MYKOS_COLORS.bg} !important;
            --conversation-panel-border: ${MYKOS_COLORS.line} !important;

            /* Paneles laterales */
            --panel-background: ${MYKOS_COLORS.bgSecondary} !important;
            --panel-background-deeper: ${MYKOS_COLORS.bg} !important;
            --panel-background-colored: ${MYKOS_COLORS.panel} !important;
            --panel-background-colored-deeper: ${MYKOS_COLORS.bgSecondary} !important;
            --panel-background-hover: ${MYKOS_COLORS.panelHover} !important;
            --panel-background-active: ${MYKOS_COLORS.panelActive} !important;
            --panel-header-background: ${MYKOS_COLORS.panel} !important;
            --panel-header-icon: ${MYKOS_COLORS.accent} !important;

            /* Burbujas de chat */
            --incoming-background: ${MYKOS_COLORS.contactBubble} !important;
            --incoming-background-deeper: ${MYKOS_COLORS.bg} !important;
            --incoming-background-highlight: ${MYKOS_COLORS.panelHover} !important;
            --outgoing-background: ${MYKOS_COLORS.userBubble} !important;
            --outgoing-background-deeper: ${MYKOS_COLORS.userBubbleDeeper} !important;
            --outgoing-background-highlight: ${MYKOS_COLORS.panelActive} !important;
            --system-message-background: ${MYKOS_COLORS.panel} !important;
            --system-message-text: ${MYKOS_COLORS.textDim} !important;

            /* Textos */
            --primary: ${MYKOS_COLORS.text} !important;
            --primary-strong: #ffffff !important;
            --primary-title: ${MYKOS_COLORS.text} !important;
            --secondary: ${MYKOS_COLORS.textDim} !important;
            --secondary-lighter: #637d99 !important;
            --message-primary: ${MYKOS_COLORS.text} !important;
            --message-secondary: ${MYKOS_COLORS.textDim} !important;

            /* Líneas y bordes */
            --border-default: ${MYKOS_COLORS.line} !important;
            --border-strong: ${MYKOS_COLORS.lineStrong} !important;
            --border-list: #1a3556 !important;
            --border-panel: ${MYKOS_COLORS.line} !important;
            --border-bubble: ${MYKOS_COLORS.line} !important;

            /* Acentos de marca */
            --accent: ${MYKOS_COLORS.accent} !important;
            --teal: ${MYKOS_COLORS.accent} !important;
            --primary-emphasis: ${MYKOS_COLORS.accent} !important;
            --icon: ${MYKOS_COLORS.accent} !important;
            --icon-lighter: ${MYKOS_COLORS.textDim} !important;
            --icon-search: ${MYKOS_COLORS.accent} !important;
            --icon-fixed: ${MYKOS_COLORS.accent} !important;
            --button-primary-background: ${MYKOS_COLORS.panel} !important;
            --button-primary-background-hover: ${MYKOS_COLORS.panelHover} !important;

            /* Caja de texto inferior (Compose) */
            --compose-input-background: ${MYKOS_COLORS.bgSecondary} !important;
            --compose-input-background-focused: ${MYKOS_COLORS.panel} !important;
            --compose-input-border: ${MYKOS_COLORS.line} !important;
            --compose-input-border-focused: ${MYKOS_COLORS.accent} !important;
            --compose-panel-background: ${MYKOS_COLORS.panel} !important;
            --search-input-background: ${MYKOS_COLORS.bgSecondary} !important;

            /* Desplegables y modales */
            --dropdown-background: ${MYKOS_COLORS.panel} !important;
            --dropdown-background-hover: ${MYKOS_COLORS.panelHover} !important;
            --modal-background: ${MYKOS_COLORS.bgSecondary} !important;
            --modal-backdrop: rgba(2, 6, 12, 0.88) !important;

            /* Badges de no leídos */
            --unread-marker-background: ${MYKOS_COLORS.accent} !important;
            --unread-marker-text: ${MYKOS_COLORS.bg} !important;
            --badge-background: ${MYKOS_COLORS.accent} !important;
            --badge-text: ${MYKOS_COLORS.bg} !important;
            --badge-count-background: ${MYKOS_COLORS.accent} !important;
            --badge-count-color: ${MYKOS_COLORS.bg} !important;
        }

        /* 2. Reglas estructurales y fondos directos */
        html, body, #app, .app {
            background-color: ${MYKOS_COLORS.bg} !important;
            background: ${MYKOS_COLORS.bg} !important;
            color: ${MYKOS_COLORS.text} !important;
        }

        /* Panel izquierdo de chats */
        [id^="pane-side"], [data-testid="chat-list"], [data-testid="drawer-left"] {
            background-color: ${MYKOS_COLORS.bgSecondary} !important;
            border-right: 1px solid ${MYKOS_COLORS.line} !important;
        }

        /* Headers de ventana */
        header, [data-testid="chat-list-header"], [data-testid="conversation-header"] {
            background-color: ${MYKOS_COLORS.panel} !important;
            border-bottom: 1px solid ${MYKOS_COLORS.line} !important;
        }

        /* Área central de mensajes y fondo */
        [data-testid="conversation-panel-wrapper"],
        [data-testid="conversation-panel-messages"],
        div[class*="conversation-panel"] {
            background-color: ${MYKOS_COLORS.bg} !important;
            background-image: none !important;
        }

        /* Burbujas salientes (Tú) */
        .message-out,
        [data-testid="msg-container"] .message-out,
        div[class*="message-out"] > div {
            background-color: ${MYKOS_COLORS.userBubble} !important;
            color: ${MYKOS_COLORS.text} !important;
            border-radius: 12px !important;
            box-shadow: 0 2px 8px rgba(0, 0, 0, 0.35) !important;
        }

        /* Burbujas entrantes (Contacto) */
        .message-in,
        [data-testid="msg-container"] .message-in,
        div[class*="message-in"] > div {
            background-color: ${MYKOS_COLORS.contactBubble} !important;
            color: ${MYKOS_COLORS.text} !important;
            border: 1px solid ${MYKOS_COLORS.line} !important;
            border-radius: 12px !important;
            box-shadow: 0 2px 8px rgba(0, 0, 0, 0.35) !important;
        }

        /* Input de escritura inferior */
        footer,
        [data-testid="conversation-compose-box"],
        div[class*="compose-box"] {
            background-color: ${MYKOS_COLORS.panel} !important;
            border-top: 1px solid ${MYKOS_COLORS.line} !important;
        }

        /* Caja de texto donde escribes */
        footer [contenteditable="true"],
        footer [data-testid="conversation-compose-box-input"],
        div[class*="selectable-text"][contenteditable="true"] {
            background-color: ${MYKOS_COLORS.bgSecondary} !important;
            color: ${MYKOS_COLORS.text} !important;
            border: 1px solid ${MYKOS_COLORS.line} !important;
            border-radius: 8px !important;
            padding: 8px 12px !important;
        }

        /* Iconos y botones SVG */
        span[data-icon] svg, [data-testid="icon"] svg, button svg {
            color: ${MYKOS_COLORS.accent} !important;
            fill: ${MYKOS_COLORS.accent} !important;
        }

        /* Ticks de leído (doble check) en azul neón */
        [data-icon="msg-dblcheck"] svg, [data-icon="msg-check"] svg {
            color: ${MYKOS_COLORS.accent} !important;
            fill: ${MYKOS_COLORS.accent} !important;
        }

        /* Barras de desplazamiento (Scrollbars) */
        ::-webkit-scrollbar {
            width: 6px !important;
            height: 6px !important;
        }
        ::-webkit-scrollbar-track {
            background: ${MYKOS_COLORS.bg} !important;
        }
        ::-webkit-scrollbar-thumb {
            background: ${MYKOS_COLORS.line} !important;
            border-radius: 4px !important;
        }
        ::-webkit-scrollbar-thumb:hover {
            background: ${MYKOS_COLORS.accent} !important;
        }

        /* Menús emergentes y modales */
        [role="menu"], [role="listbox"], div[class*="dropdown"], [data-testid="menu"] {
            background-color: ${MYKOS_COLORS.panel} !important;
            border: 1px solid ${MYKOS_COLORS.line} !important;
            border-radius: 12px !important;
            box-shadow: 0 10px 30px rgba(0, 0, 0, 0.6) !important;
        }
        [role="menuitem"]:hover, [role="option"]:hover {
            background-color: ${MYKOS_COLORS.panelHover} !important;
        }

        /* Código QR de vinculación inicial */
        [data-testid="qrcode"] canvas, [data-testid="qr-code"] {
            border: 3px solid ${MYKOS_COLORS.accent} !important;
            border-radius: 14px !important;
            padding: 8px !important;
            background: #ffffff !important;
            box-shadow: 0 0 24px rgba(79, 168, 255, 0.35) !important;
        }
    `;

    function getTarget() {
        return document.head || document.documentElement || document.body;
    }

    function applyTheme() {
        // 1. Forzar clase 'dark' para que WhatsApp use su motor oscuro como base
        if (document.documentElement) {
            document.documentElement.classList.add('dark');
            document.documentElement.classList.remove('light');
            document.documentElement.setAttribute('data-color-mode', 'dark');
        }
        if (document.body) {
            document.body.classList.add('dark');
            document.body.classList.remove('light');
            document.body.setAttribute('data-color-mode', 'dark');
        }

        // 2. Inyectar o actualizar el bloque de estilos MyKOS
        let style = document.getElementById('mykos-theme-override');
        if (!style) {
            style = document.createElement('style');
            style.id = 'mykos-theme-override';
            const target = getTarget();
            if (target) {
                target.appendChild(style);
            }
        }
        if (style && style.textContent !== MYKOS_CSS) {
            style.textContent = MYKOS_CSS;
        }
    }

    function init() {
        applyTheme();

        // Observer para re-aplicar solo si WhatsApp intenta cambiar el color o quitar la clase dark
        const root = document.documentElement || document.body;
        if (root) {
            const observer = new MutationObserver((mutations) => {
                // Solo re-aplicar si se modificó algo relevante
                const relevant = mutations.some(m =>
                    ['class', 'data-color-mode', 'style'].includes(m.attributeName)
                );
                if (relevant) applyTheme();
            });
            observer.observe(root, {
                attributes: true,
                attributeFilter: ['class', 'data-color-mode', 'style']
            });
        }

        // No se usa setInterval: el MutationObserver ya cubre los cambios.
        // Se re-aplica al cargar el DOM por seguridad (una sola vez).

        window.MyKOSTheme = {
            colors: MYKOS_COLORS,
            forceTheme: applyTheme,
            version: '2026.11.02-mykos-v2'
        };
    }

    if (document.readyState === 'loading') {
        document.addEventListener('DOMContentLoaded', init);
    } else {
        init();
    }
})();
