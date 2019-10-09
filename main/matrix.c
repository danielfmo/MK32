/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 * Copyright 2018 Gal Zaidenstein.
 */

#ifndef MATRIX_C
#define MATRIX_C

//GPIO libraries
#include "esp_system.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "keyboard_config.h"
#include "esp_sleep.h"
#include "esp_log.h"
#define GPIO_TAG "GPIO"
#define NOP() asm volatile ("nop")
/* Define pins, notice that:
 * GPIO6-11 are usually used for SPI flash
 * GPIO34-39 can only be set as input mode and do not have software pullup or pulldown functions.
 * GPIOS 0,2,4,12-15,25-27,32-39 Can be used as RTC GPIOS as well (please read about power management in ReadMe)
 */
const gpio_num_t MATRIX_ROWS_PINS[] = { GPIO_NUM_33, GPIO_NUM_36, GPIO_NUM_37,
					GPIO_NUM_38,GPIO_NUM_39,GPIO_NUM_35};

const gpio_num_t MATRIX_COLS_PINS[] = { GPIO_NUM_0, GPIO_NUM_2, GPIO_NUM_17, GPIO_NUM_4, GPIO_NUM_16,  
					GPIO_NUM_15, GPIO_NUM_19, GPIO_NUM_23, GPIO_NUM_22, GPIO_NUM_5,                        
					GPIO_NUM_18, GPIO_NUM_32, GPIO_NUM_26, GPIO_NUM_14, GPIO_NUM_21,
					GPIO_NUM_27, GPIO_NUM_25};

// matrix states
uint8_t MATRIX_STATE[MATRIX_ROWS][MATRIX_COLS] = { 0 };
uint8_t PREV_MATRIX_STATE[MATRIX_ROWS][MATRIX_COLS] = { 0 };
uint8_t SLAVE_MATRIX_STATE[MATRIX_ROWS][MATRIX_COLS] = { 0 };

uint32_t lastDebounceTime = 0;

uint8_t (*matrix_states[])[MATRIX_ROWS][MATRIX_COLS] = { &MATRIX_STATE,
		&SLAVE_MATRIX_STATE, };

//for microseconds
//If the matrix scans too quickly and the inputs are in Pulldown state, 
//the resistor(50K) built on the esp32 doesnt drain quickly enough...
//which registers the next time the pin is read. The sleep lets it drain...only need 4 µseconds.
//TODO Make this better..right now its a tight loop which might be worse for battery(?)
unsigned long IRAM_ATTR micros()
{
    return (unsigned long) (esp_timer_get_time());
}
void IRAM_ATTR delayMicroseconds(uint32_t us)
{
    uint32_t m = micros();
    if(us){
        uint32_t e = (m + us);
        if(m > e){ //overflow
            while(micros() > e){
                NOP();
            }
        }
        while(micros() < e){
            NOP();
        }
    }
}


//used for debouncing
static uint32_t millis() {
	return esp_timer_get_time() / 1000;
}

// deinitializing rtc matrix pins on  deep sleep wake up
void rtc_matrix_deinit(void) {

	// Deinitializing columns
	for (uint8_t col = 0; col < MATRIX_COLS; col++) {

		if (rtc_gpio_is_valid_gpio(MATRIX_COLS_PINS[col]) == 1) {
			rtc_gpio_set_level(MATRIX_COLS_PINS[col], 0);
			rtc_gpio_set_direction(MATRIX_COLS_PINS[col],
					RTC_GPIO_MODE_DISABLED);
			gpio_reset_pin(MATRIX_COLS_PINS[col]);
		}
	}

	// Deinitializing rows
	for (uint8_t row = 0; row < MATRIX_ROWS; row++) {

		if (rtc_gpio_is_valid_gpio(MATRIX_ROWS_PINS[row]) == 1) {
			rtc_gpio_set_level(MATRIX_ROWS_PINS[row], 0);
			rtc_gpio_set_direction(MATRIX_ROWS_PINS[row],
					RTC_GPIO_MODE_DISABLED);
			gpio_reset_pin(MATRIX_ROWS_PINS[row]);
		}
	}
}

