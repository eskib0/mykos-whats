# MyKOS-Whats — WhatsApp Web para Linux

Aplicación nativa de Qt6 que envuelve WhatsApp Web con bloqueo local y personalización.

## 🚀 Características

- **Pantalla de bloqueo local:** al arrancar con passcode configurado, el contenido queda oculto hasta introducirlo. Passcode cifrado con PBKDF2-HMAC-SHA256 (100.000 iteraciones; los passcodes antiguos de 10.000 se migran solos al cambiarlos).
- **Bloqueo por inactividad:** configurable (1–60 min) y persistente entre sesiones.
- **Bloqueo manual:** `Ctrl+L` o menú *Configuración → Bloquear ahora*.
- **Tema oscuro MyKOS:** aplicado sobre WhatsApp Web (variables CSS + reglas de refuerzo).
- **Watchdog de conexión:** banner offline y recarga con backoff exponencial solo si WhatsApp sigue desconectado.
- **Polyfills:** arrastrar y soltar archivos (MutationObserver) y recuperación de Service Workers corruptos.
- **Perfil persistente:** la sesión de WhatsApp sobrevive entre arranques.

## 🛠️ Requisitos

- Qt 6 (Core, Gui, Widgets, WebEngineWidgets, Network)
- CMake 3.16+
- GCC o Clang con soporte C++17

## 🔨 Compilación

```bash
chmod +x build.sh
./build.sh
```

O manualmente:

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

## ▶️ Ejecución

```bash
./build/MyKOS-Whats
```

## 📁 Estructura del Proyecto

```
whatsie/
├── CMakeLists.txt
├── build.sh
├── README.md
└── src/
    ├── main.cpp
    ├── core/
    │   ├── app_lock.h
    │   └── app_lock.cpp
    ├── ui/
    │   ├── lockoverlay.h
    │   ├── lockoverlay.cpp
    │   ├── mainwindow.h
    │   └── mainwindow.cpp
    └── web/
        └── scripts/
            ├── activity-tracker.js
            ├── connection-watchdog.js
            ├── file-drop.js
            ├── sw-recovery.js
            └── theme-control.js
```

## 🔒 Modelo de Seguridad y Limitaciones

- **Protección a nivel de UI:** La pantalla de bloqueo local y el temporizador de inactividad ocultan la interfaz para evitar miradas indiscretas o acceso casual no autorizado en el entorno físico del usuario.
- **Protección contra fuerza bruta:** Emplea PBKDF2-HMAC-SHA256 (100.000 iteraciones, salt aleatorio de 16 bytes y comparación en tiempo constante) combinado con un bloqueo temporal (*rate limiting*) exponencial tras intentos fallidos consecutivos.
- **Restablecimiento destructivo:** Si se restablece la contraseña desde la pantalla de bloqueo, se eliminan completamente las cookies, caché y datos del perfil de Chromium/QWebEngine, cerrando la sesión de WhatsApp para asegurar que un atacante físico no acceda a las conversaciones.
- **Limitación en disco:** La sesión web y las cookies de QWebEngine se guardan en el directorio de usuario (`~/.config` y `~/.local/share`) según el comportamiento estándar de Chromium. Esta protección está diseñada como barrera visual/local y no sustituye al cifrado completo de disco (LUKS) ni protege frente a accesos directos al sistema de archivos con permisos de usuario o superusuario.

## ⚖️ Aviso Legal (Disclaimer)

Este proyecto es un cliente de escritorio no oficial basado en la interfaz web oficial de WhatsApp Web. No está afiliado, asociado, autorizado, respaldado ni conectado de ninguna manera oficial con WhatsApp LLC, Meta Platforms, Inc. o cualquiera de sus subsidiarias o filiales. El nombre *WhatsApp*, así como los nombres, marcas, insignias e imágenes relacionados son marcas registradas de sus respectivos propietarios.

## 📜 Licencia

MIT
