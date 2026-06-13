// Copyright 2024 Wind (@yelishang)
// SPDX-License-Identifier: GPL-2.0-or-later
#include "print.h"
#include QMK_KEYBOARD_H

#ifndef RGB_TOG
#    define RGB_TOG  0x7E00  
#    define RGB_MOD  0x7E01  
#    define RGB_HUI  0x7E04  
#    define RGB_HUD  0x7E05  
#    define RGB_SAI  0x7E06  
#    define RGB_SAD  0x7E07  
#    define RGB_VAI  0x7E08  
#    define RGB_VAD  0x7E09  
#    define RGB_SPI  0x7E0A  
#    define RGB_SPD  0x7E0B  
#endif

// Custom variables to handle animation state
static bool numberFiveActive = false;
#define TOTAL_ROWS 6
#define MAX_Y_COORD 64

// ==========================================
// INDEX-BASED SNAKE GAME ENGINE DEFINITIONS
// ==========================================
#define SNAKE_MAX_LENGTH 16

static bool    game_active = false;
static uint8_t snake_body[SNAKE_MAX_LENGTH]; // Stores absolute LED index IDs directly
static uint8_t snake_len = 3;
static int8_t  snake_dir = 1;                 // 1 = Right, -1 = Left, -17 = Up, 17 = Down
static uint8_t food_led = 25;
static uint32_t last_game_tick = 0;

void keyboard_post_init_user(void) {
    rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR); 
    rgb_matrix_sethsv_noeeprom(0, 255, 255); // Solid Red default
}

enum custom_keycodes {
    ANIM_TOG = SAFE_RANGE, // Custom keycode to toggle our animation
    SNAKE_TOG              // Increments correctly automatically
};

enum layers {
    WIN_B,
    WIN_FN,
    MAC_B,
    MAC_FN,
    GAME,
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
		_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, RGB_TOG, RGB_MOD, _______, _______,
		DF(2),   KC_BT1,  KC_BT2,  KC_BT3,  KC_2G4,  KC_USB,  _______, RGB_SAD, RGB_SAI, _______, _______, _______, _______, _______, RGB_HUI, _______, _______,
		_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
		_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,          RGB_VAI,
		_______, GU_TOGG, _______, _______,                   _______,                   _______, SNAKE_TOG, _______, _______, _______, RGB_SPD, RGB_VAD, RGB_SPI
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
		_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, RGB_TOG, RGB_MOD, _______, _______,
		DF(0),   KC_BT1,  KC_BT2,  KC_BT3,  KC_2G4,  KC_USB,  _______, RGB_SAD, RGB_SAI, _______, _______, _______, _______, _______, RGB_HUI, _______, _______,
		_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
		_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,          RGB_VAI,
		_______, _______, _______, _______,                   _______,                   _______,_______, _______, _______, _______, RGB_SPD, RGB_VAD, RGB_SPI
    )
};
// clang-format on

void spawn_food(void) {
    uint32_t seed = timer_read32();
    food_led = (seed % (RGB_MATRIX_LED_COUNT - 1)) + 1;
    
    // Ensure food doesn't spawn inside the snake body
    for (uint8_t i = 0; i < snake_len; i++) {
        if (food_led == snake_body[i]) {
            food_led = (food_led + 5) % RGB_MATRIX_LED_COUNT;
        }
    }
}

void reset_snake_game(void) {
    snake_len = 3;
    snake_dir = 1; // Default moving right
    
    // Initialize standard starting LED indices on the home row area
    snake_body[0] = 20; // Head
    snake_body[1] = 19; // Body
    snake_body[2] = 18; // Tail
    spawn_food();
}

// UNIFIED KEYSTROKE HANDLER
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (keycode == SNAKE_TOG && record->event.pressed) {
        game_active = !game_active;
        if (game_active) {
            rgb_matrix_disable(); // Shut off competing lighting routines
            layer_on(GAME);
            reset_snake_game();
        } else {
            layer_off(GAME);
            rgb_matrix_enable();
        }
        return false;
    }

    if (game_active) {
        if (record->event.pressed) {
            switch (keycode) {
                // Approximate rows mathematically on a 17-column ANSI form factor layout
                case KC_UP:    if (snake_dir != 17)  snake_dir = -17; break;
                case KC_DOWN:  if (snake_dir != -17) snake_dir = 17;  break;
                case KC_LEFT:  if (snake_dir != 1)   snake_dir = -1;  break;
                case KC_RIGHT: if (snake_dir != -1)  snake_dir = 1;   break;
                case KC_ESC:
                    game_active = false;
                    layer_off(GAME);
                    rgb_matrix_enable();
                    break;
            }
        }
        return false; // Intercept key drops completely
    }

    // Standard Key Override logic when Snake game is off
    switch (keycode) {
        case KC_ESC:
            if (record->event.pressed) {
                uprintf("DEBUG: Escape Pressed. Clearing active flags.\n");
                numberFiveActive = false; 
                rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR); 
                rgb_matrix_sethsv_noeeprom(0, 255, 255);
                return true; 
            }
            break;

        case ANIM_TOG:
            if (record->event.pressed) {
                numberFiveActive = !numberFiveActive;
                uprintf("DEBUG: ANIM_TOG Pressed! numberFiveActive status is now: %d\n", numberFiveActive);
                if (!numberFiveActive) {
                    rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
                    rgb_matrix_sethsv_noeeprom(0, 255, 255);
                }
            }
            return false; 
    }
    return true;
}

void matrix_scan_user(void) {
    if (!game_active) return;

    uint32_t now = timer_read32();
    if (timer_elapsed32(last_game_tick) > 200) { // Steady 200ms game loop tick speed
        last_game_tick = now;

        // Shift physical node array indices forward (tail tracking shift)
        for (uint8_t i = snake_len - 1; i > 0; i--) {
            snake_body[i] = snake_body[i - 1];
        }

        // Advance head node relative to array dimensions
        int16_t next_head = (int16_t)snake_body[0] + snake_dir;

        // Handle array wrapping parameters cleanly
        if (next_head >= RGB_MATRIX_LED_COUNT) next_head -= RGB_MATRIX_LED_COUNT;
        if (next_head < 0)                      next_head += RGB_MATRIX_LED_COUNT;

        snake_body[0] = (uint8_t)next_head;

        // Crash Detection Loop (Self-bite crashes reset game parameters)
        for (uint8_t i = 1; i < snake_len; i++) {
            if (snake_body[0] == snake_body[i]) {
                reset_snake_game();
                return;
            }
        }

        // Eating/Proximity condition capture
        if (snake_body[0] == food_led) {
            if (snake_len < SNAKE_MAX_LENGTH) {
                snake_len++;
            }
            spawn_food();
        }

        // ==========================================
        // FRAME RENDERER (Pushes indices straight to hardware lines)
        // ==========================================
        rgb_matrix_set_color_all(0, 0, 0); // Flush layout frame black

        // Paint Target Food (Green)
        rgb_matrix_set_color(food_led, 0, 255, 0);

        // Paint Snake Track Segments
        for (uint8_t i = 0; i < snake_len; i++) {
            if (i == 0) {
                rgb_matrix_set_color(snake_body[i], 255, 255, 0); // Yellow Head Node
            } else {
                rgb_matrix_set_color(snake_body[i], 0, 0, 255);   // Blue Tail Segments
            }
        }
    }
}

