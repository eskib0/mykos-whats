// connection-watchdog.js - Monitor de conexión para WhatsApp Web
(function() {
    'use strict';

    console.log('[connection-watchdog] Inicializando monitor de conexión...');

    let lastStatus = null;
    let reconnectAttempts = 0;
    const MAX_RECONNECT_ATTEMPTS = 5;

    // Parchear WebSocket para detectar caídas
    const OriginalWebSocket = window.WebSocket;

    window.WebSocket = function(url, protocols) {
        const ws = new OriginalWebSocket(url, protocols);

        ws.addEventListener('close', (event) => {
            console.warn('[connection-watchdog] WebSocket cerrado:', event.code, event.reason);
            handleConnectionLoss();
        });

        ws.addEventListener('error', (event) => {
            console.error('[connection-watchdog] WebSocket error:', event);
            handleConnectionLoss();
        });

        ws.addEventListener('open', () => {
            console.log('[connection-watchdog] WebSocket abierto');
            reconnectAttempts = 0; // Resetear intentos al reconectar
        });

        return ws;
    };

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
                // Si estábamos desconectados, intentar reconectar
                if (reconnectAttempts < MAX_RECONNECT_ATTEMPTS) {
                    attemptReconnect();
                }
            }
        }

        return status;
    }

    // Mostrar banner de desconexión (estilo MyKOS)
    function showOfflineBanner() {
        let banner = document.getElementById('whatsie-offline-banner');
        if (!banner) {
            banner = document.createElement('div');
            banner.id = 'whatsie-offline-banner';
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
        const banner = document.getElementById('whatsie-offline-banner');
        if (banner) banner.style.display = 'none';
    }

    // Intentar reconectar
    function attemptReconnect() {
        reconnectAttempts++;
        console.log('[connection-watchdog] Intento de reconexión:', reconnectAttempts);
        window.location.reload();
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
    checkConnectionStatus();

    console.log('[connection-watchdog] Monitor de conexión activado');
})();
