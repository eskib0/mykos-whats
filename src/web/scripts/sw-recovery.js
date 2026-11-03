// sw-recovery.js - Recuperación de Service Workers corruptos
(function() {
    'use strict';

    console.log('[sw-recovery] Inicializando recuperación de Service Workers...');

    if (!('serviceWorker' in navigator)) {
        console.log('[sw-recovery] Service Workers no soportados, polyfill desactivado');
        return;
    }

    // Monitorear errores de Service Worker
    navigator.serviceWorker.addEventListener('error', (event) => {
        console.error('[sw-recovery] Error de Service Worker:', event);
        recoverServiceWorker();
    });

    // Verificar integridad del SW cada 30 segundos
    setInterval(checkServiceWorkerIntegrity, 30 * 1000);

    async function checkServiceWorkerIntegrity() {
        try {
            const registrations = await navigator.serviceWorker.getRegistrations();
            
            for (const registration of registrations) {
                if (!registration.active && !registration.installing) {
                    console.warn('[sw-recovery] Service Worker sin estado activo, desregistrando...');
                    await registration.unregister();
                    
                    // Forzar recarga limpia
                    setTimeout(() => {
                        window.location.reload();
                    }, 1000);
                    return;
                }

                if (registration.active && registration.active.state !== 'activated') {
                    console.warn('[sw-recovery] Service Worker en estado:', registration.active.state);
                }
            }
        } catch (err) {
            console.error('[sw-recovery] Error verificando SW:', err);
            recoverServiceWorker();
        }
    }

    async function recoverServiceWorker() {
        console.log('[sw-recovery] Iniciando recuperación de Service Workers...');
        
        try {
            const registrations = await navigator.serviceWorker.getRegistrations();
            
            for (const registration of registrations) {
                // Desregistrar todos los SWs de WhatsApp
                if (registration.scope.includes('whatsapp') || 
                    registration.scope.includes('web.whatsapp')) {
                    await registration.unregister();
                    console.log('[sw-recovery] Service Worker desregistrado:', registration.scope);
                }
            }

            // Limpiar caché relacionado
            const cacheNames = await caches.keys();
            for (const cacheName of cacheNames) {
                if (cacheName.includes('whatsapp') || cacheName.includes('web.whatsapp')) {
                    await caches.delete(cacheName);
                    console.log('[sw-recovery] Caché eliminado:', cacheName);
                }
            }

            // Forzar recarga limpia
            setTimeout(() => {
                window.location.reload();
            }, 2000);

        } catch (err) {
            console.error('[sw-recovery] Error en recuperación:', err);
        }
    }

    console.log('[sw-recovery] Monitor de Service Workers activado');
})();
