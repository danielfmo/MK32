#ifndef KEYBOARD_CONFIG_H
#define KEYBOARD_CONFIG_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>
#include "driver/gpio.h"
#include "driver/touch_pad.h"
#include "driver/adc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#define MODULE_ID "LOLIN 32"
#define GATTS_TAG "Backpack Keyboard"  // The device's name
#define MAX_BT_DEVICENAME_LENGTH 40

#define MASTER  // undefine if you are flashing the slave controller

#define DEBOUNCE 5  // debounce time in ms

// Define matrix
#define KEYPADS 1  // intended in order to create a Multiple keypad split boards
#define MATRIX_ROWS 6
#define MATRIX_COLS 17  // For split keyboards, define columns for one side only.

//#define NKRO // does not work on Android and iOS!,  we can get 18KRO on those
#define LAYERS 3  // number of layers defined

// Select diode direction
#define COL2ROW
//#define ROW2COL

/*Battery monitoring
 * Please read check battery_monitor.h for resistor values before applying
 * use ADC1 only,  */

#define BATT_STAT                // define to enable battery monitoring
#define BATT_PIN ADC1_CHANNEL_6  // gpio pin 34, refer to the esp32 before modifying

// deep sleep parameters, mind that reconnecting after deep sleep might take a minute or two
#define SLEEP_MINS 45  // undefine if you do not need deep sleep, otherwise define number of minutes for deepsleep

/*
 *---------------------------- Everything below here should not be modified for standard usage----------------------
 *
 * */
#define CHECK_BIT(var, pos) ((var) & (1 << (pos)))
#define SET_BIT(var, pos) (var |= 1UL << pos);

#define MAX_LAYER (LAYERS - 1)
#define MOD_LED_BYTES 2                                                     // bytes for led status and modifiers
#define MACRO_LEN 3                                                         // keys for macros
#define KEYMAP_COLS MATRIX_COLS* KEYPADS                                    // used for a symmetrical split keyboard
#define REPORT_LEN (MOD_LED_BYTES + MACRO_LEN + MATRIX_ROWS * KEYMAP_COLS)  // size of hid reports with NKRO and room for 3 key macro
#define REPORT_COUNT_BYTES (MATRIX_ROWS * KEYMAP_COLS + MACRO_LEN)

#define PLUGIN_BASE_VAL 0x135
#define LAYER_HOLD_MAX_VAL 0x134
#define LAYER_HOLD_BASE_VAL 0x123
#define MACRO_BASE_VAL 0x103
#define LAYERS_BASE_VAL 0xFF

typedef struct config_data {
    char bt_device_name[MAX_BT_DEVICENAME_LENGTH];
} config_data_t;

extern uint8_t current_layout;
extern uint8_t curr_led;

#define MAX_LAYOUT_NAME_LENGTH 15
// array to hold names of layouts for oled
extern char default_layout_names[LAYERS][MAX_LAYOUT_NAME_LENGTH];

extern TaskHandle_t xKeyreportTask;

#endif
//
