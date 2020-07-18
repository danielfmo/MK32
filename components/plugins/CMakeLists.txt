idf_component_register(SRCS "layout_server.c" "plugin_manager.c" "plugins.c" "testFunc.c"
                       INCLUDE_DIRS "." "../../main" "../ble_new" "../nvs_funcs" "../hid_keycode_conv" "../wifi_manager"
                       REQUIRES "bt" "freertos" "nvs_flash" "json" "hid_keycode_conv" "wifi_manager")

