import sys
import hid

vendor_id     = 0x342d
product_id    = 0xe401

usage_page    = 0xFF60
usage         = 0x61
report_length = 32
skipColorRequest = 1

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
        # We read with a tiny timeout (10ms) or omit if QMK doesn't respond, 
        # so Python doesn't hang waiting for a response.
        interface.read(report_length, 10) 
        return True
    except Exception as e:
        print(f"Error sending data: {e}")
        return False

def get_valid_int(prompt, min_val, max_val):
    while True:
        try:
            val = int(input(prompt))
            if min_val <= val <= max_val:
                return val
            print(f"Value must be between {min_val} and {max_val}.")
        except ValueError:
            print("Please enter a valid integer.")

if __name__ == '__main__':
    # 1. Open the connection ONCE at launch
    keyboard = get_raw_hid_interface()
    if keyboard:
        print(f"Connected to: {keyboard.get_product_string()} (Instant Mode)")
    else:
        print("Stars80 Keyboard not found! Check connection.")
        sys.exit(1)

    print("\n--- Stars80 RGB Console Interface ---")
    print("Type 'exit' or press Ctrl+C at any prompt to quit.\n")

    try:
        while True:
            user_input = input("Enter LED Index (0-255) or 'exit': ").strip()
            if user_input.lower() == 'exit':
                break
            
            try:
                led_idx = int(user_input)
                if led_idx < 0:
                    print("Index cannot be negative.")
                    continue
            except ValueError:
                print("Invalid index. Please enter a valid number.")
                continue

            if skipColorRequest == 1:
                r = 255
                g = 255
                b = 255
            else:
                r = get_valid_int("Enter Red (0-255): ", 0, 255)
                g = get_valid_int("Enter Green (0-255): ", 0, 255)
                b = get_valid_int("Enter Blue (0-255): ", 0, 255)

            payload = [0x00] * 32
            payload[0] = led_idx
            payload[1] = r
            payload[2] = g
            payload[3] = b

            print(f"\nSending RGB({r},{g},{b}) to LED index {led_idx}...")
            
            # 2. Pass the active persistent connection
            if send_raw_report(keyboard, payload):
                print("Data sent instantly!\n")

    except KeyboardInterrupt:
        print("\nExiting console interface.")
    finally:
        # 3. Cleanly close it only when the loop terminates
        keyboard.close()
        print("Connection closed.")
