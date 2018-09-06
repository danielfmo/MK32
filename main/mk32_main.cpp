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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "driver/touch_pad.h"
#include "esp_timer.h"
#include "esp_sleep.h"
#include "esp_pm.h"


//HID Ble functions
#include "HID_kbdmousejoystick.h"

//MK32 functions
#include "matrix.c"
#include "keypress_handles.c"
#include "keyboard_config.h"
#include "espnow_recieve.h"
#include "espnow_send.h"
#include "r_encoder.h"
#include "oled_tasks.h"
#include "oled_tasks.h"

#define KEY_REPORT_TAG "KEY_REPORT"
#define SYSTEM_REPORT_TAG "KEY_REPORT"

static config_data_t config;
QueueHandle_t espnow_recieve_q;

bool DEEP_SLEEP = true; // flag to check if we need to go to deep sleep
bool BLE_SLEEP= true;


extern "C" void key_reports(void *pvParameters)
{
	// Arrays for holding the report at various stages
	uint8_t past_report[REPORT_LEN]={0};
	uint8_t report_state[REPORT_LEN];

	bool CON_LOG_FLAG = false; // Just because I don't want it to keep logging the same thing a billion times

	while(1){
		while(HID_kbdmousejoystick_isConnected() == 0) {
			if (CON_LOG_FLAG == false){
				ESP_LOGI(KEY_REPORT_TAG,"Not connected, waiting for connection ");
			}
#ifdef OLED_ENABLE
			waiting_oled();
#endif
			DEEP_SLEEP = false;
			CON_LOG_FLAG = true;
		}

#ifdef OLED_ENABLE
			ble_connected_oled();
#endif

		while(HID_kbdmousejoystick_isConnected() != 0) {
			memcpy(report_state, check_key_state(*layouts[current_layout]), sizeof report_state);

#ifdef OLED_ENABLE
			update_oled();
#endif
			//Do not send anything if queues are uninitialized
			if(mouse_q == NULL || keyboard_q == NULL || joystick_q == NULL)
			{
				ESP_LOGE(KEY_REPORT_TAG,"queues not initialized");
			}
			else{
				if(memcmp(past_report, report_state, sizeof past_report)!=0){
					DEEP_SLEEP = false;
					memcpy(past_report,report_state, sizeof past_report );
					xQueueSend(keyboard_q,(void*)&report_state, (TickType_t) 0);
					vTaskDelay(3);

				}

			}
		}
		CON_LOG_FLAG = false;
	}

}

extern "C" void encoder_report(void *pvParameters){
	uint8_t encoder_state[1]={0};
	uint8_t past_encoder_state[1]={0};

	while(1){
		encoder_state[0]=r_encoder_state();
		if(encoder_state[0]!=past_encoder_state[0]){
			DEEP_SLEEP = false;
			xQueueSend(media_q,(void*)&encoder_state, (TickType_t) 0);
			past_encoder_state[0] = encoder_state[0];
		}

	}
}

extern "C" void slave_encoder_report(void *pvParameters){
	uint8_t encoder_state[1]={0};
	uint8_t past_encoder_state[1]={0};

	while(1){
		encoder_state[0]=r_encoder_state();
		if(encoder_state[0]!=past_encoder_state[0]){
			DEEP_SLEEP = false;
			xQueueSend(espnow_encoder_send_q,(void*)&encoder_state, (TickType_t) 0);
			past_encoder_state[0] = encoder_state[0];
		}

	}
}


//Function for sending out the modified matrix
extern "C" void slave_scan(void *pvParameters){

	uint8_t PAST_MATRIX[MATRIX_ROWS][MATRIX_COLS]={0};

	while(1){
		scan_matrix();
		if(memcmp(&PAST_MATRIX, &MATRIX_STATE, sizeof MATRIX_STATE)!=0){
			DEEP_SLEEP = false;
			memcpy(&PAST_MATRIX, &MATRIX_STATE, sizeof MATRIX_STATE );

			xQueueSend(espnow_matrix_send_q,(void*)&MATRIX_STATE, (TickType_t) 0);
		}
	}
}

//Update the matrix state via reports recieved by espnow
extern "C" void espnow_update_matrix(void *pvParameters){


	uint8_t CURRENT_MATRIX[MATRIX_ROWS][MATRIX_COLS]={0};
	while(1){
		if(xQueueReceive(espnow_recieve_q,&CURRENT_MATRIX,10000))
		{
			DEEP_SLEEP = false;
			memcpy(&SLAVE_MATRIX_STATE, &CURRENT_MATRIX, sizeof CURRENT_MATRIX );
		}
	}
}

//what to do after waking from deep sleep
extern "C" void esp_wake_deep_sleep(void) {
    esp_default_wake_deep_sleep();
    touch_pad_deinit();
}

