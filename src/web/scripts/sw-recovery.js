// sw-recovery.js - Recuperación de Service Workers corruptos y Storage API polyfills
(function() {
    'use strict';

    console.log('[sw-recovery] Inicializando recuperación de Service Workers y Storage...');

    // Polyfill de StorageManager.persist para evitar el error de denegación de persistencia
    // en WhatsApp Web bajo QtWebEngine ("storage bucket persistence denied")
    if (navigator.storage) {
        if (typeof navigator.storage.persist === 'function') {
            const origPersist = navigator.storage.persist.bind(navigator.storage);
            navigator.storage.persist = async function() {
                try {
                    await origPersist();
                } catch (e) {}
                return true;
            };
        }
        if (typeof navigator.storage.persisted === 'function') {
            navigator.storage.persisted = async function() {
                return true;
            };
        }
    }

    if (!('serviceWorker' in navigator)) {
        console.log('[sw-recovery] Service Workers no soportados, polyfill desactivado');
        return;
    }

    const RECOVERY_COOLDOWN_MS = 2 * 60 * 1000; // 2 minutos de enfriamiento para evitar bucles de recarga

    function canTriggerRecovery() {
        try {
            const last = parseInt(sessionStorage.getItem('mykos_sw_last_recovery') || '0', 10);
            return (Date.now() - last) > RECOVERY_COOLDOWN_MS;
        } catch (e) {
            return false;
        }
    }

    function recordRecovery() {
        try {
            sessionStorage.setItem('mykos_sw_last_recovery', String(Date.now()));
        } catch (e) {}
    }

    // Verificar integridad del SW cada 45 segundos
    setInterval(checkServiceWorkerIntegrity, 45 * 1000);

    async function checkServiceWorkerIntegrity() {
        try {
            const registrations = await navigator.serviceWorker.getRegistrations();

            for (const registration of registrations) {
                // Un Service Worker es válido si está activo, instalando O esperando activación (waiting).
                // Solo se considera huérfano/corrupto si no tiene ningún worker en ninguna fase.
                const hasWorker = registration.active || registration.installing || registration.waiting;
                if (!hasWorker) {
                    console.warn('[sw-recovery] Registro de Service Worker sin workers válidos detectado.');
                    if (canTriggerRecovery()) {
                        await recoverServiceWorker();
                    } else {
                        console.warn('[sw-recovery] Recuperación omitida: en periodo de enfriamiento anti-bucle.');
                    }
                    return;
                }

                // Monitorear errores durante la instalación de nuevos workers
                if (registration.installing) {
                    registration.installing.addEventListener('error', (err) => {
                        console.error('[sw-recovery] Error durante la instalación del Service Worker:', err);
                    });
                }
            }
        } catch (err) {
            console.error('[sw-recovery] Error verificando SW:', err);
        }
    }

    async function recoverServiceWorker() {
        if (!canTriggerRecovery()) {
            console.warn('[sw-recovery] Recuperación omitida por enfriamiento');
            return;
        }
        recordRecovery();
        console.log('[sw-recovery] Iniciando recuperación limpia de Service Workers...');

        try {
            const registrations = await navigator.serviceWorker.getRegistrations();
            for (const registration of registrations) {
                if (registration.scope.includes('whatsapp') || registration.scope.includes('web.whatsapp')) {
                    await registration.unregister();
                    console.log('[sw-recovery] Service Worker desregistrado:', registration.scope);
                }
            }

            // Limpiar caché relacionado de Service Worker
            if ('caches' in window) {
                const cacheNames = await caches.keys();
                for (const cacheName of cacheNames) {
                    if (cacheName.includes('whatsapp') || cacheName.includes('web.whatsapp')) {
                        await caches.delete(cacheName);
                        console.log('[sw-recovery] Caché eliminado:', cacheName);
                    }
                }
            }

            // Forzar recarga limpia tras limpiar estado corrupto
            setTimeout(() => {
                window.location.reload();
            }, 1500);

        } catch (err) {
            console.error('[sw-recovery] Error en recuperación:', err);
        }
    }

    console.log('[sw-recovery] Monitor de Service Workers activado');
})();
