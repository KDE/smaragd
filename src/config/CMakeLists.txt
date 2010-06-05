set (kwin_smaragd_config_SOURCES
    kwin_smaragd_config.cpp
)

kde4_add_ui_files(kwin_smaragd_config_SOURCES kwin_smaragd_dialog.ui)

kde4_add_plugin (kwin_smaragd_config ${kwin_smaragd_config_SOURCES})
target_link_libraries (kwin_smaragd_config ${KDE4_KDEUI_LIBS})

install (TARGETS kwin_smaragd_config DESTINATION ${PLUGIN_INSTALL_DIR})

