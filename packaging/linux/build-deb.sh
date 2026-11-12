#!/usr/bin/env bash
# ==============================================================================
# Constructor de paquete Debian (.deb) oficial para MyKOS-Whats
# ==============================================================================
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
DIST_DIR="$PROJECT_ROOT/dist"

# 1. Extraer versión desde CMakeLists.txt
VERSION="$(grep -oP 'project\(MyKOS-Whats VERSION \K[0-9]+\.[0-9]+\.[0-9]+' "$PROJECT_ROOT/CMakeLists.txt" || echo "1.0.0")"
ARCH="amd64"
PKG_NAME="mykos-whats_${VERSION}_${ARCH}"
STAGE_DIR="$DIST_DIR/staging/$PKG_NAME"
BUILD_DIR="$PROJECT_ROOT/build-package"

echo "============================================================"
echo "  Construyendo paquete Debian: $PKG_NAME.deb"
echo "  Versión: $VERSION | Arquitectura: $ARCH"
echo "============================================================"

# 2. Compilar con Qt del sistema para garantizar portabilidad
echo "▶ 1/5 Compilando con Qt6 del sistema (sin rutas locales embebidas)..."
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"
cmake "$PROJECT_ROOT" \
    -DCMAKE_BUILD_TYPE=Release \
    -DUSE_SYSTEM_QT=ON \
    -DCMAKE_INSTALL_PREFIX=/usr

make -j"$(nproc)"

# 3. Ejecutar pruebas unitarias obligatorias antes de empaquetar
echo "▶ 2/5 Ejecutando suite de pruebas ctest..."
ctest --output-on-failure

# 4. Preparar árbol de instalación del paquete
echo "▶ 3/5 Preparando estructura del paquete..."
rm -rf "$STAGE_DIR"
mkdir -p "$STAGE_DIR/DEBIAN"
mkdir -p "$STAGE_DIR/usr/bin"
mkdir -p "$STAGE_DIR/usr/share/applications"

# Copiar binario principal y dar permisos
cp "$BUILD_DIR/MyKOS-Whats" "$STAGE_DIR/usr/bin/MyKOS-Whats"
chmod 755 "$STAGE_DIR/usr/bin/MyKOS-Whats"

# Crear enlace simbólico en minúsculas (convención Debian)
ln -sf "MyKOS-Whats" "$STAGE_DIR/usr/bin/mykos-whats"

# Copiar lanzador .desktop
cp "$PROJECT_ROOT/data/mykos-whats.desktop" "$STAGE_DIR/usr/share/applications/mykos-whats.desktop"
chmod 644 "$STAGE_DIR/usr/share/applications/mykos-whats.desktop"

# Copiar iconos en todas las resoluciones estándar
for SIZE in 16 24 32 48 64 128 256 512; do
    ICON_SRC="$PROJECT_ROOT/resources/icons/mykos-whats_${SIZE}x${SIZE}.png"
    if [ "$SIZE" -eq 512 ] && [ ! -f "$ICON_SRC" ]; then
        ICON_SRC="$PROJECT_ROOT/resources/icons/mykos-whats.png"
    fi
    if [ -f "$ICON_SRC" ]; then
        ICON_DEST="$STAGE_DIR/usr/share/icons/hicolor/${SIZE}x${SIZE}/apps"
        mkdir -p "$ICON_DEST"
        cp "$ICON_SRC" "$ICON_DEST/mykos-whats.png"
        chmod 644 "$ICON_DEST/mykos-whats.png"
    fi
done

# 5. Generar archivo de control de Debian
echo "▶ 4/5 Generando metadatos DEBIAN/control y scripts de ciclo de vida..."
cat > "$STAGE_DIR/DEBIAN/control" <<EOF
Package: mykos-whats
Version: ${VERSION}
Section: net
Priority: optional
Architecture: ${ARCH}
Maintainer: Ruben <eskib0@github.com>
Depends: libc6 (>= 2.34), libstdc++6 (>= 11), libqt6core6t64 | libqt6core6 (>= 6.4.0), libqt6gui6t64 | libqt6gui6 (>= 6.4.0), libqt6widgets6t64 | libqt6widgets6 (>= 6.4.0), libqt6webenginewidgets6 (>= 6.4.0), libqt6webenginecore6 (>= 6.4.0), libqt6network6t64 | libqt6network6 (>= 6.4.0), libqt6webchannel6 (>= 6.4.0)
Homepage: https://github.com/eskib0/mykos-whats
Description: Cliente nativo de WhatsApp Web con seguridad y tema MyKOS
 MyKOS-Whats es un cliente nativo y liviano de WhatsApp Web desarrollado en
 C++ con Qt6 y WebEngine. Incluye bloqueo por PIN con cifrado PBKDF2,
 ocultacion de notificaciones en bloqueo, auto-bloqueo por inactividad,
 proteccion contra bucles de recarga y personalizacion visual MyKOS.
EOF
chmod 644 "$STAGE_DIR/DEBIAN/control"

# Scripts de post-instalación y post-desinstalación
cat > "$STAGE_DIR/DEBIAN/postinst" <<'EOF'
#!/bin/sh
set -e
if command -v update-desktop-database >/dev/null 2>&1; then
    update-desktop-database -q /usr/share/applications || true
fi
if command -v gtk-update-icon-cache >/dev/null 2>&1; then
    gtk-update-icon-cache -q -t -f /usr/share/icons/hicolor || true
fi
exit 0
EOF
chmod 755 "$STAGE_DIR/DEBIAN/postinst"

cat > "$STAGE_DIR/DEBIAN/postrm" <<'EOF'
#!/bin/sh
set -e
if command -v update-desktop-database >/dev/null 2>&1; then
    update-desktop-database -q /usr/share/applications || true
fi
if command -v gtk-update-icon-cache >/dev/null 2>&1; then
    gtk-update-icon-cache -q -t -f /usr/share/icons/hicolor || true
fi
exit 0
EOF
chmod 755 "$STAGE_DIR/DEBIAN/postrm"

# 6. Construir paquete .deb
echo "▶ 5/5 Empaquetando con dpkg-deb..."
mkdir -p "$DIST_DIR"
dpkg-deb --build --root-owner-group "$STAGE_DIR" "$DIST_DIR/$PKG_NAME.deb"

# 7. Verificación final y control de calidad
echo ""
echo "============================================================"
echo "  ✅ Paquete generado con éxito:"
echo "     $DIST_DIR/$PKG_NAME.deb"
echo "============================================================"
echo ""
echo "--- Resumen del paquete (dpkg-deb -I) ---"
dpkg-deb -I "$DIST_DIR/$PKG_NAME.deb"
echo ""
echo "--- Contenido del paquete (dpkg-deb -c) ---"
dpkg-deb -c "$DIST_DIR/$PKG_NAME.deb"
echo ""

# Limpieza opcional del directorio staging
rm -rf "$STAGE_DIR"
