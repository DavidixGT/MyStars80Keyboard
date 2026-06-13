// Copyright 2024 Wind (@yelishang)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

#ifndef RGB_TOG
#    define RGB_TOG  0x7E00  // Custom/Standard RGB Toggle
#    define RGB_MOD  0x7E01  // Step through modes
#    define RGB_HUI  0x7E04  // Increase Hue
#    define RGB_HUD  0x7E05  // Decrease Hue
#    define RGB_SAI  0x7E06  // Increase Saturation
#    define RGB_SAD  0x7E07  // Decrease Saturation
#    define RGB_VAI  0x7E08  // Increase Value (Brightness)
#    define RGB_VAD  0x7E09  // Decrease Value (Brightness)
#    define RGB_SPI  0x7E0A  // Increase Effect Speed
#    define RGB_SPD  0x7E0B  // Decrease Effect Speed
#endif

void keyboard_post_init_user(void) {
    // Force the keyboard to jump to a solid/static mode right after booting up
    rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR); 
    rgb_matrix_sethsv_noeeprom(0, 255, 255); // Sets it to Solid Red
}

enum layers {
    WIN_B,
    WIN_FN,
    MAC_B,
    MAC_FN,
    GAME,
};

enum custom_keycodes {
    SNAKE_TOG = SAFE_RANGE
};

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [WIN_B] = LAYOUT_ansi(
		KC_ESC,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_F13,  KC_PSCR, KC_SCRL, KC_PAUS,
		KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC, KC_INS,  KC_HOME, KC_PGUP,
		KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS, KC_DEL,  KC_END,  KC_PGDN,
		KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT, KC_NUHS, KC_ENT,
		KC_LSFT, KC_NUBS, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,          KC_RSFT,          KC_UP,
		KC_LCTL, KC_LGUI, KC_LALT, KC_LNG2,                   KC_SPC,                    KC_LNG1, KC_RALT, MO(WIN_FN), KC_APP, KC_RCTL, KC_LEFT, KC_DOWN, KC_RGHT
    ),

    [WIN_FN] = LAYOUT_ansi(
		EE_CLR,  KC_MYCM, KC_WHOM, KC_MAIL, KC_CALC, KC_MSEL, KC_MSTP, KC_MPRV, KC_MPLY, KC_MNXT, KC_MUTE, KC_VOLD, KC_VOLU, _______, _______, _______, _______,
		_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, RGB_TOG, RGB_MOD, _______, _______,
		DF(2),   KC_BT1,  KC_BT2,  KC_BT3,  KC_2G4,  KC_USB,  _______, RGB_SAD, RGB_SAI, _______, _______, _______, _______, _______, RGB_HUI, _______, _______,
		_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
		_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,          RGB_VAI,
		_______, GU_TOGG, _______, _______,                   _______,                   SNAKE_TOG, _______, _______, _______, _______, RGB_SPD, RGB_VAD, RGB_SPI
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
		_______, _______, _______, _______,                   _______,                   _______, _______, _______, _______, _______, RGB_SPD, RGB_VAD, RGB_SPI
    )
};
// clang-format on

// ==========================================
// SNAKE GAME CORE LOGIC
// ==========================================
#define SNAKE_MAX_LENGTH 32
#define GRID_COLS 17
#define GRID_ROWS 6

static const uint8_t led_grid[GRID_ROWS][GRID_COLS] = {
    {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16},
    {17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33},
    {34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50},
    {51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 0,  0,  0 },
    {65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 0,  77, 0,  78, 0 },
    {79, 80, 81, 82, 0,  0,  83, 0,  0,  84, 85, 86, 87, 88, 89, 90, 91}
};

typedef enum { DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT } direction_t;

static bool game_active = false;
static int8_t snake_x[SNAKE_MAX_LENGTH];
static int8_t snake_y[SNAKE_MAX_LENGTH];
static uint8_t snake_len = 3;
static direction_t current_dir = DIR_RIGHT;
static int8_t food_x = 5;
static int8_t food_y = 2;
static uint16_t last_game_tick = 0;

