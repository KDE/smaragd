project (smaragd)

cmake_minimum_required(VERSION 3.16)

find_package (ECM 1.0.0 REQUIRED NO_MODULE)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
    ${ECM_MODULE_PATH}
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules
)

find_package (Qt5 REQUIRED CONFIG COMPONENTS Core Gui Widgets)
find_package (KF5 REQUIRED COMPONENTS Config CoreAddons I18n WidgetsAddons)
find_package (KDecoration2 REQUIRED)
find_package (GLIB2 REQUIRED)
find_package (Cairo REQUIRED)
find_package (Intl REQUIRED)

include (KDEInstallDirs)
include (KDECMakeSettings)
include (KDECompilerSettings)

include_directories (
    ${CMAKE_BINARY_DIR}
    ${CAIRO_INCLUDE_DIRS}
    ${GLIB2_INCLUDE_DIR}
)

add_subdirectory (src)

