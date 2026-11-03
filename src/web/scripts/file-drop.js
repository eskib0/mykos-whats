// file-drop.js - Soporte para arrastrar y soltar archivos
(function() {
    'use strict';

    console.log('[file-drop] Inicializando soporte de arrastre...');

    function initFileDrop() {
        // Escuchar eventos de drop en toda la ventana
        window.addEventListener('dragover', (e) => {
            e.preventDefault();
            e.stopPropagation();
            if (e.dataTransfer) {
                e.dataTransfer.dropEffect = 'copy';
            }
        });

        window.addEventListener('drop', async (e) => {
            e.preventDefault();
            e.stopPropagation();

            if (!e.dataTransfer || !e.dataTransfer.files || e.dataTransfer.files.length === 0) {
                return;
            }

            const files = Array.from(e.dataTransfer.files);
            console.log('[file-drop] Archivos detectados:', files.map(f => f.name));

            // Procesar cada archivo
            for (const file of files) {
                try {
                    await injectFileToWhatsApp(file);
                } catch (err) {
                    console.error('[file-drop] Error procesando archivo:', err);
                }
            }
        });
    }

    async function injectFileToWhatsApp(file) {
        // Buscar el campo de entrada de archivos de WhatsApp Web
        const fileInputs = document.querySelectorAll('input[type="file"]');
        let targetInput = null;

        for (const input of fileInputs) {
            if (input.accept && input.accept.includes('image')) {
                targetInput = input;
                break;
            }
        }

        // Si no encontramos input específico, buscar cualquiera visible
        if (!targetInput) {
            for (const input of fileInputs) {
                if (input.offsetParent !== null) {
                    targetInput = input;
                    break;
                }
            }
        }

        if (targetInput) {
            const dataTransfer = new DataTransfer();
            dataTransfer.items.add(file);
            targetInput.files = dataTransfer.files;

            // Disparar eventos
            targetInput.dispatchEvent(new Event('change', { bubbles: true }));
            targetInput.dispatchEvent(new Event('input', { bubbles: true }));

            console.log('[file-drop] Archivo inyectado en input:', file.name);
            return;
        }

        console.warn('[file-drop] No se encontró input de archivo disponible');
    }

    // Inicializar cuando el DOM esté listo
    if (document.readyState === 'loading') {
        document.addEventListener('DOMContentLoaded', initFileDrop);
    } else {
        initFileDrop();
    }

    console.log('[file-drop] Soporte de arrastre activado');
})();
