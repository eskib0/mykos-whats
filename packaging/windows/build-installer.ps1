# ==============================================================================
# Helper para compilar el instalador de Windows (Inno Setup 6) para MyKOS-Whats
# ==============================================================================
param(
    [string]$BuildDir = "build",
    [string]$Config = "Release"
)
$ErrorActionPreference = "Stop"

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$ProjectRoot = Split-Path -Parent (Split-Path -Parent $ScriptDir)
$DistDir = Join-Path $ProjectRoot "dist"
$Win64Dir = Join-Path $DistDir "win64"

Write-Host "============================================================" -ForegroundColor Cyan
Write-Host "  Compilador de Instalador Windows para MyKOS-Whats" -ForegroundColor Cyan
Write-Host "============================================================" -ForegroundColor Cyan

# 1. Verificar binario compilado
$ExePath = Join-Path $ProjectRoot "$BuildDir\$Config\MyKOS-Whats.exe"
if (-not (Test-Path $ExePath)) {
    $ExePath = Join-Path $ProjectRoot "$BuildDir\MyKOS-Whats.exe"
}

if (-not (Test-Path $ExePath)) {
    Write-Host "▶ Compilando proyecto con CMake..." -ForegroundColor Cyan
    Set-Location $ProjectRoot
    cmake -B $BuildDir -DCMAKE_BUILD_TYPE=$Config
    cmake --build $BuildDir --config $Config -j $env:NUMBER_OF_PROCESSORS
}

if (-not (Test-Path $ExePath)) {
    Write-Error "❌ No se encontró el ejecutable MyKOS-Whats.exe en $ExePath"
    exit 1
}

# 2. Preparar carpeta staging dist/win64 y desplegar dependencias de Qt
Write-Host "▶ Desplegando runtime de Qt con windeployqt..." -ForegroundColor Cyan
if (Test-Path $Win64Dir) {
    Remove-Item -Recurse -Force $Win64Dir
}
New-Item -ItemType Directory -Force -Path $Win64Dir | Out-Null

Copy-Item $ExePath -Destination (Join-Path $Win64Dir "MyKOS-Whats.exe") -Force

# Ejecutar windeployqt para copiar DLLs, plugins y recursos de WebEngine
$windeployqt = "windeployqt.exe"
if (Get-Command $windeployqt -ErrorAction SilentlyContinue) {
    & $windeployqt --release --compiler-runtime --webengine (Join-Path $Win64Dir "MyKOS-Whats.exe")
} else {
    Write-Host "⚠️ windeployqt.exe no está en el PATH. Asumiendo entorno pre-desplegado." -ForegroundColor Yellow
}

# 3. Localizar Inno Setup (ISCC.exe)
$isccPaths = @(
    "${env:ProgramFiles(x86)}\Inno Setup 6\ISCC.exe",
    "${env:ProgramFiles}\Inno Setup 6\ISCC.exe",
    "C:\Program Files (x86)\Inno Setup 6\ISCC.exe",
    "C:\Program Files\Inno Setup 6\ISCC.exe"
)
$iscc = $isccPaths | Where-Object { Test-Path $_ } | Select-Object -First 1

if (-not $iscc) {
    if (Get-Command "ISCC.exe" -ErrorAction SilentlyContinue) {
        $iscc = "ISCC.exe"
    } else {
        Write-Error "❌ No se encontró Inno Setup 6 (ISCC.exe). Instálalo con: winget install JRSoftware.InnoSetup o choco install innosetup"
        exit 1
    }
}

Write-Host "▶ Compilando instalador con Inno Setup..." -ForegroundColor Cyan
& $iscc (Join-Path $ScriptDir "mykos-whats-setup.iss")

Write-Host "============================================================" -ForegroundColor Green
Write-Host "  ✅ Instalador compilado con éxito en dist\" -ForegroundColor Green
Write-Host "============================================================" -ForegroundColor Green
