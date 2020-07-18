idf_component_register(SRCS "wifi_manager.c"
                       INCLUDE_DIRS "." "../../main" "../.." "../plugins" "../hid_keycode_conv"
                       REQUIRES "freertos" "esp_wifi" "nvs_flash" "tcpip_adapter" "lwip")