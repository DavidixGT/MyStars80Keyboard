// Copyright 2024 Wind (@yelishang)
// SPDX-License-Identifier: GPL-2.0-or-later
#include "print.h"
#include QMK_KEYBOARD_H
#include "raw_hid.h"
// Create a custom keycode block bypassing default QMK bindings
enum custom_keycodes {
    MY_CYCLE_KEY = SAFE_RANGE, // Bypassed standard QMK cycle trigger (Insert key)
    MY_SPEED_DOWN,             // Custom override to decrease animation speed
    MY_SPEED_UP,               // Custom override to increase animation speed
    MY_BRIGHTNESS_DOWN,        // Custom override to decrease brightness
    MY_BRIGHTNESS_UP,          // Custom override to increase brightness
    MY_HUE_DOWN,               // Custom override to shift hue down
    MY_HUE_UP,                 // Custom override to shift hue up
    MY_SATURATION_DOWN,        // Custom override to decrease saturation
    MY_SATURATION_UP,          // Custom override to increase saturation
    MY_LIGHTING_TOGGLE,        // Custom override to toggle lighting power on/off
    MY_CUSTOM_CYCLE            // Dedicated shortcut exclusively for your user loop (Home key)
};

enum layers {
    WIN_B,
    WIN_FN,
    MAC_B,
    MAC_FN,
};

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [WIN_B] = LAYOUT_ansi(
		KC_ESC,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_F13,  KC_PSCR, KC_SCRL, KC_PAUS,
		KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC, KC_INS,  KC_HOME, KC_PGUP,
		KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS, KC_DEL,  KC_END,  KC_PGDN,
		KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT, KC_NUHS, KC_ENT,
		KC_LSFT, KC_NUBS, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,          KC_RSFT,          KC_UP,
		KC_LCTL, KC_LGUI, KC_LALT, KC_LNG2,                   KC_SPC,                    KC_LNG1, KC_RALT,MO(WIN_FN),KC_APP,KC_RCTL, KC_LEFT, KC_DOWN, KC_RGHT
    ),

    [WIN_FN] = LAYOUT_ansi(
		EE_CLR,  KC_MYCM, KC_WHOM, KC_MAIL, KC_CALC, KC_MSEL, KC_MSTP, KC_MPRV, KC_MPLY, KC_MNXT, KC_MUTE, KC_VOLD, KC_VOLU, _______, _______, _______, _______,
		_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, MY_LIGHTING_TOGGLE, MY_CYCLE_KEY, MY_CUSTOM_CYCLE, _______,
		DF(2),   KC_BT1,  KC_BT2,  KC_BT3,  KC_2G4,  KC_USB,  _______, MY_SATURATION_DOWN, MY_SATURATION_UP, _______, _______, _______, _______, _______, MY_HUE_UP, _______, _______,
		_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
		_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,          MY_BRIGHTNESS_UP,
		_______, GU_TOGG, _______, _______,                   _______,                   _______, _______, _______, _______, _______, MY_SPEED_DOWN, MY_BRIGHTNESS_DOWN, MY_SPEED_UP
    ),

    [MAC_B] = LAYOUT_ansi(
		KC_ESC,  KC_BRID, KC_BRIU, KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_MPRV, KC_MPLY, KC_MNXT, KC_MUTE, KC_VOLD, KC_VOLU, KC_F13,  KC_PSCR, KC_SCRL, KC_PAUS,
		KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC, KC_INS,  KC_HOME, KC_PGUP,
		KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS, KC_DEL,  KC_END,  KC_PGDN,
		KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT, KC_NUHS, KC_ENT,
		KC_LSFT, KC_NUBS, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,          KC_RSFT,          KC_UP,
		KC_LCTL, KC_LALT, KC_LGUI, KC_LNG2,                   KC_SPC,                    KC_LNG1, KC_RGUI,MO(MAC_FN),KC_APP,KC_RCTL, KC_LEFT, KC_DOWN, KC_RGHT
    ),

    [MAC_FN] = LAYOUT_ansi(
		EE_CLR,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  _______, _______, _______, _______,
		_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, MY_LIGHTING_TOGGLE, MY_CYCLE_KEY, MY_CUSTOM_CYCLE, _______,
		DF(0),   KC_BT1,  KC_BT2,  KC_BT3,  KC_2G4,  KC_USB,  _______, MY_SATURATION_DOWN, MY_SATURATION_UP, _______, _______, _______, _______, _______, MY_HUE_UP, _______, _______,
		_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
		_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,          MY_BRIGHTNESS_UP,
		_______, _______, _______, _______,                   _______,                   _______, _______, _______, _______, _______, MY_SPEED_DOWN, MY_BRIGHTNESS_DOWN, MY_SPEED_UP
    )
};
// clang-format on

