import sys
import hid
import time
import random
import keyboard as kb  # Requires: pip install keyboard

vendor_id     = 0x342d
product_id    = 0xe401
usage_page    = 0xFF60
usage         = 0x61
report_length = 32

# 6 Rows x 17 Columns Grid Map
LAYOUT_MAP = [
    # Row 0: Esc (0) -> F13 (13) -> PrtSc (14) -> Pause (16) [Left to Right]
    [0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16],
    
    # Row 1: PgUp (17) -> Ins (19) -> Backspace (20) -> ` (33) [Right to Left Snake]
    [33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17],
    
    # Row 2: Tab (34) -> ] (46) -> Del (48) -> PgDn (50) [Left to Right]
    [34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50],
    
    # Row 3: Caps (51) -> Enter (64) [Right to Left Snake]
    # (Padded with 64 at the end for shorter/uneven modular layout keys)
    [64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 49, 49, 50],
    
    # Row 4: LShift (65) -> RShift (76) -> Up Arrow (77) [Left to Right]
    [65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 76, 77, 77, 77, 77],
    
    # Row 5: Ctrl (78) -> Spacebar (83) -> Alt (84) -> Right Arrow (87) [Right to Left Snake]
    [85, 85, 85, 84, 83, 83, 83, 83, 83, 82, 81, 80, 80, 79, 78, 78, 78]
]

ROWS = 6
COLS = 17

def get_raw_hid_interface():
    device_interfaces = hid.enumerate(vendor_id, product_id)
    raw_hid_interfaces = [i for i in device_interfaces if i['usage_page'] == usage_page and i['usage'] == usage]
    if len(raw_hid_interfaces) == 0: return None
    interface = hid.device()
    interface.open_path(raw_hid_interfaces[0]['path'])
    return interface

def send_raw_report(interface, data):
    request_data = [0x00] * (report_length + 1)
    request_data[1:len(data) + 1] = data
    try:
        interface.write(bytes(request_data))
        interface.read(report_length, 5)
        return True
    except Exception:
        return False

def render_batch(interface, key_color_dict):
    """Chunks layout parameters into valid 0x02 packets (Max 7 keys per frame)."""
    items = list(key_color_dict.items())
    for i in range(0, len(items), 7):
        chunk = items[i:i+7]
        payload = [0x00] * 32
        payload[0] = 0x02            # Batch Update Command Mode
        payload[1] = len(chunk)      # Amount of keys contained
        offset = 2
        for led_idx, (r, g, b) in chunk:
            payload[offset]     = led_idx
            payload[offset + 1] = r
            payload[offset + 2] = g
            payload[offset + 3] = b
            offset += 4
        send_raw_report(interface, payload)

def clear_keyboard(interface):
    """Sends the 0x01 clear protocol to instantly turn off all custom LEDs."""
    clear_payload = [0x00] * 32
    clear_payload[0] = 0x01
    send_raw_report(interface, clear_payload)

