// connection-watchdog.js - Monitor de conexión para WhatsApp Web
// Usa navigator.onLine + detección del estado en el DOM de WhatsApp.
// No se parchea WebSocket: la inyección tardía no afecta a sockets ya
// creados por el bundle de WhatsApp ni a los que vivan en Workers.
(function() {
    'use strict';

    console.log('[connection-watchdog] Inicializando monitor de conexión...');

    let lastStatus = navigator.onLine ? 'online' : 'offline';
    let reconnectAttempts = 0;
    const MAX_RECONNECT_ATTEMPTS = 5;

    // Detectar si WhatsApp muestra pantalla de "conectando" / error de conexión
    function isWhatsAppDisconnected() {
        if (!navigator.onLine) return true;
        const alertEl = document.querySelector('[data-testid="alert-phone"], [data-icon="alert-phone"], [data-testid="banner"], div[role="alert"]');
        if (alertEl && /conectando|connecting|reintentar|retry|sin conexión|check your phone|phone not connected/i.test(alertEl.textContent)) {
            return true;
        }
        return false;
    }

    // Mostrar banner de desconexión (estilo MyKOS)
    function showOfflineBanner() {
        let banner = document.getElementById('mykos-whats-offline-banner');
        if (!banner) {
            banner = document.createElement('div');
            banner.id = 'mykos-whats-offline-banner';
            banner.style.cssText = `
                position: fixed; top: 0; left: 0; right: 0;
                background: #ef4444; color: #f4f8ff; text-align: center;
                padding: 10px; z-index: 99999; font-family: "IBM Plex Sans", system-ui, sans-serif;
                font-size: 14px; font-weight: 500;
                border-bottom: 1px solid #284c78;
                box-shadow: 0 4px 16px rgba(239, 68, 68, 0.3);
            `;
            banner.textContent = '⚠️ Sin conexión a Internet';
            document.body.appendChild(banner);
        }
        banner.style.display = 'block';
    }

    // Ocultar banner de desconexión
    function hideOfflineBanner() {
        const banner = document.getElementById('mykos-whats-offline-banner');
        if (banner) banner.style.display = 'none';
    }

    // Intentar reconectar con backoff exponencial y solo si WhatsApp sigue caído.
    // Evita cortar chats o llamadas recargando en cada parpadeo de la conexión.
    function attemptReconnect() {
        if (reconnectAttempts >= MAX_RECONNECT_ATTEMPTS) {
            console.warn('[connection-watchdog] Máximo de intentos alcanzado');
            return;
        }
        reconnectAttempts++;
        const delay = Math.min(5000 * reconnectAttempts, 30000);
        console.log('[connection-watchdog] Reconexión programada en ' + delay + 'ms (intento ' + reconnectAttempts + ')');
        setTimeout(() => {
            if (navigator.onLine && isWhatsAppDisconnected()) {
                console.log('[connection-watchdog] WhatsApp sigue desconectado, recargando...');
                window.location.reload();
            } else {
                // La conexión volvió por su cuenta
                reconnectAttempts = 0;
            }
        }, delay);
    }

    // Monitor de estado de conexión del navegador
    function checkConnectionStatus() {
        const online = navigator.onLine;
        const status = online ? 'online' : 'offline';

        if (status !== lastStatus) {
            console.log('[connection-watchdog] Cambio de estado:', status);
            lastStatus = status;

            if (!online) {
                showOfflineBanner();
            } else {
                hideOfflineBanner();
                attemptReconnect();
            }
        }

        return status;
    }

    // Escuchar eventos de conexión del navegador
    window.addEventListener('online', () => {
        console.log('[connection-watchdog] Conexión restaurada');
        checkConnectionStatus();
    });

    window.addEventListener('offline', () => {
        console.warn('[connection-watchdog] Conexión perdida');
        checkConnectionStatus();
    });

    // Verificar estado cada 5 segundos
    setInterval(checkConnectionStatus, 5000);

    // Estado inicial
    if (!navigator.onLine) {
        showOfflineBanner();
    }
    checkConnectionStatus();

    console.log('[connection-watchdog] Monitor de conexión activado');
})();
