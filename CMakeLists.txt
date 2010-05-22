project (smaragd)

cmake_minimum_required (VERSION 2.6)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules
)

find_package (KDE4 REQUIRED)
find_package (KDE4Workspace REQUIRED)
find_package (GLIB2 REQUIRED)
find_package (Cairo REQUIRED)

include (KDE4Defaults)
include (MacroLibrary)

add_definitions (
    ${QT_DEFINITIONS}
    ${KDE4_DEFINITIONS}
)
include_directories (
    ${KDE4_INCLUDES}
    ${KDE4WORKSPACE_INCLUDE_DIR}
    ${CMAKE_BINARY_DIR}
    ${CAIRO_INCLUDE_DIRS}
    ${GLIB2_INCLUDE_DIR}
)

add_subdirectory (src)