void spawn_food(void) {
    food_x = rand() % GRID_COLS;
    food_y = rand() % GRID_ROWS;
    while (led_grid[food_y][food_x] == 0 && (food_x != 0 || food_y != 0)) {
        food_x = rand() % GRID_COLS;
        food_y = rand() % GRID_ROWS;
    }
}

void reset_snake_game(void) {
    snake_len = 3;
    current_dir = DIR_RIGHT;
    for (uint8_t i = 0; i < snake_len; i++) {
        snake_x[i] = 3 - i;
        snake_y[i] = 2;
    }
    spawn_food();
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (keycode == SNAKE_TOG && record->event.pressed) {
        game_active = !game_active;
        if (game_active) {
            layer_on(GAME);
            reset_snake_game();
        } else {
            layer_off(GAME);
        }
        return false;
    }

    if (game_active) {
        if (record->event.pressed) {
            switch (keycode) {
                case KC_UP:    if (current_dir != DIR_DOWN) current_dir = DIR_UP;    return false;
                case KC_DOWN:  if (current_dir != DIR_UP)   current_dir = DIR_DOWN;  return false; // Fixed DIR_TOP bug
                case KC_LEFT:  if (current_dir != DIR_RIGHT) current_dir = DIR_LEFT;  return false;
                case KC_RIGHT: if (current_dir != DIR_LEFT)  current_dir = DIR_RIGHT; return false;
                case KC_ESC:
                    game_active = false;
                    layer_off(GAME);
                    return false;
            }
        }
        return false; // Freeze normal inputs when inside game loop
    }
    return true;
}

void matrix_scan_user(void) {
    if (!game_active) return;

    uint16_t now = timer_read();
    if (timer_elapsed(last_game_tick) > 250) { 
        last_game_tick = now;

        for (uint8_t i = snake_len - 1; i > 0; i--) {
            snake_x[i] = snake_x[i - 1];
            snake_y[i] = snake_y[i - 1];
        }

        switch (current_dir) {
            case DIR_UP:    snake_y[0]--; break;
            case DIR_DOWN:  snake_y[0]++; break;
            case DIR_LEFT:  snake_x[0]--; break;
            case DIR_RIGHT: snake_x[0]++; break;
        }

        if (snake_x[0] >= GRID_COLS) snake_x[0] = 0;
        if (snake_x[0] < 0) snake_x[0] = GRID_COLS - 1;
        if (snake_y[0] >= GRID_ROWS) snake_y[0] = 0;
        if (snake_y[0] < 0) snake_y[0] = GRID_ROWS - 1;

        for (uint8_t i = 1; i < snake_len; i++) {
            if (snake_x[0] == snake_x[i] && snake_y[0] == snake_y[i]) {
                reset_snake_game();
                return;
            }
        }

        if (snake_x[0] == food_x && snake_y[0] == food_y) {
            if (snake_len < SNAKE_MAX_LENGTH) {
                snake_len++;
            }
            spawn_food();
        }
    }
}

// Single combined layout painting function (fixes the duplication issue)
bool rgb_matrix_indicators_user(void) {
    if (game_active) {
        rgb_matrix_set_color_all(0, 0, 0); // Black out canvas

        // Draw Target Food (Green)
        uint8_t food_idx = led_grid[food_y][food_x];
        if (food_idx != 0 || (food_x == 0 && food_y == 0)) {
            rgb_matrix_set_color(food_idx, 0, 255, 0);
        }

        // Draw Snake Body
        for (uint8_t i = 0; i < snake_len; i++) {
            uint8_t body_idx = led_grid[snake_y[i]][snake_x[i]];
            if (body_idx != 0 || (snake_x[i] == 0 && snake_y[i] == 0)) {
                if (i == 0) {
                    rgb_matrix_set_color(body_idx, 255, 255, 0); // Yellow Head
                } else {
                    rgb_matrix_set_color(body_idx, 0, 0, 255);   // Blue Segments
                }
            }
        }
    } else {
        // Normal configuration highlight layer fallback
        if (IS_LAYER_ON(WIN_FN) || IS_LAYER_ON(MAC_FN)) {
            rgb_matrix_set_color(0, 255, 255, 255); // White Escape Key
        }
    }
    return true;
}