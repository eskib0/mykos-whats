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

# Configurar con CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

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
