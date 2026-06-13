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
#define DRIVER_MAX_LEDS 88
uint8_t global_red_buffer[DRIVER_MAX_LEDS]   = {0};
uint8_t global_green_buffer[DRIVER_MAX_LEDS] = {0};
uint8_t global_blue_buffer[DRIVER_MAX_LEDS]  = {0};

void raw_hid_receive(uint8_t *data, uint8_t length) {
    if (length == 32) {
        uint8_t command_type = data[0]; // Protocol index

        // Command 0x01: Dynamic Reset (Turn off all custom keys)
        if (command_type == 0x01) {
            for (uint8_t i = 0; i < DRIVER_MAX_LEDS; i++) {
                global_red_buffer[i]   = 0;
                global_green_buffer[i] = 0;
                global_blue_buffer[i]  = 0;
            }
        }
        // Command 0x02: Batch Key Update [Type, NumKeys, Key1, R1, G1, B1, Key2, R2...]
        else if (command_type == 0x02) {
            uint8_t num_keys = data[1];
            uint8_t offset   = 2;

            for (uint8_t k = 0; k < num_keys; k++) 
            {
                if (offset + 3 >= 32) break; // Buffer guard
                uint8_t target_idx = data[offset];
                if (target_idx < DRIVER_MAX_LEDS) 
                {
                    global_red_buffer[target_idx]   = data[offset + 1];global_green_buffer[target_idx] = data[offset + 2];
                    global_blue_buffer[target_idx]  = data[offset + 3];
                }
                    offset += 4; // Step to the next batch segment
            }      
        }  
    }   
}
bool rgb_matrix_indicators_user(void) 
{
    #if defined(RGB_MATRIX_ENABLE)// Continuously paint the color memory maps over the keys
    
    for (uint8_t i = 0; i < DRIVER_MAX_LEDS; i++)
    if (global_red_buffer[i] > 0 || global_green_buffer[i] > 0 || global_blue_buffer[i] > 0) 
    {
        rgb_matrix_set_color(i, global_red_buffer[i], global_green_buffer[i], global_blue_buffer[i]);
    }
    #endif
    return true;
}