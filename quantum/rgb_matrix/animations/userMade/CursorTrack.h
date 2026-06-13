RGB_MATRIX_EFFECT(CURSOR_TRACK)

#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

static bool CURSOR_TRACK(effect_params_t* params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    // Tell the compiler these variables are globally managed outside this file
    extern uint8_t cursor_x;
    extern uint8_t cursor_y;

    uint8_t user_hue = rgb_matrix_config.hsv.h;
    uint8_t user_sat = rgb_matrix_config.hsv.s;
    uint8_t user_val = rgb_matrix_config.hsv.v;

    for (uint8_t i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS();

        uint8_t k_x = g_led_config.point[i].x; // 0 to 224
        uint8_t k_y = g_led_config.point[i].y; // 0 to 64

        // Calculate distance from this specific key to the cursor position
        int dist_x = abs((int)k_x - (int)cursor_x);
        int dist_y = abs((int)k_y - (int)cursor_y);
        
        uint8_t final_val = 0;

        // If the key is close to the cursor, light it up (creates a small glowing dot)
        if (dist_x < 35 && dist_y < 25) {
            // Keys closer to the cursor look brighter
            final_val = user_val - (dist_x + dist_y) * 2; 
        }

        RGB rgb = hsv_to_rgb((HSV){user_hue, user_sat, final_val});
        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }
    return rgb_matrix_check_finished_leds(led_max);
}

#endif
