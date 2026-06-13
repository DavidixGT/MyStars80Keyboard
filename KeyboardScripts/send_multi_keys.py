import sys
import hid
import time

vendor_id     = 0x342d
product_id    = 0xe401
usage_page    = 0xFF60
usage         = 0x61
report_length = 32

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
        interface.read(report_length, 10)
        return True
    except Exception as e:
        print(f"Error: {e}")
        return False

def update_keys_simultaneously(interface, key_color_dict, reset_others=False):
    """
    Accepts a dictionary of multiple keys: { index: (R, G, B), index2: (R, G, B) }
    Sends them in optimized chunks to QMK.
    """
    # Step 1: If requested, clear out old states first
    if reset_others:
        reset_payload = [0x00] * 32
        reset_payload[0] = 0x01 # Trigger Reset Command
        send_raw_report(interface, reset_payload)

    # Step 2: Chunk updates (7 keys max fit into a single 32-byte packet: 1 + 1 + 7*4 = 30 bytes)
    items = list(key_color_dict.items())
    for i in range(0, len(items), 7):
        chunk = items[i:i+7]
        
        payload = [0x00] * 32
        payload[0] = 0x02            # Batch Update Command Type
        payload[1] = len(chunk)      # Number of keys inside this specific packet
        
        offset = 2
        for led_idx, (r, g, b) in chunk:
            payload[offset]     = led_idx
            payload[offset + 1] = r
            payload[offset + 2] = g
            payload[offset + 3] = b
            offset += 4
            
        send_raw_report(interface, payload)

if __name__ == '__main__':
    keyboard = get_raw_hid_interface()
    if not keyboard:
        print("Stars80 Keyboard not found!")
        sys.exit(1)

    try:
        print("Sending simultaneous key matrix colors...")
        
        # Define a test map lighting up 5 keys at the exact same moment:
        # Key 0 (Esc), Key 13 (Backspace), and a few arrow indicators
        target_cluster = {
            0:  (255, 0, 0),    # Esc turns Red
            13: (0, 255, 0),    # Backspace turns Green
            77: (0, 0, 255),    # Up Arrow turns Blue
            85: (255, 255, 0),  # Down Arrow turns Yellow
            86: (255, 0, 255)   # Right Arrow turns Magenta
        }
        
        # Send everything in one burst!
        update_keys_simultaneously(keyboard, target_cluster, reset_others=True)
        print("Keys illuminated simultaneously.")
        
        time.sleep(5) # Hold layout pattern for 5 seconds
        
        # Turn them all off at once
        update_keys_simultaneously(keyboard, {}, reset_others=True)
        print("Cleaned layout array.")

    finally:
        keyboard.close()