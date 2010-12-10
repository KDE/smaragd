include_directories (
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/engines
)

# Allow // comments in .c files
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

set (emerald_engines_SOURCES
    engines/legacy.c
    engines/line.c
    engines/oxygen.c
    engines/pixmap.c
    engines/truglass.c
    engines/vrunner.c
    engines/zootreeves.c
)

set (smaragd_SOURCES
    kwin_smaragd.cpp
    emerald.c
    main.c
    shadowengine.cpp
)

kde4_add_plugin (kwin3_smaragd ${smaragd_SOURCES} ${emerald_engines_SOURCES})
target_link_libraries(kwin3_smaragd
    ${KDE4_KDEUI_LIBS}
    ${KDE4_PLASMA_LIBS}
    ${CAIRO_LIBRARIES}
    ${GLIB2_LIBRARIES}
    ${KDE4WORKSPACE_KDECORATIONS_LIBS}
)

install (TARGETS kwin3_smaragd DESTINATION ${PLUGIN_INSTALL_DIR})
install (FILES smaragd.desktop DESTINATION ${DATA_INSTALL_DIR}/kwin)
install (FILES smaragd.knsrc DESTINATION ${CONFIG_INSTALL_DIR})

add_subdirectory (config)

