// file-drop.js - Soporte mejorado para arrastrar y soltar archivos
// Usa MutationObserver para detectar inputs de WhatsApp Web dinámicos
(function() {
    'use strict';

    console.log('[file-drop] Inicializando soporte de arrastre (v2 con MutationObserver)...');

    let targetInput = null;
    let observer = null;

    // Buscar el input de archivos correcto en WhatsApp Web
    function findTargetInput() {
        const inputs = document.querySelectorAll('input[type="file"]');
        
        for (const input of inputs) {
            if (input.accept && input.accept.includes('image')) {
                return input;
            }
        }

        // Fallback: buscar cualquier input visible
        for (const input of inputs) {
            const rect = input.getBoundingClientRect();
            if (rect.width > 0 && rect.height > 0) {
                return input;
            }
        }

        return null;
    }

    // Buscar o actualizar el target input bajo demanda (en drag o drop)
    function getTargetInput() {
        if (targetInput && document.body.contains(targetInput)) {
            return targetInput;
        }
        targetInput = findTargetInput();
        return targetInput;
    }

    // Inyectar un archivo en el input de WhatsApp Web
    async function injectFileToWhatsApp(file) {
        const input = getTargetInput();
        if (!input) {
            console.warn('[file-drop] No se encontró input de archivo disponible');
            return;
        }

        const dataTransfer = new DataTransfer();
        dataTransfer.items.add(file);
        
        // Usar Object.defineProperty para asignar files (en WA Web es necesario)
        Object.defineProperty(input, 'files', {
            writable: true,
            value: dataTransfer.files
        });

        // Disparar eventos en el orden correcto
        input.dispatchEvent(new Event('input', { bubbles: true }));
        input.dispatchEvent(new Event('change', { bubbles: true }));

        console.log('[file-drop] Archivo inyectado:', file.name);
    }

    // Inicializar listeners de drag & drop
    function initFileDrop() {
        window.addEventListener('dragenter', () => {
            getTargetInput();
        });

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

            for (const file of files) {
                try {
                    await injectFileToWhatsApp(file);
                } catch (err) {
                    console.error('[file-drop] Error procesando archivo:', err);
                }
            }
        });
    }

    // Inicializar cuando el DOM esté listo
    if (document.readyState === 'loading') {
        document.addEventListener('DOMContentLoaded', initFileDrop);
    } else {
        initFileDrop();
    }

    console.log('[file-drop] Soporte de arrastre activado (v2)');
})();
