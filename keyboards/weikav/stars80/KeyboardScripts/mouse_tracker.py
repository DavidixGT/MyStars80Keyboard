import sys
import hid
import time
import mouse  # Requires: pip install mouse
import keyboard as kb_hotkey  # Requires: pip install keyboard

vendor_id     = 0x342d
product_id    = 0xe401

usage_page    = 0xFF60
usage         = 0x61
report_length = 32

# Stars80 standard TKL grid configuration
COLS = 17  # Total columns from Esc to Pause / ` to PgUp
ROWS = 6   # Total horizontal key rows

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
    request_data = [0x00] * (report_length + 1) # First byte is Report ID
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
        print(f"Connected to: {keyboard.get_product_string()} (Snaking Matrix Mode)")
    else:
        print("Stars80 Keyboard not found! Check connection.")
        sys.exit(1)

    print("\n--- Stars80 Absolute Coordinate Grid Tracker ---")
    print("Move your mouse! The layout grid mirrors your display grid.")
    print("► PRESS THE 'ESC' KEY ON YOUR KEYBOARD TO EXIT INSTANTLY ◄\n")

    last_led_idx = -1

    try:
        while True:
            # Emergency Exit
            #if kb_hotkey.is_pressed('esc'):
             #   print("\nExit hotkey detected!")
             #   break

            # 1. Capture cursor position
            mx, my = mouse.get_position()
            
            # Change these if you use a 2K (2560x1440) or 4K (3840x2160) monitor
            screen_width = 1920
            screen_height = 1080
            
            mx = max(0, min(mx, screen_width - 1))
            my = max(0, min(my, screen_height - 1))

            # 2. Map mouse coordinates into keyboard layout matrix cell boundaries
            col = int((mx / screen_width) * COLS)
            row = int((my / screen_height) * ROWS)

            # 3. CONVERT GRID TO SNAKING INDEX SYSTEM
            # Even Rows (0, 2, 4): Left-to-Right layout (Esc to Pause)
            if row % 2 == 0:
                led_idx = (row * COLS) + col
            # Odd Rows (1, 3, 5): Right-to-Left layout (PgUp back to `)
            else:
                led_idx = (row * COLS) + (COLS - 1 - col)

            # Keep index bounded within physical trace parameters (0-86)
            led_idx = max(0, min(led_idx, 86))

            # 4. Generate dynamic color palettes based on active cursor position
            r = int((1 - (my / screen_height)) * 255)
            g = 122
            b = int((my / screen_height) * 255)

            # 5. Process screen rendering updates
            if led_idx != last_led_idx:
                # Turn off the previous coordinate trace
                if last_led_idx != -1:
                    clear_payload = [0x00] * 32
                    clear_payload[0] = last_led_idx
                    clear_payload[1] = 0
                    clear_payload[2] = 0
                    clear_payload[3] = 0
                    send_raw_report(keyboard, clear_payload)

                # Light up the current target index location
                payload = [0x00] * 32
                payload[0] = led_idx
                payload[1] = r
                payload[2] = g
                payload[3] = b
                send_raw_report(keyboard, payload)
                
                print(f"Mouse -> Grid[{row},{col}] -> LED Index: {led_idx:2d} | RGB({r},{g},{b})      ", end="\r")
                last_led_idx = led_idx

            time.sleep(0.01)

    except KeyboardInterrupt:
        print("\nTracker stopped by user.")
    finally:
        # Turn off left-over trace indicators before closing execution hooks
        if last_led_idx != -1:
            clear_payload = [0x00] * 32
            clear_payload[0] = last_led_idx
            send_raw_report(keyboard, clear_payload)
            
        keyboard.close()
        print("Connection closed safely.")
