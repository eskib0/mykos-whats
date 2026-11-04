#!/bin/bash
# Script de compilación de MyKOS-Whats

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

BUILD_DIR="build"

echo "🔨 Compilando MyKOS-Whats..."

# Crear directorio de build
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configurar con CMake (usando Qt 6.11 si está disponible en el entorno)
if [ -d "/home/ruben/Qt/6.11.3/gcc_64" ]; then
    cmake .. -DCMAKE_PREFIX_PATH="/home/ruben/Qt/6.11.3/gcc_64" -DCMAKE_BUILD_TYPE=Release
else
    cmake .. -DCMAKE_BUILD_TYPE=Release
fi

# Compilar
make -j$(nproc)

echo "✅ ¡MyKOS-Whats compilado correctamente!"
echo "📦 Binario en: $BUILD_DIR/MyKOS-Whats"
echo ""
echo "Para ejecutar:"
echo "  ./build/MyKOS-Whats"
echo ""
echo "Para instalar (requiere sudo):"
echo "  sudo make install"
