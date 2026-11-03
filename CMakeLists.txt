cmake_minimum_required(VERSION 3.16)

project(MyKOS-Whats VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui WebEngineWidgets Widgets Network)

qt_standard_project_setup()

include(GNUInstallDirs)
include_directories(${CMAKE_SOURCE_DIR}/src)

# Scripts JS inyectados en el WebView. GLOB con CONFIGURE_DEPENDS: cualquier
# .js nuevo en src/web/scripts se recoge al recompilar sin tocar este fichero.
file(GLOB JS_SCRIPTS CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/web/scripts/*.js")
list(SORT JS_SCRIPTS)

# Generar el .qrc con los scripts en tiempo de configuración
set(RESOURCES_CONTENT "<RCC>\n  <qresource prefix=\"/web/scripts/\">\n")

foreach(js_file ${JS_SCRIPTS})
    get_filename_component(js_name ${js_file} NAME)
    set(RESOURCES_CONTENT "${RESOURCES_CONTENT}    <file alias=\"${js_name}\">${js_file}</file>\n")
endforeach()

set(RESOURCES_CONTENT "${RESOURCES_CONTENT}  </qresource>\n</RCC>\n")
file(WRITE "${CMAKE_BINARY_DIR}/resources.qrc" ${RESOURCES_CONTENT})

qt_add_executable(MyKOS-Whats
    src/main.cpp
    src/core/app_lock.cpp
    src/core/app_lock.h
    src/ui/lockoverlay.cpp
    src/ui/lockoverlay.h
    src/ui/mainwindow.cpp
    src/ui/mainwindow.h
    ${CMAKE_BINARY_DIR}/resources.qrc
)

target_link_libraries(MyKOS-Whats PRIVATE
    Qt::Core
    Qt::Gui
    Qt::WebEngineWidgets
    Qt::Widgets
    Qt::Network
)

install(TARGETS MyKOS-Whats
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
