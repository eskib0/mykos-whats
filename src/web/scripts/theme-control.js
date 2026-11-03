// theme-control.js - Control de temas para WhatsApp Web en MyKOS
(function() {
    'use strict';

    let currentTheme = 'whatsapp-dark'; // 'whatsapp-dark' (defecto), 'mykos', 'whatsapp-light'

    // Estilos MyKOS refinados: solo variables CSS y scrollbars, sin destruir componentes React internos
    const MYKOS_CSS = `
        :root.dark, html.dark {
            --conversation-panel-background: #080e18 !important;
            --app-background: #080e18 !important;
            --panel-background: #0f172a !important;
            --panel-background-deeper: #0b1120 !important;
            --panel-header-background: #131e33 !important;
            --incoming-background: #1e293b !important;
            --incoming-background-deeper: #172033 !important;
            --outgoing-background: #1e3a5f !important;
            --outgoing-background-deeper: #172d4a !important;
            --primary: #f8fafc !important;
            --primary-strong: #ffffff !important;
            --secondary: #94a3b8 !important;
            --secondary-lighter: #64748b !important;
            --border-default: #334155 !important;
            --border-strong: #475569 !important;
            --accent: #38bdf8 !important;
            --icon-fixed: #38bdf8 !important;
            --teal: #38bdf8 !important;
        }

        /* Scrollbars estilo MyKOS */
        ::-webkit-scrollbar {
            width: 6px !important;
            height: 6px !important;
        }
        ::-webkit-scrollbar-track {
            background: #080e18 !important;
        }
        ::-webkit-scrollbar-thumb {
            background: #334155 !important;
            border-radius: 4px !important;
        }
        ::-webkit-scrollbar-thumb:hover {
            background: #38bdf8 !important;
        }
    `;

    function applyTheme(themeName) {
        if (themeName) {
            currentTheme = themeName;
            try {
                localStorage.setItem('mykos_theme_preference', themeName);
            } catch(e) {}
        } else {
            try {
                const saved = localStorage.getItem('mykos_theme_preference');
                if (saved) currentTheme = saved;
            } catch(e) {}
        }

        const docEl = document.documentElement;
        if (!docEl) return;

        let style = document.getElementById('mykos-theme-override');

        if (currentTheme === 'whatsapp-light') {
            docEl.classList.remove('dark');
            docEl.classList.add('light');
            docEl.setAttribute('data-color-mode', 'light');
            if (document.body) {
                document.body.classList.remove('dark');
                document.body.classList.add('light');
                document.body.setAttribute('data-color-mode', 'light');
            }
            if (style) style.remove();
        } else if (currentTheme === 'mykos') {
            docEl.classList.add('dark');
            docEl.classList.remove('light');
            docEl.setAttribute('data-color-mode', 'dark');
            if (document.body) {
                document.body.classList.add('dark');
                document.body.classList.remove('light');
                document.body.setAttribute('data-color-mode', 'dark');
            }
            if (!style) {
                style = document.createElement('style');
                style.id = 'mykos-theme-override';
                const target = document.head || docEl || document.body;
                if (target) target.appendChild(style);
            }
            if (style && style.textContent !== MYKOS_CSS) {
                style.textContent = MYKOS_CSS;
            }
        } else {
            // 'whatsapp-dark' (Modo oscuro oficial de WhatsApp Web: 100% legible y probado)
            docEl.classList.add('dark');
            docEl.classList.remove('light');
            docEl.setAttribute('data-color-mode', 'dark');
            if (document.body) {
                document.body.classList.add('dark');
                document.body.classList.remove('light');
                document.body.setAttribute('data-color-mode', 'dark');
            }
            // Eliminar overrides para que WhatsApp aplique su modo oscuro oficial sin interferencias
            if (style) style.remove();
        }
    }

    function init() {
        applyTheme();

        const root = document.documentElement;
        if (root) {
            const observer = new MutationObserver(() => {
                if (currentTheme !== 'whatsapp-light') {
                    const isDark = document.documentElement.classList.contains('dark') &&
                                   document.documentElement.getAttribute('data-color-mode') === 'dark';
                    if (!isDark) {
                        observer.disconnect();
                        applyTheme();
                        observer.observe(root, {
                            attributes: true,
                            attributeFilter: ['class', 'data-color-mode']
                        });
                    }
                }
            });

            observer.observe(root, {
                attributes: true,
                attributeFilter: ['class', 'data-color-mode']
            });
        }

        window.MyKOSTheme = {
            setTheme: applyTheme,
            forceTheme: () => applyTheme(),
            getTheme: () => currentTheme,
            version: '2026.11.03-mykos-v4'
        };
    }

    if (document.readyState === 'loading') {
        document.addEventListener('DOMContentLoaded', init);
    } else {
        init();
    }
})();
