cmake_minimum_required(VERSION 3.16)

project(MyKOS-Whats VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

option(USE_SYSTEM_QT "Forzar el uso de bibliotecas Qt6 del sistema (para empaquetado .deb)" OFF)
if(NOT USE_SYSTEM_QT)
    if(DEFINED ENV{QT_ROOT} AND EXISTS "$ENV{QT_ROOT}")
        list(PREPEND CMAKE_PREFIX_PATH "$ENV{QT_ROOT}")
    elseif(DEFINED QT_ROOT AND EXISTS "${QT_ROOT}")
        list(PREPEND CMAKE_PREFIX_PATH "${QT_ROOT}")
    elseif(EXISTS "/home/ruben/Qt/6.11.3/gcc_64")
        list(PREPEND CMAKE_PREFIX_PATH "/home/ruben/Qt/6.11.3/gcc_64")
    endif()
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Gui WebEngineWidgets Widgets Network)
find_package(Qt6 OPTIONAL_COMPONENTS Test)

qt_standard_project_setup()

include(GNUInstallDirs)
include_directories(${CMAKE_SOURCE_DIR}/src)

# Scripts JS inyectados en el WebView. GLOB con CONFIGURE_DEPENDS: cualquier
# .js nuevo en src/web/scripts se recoge al recompilar sin tocar este fichero.
file(GLOB JS_SCRIPTS CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/web/scripts/*.js")
list(SORT JS_SCRIPTS)

# Generar el .qrc con los scripts y recursos en tiempo de configuración
set(RESOURCES_CONTENT "<RCC>\n  <qresource prefix=\"/web/scripts/\">\n")

foreach(js_file ${JS_SCRIPTS})
    get_filename_component(js_name ${js_file} NAME)
    set(RESOURCES_CONTENT "${RESOURCES_CONTENT}    <file alias=\"${js_name}\">${js_file}</file>\n")
endforeach()

set(RESOURCES_CONTENT "${RESOURCES_CONTENT}  </qresource>\n")
set(RESOURCES_CONTENT "${RESOURCES_CONTENT}  <qresource prefix=\"/icons/\">\n")
set(RESOURCES_CONTENT "${RESOURCES_CONTENT}    <file alias=\"app.png\">${CMAKE_SOURCE_DIR}/resources/icons/mykos-whats.png</file>\n")
set(RESOURCES_CONTENT "${RESOURCES_CONTENT}  </qresource>\n</RCC>\n")
file(WRITE "${CMAKE_BINARY_DIR}/resources.qrc" ${RESOURCES_CONTENT})

set(WINDOWS_RESOURCES "")
if(WIN32)
    set(WINDOWS_RESOURCES resources/mykos-whats.rc)
endif()

qt_add_executable(MyKOS-Whats
    src/main.cpp
    src/core/app_lock.cpp
    src/core/app_lock.h
    src/ui/lockoverlay.cpp
    src/ui/lockoverlay.h
    src/ui/mainwindow.cpp
    src/ui/mainwindow.h
    ${CMAKE_BINARY_DIR}/resources.qrc
    ${WINDOWS_RESOURCES}
)

if(WIN32)
    set_target_properties(MyKOS-Whats PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

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

install(FILES data/mykos-whats.desktop
    DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
)

install(FILES resources/icons/mykos-whats.png
    DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/512x512/apps
)

# Empaquetado CPack oficial
set(CPACK_PACKAGE_NAME "mykos-whats")
set(CPACK_PACKAGE_VENDOR "MyKOS")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "WhatsApp Web nativo con pantalla de bloqueo y tema MyKOS")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/eskib0/mykos-whats")
set(CPACK_PACKAGE_CONTACT "Ruben <eskib0@github.com>")
set(CPACK_DEBIAN_PACKAGE_SECTION "net")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.34), libstdc++6 (>= 11), libqt6core6t64 | libqt6core6 (>= 6.4.0), libqt6gui6t64 | libqt6gui6 (>= 6.4.0), libqt6widgets6t64 | libqt6widgets6 (>= 6.4.0), libqt6webenginewidgets6 (>= 6.4.0), libqt6webenginecore6 (>= 6.4.0), libqt6network6t64 | libqt6network6 (>= 6.4.0), libqt6webchannel6 (>= 6.4.0)")
include(CPack)

if(TARGET Qt6::Test)
    enable_testing()
    qt_add_executable(test_app_lock
        tests/test_app_lock.cpp
        src/core/app_lock.cpp
        src/core/app_lock.h
    )
    target_link_libraries(test_app_lock PRIVATE
        Qt::Core
        Qt::Network
        Qt::Test
    )
    add_test(NAME test_app_lock COMMAND test_app_lock)
endif()
