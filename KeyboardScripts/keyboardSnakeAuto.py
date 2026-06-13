import sys
import hid
import time
import random
from collections import deque
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
    items = list(key_color_dict.items())
    for i in range(0, len(items), 7):
        chunk = items[i:i+7]
        payload = [0x00] * 32
        payload[0] = 0x02            
        payload[1] = len(chunk)      
        offset = 2
        for led_idx, (r, g, b) in chunk:
            payload[offset]     = led_idx
            payload[offset + 1] = r
            payload[offset + 2] = g
            payload[offset + 3] = b
            offset += 4
        send_raw_report(interface, payload)

def clear_keyboard(interface):
    clear_payload = [0x00] * 32
    clear_payload[0] = 0x01
    send_raw_report(interface, clear_payload)

def bfs_find_path(start, target, obstacles):
    queue = deque([[start]])
    visited = set([start])
    moves = [(-1, 0), (1, 0), (0, -1), (0, 1)]

    while queue:
        path = queue.popleft()
        curr = path[-1]

        if curr == target:
            return path

        for dr, dc in moves:
            nr = (curr[0] + dr) % ROWS
            nc = (curr[1] + dc) % COLS
            neighbor = (nr, nc)

            if neighbor not in visited and neighbor not in obstacles:
                visited.add(neighbor)
                new_path = list(path)
                new_path.append(neighbor)
                queue.append(new_path)
    return None

if __name__ == '__main__':
    keyboard = get_raw_hid_interface()
    if not keyboard:
        print("Stars80 Keyboard not found!")
        sys.exit(1)

    print("\n=============================================")
    print("    STARS80 AUTOPILOT AI SNAKE MODE ACTIVE   ")
    print("=============================================")
    print("• Auto-Reset Limit: 8 Points reached")
    print("• Manual Reset     : Press PAGE UP key")
    print("• Exit Script      : Press PAUSE key to quit\n")

    global_running = True

    try:
        while global_running:
            print("\nSpawning AI instance match...")
            clear_keyboard(keyboard)
            
            snake = [(3, 5), (3, 4), (3, 3)] 
            direction = (0, 1) 
            
            def spawn_food():
                while True:
                    f = (random.randint(0, ROWS - 1), random.randint(0, COLS - 1))
                    if f not in snake: return f
            food = spawn_food()

            active_leds = {}
            match_running = True

            while match_running:
                if kb.is_pressed('pause'):
                    print("\nExit request caught.")
                    global_running = False
                    break

                if kb.is_pressed('page up'):
                    print("\nManual override restart caught.")
                    match_running = False
                    time.sleep(0.3)
                    continue

                obstacles = set(snake[:-1])
                head = snake[0]
                
                calculated_path = bfs_find_path(head, food, obstacles)
                
                if calculated_path and len(calculated_path) > 1:
                    next_step = calculated_path[1]
                    dr = next_step[0] - head[0]
                    dc = next_step[1] - head[1]
                    
                    if dr == ROWS - 1: dr = -1
                    elif dr == -(ROWS - 1): dr = 1
                    if dc == COLS - 1: dc = -1
                    elif dc == -(COLS - 1): dc = 1
                    
                    direction = (dr, dc)
                else:
                    survival_found = False
                    for dr, dc in [(-1, 0), (1, 0), (0, -1), (0, 1)]:
                        test_r = (head[0] + dr) % ROWS
                        test_c = (head[1] + dc) % COLS
                        if (test_r, test_c) not in obstacles:
                            direction = (dr, dc)
                            survival_found = True
                            break

                head_row = (head[0] + direction[0]) % ROWS
                head_col = (head[1] + direction[1]) % COLS
                new_head = (head_row, head_col)

                if new_head in snake[:-1]:
                    print(f"\nAI Trapped itself! Final length: {len(snake)}")
                    death_flash = {LAYOUT_MAP[r][c]: (255, 0, 0) for r, c in snake}
                    render_batch(keyboard, death_flash)
                    time.sleep(1.5)
                    break

                snake.insert(0, new_head)

                if new_head == food:
                    print(f"AI Core Consumption Success! Score/Length: {len(snake)}     ", end="\r")
                    
                    # FIXED: Added the milestone score checker condition
                    if len(snake) >= 20:
                        print(f"\nScore 8 reached! Flashing victory frame and auto-resetting...")
                        # Flash the snake gold/yellow to indicate milestone victory
                        victory_flash = {LAYOUT_MAP[r][c]: (255, 215, 0) for r, c in snake}
                        render_batch(keyboard, victory_flash)
                        time.sleep(1.0)
                        match_running = False # Drops current game loop iteration to prompt initialization loop
                        break
                        
                    food = spawn_food()
                else:
                    snake.pop()

                next_frame_leds = {}
                food_led = LAYOUT_MAP[food[0]][food[1]]
                next_frame_leds[food_led] = (255, 0, 0)

                for idx, (r, c) in enumerate(snake):
                    led = LAYOUT_MAP[r][c]
                    next_frame_leds[led] = (0, 150, 255) if idx == 0 else (0, 255, 0)

                for old_led in active_leds:
                    if old_led not in next_frame_leds:
                        next_frame_leds[old_led] = (0, 0, 0)

                render_batch(keyboard, next_frame_leds)
                active_leds = {k: v for k, v in next_frame_leds.items() if v != (0, 0, 0)}

                time.sleep(0.1)

    except KeyboardInterrupt:
        print("\nHalting automation loop.")
    finally:
        print("Cleaning up matrix buffers...")
        clear_keyboard(keyboard)
        keyboard.close()
        print("Done.")
