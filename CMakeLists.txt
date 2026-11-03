cmake_minimum_required(VERSION 3.16)

project(MyKOS-Whast VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui WebEngineWidgets Widgets Network)

qt_standard_project_setup()

# Archivo de recursos para los scripts JS
set(RESOURCES_CONTENT "<RCC>\n  <qresource prefix=\"/scripts\">\n")
# Listar scripts JS explícitamente para que CMake los incluya sin regenerar.
# Si se añaden nuevos .js, actualizar esta lista.
set(JS_SCRIPTS
    "${CMAKE_SOURCE_DIR}/src/web/scripts/connection-watchdog.js"
    "${CMAKE_SOURCE_DIR}/src/web/scripts/file-drop.js"
    "${CMAKE_SOURCE_DIR}/src/web/scripts/sw-recovery.js"
    "${CMAKE_SOURCE_DIR}/src/web/scripts/theme-control.js"
)
foreach(js_file ${JS_SCRIPTS})
    get_filename_component(js_name ${js_file} NAME)
    set(RESOURCES_CONTENT "${RESOURCES_CONTENT}    <file alias=\"${js_name}\">${js_file}</file>\n")
endforeach()
set(RESOURCES_CONTENT "${RESOURCES_CONTENT}  </qresource>\n</RCC>\n")

file(WRITE "${CMAKE_BINARY_DIR}/resources.qrc" ${RESOURCES_CONTENT})

qt_add_executable(MyKOS-Whast
    src/main.cpp
    src/core/app_lock.cpp
    src/core/app_lock.h
    ${CMAKE_BINARY_DIR}/resources.qrc
)

target_link_libraries(MyKOS-Whast PRIVATE
    Qt::Core
    Qt::Gui
    Qt::WebEngineWidgets
    Qt::Widgets
    Qt::Network
)

install(TARGETS MyKOS-Whast
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
