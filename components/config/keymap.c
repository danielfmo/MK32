#ifndef KEYMAP_C
#define KEYMAP_C

#include "key_definitions.h"
#include "keyboard_config.h"
#include "keymap.h"

// A bit different from QMK, default returns you to the first layer, LOWER and raise increase/lower layer by order.
#define DEFAULT 0x100
#define LOWER 0x101
#define RAISE 0x102

// Keymaps are designed to be relatively interchangeable with QMK
enum custom_keycodes {
    QWERTY,
    NUM,
    PLUGINS,
};

// Set these for each layer and use when layers are needed in a hold-to use layer
enum layer_holds { QWERTY_H = LAYER_HOLD_BASE_VAL, NUM_H, FUNCS_H };

// array to hold names of layouts for oled
char default_layout_names[LAYERS][MAX_LAYOUT_NAME_LENGTH] = {
    "QWERTY",
    "NUM",
    "Plugins",
};

/* select a keycode for your macro
 * important - first macro must be initialized as MACRO_BASE_VAL
 * */

#define MACROS_NUM 2
enum custom_macros {
    KC_CTRL_ALT_DELETE = MACRO_BASE_VAL,
    KC_ALT_F4,
};

/*define what the macros do
 * important- make sure you you put the macros in the same order as the their enumeration
 */
uint16_t macros[MACROS_NUM][MACRO_LEN] = {
    // CTRL+ALT+DEL
    {KC_LCTRL, KC_LALT, KC_DEL},
    // ALT +F4
    {KC_RALT, KC_LALT, KC_NO}};

/*Encoder keys for each layer by order, and for each pad
 * First variable states what usage the encoder has
 */

// uint16_t default_encoder_map[LAYERS][ENCODER_SIZE] = {
//     // |VOL + | VOL - | MUTE |
//     {MEDIA_ENCODER, KC_AUDIO_VOL_UP, KC_AUDIO_VOL_DOWN, KC_AUDIO_MUTE},
//     // |Y+|Y-| LEFT CLICK|
//     {MOUSE_ENCODER, KC_MS_UP, KC_MS_DOWN, KC_MS_BTN1},

//     {MOUSE_ENCODER, KC_MS_UP, KC_MS_DOWN, KC_MS_BTN1}};

// uint16_t default_slave_encoder_map[LAYERS][ENCODER_SIZE] = {
//     // |VOL + | VOL - | MUTE |
//     {MEDIA_ENCODER, KC_AUDIO_VOL_UP, KC_AUDIO_VOL_DOWN, KC_AUDIO_MUTE},
//     // |Y+|Y-| LEFT CLICK|
//     {MOUSE_ENCODER, KC_MS_RIGHT, KC_MS_LEFT, KC_MS_BTN2},
//     // |Y+|Y-| LEFT CLICK|
//     {MOUSE_ENCODER, KC_MS_RIGHT, KC_MS_LEFT, KC_MS_BTN2}};

// Fillers to make layering more clear
#define _______ KC_TRNS
#define XXXXXXX KC_NO