// Initializing rtc matrix pins for deep sleep wake up
void rtc_matrix_setup(void) {
	uint64_t rtc_mask = 0;
#ifdef COL2ROW
	// Initializing columns-Output
	for (uint8_t col = 0; col < MATRIX_COLS; col++) {

		if (rtc_gpio_is_valid_gpio(MATRIX_COLS_PINS[col]) == 1) {
			rtc_gpio_init((MATRIX_COLS_PINS[col]));
			rtc_gpio_set_direction(MATRIX_COLS_PINS[col],
					RTC_GPIO_MODE_INPUT_OUTPUT);
			rtc_gpio_set_level(MATRIX_COLS_PINS[col], 1);

			ESP_LOGI(GPIO_TAG,"%d is level %d", MATRIX_COLS_PINS[col],
					gpio_get_level(MATRIX_COLS_PINS[col]));
		}
	}

	// Initializing rows-only input
	for (uint8_t row = 0; row < MATRIX_ROWS; row++) {

		if (rtc_gpio_is_valid_gpio(MATRIX_ROWS_PINS[row]) == 1) {
			rtc_gpio_init((MATRIX_ROWS_PINS[row]));
			rtc_gpio_set_direction(MATRIX_ROWS_PINS[row],
					RTC_GPIO_MODE_INPUT_ONLY);//if using RTC_GPIO_MODE_INPUT_OUTPUT this fails on input only pins...hence INPUT_ONLY
			rtc_gpio_wakeup_enable(MATRIX_ROWS_PINS[row], GPIO_INTR_HIGH_LEVEL);
			SET_BIT(rtc_mask, MATRIX_ROWS_PINS[row]);

			ESP_LOGI(GPIO_TAG,"%d is level %d", MATRIX_ROWS_PINS[row],
					gpio_get_level(MATRIX_ROWS_PINS[row]));
		}
		esp_sleep_enable_ext1_wakeup(rtc_mask, ESP_EXT1_WAKEUP_ANY_HIGH);
	}
#ifdef ROW2COL
	// Initializing rows-output
	for (uint8_t row = 0; col < MATRIX_ROWS; row++) {

		if (rtc_gpio_is_valid_gpio(MATRIX_ROWS_PINS[row]) == 1) {
			rtc_gpio_init((MATRIX_ROWS_PINS[row]));
			rtc_gpio_set_direction(MATRIX_ROWS_PINS[row],
					RTC_GPIO_MODE_INPUT_OUTPUT);
			ESP_LOGI(GPIO_TAG,"%d is level %d", MATRIX_ROWS_PINS[row],
					gpio_get_level(MATRIX_ROWS_PINS[row]));
		}
	}

	// Initializing columns -Input
	for (uint8_t col = 0; col < MATRIX_COLS; col++) {

		if (rtc_gpio_is_valid_gpio(MATRIX_COLS_PINS[col]) == 1) {
			rtc_gpio_init((MATRIX_COLS_PINS[col]));
			rtc_gpio_set_direction(MATRIX_COLS_PINS[col],
					RTC_GPIO_MODE_INPUT_ONLY);
			rtc_gpio_wakeup_enable(MATRIX_COLS_PINS[col], GPIO_INTR_HIGH_LEVEL);
			SET_BIT(rtc_mask, MATRIX_COLS_PINS[col]);

			ESP_LOGI(GPIO_TAG,"%d is level %d", MATRIX_COLS_PINS[col],
					gpio_get_level(MATRIX_COLS_PINS[col]));
		}
		esp_sleep_enable_ext1_wakeup(rtc_mask, ESP_EXT1_WAKEUP_ANY_HIGH);
	}
}