/*If no key press has been recieved in SLEEP_MINS amount of minutes, put device into deep sleep
 *  wake up on touch on GPIO pin 2
 *  */
extern "C" void deep_sleep(void *pvParameters){


	uint64_t initial_time = esp_timer_get_time(); // notice that timer returns time passed in microseconds!
	uint64_t current_time_passed = 0;
	while(1){

		current_time_passed=(esp_timer_get_time()-initial_time);

		if(DEEP_SLEEP == false){
			current_time_passed  = 0;
			initial_time = esp_timer_get_time();
			DEEP_SLEEP= true;
		}
		//
		if(current_time_passed>=1000000*60*SLEEP_MINS){
			if(DEEP_SLEEP == true){
				ESP_LOGE(SYSTEM_REPORT_TAG,"going to sleep!");
#ifdef OLED_ENABLE
				deinit_oled();
#endif
				esp_bluedroid_disable();
				esp_bt_controller_disable();
				esp_wifi_stop();
				// wake up esp32 using touch sensor
				touch_pad_init();
				touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER);
				touch_pad_config(TOUCH_PAD_NUM2,TOUCH_THRESHOLD);
				esp_sleep_enable_touchpad_wakeup();
				esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH,ESP_PD_OPTION_AUTO);
				esp_deep_sleep_start();

			}
			if(DEEP_SLEEP == false){
				current_time_passed = 0;
				initial_time = esp_timer_get_time();
				DEEP_SLEEP= true;
			}
		}


	}

}

extern "C" void app_main()
{
	//	esp_pm_config_esp32_t pm_config;
	//	pm_config.max_freq_mhz = 10;
	//	pm_config.min_freq_mhz = 10;
	//	esp_pm_configure(&pm_config);
	matrix_setup();
	esp_err_t ret;

	// Initialize NVS.
	ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK( ret );

	// Read config
	nvs_handle my_handle;
	ESP_LOGI("MAIN","loading configuration from NVS");
	ret = nvs_open("config_c", NVS_READWRITE, &my_handle);
	if(ret != ESP_OK) ESP_LOGE("MAIN","error opening NVS");
	size_t available_size = MAX_BT_DEVICENAME_LENGTH;
	strcpy(config.bt_device_name, GATTS_TAG);
	nvs_get_str (my_handle, "btname", config.bt_device_name, &available_size);
	if(ret != ESP_OK)
	{
		ESP_LOGE("MAIN","error reading NVS - bt name, setting to default");
		strcpy(config.bt_device_name, GATTS_TAG);
	} else ESP_LOGI("MAIN","bt device name is: %s",config.bt_device_name);

	esp_log_level_set("*", ESP_LOG_INFO);

	//activate oled
#ifdef	OLED_ENABLE
	init_oled();
#endif

	//If the device is a slave initialize sending reports to master
#ifdef SLAVE
	xTaskCreatePinnedToCore(slave_scan, "Scan matrix changes for slave", 4096, NULL, configMAX_PRIORITIES, NULL,1);

#ifdef R_ENCODER_SLAVE
	xTaskCreatePinnedToCore(slave_encoder_report, "Scan encoder changes for slave", 4096, NULL, configMAX_PRIORITIES, NULL,1);
#endif

#ifdef OLED_ENABLE
	ble_slave_oled();
#endif

	espnow_send();
#endif



	//If the device is a master for split board initialize receiving reports from slave
#ifdef SPLIT_MASTER

	uint8_t array_sample[REPORT_LEN];
	espnow_recieve_q = xQueueCreate(32,sizeof(array_sample));
	espnow_recieve();
	xTaskCreatePinnedToCore(espnow_update_matrix, "ESP-NOW slave matrix state", 4096, NULL, configMAX_PRIORITIES, NULL,1);
#endif

#ifdef MASTER
	//activate keyboard BT stack
	HID_kbdmousejoystick_init(1,1,0,0,config.bt_device_name);
	ESP_LOGI("HIDD","MAIN finished...");
	//activate encoder functions
#ifdef	R_ENCODER
	r_encoder_setup();
	xTaskCreatePinnedToCore(encoder_report, "encoder report", 4096, NULL, configMAX_PRIORITIES, NULL,1);
#endif

	// Start the keyboard Tasks
	// Create the key scanning task on core 1 (otherwise it will crash)
	xTaskCreatePinnedToCore(key_reports, "key report task", 4096, NULL, configMAX_PRIORITIES, NULL,1);
#endif

#ifdef SLEEP_MINS
	xTaskCreatePinnedToCore(deep_sleep, "deep sleep task", 4096, NULL, configMAX_PRIORITIES, NULL,1);
#endif


}
