# MyKOS-Whast - Clon de WhatsApp para Linux

Una aplicación nativa de Qt6 que envuelve WhatsApp Web con características adicionales de seguridad y personalización.

## 🚀 Características

- **Lock Screen local:** Autenticación con passcode cifrado (PBKDF2-HMAC-SHA256, 10.000 iteraciones)
- **Tema Claro/Oscuro:** Sincronizado automáticamente
- **Privacidad:** Difuminado de mensajes no seleccionados
- **Integración Linux:** Notificaciones nativas, autostart XDG
- **Polyfills para Flatpak:** File drop, Service Worker recovery

## 🛠️ Requisitos

- Qt 6.7+ (Qt 5.10+ para QPasswordDigestor)
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
./build/whatsie
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
    │   ├── mainwindow.h
    │   ├── mainwindow.cpp
    │   └── MainWindow.html
    └── web/
        └── scripts/
            ├── connection-watchdog.js
            ├── file-drop.js
            ├── sw-recovery.js
            └── theme-control.js
```

## 📜 Licencia

MIT
