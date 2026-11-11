// file-drop.js - Soporte mejorado para arrastrar y soltar archivos en MyKOS-Whats
// NOTA DE MANTENIMIENTO:
// WhatsApp Web gestiona inputs ocultos tipo 'file' para adjuntar multimedia o documentos.
// Puntos sensibles a vigilar ante cambios de DOM en WhatsApp:
// 1. Selector 'input[type="file"]': WhatsApp podría mover los inputs a Shadow DOM.
// 2. Atributo 'accept': distingue 'image,video' de otros documentos.
// 3. Inyección DataTransfer: React espera eventos 'input' y 'change' para registrar la carga.
(function() {
    'use strict';

    console.log('[file-drop] Inicializando soporte de arrastre de archivos MyKOS...');

    function isMediaFile(file) {
        if (!file || !file.type) return false;
        return file.type.startsWith('image/') || file.type.startsWith('video/');
    }

    // Buscar el input de archivos adecuado según el tipo (multimedia vs documento)
    function findTargetInput(isMedia) {
        const inputs = Array.from(document.querySelectorAll('input[type="file"]'));
        if (inputs.length === 0) {
            console.warn('[file-drop] DIAGNÓSTICO: No hay inputs[type="file"] en el DOM.');
            return null;
        }

        if (isMedia) {
            // Input de fotos y vídeos (accept contiene "image" o "video")
            const mediaInput = inputs.find(i => i.accept && (i.accept.includes('image') || i.accept.includes('video')));
            if (mediaInput) return mediaInput;
        } else {
            // Input de documentos (accept="*" o no restringido exclusivamente a imágenes)
            const docInput = inputs.find(i => !i.accept || i.accept === '*' || !i.accept.includes('image'));
            if (docInput) return docInput;
        }

        // Fallback: input con dimensiones visibles o el primer input encontrado
        const visible = inputs.find(i => {
            const rect = i.getBoundingClientRect();
            return rect.width > 0 && rect.height > 0;
        });
        if (visible) {
            console.log('[file-drop] Usando fallback de input visible.');
            return visible;
        }
        console.log('[file-drop] Usando primer input disponible.');
        return inputs[0] || null;
    }

    // Inyectar un lote completo de archivos en el input de WhatsApp Web
    function injectFilesToWhatsApp(files, isMedia) {
        if (!files || files.length === 0) return false;

        const input = findTargetInput(isMedia);
        if (!input) {
            console.warn('[file-drop] No se encontró input de archivo disponible (media=' + isMedia + ')');
            return false;
        }

        // Usar un único DataTransfer con todos los archivos para evitar perder elementos
        const dataTransfer = new DataTransfer();
        for (const file of files) {
            dataTransfer.items.add(file);
        }

        try {
            Object.defineProperty(input, 'files', {
                writable: true,
                value: dataTransfer.files
            });
        } catch (e) {
            input.files = dataTransfer.files;
        }

        // Disparar eventos para que React / WhatsApp Web detecte los archivos
        input.dispatchEvent(new Event('input', { bubbles: true }));
        input.dispatchEvent(new Event('change', { bubbles: true }));

        console.log('[file-drop] Inyectados ' + files.length + ' archivo(s) (media=' + isMedia + '):',
                    files.map(f => f.name));
        return true;
    }

    // Inicializar listeners de drag & drop
    function initFileDrop() {
        window.addEventListener('dragover', (e) => {
            e.preventDefault();
            e.stopPropagation();
            if (e.dataTransfer) {
                e.dataTransfer.dropEffect = 'copy';
            }
        });

        window.addEventListener('drop', (e) => {
            e.preventDefault();
            e.stopPropagation();

            if (!e.dataTransfer || !e.dataTransfer.files || e.dataTransfer.files.length === 0) {
                return;
            }

            const files = Array.from(e.dataTransfer.files);
            console.log('[file-drop] Archivos detectados en drop:', files.map(f => f.name));

            const mediaFiles = files.filter(isMediaFile);
            const docFiles = files.filter(f => !isMediaFile(f));

            if (mediaFiles.length > 0 && docFiles.length === 0) {
                injectFilesToWhatsApp(mediaFiles, true);
            } else if (docFiles.length > 0 && mediaFiles.length === 0) {
                injectFilesToWhatsApp(docFiles, false);
            } else {
                // Lote mixto: inyectar documentos primero y multimedia con breve retraso
                injectFilesToWhatsApp(docFiles, false);
                setTimeout(() => {
                    injectFilesToWhatsApp(mediaFiles, true);
                }, 300);
            }
        });
    }

    // Inicializar cuando el DOM esté listo
    if (document.readyState === 'loading') {
        document.addEventListener('DOMContentLoaded', initFileDrop);
    } else {
        initFileDrop();
    }

    console.log('[file-drop] Soporte de arrastre activado');
})();