if __name__ == '__main__':
    keyboard = get_raw_hid_interface()
    if not keyboard:
        print("Stars80 Keyboard not found!")
        sys.exit(1)

    print("\n=============================================")
    print("      STARS80 KEYBOARD SNAKE GAME STARTED    ")
    print("=============================================")
    print("• Controls: Use ARROW KEYS on your keyboard")
    print("• Restart : Press PAGE UP key to restart match")
    print("• Exit    : Press PAUSE key to quit\n")

    global_running = True

    try:
        while global_running:
            print("\nInitializing fresh match...")
            clear_keyboard(keyboard)
            
            # Snake structural initialization (Starts at center grid moving right)
            snake = [(3, 5), (3, 4), (3, 3)] 
            direction = (0, 1) # (Row Delta, Col Delta)
            
            # Food spawn placement helper
            def spawn_food():
                while True:
                    f = (random.randint(0, ROWS - 1), random.randint(0, COLS - 1))
                    if f not in snake: return f
            food = spawn_food()

            # Track currently lit LEDs to clear them in the next frame
            active_leds = {}
            match_running = True

            while match_running:
                # 1. Listen for global exit button hook (Pause key)
                if kb.is_pressed('pause'):
                    print("\nPause key pressed. Exiting game gracefully...")
                    global_running = False
                    break

                # 2. Listen for global restart button hook (Page Up key)
                if kb.is_pressed('page up'):
                    print("\nPage Up pressed! Restarting game...")
                    match_running = False  # Breaks out of current match to restart loop
                    time.sleep(0.3)        # Small debounce delay to prevent rapid double-clicks
                    continue

                # 3. Parse global directional arrow keys steering paths
                if kb.is_pressed('up') and direction != (1, 0):    direction = (-1, 0)
                elif kb.is_pressed('down') and direction != (-1, 0): direction = (1, 0)
                elif kb.is_pressed('left') and direction != (0, 1):  direction = (0, -1)
                elif kb.is_pressed('right') and direction != (0, -1): direction = (0, 1)

                # 4. Calculate new head position
                head_row = snake[0][0] + direction[0]
                head_col = snake[0][1] + direction[1]

                # 5. Check for screen wall collisions (Wrap-around mechanics)
                head_row = head_row % ROWS
                head_col = head_col % COLS
                new_head = (head_row, head_col)

                # 6. Check for self-collision death criteria
                if new_head in snake:
                    print("\nGame Over! The snake crashed into itself.")
                    print("Press PAGE UP to try again, or PAUSE to quit.")
                    
                    # Flash the final dead cell structure Red to indicate game over
                    death_flash = {LAYOUT_MAP[r][c]: (255, 0, 0) for r, c in snake}
                    render_batch(keyboard, death_flash)
                    
                    # Wait in a loop until user chooses to Restart or Exit
                    waiting_for_input = True
                    while waiting_for_input:
                        if kb.is_pressed('page up'):
                            match_running = False
                            waiting_for_input = False
                            time.sleep(0.3)
                        elif kb.is_pressed('pause'):
                            global_running = False
                            match_running = False
                            waiting_for_input = False
                        time.sleep(0.05)
                    break

                # 7. Insert new segment head point
                snake.insert(0, new_head)

                # 8. Check for food tracking consumption indices
                if new_head == food:
                    food = spawn_food()
                    print(f"Apple eaten! Score/Length: {len(snake)}     ", end="\r")
                else:
                    # Remove tail segment if not growing
                    snake.pop()

                # 9. Build full coordinate visual render frame
                next_frame_leds = {}
                
                # Map Food/Apple -> Bright Red (255, 0, 0)
                food_led = LAYOUT_MAP[food[0]][food[1]]
                next_frame_leds[food_led] = (255, 0, 0)

                # Map Snake Body -> Neon Green (0, 255, 0)
                for idx, (r, c) in enumerate(snake):
                    led = LAYOUT_MAP[r][c]
                    # Make the head a lighter green hue than the trailing tail links
                    next_frame_leds[led] = (100, 255, 100) if idx == 0 else (0, 255, 0)

                # Find items that were illuminated last frame but are empty now, and turn them off
                for old_led in active_leds:
                    if old_led not in next_frame_leds:
                        next_frame_leds[old_led] = (0, 0, 0)

                # 10. Commit frame down the HID pipeline to display on keyboard keys
                render_batch(keyboard, next_frame_leds)
                active_leds = {k: v for k, v in next_frame_leds.items() if v != (0, 0, 0)}

                # Control Game Speed (0.2 seconds per tick)
                time.sleep(0.2)

    except KeyboardInterrupt:
        print("\nGame closed via console exit command.")
    finally:
        # Turn off all game lights, returning keyboard back to stock firmware animations
        print("Cleaning up matrix buffers...")
        clear_keyboard(keyboard)
        keyboard.close()
        print("Done.")