// Each keymap is represented by an array, with an array that points to all the keymaps  by order
// TODO: Update to a 87 keys / 75%
uint16_t _QWERTY[MATRIX_ROWS][MATRIX_COLS] = {
    /* Qwerty
     *	     0      2      17     4      16    15      19     23     22      5     18     32     26    14     21     27    25
     *   ,---------------------------------------------------------------------------------------------------------------------.
     *33 | Esc  |------|  F1  |  F2  |  F3  |  F4  |------|  F5  |  F6  |  F7  |  F8  |  F9  |  F10 |  F11 |  F12 |------|------|
     *   |------+------+------+------+------+-------------+------+------+------+------+------+------+------+------+------+------|
     *36 | ~ `  |   1  |   2  |   3  |   4  |   5  |   6  |   7  |   8  |   9  |   0  |  _-  |  +=  | BkSp | NumLk|  /   | *    |
     *   |------+------+------+------+------+------|------+------+------+------+------+------+------+------+------+------+------|
     *37 | TAB  |   Q  |   W  |   E  |   R  |   T  |   Y  |   U  |   I  |   O  |   P  |   {  |   }  |   \  |   7  |   8  |  9   |
     *   |------+------+------+------+------+------|------+------+------+------+------+------+------+------+------+------+------|
     *38 | Caps |   A  |   S  |   D  |   F  |   G  |   H  |   J  |   K  |   L  |   ;  |   '  |----- |ENTER |   4  |   5  |  6   |
     *   |------+------+------+------+------+------+------+------+------+------+------+------+------+------+------+------+------|
     *39 | Shift|   Z  |   X  |   C  |   V  |   B  |   N  |   M  |   ,  |   .  |   /  |------|SHIFT |------|   1  |   2  |  3   |
     *   |------+------+------+------+------+------+------+------+------+------+------+------+------+------+------+------+------|
     *35 | Ctrl | GUI  |  Alt |------|------|Space |   -  |   -  |  Alt |ENTER |  GUI | fn   | Ctrl |   +  |   0  |   00 |  .   |
     *   `----------------------------------------------------------------------------------------------------------------------'
     */

    {KC_ESC, KC_ESC, KC_F1, KC_F2, KC_F3, KC_F4, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12, KC_F12, KC_F12},
    {KC_GRV, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_MINS, KC_EQL, KC_BSPC, KC_NUMLOCK, KC_PSLS, KC_PAST},
    {KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_LBRC, KC_RBRC, KC_NUHS, KC_KP_7, KC_KP_8, KC_KP_9},
    {KC_CAPS, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_QUOT, KC_ENT, KC_ENT, KC_KP_4, KC_KP_5, KC_KP_6},
    {KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_RSFT, KC_RSFT, KC_RSFT, KC_KP_1, KC_KP_2, KC_KP_3},
    {KC_LCTRL, KC_LGUI, KC_LALT, KC_SPC, KC_SPC, KC_SPC, KC_KP_MINUS, KC_KP_MINUS, KC_RALT, KC_KP_ENTER, KC_RGUI, KC_MENU, KC_RCTRL, KC_KP_PLUS, KC_KP_0, KC_KP_0, KC_KP_DOT}

};

uint16_t _NUM[MATRIX_ROWS][MATRIX_COLS] = {

    /* numpad
     * ,-----------------------------------------------------------------------------------------------------------------------.
     * | Esc  |------|  F1  |  F2   |  F3  |  F4  |------|  F5  |  F6  |  F7  |  F8  |  F9  |  F10 |  F11 |  F12 |-------|------|
     * |------+------+------+-------+------+-------------+------+------+------+------+------+------+------+------+-------+------|
     * | ~ `  |   1  |   2  |   3   |   4  |   5  |   6  |   7  |   8  |   9  |   0  |  _-  |  +=  | BkSp | NumLk|  /    | *    |
     * |------+------+------+-------+------+------|------+------+------+------+------+------+------+------+------+-------+------|
     * | TAB  |   Q  |   W  |   E   |   R  |   T  |   Y  |   U  |   I  |   O  |   P  |   {  |   }  |   \  |   7  |   8   |  9   |
     * |------+------+------+-------+------+------|------+------+------+------+------+------+------+------+------+-------+------|
     * | Caps |   A  |   S  |   D   |   F  |   G  |   H  |   J  |   K  |   L  |   ;  |   '  |----- |ENTER |   4  |   5   |  6   |
     * |------+------+------+-------+------+------+------+------+------+------+------+------+------+------+------+-------+------|
     * | Shift|   Z  |   X  |   C   |   V  |   B  |   N  |   M  |   ,  |   .  |   /  |   M  |SHIFT |------|   1  |   2   |  3   |
     * |------+------+------+-------+------+------+------+------+------+------+------+------+------+------+------+-------+------|
     * | Ctrl | GUI  |  Alt |-------|------|Space |   -  |   -  |  Alt |------|  Fn  |ENTER | Ctrl |   +  |   0  |   00  |  .   |
     * `------------------------------------------------------------------------------------------------------------------------'
     */

    {KC_ESC, KC_F1, KC_F1, KC_F2, KC_F3, KC_F4, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12, KC_F12, KC_F12},
    {KC_GRV, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_MINS, KC_EQL, KC_BSPC, KC_NUMLOCK, KC_PSLS, KC_PAST},
    {KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_RBRC, KC_NUHS, KC_BSLS, KC_KP_7, KC_KP_8, KC_KP_9},
    {KC_CAPS, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_QUOT, KC_ENT, KC_ENT, KC_KP_4, KC_KP_5, KC_KP_6},
    {KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_RSFT, KC_RSFT, KC_RSFT, KC_KP_1, KC_KP_2, KC_KP_3},
    {KC_LCTRL, KC_LGUI, KC_LALT, KC_SPC, KC_SPC, KC_SPC, KC_KP_MINUS, KC_KP_MINUS, KC_RALT, KC_RALT, KC_RGUI, KC_KP_ENTER, KC_RCTRL, KC_KP_PLUS, KC_KP_0, KC_KP_0, KC_KP_DOT}

};

