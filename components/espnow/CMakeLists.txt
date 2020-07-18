idf_component_register(SRCS "espnow_recieve.c" "espnow_send.c"
                       INCLUDE_DIRS "." "../../main" "../ble_new" "../nvs_funcs"
                       REQUIRES "bt" "freertos" "nvs_flash")

