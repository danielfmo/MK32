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
 * Copyright 2017 Benjamin Aigner <beni@asterics-foundation.org>
 */

/** @file
 * @brief This file is a C compatible wrapper for Neil Kolbans CPP utils
 * 
 * It initializes 3 queues for sending mouse, keyboard and joystick data
 * from within the C-side. C++ classes are instantiated from here.
 * If you want to have a different BLE HID device, you need to adapt this file.
 * 
 * @note Thank you very much Neil Kolban for this impressive work!
*/

#ifndef _HID_kbdmousejoystick_H_
#define _HID_kbdmousejoystick_H_


#ifdef __cplusplus
extern "C" {
#endif

#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <freertos/queue.h>
#include <esp_log.h>

/** @brief Queue for sending mouse reports
 * @see mouse_command_t */
extern QueueHandle_t mouse_q;

/** @brief Queue for sending keyboard reports
 * @see keyboard_command_t */
extern QueueHandle_t keyboard_q;

/** @brief Queue for sending joystick reports
 * @see joystick_command_t */
extern QueueHandle_t joystick_q;

/** @brief Queue for sending media reports
 * @see media_command_t */
extern QueueHandle_t media_q;


/** @brief Main init function to start HID interface
 * 
 * @param enableKeyboard If != 0, keyboard will be active
 *  * @param enableMedia If != 0, media will be active
 * @param enableMouse If != 0, mouse will be active
 * @param enableJoystick If != 0, joystick will be active
 * @note After init, just use the queues! */
esp_err_t HID_kbdmousejoystick_init(uint8_t enableKeyboard,uint8_t enableMedia, uint8_t enableMouse, uint8_t enableJoystick, char * name);

/** @brief Activate pairing, disconnect from paired device
 * */
esp_err_t HID_kbdmousejoystick_activatePairing(void);

/** @brief Deactivate pairing, disconnect from paired device
 * */
esp_err_t HID_kbdmousejoystick_deactivatePairing(void);

/** @brief Is the BLE currently connected?
 * @return 0 if not connected, 1 if connected */  
uint8_t HID_kbdmousejoystick_isConnected(void);

/** @brief Is the BLE currently paired?
 * @return 0 if not paired, 1 if paired */
uint8_t HID_kbdmousejoystick_isPaired(void);


/** @brief One mouse command (report) to be sent via BLE mouse profile
 * @see mouse_q */
typedef struct mouse_command {
  int8_t x;
  int8_t y;
  int8_t wheel;
  uint8_t buttons;
} mouse_command_t;

/** @brief One command (report) to be issued via BLE joystick profile
 * @see joystick_q */
typedef struct joystick_command {
  /** @brief Button mask, allows up to 32 different buttons */
  uint32_t buttonmask;
  /** @brief X-axis value, 0-1023 */
  uint16_t Xaxis;
  /** @brief Y-axis value, 0-1023 */
  uint16_t Yaxis;
  /** @brief Z-axis value, 0-1023 */
  uint16_t Zaxis;
  /** @brief Z-rotate value, 0-1023 */
  uint16_t Zrotate;
  /** @brief Slider left value, 0-1023 */
  uint16_t sliderLeft;
  /** @brief Slider right value, 0-1023 */
  uint16_t sliderRight;
  /** @brief Hat position (0-360), mapped to 8 directions. Use <0 for no pressing*/
  int16_t hat;
} joystick_command_t;


#ifdef __cplusplus
}
#endif

#endif /* _HID_kbdmousejoystick_H_ */