uint16_t _PLUGINS[MATRIX_ROWS][MATRIX_COLS] = {

    /* Qwerty
     * ,-----------------------------------------------------------------------------------------------------------------------.
     * | Esc  |------|  F1  |  F2   |  F3  |  F4  |------|  F5  |  F6  |  F7  |  F8  |  F9  |  F10 |  F11 |  F12 |-------|------|
     * |------+------+------+-------+------+-------------+------+------+------+------+------+------+------+------+-------+------|
     * | ~ `  |   1  |   2  |   3   |   4  |   5  |   6  |   7  |   8  |   9  |   0  |  _-  |  +=  | BkSp | NumLk|  /    | *    |
     * |------+------+------+-------+------+------|------+------+------+------+------+------+------+------+------+-------+------|
     * | TAB  |   Q  |   W  |   E   |   R  |   T  |   Y  |   U  |   I  |   O  |   P  |   {  |   }  |   \  |   7  |   8   |  9   |
     * |------+------+------+-------+------+------|------+------+------+------+------+------+------+------+------+-------+------|
     * | Caps |   A  |   S  |   D   |   F  |   G  |   H  |   J  |   K  |   L  |   ;  |   '  |----- |ENTER |   4  |   5   |  6   |
     * |------+------+------+-------+------+------+------+------+------+------+------+------+------+------+------+-------+------|
     * | Shift|   Z  |   X  |   C   |   V  |   B  |   N  |   M  |   ,  |   .  |   /  |   M  |SHIFT |------|   1  |   2   |  3   |
     * |------+------+------+-------+------+------+------+------+------+------+------+------+------+------+------+-------+------|
     * | Ctrl | GUI  |  Alt |-------|------|Space |   -  |   -  |  Alt |------|  Fn  |ENTER | Ctrl |   +  |   0  |   00  |  .   |
     * `------------------------------------------------------------------------------------------------------------------------'
     */

    {KC_ESC, KC_F1, KC_F1, KC_F2, KC_F3, KC_F4, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12, KC_F12, KC_F12},
    {KC_GRV, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_MINS, KC_EQL, KC_BSPC, KC_NUMLOCK, KC_PSLS, KC_PAST},
    {KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_RBRC, KC_NUHS, KC_BSLS, KC_KP_7, KC_KP_8, KC_KP_9},
    {KC_CAPS, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_QUOT, KC_ENT, KC_ENT, KC_KP_4, KC_KP_5, KC_KP_6},
    {KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_RSFT, KC_RSFT, KC_RSFT, KC_KP_1, KC_KP_2, KC_KP_3},
    {KC_LCTRL, KC_LGUI, KC_LALT, KC_SPC, KC_SPC, KC_SPC, KC_KP_MINUS, KC_KP_MINUS, KC_RALT, KC_RALT, KC_RGUI, KC_KP_ENTER, KC_RCTRL, KC_KP_PLUS, KC_KP_0, KC_KP_0, KC_KP_DOT}

};

// Create an array that points to the various keymaps
uint16_t (*default_layouts[])[MATRIX_ROWS][MATRIX_COLS] = {&_QWERTY, &_NUM, &_PLUGINS};

uint8_t current_layout = 0;

#endif