// Initializing matrix pins
void matrix_setup(void) {

#ifdef COL2ROW
	// Initializing columns-Output Only
	for (uint8_t col = 0; col < MATRIX_COLS; col++) {

		gpio_pad_select_gpio(MATRIX_COLS_PINS[col]);
		gpio_set_direction(MATRIX_COLS_PINS[col], GPIO_MODE_INPUT_OUTPUT);
		gpio_set_level(MATRIX_COLS_PINS[col], 0);

		ESP_LOGI(GPIO_TAG,"%d is level %d", MATRIX_COLS_PINS[col],
				gpio_get_level(MATRIX_COLS_PINS[col]));
	}

	// Initializing rows-Input Only
	for (uint8_t row = 0; row < MATRIX_ROWS; row++) {

		gpio_pad_select_gpio(MATRIX_ROWS_PINS[row]);
		gpio_set_direction(MATRIX_ROWS_PINS[row], GPIO_MODE_INPUT);
		gpio_set_pull_mode(MATRIX_ROWS_PINS[row], GPIO_PULLDOWN_ONLY);
		gpio_pulldown_en(MATRIX_ROWS_PINS[row]);	
	}
#endif
#ifdef ROW2COL
	// Initializing rows
	for(uint8_t row=0; row < MATRIX_ROWS; row++) {

		gpio_pad_select_gpio(MATRIX_ROWS_PINS[row]);
		gpio_set_direction(MATRIX_ROWS_PINS[row], GPIO_MODE_INPUT_OUTPUT);
		gpio_set_level(MATRIX_ROWS_PINS[row], 0);
		ESP_LOGI(GPIO_TAG,"%d is level %d",MATRIX_ROWS_PINS[row],gpio_get_level(MATRIX_ROWS_PINS[row]));
	}

	// Initializing columns
	for(uint8_t col=0; col < MATRIX_COLS; col++) {

		gpio_pad_select_gpio(MATRIX_COLS_PINS[col]);
	        gpio_set_direction(MATRIX_COLS_PINS[col], GPIO_MODE_INPUT);
                gpio_set_pull_mode(MATRIX_COLS_PINS[col], GPIO_PULLDOWN_ONLY);
                gpio_pulldown_en(MATRIX_COLS_PINS[col]);
	}
#endif
}

uint8_t curState = 0;
uint32_t DEBOUNCE_MATRIX[MATRIX_ROWS][MATRIX_COLS] = { 0 };
// Scanning the matrix for input
void scan_matrix(void) {
#ifdef COL2ROW
	// Setting column pin as high, and checking if the input of a row pin changes.
	for (uint8_t col = 0; col < MATRIX_COLS; col++) {
		gpio_set_level(MATRIX_COLS_PINS[col], 1);
		for (uint8_t row = 0; row < MATRIX_ROWS; row++) {

			curState = gpio_get_level(MATRIX_ROWS_PINS[row]);
			if (PREV_MATRIX_STATE[row][col] != curState) {
				DEBOUNCE_MATRIX[row][col] = millis();
			}
			PREV_MATRIX_STATE[row][col] = curState;
			if ((millis() - DEBOUNCE_MATRIX[row][col]) > DEBOUNCE) {

				if (MATRIX_STATE[row][col] != curState) {
					MATRIX_STATE[row][col] = curState;
				}

			}
		}
		gpio_set_level(MATRIX_COLS_PINS[col], 0);
		delayMicroseconds(4);
	}

#endif
#ifdef ROW2COL
	// Setting row pin as high, and checking if the input of a column pin changes.
	for(uint8_t row=0; row < MATRIX_ROWS; row++) {
		gpio_set_level(MATRIX_ROWS_PINS[row], 1);

		for(uint8_t col=0; col <MATRIX_COLS; col++) {

			curState = gpio_get_level(MATRIX_ROWS_PINS[row]);
			if( PREV_MATRIX_STATE[row][col] != curState) {
				DEBOUNCE_MATRIX[row][col] = millis();
			}
			PREV_MATRIX_STATE[row][col] = curState;
			if( (millis() - DEBOUNCE_MATRIX[row][col]) > DEBOUNCE) {

				if( MATRIX_STATE[row][col] != curState) {
					MATRIX_STATE[row][col] = curState;
				}

			}
		}
		gpio_set_level(MATRIX_ROWS_PINS[row], 0);
		delayMicroseconds(4);
	}
#endif

}

#endif
