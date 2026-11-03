// activity-tracker.js — Registra la última actividad del usuario dentro de la página.
// main.cpp consulta window.MyKOSActivity.lastActivity periódicamente para
// decidir si debe activar el bloqueo por inactividad.
(function () {
    'use strict';

    window.MyKOSActivity = {
        lastActivity: Date.now()
    };

    const EVENTS = ['mousemove', 'mousedown', 'mouseup', 'keydown', 'keyup',
                    'wheel', 'touchstart', 'touchmove', 'click', 'scroll', 'input'];

    function markActivity() {
        window.MyKOSActivity.lastActivity = Date.now();
    }

    EVENTS.forEach(function (ev) {
        window.addEventListener(ev, markActivity, { passive: true, capture: true });
    });

    // Si la pestaña vuelve a estar visible, también cuenta como actividad
    document.addEventListener('visibilitychange', function () {
        if (!document.hidden) markActivity();
    });
})();
