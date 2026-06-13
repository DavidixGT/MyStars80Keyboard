// 1. Declare the name of your unique custom effect macro
RGB_MATRIX_EFFECT(CYBERPULSE_RAIN)

// 2. Write the execution logic inside the implementation block
#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

static bool CYBERPULSE_RAIN(effect_params_t* params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    // Read live values directly from the classical QMK system configurations
    uint8_t user_hue = rgb_matrix_config.hsv.h;
    uint8_t user_sat = rgb_matrix_config.hsv.s;
    uint8_t user_val = rgb_matrix_config.hsv.v; // Master brightness set by user

    // Tracks the vertical Y drop animations over time (Max grid boundary is 64)
    #define MAX_RAIN_COLUMNS 16
    static uint8_t drop_y[MAX_RAIN_COLUMNS] = {0};
    static uint32_t last_time = 0;

    // Use a millisecond timer instead of global ticker variables to bypass early compile errors
    uint32_t current_time = timer_read32();
    if (current_time - last_time > 30) { // Lower number makes rain fall faster
        last_time = current_time;

        for (uint8_t c = 0; c < MAX_RAIN_COLUMNS; c++) {
            if (drop_y[c] > 0) {
                drop_y[c] += 2; // Advance droplet downwards vertically
                if (drop_y[c] > 68) { // Past bottom edge? Reset.
                    drop_y[c] = 0;
                }
            } else {
                // Use milliseconds as a randomized seed to spawn new droplets at the top
                if ((current_time ^ (c * 17)) % 100 < 2) { 
                    drop_y[c] = 1; // Start falling from top row (Y = 1)
                }
            }
        }
    }

    // Loop through every physical key LED on the keyboard matrix array
    for (uint8_t i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS(); // Respect global/layer lighting masks

        // Fetch this specific key's real grid coordinates
        uint8_t x_pos = g_led_config.point[i].x; // Range 0 to 224
        uint8_t y_pos = g_led_config.point[i].y; // Range 0 to 64

        // Map the physical X coordinate mapping cleanly to our tracking columns array
        uint8_t col_index = (x_pos * MAX_RAIN_COLUMNS) / 225;
        if (col_index >= MAX_RAIN_COLUMNS) col_index = MAX_RAIN_COLUMNS - 1;

        uint8_t final_val = 0;
        uint8_t droplet_head_y = drop_y[col_index];

        if (droplet_head_y > 0) {
            // Check if the pixel is right at or slightly above the leading edge of the falling raindrop
            if (y_pos <= droplet_head_y && y_pos >= (droplet_head_y - 24)) {
                // Dimmer trail math: Calculate distance away from the leading droplet head
                uint8_t distance_from_head = droplet_head_y - y_pos;
                
                // Scale brightness: 0 distance = full brightness, further away = fades down cleanly to zero
                uint8_t trail_fade = (24 - distance_from_head) * 10;
                
                // Respect master global value limit
                final_val = scale8(trail_fade, user_val); 
            }
        }

        // Apply colors to the grid. If no rain is present, value defaults to 0 (off)
        RGB rgb = hsv_to_rgb((HSV){user_hue, user_sat, final_val});
        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }
    
    return rgb_matrix_check_finished_leds(led_max);
}

#undef MAX_RAIN_COLUMNS
#endif // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