// Custom event handler matrix intercepting key presses programmatically
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case MY_CYCLE_KEY:
            if (record->event.pressed) {
                #if defined(RGB_MATRIX_ENABLE)
                    rgb_matrix_step(); 
                #elif defined(LED_MATRIX_ENABLE)
                    led_matrix_step();
                #endif
            }
            return false;

        case MY_CUSTOM_CYCLE:
            if (record->event.pressed) {
                static uint8_t custom_anim_index = 0;
                #if defined(RGB_MATRIX_ENABLE)
                    switch (custom_anim_index) {
                        case 0:
                            // Corrected mapping variable targeting your personal .inc structure bounds
                            rgb_matrix_mode_noeeprom((RGB_MATRIX_EFFECT_MAX - 2));
                            custom_anim_index = 0; 
                            break;
                    }
                #elif defined(LED_MATRIX_ENABLE)
                    switch (custom_anim_index) {
                        case 0:
                            led_matrix_mode_noeeprom((RGB_MATRIX_EFFECT_MAX - 2));
                            custom_anim_index = 0;
                            break;
                    }
                #endif
            }
            return false;

        case MY_SPEED_DOWN:
            if (record->event.pressed) {
                #if defined(RGB_MATRIX_ENABLE)
                    rgb_matrix_decrease_speed(); 
                #elif defined(LED_MATRIX_ENABLE)
                    led_matrix_decrease_speed();
                #endif
            }
            return false;

        case MY_SPEED_UP:
            if (record->event.pressed) {
                #if defined(RGB_MATRIX_ENABLE)
                    rgb_matrix_increase_speed(); 
                #elif defined(LED_MATRIX_ENABLE)
                    led_matrix_increase_speed();
                #endif
            }
            return false;

        case MY_BRIGHTNESS_DOWN:
            if (record->event.pressed) {
                #if defined(RGB_MATRIX_ENABLE)
                    rgb_matrix_decrease_val(); 
                #elif defined(LED_MATRIX_ENABLE)
                    led_matrix_decrease_val();
                #endif
            }
            return false;

        case MY_BRIGHTNESS_UP:
            if (record->event.pressed) {
                #if defined(RGB_MATRIX_ENABLE)
                    rgb_matrix_increase_val(); 
                #elif defined(LED_MATRIX_ENABLE)
                    led_matrix_increase_val();
                #endif
            }
            return false;

        case MY_HUE_DOWN:
            if (record->event.pressed) {
                #if defined(RGB_MATRIX_ENABLE)
                    rgb_matrix_decrease_hue(); 
                #elif defined(LED_MATRIX_ENABLE)
                    led_matrix_decrease_hue();
                #endif
            }
            return false;

        case MY_HUE_UP:
            if (record->event.pressed) {
                #if defined(RGB_MATRIX_ENABLE)
                    rgb_matrix_increase_hue(); 
                #elif defined(LED_MATRIX_ENABLE)
                    led_matrix_increase_hue();
                #endif
            }
            return false;

        case MY_SATURATION_DOWN:
            if (record->event.pressed) {
                #if defined(RGB_MATRIX_ENABLE)
                    rgb_matrix_decrease_sat(); 
                #elif defined(LED_MATRIX_ENABLE)
                    led_matrix_decrease_sat();
                #endif
            }
            return false;

        case MY_SATURATION_UP:
            if (record->event.pressed) {
                #if defined(RGB_MATRIX_ENABLE)
                    rgb_matrix_increase_sat(); 
                #elif defined(LED_MATRIX_ENABLE)
                    led_matrix_increase_sat();
                #endif
            }
            return false;

        case MY_LIGHTING_TOGGLE:
            if (record->event.pressed) {
                #if defined(RGB_MATRIX_ENABLE)
                    rgb_matrix_toggle(); 
                #elif defined(LED_MATRIX_ENABLE)
                    led_matrix_toggle();
                #endif
            }
            return false;

        default:
            return true; // Pass through all other keys normally

            
    }
}
/*

// Storage variables for cursor tracking (scaled 0-255)
uint8_t cursor_x = 0;
uint8_t cursor_y = 0;


// Intercept data packets coming from the PC background script
void raw_hid_receive(uint8_t *data, uint8_t length) {
    // Check all possible index offsets where the X coordinate could land (index 0, 1, or 2)
    // This allows the values to catch instantly regardless of Windows USB shifting or padding variations
    if (data[0] > 0 || data[1] > 0 || data[2] > 0) {
        
        // If data[0] is our header ID, pull coordinates from standard placement
        if (data[0] == 0x99) {
            cursor_x = data[1];
            cursor_y = data[2];
        } 
        // If Windows shifts index 0 to become 0x99, grab indices 1 and 2
        else if (data[1] == 0x99) {
            cursor_x = data[2];
            cursor_y = data[3];
        }
        // Fallback: If your OS entirely blocks the 0x99 ID tag, automatically bind the two largest packet values
        else {
            cursor_x = (data[0] > 0) ? data[0] : data[1];
            cursor_y = (data[1] > data[0]) ? data[1] : data[2];
        }
    }
}
*/

// !!! IMPORTANT: Replace 12 with your board's actual Backspace LED index !!!


uint8_t python_red   = 0;
uint8_t python_green = 0;
uint8_t python_blue  = 0;
uint8_t keyIndex = 0;
void raw_hid_receive(uint8_t *data, uint8_t length) {
    uint8_t response[length];
    memset(response, 0, length);
    response[0] = 'B';
    keyIndex = data[0];
    python_red = data[1];
    python_green = data[2];
    python_blue = data[3];
}

bool rgb_matrix_indicators_user(void) {
    #ifdef RGB_MATRIX_ENABLE
        // This function draws last, effectively locking your custom color down
        //rgb_matrix_set_color(1, 255, 255, 255);
        rgb_matrix_set_color(keyIndex, python_red, python_green, python_blue);
    #endif
    return true;
}
