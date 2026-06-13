import sys
import hid
import time

vendor_id     = 0x342d
product_id    = 0xe401

usage_page    = 0xFF60
usage         = 0x61
report_length = 32

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
        print(f"Connected to: {keyboard.get_product_string()} (Sweep Mode)")
    else:
        print("Stars80 Keyboard not found! Check connection.")
        sys.exit(1)

    print("\n--- Stars80 LED Enumeration Sweep ---")
    print("Press Ctrl+C at any time to halt the loop.\n")

    try:
        # Loop indices 0 to 120 (covering standard TKL/80% layout bounds)
        for led_idx in range(0, 87):
            print(f"Illuminating LED Index: {led_idx}")
            
            # 1. Turn the current key pure white
            payload = [0x00] * 32
            payload[0] = led_idx
            payload[1] = 255  # Red
            payload[2] = 255  # Green
            payload[3] = 255  # Blue
            send_raw_report(keyboard, payload)
            
            # 2. Keep the key lit for 0.15 seconds
            time.sleep(0.15)
            
            # 3. Clear the current key (set it to 0,0,0) before moving to the next
            # This makes the light visibly crawl across the keyboard
            clear_payload = [0x00] * 32
            clear_payload[0] = led_idx
            clear_payload[1] = 0
            clear_payload[2] = 0
            clear_payload[3] = 0
            send_raw_report(keyboard, clear_payload)

        print("\nEnumeration sweep complete!")

    except KeyboardInterrupt:
        print("\nSweep halted by user.")
    finally:
        keyboard.close()
        print("Connection closed.")
