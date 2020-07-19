idf_component_register(SRCS "keymap.c" "keypress_handles.c" "matrix.c" "mk32_main.cpp"
                       INCLUDE_DIRS "." "../components/plugins" "../components/hid_keycode_conv" "../components/wifi_manager" "../components/nvs_funcs" "../components/battery_monitor" "../components/ble_new"
                      REQUIRES "plugins")
