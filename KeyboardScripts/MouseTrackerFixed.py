import sys
import hid
import time
import mouse  # Requires: pip install mouse

vendor_id     = 0x342d
product_id    = 0xe401

usage_page    = 0xFF60
usage         = 0x61
report_length = 32

# COMPLETE STARS80 PHYSICAL MATRIX MAP (6 Rows x 17 Columns)
# Fully populated to fix the naked array syntax crash
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

ROWS = len(LAYOUT_MAP)     # Evaluates to 6
COLS = len(LAYOUT_MAP[0])  # Evaluates to 17

def get_raw_hid_interface():
    """Finds and OPENS the interface once."""
    device_interfaces = hid.enumerate(vendor_id, product_id)
    raw_hid_interfaces = [i for i in device_interfaces if i['usage_page'] == usage_page and i['usage'] == usage]

    if len(raw_hid_interfaces) == 0:
        return None

    interface = hid.device()
    interface.open_path(raw_hid_interfaces[0]['path'])
    return interface

def send_raw_report(interface, data):
    """Sends data instantly using an ALREADY open interface connection."""
    request_data = [0x00] * (report_length + 1)
    request_data[1:len(data) + 1] = data
    request_report = bytes(request_data)

    try:
        interface.write(request_report)
        interface.read(report_length, 10) 
        return True
    except Exception as e:
        print(f"Error sending data: {e}")
        return False

if __name__ == '__main__':
    keyboard = get_raw_hid_interface()
    if keyboard:
        print(f"Connected to: {keyboard.get_product_string()} (Multi-Key Packet Mode)")
    else:
        print("Stars80 Keyboard not found! Check connection.")
        sys.exit(1)

    print("\n--- Stars80 Multi-Key Protocol Tracker ---")
    print("Move your mouse anywhere on your screen to target keys!")
    print("Press Ctrl+C in this window to terminate layout tracking.\n")

    last_led_idx = -1

    try:
        while True:
            # 1. Capture cursor position
            mx, my = mouse.get_position()
            
            screen_width = 1920
            screen_height = 1080
            
            mx = max(0, min(mx, screen_width - 1))
            my = max(0, min(my, screen_height - 1))

            # 2. Map coordinates precisely to our grid row/col bounds
            col = int((mx / screen_width) * COLS)
            row = int((my / screen_height) * ROWS)

            # 3. Lookup target key index dynamically
            led_idx = LAYOUT_MAP[row][col]

            # 4. Generate dynamic colors based on vertical coordinate depth
            r = int((1 - (my / screen_height)) * 255)
            g = 255
            b = int((my / screen_height) * 255)

            # 5. Send report if layout coordinates change
            if led_idx != last_led_idx:
                # Clear trace from previous index block location using batched protocol
                if last_led_idx != -1:
                    clear_payload = [0x00] * 32
                    clear_payload[0] = 0x02            # Command: Batch Update
                    clear_payload[1] = 1               # Total keys inside packet: 1
                    clear_payload[2] = last_led_idx    # Index location targeting
                    clear_payload[3] = 0               # Reset R
                    clear_payload[4] = 0               # Reset G
                    clear_payload[5] = 0               # Reset B
                    send_raw_report(keyboard, clear_payload)

                # Illuminate the targeted cell array key using the multi-key command 0x02
                payload = [0x00] * 32
                payload[0] = 0x02            # Command: Batch Update
                payload[1] = 1               # Total keys inside packet: 1
                payload[2] = led_idx         # Index location targeting
                payload[3] = r               # Set R
                payload[4] = g               # Set G
                payload[5] = b               # Set B
                send_raw_report(keyboard, payload)
                
                print(f"Screen Grid[{row},{col}] -> LED Key Index: {led_idx:2d} | RGB({r},{g},{b})      ", end="\r")
                last_led_idx = led_idx

            time.sleep(0.01)

    except KeyboardInterrupt:
        print("\nTracking loop stopped by user request.")
    finally:
        if last_led_idx != -1:
            clear_payload = [0x00] * 32
            clear_payload[0] = 0x02
            clear_payload[1] = 1
            clear_payload[2] = last_led_idx
            send_raw_report(keyboard, clear_payload)
            
        keyboard.close()
        print("Device handle closed safely.")
