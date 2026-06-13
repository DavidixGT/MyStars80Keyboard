import hid

def find_qmk_device(keyword="Stars80"):
    """Scans connected HID devices and returns VID and PID for the Stars80."""
    devices = hid.enumerate()
    for dev in devices:
        product_name = dev.get('product_string', '') or ''
        
        # Look specifically for your Stars80 string
        if keyword.lower() in product_name.lower():
            print(f"Found Device: {product_name}")
            return dev['vendor_id'], dev['product_id']
    return None, None

if __name__ == "__main__":
    # 1. Look for 'Stars80' instead of 'qmk'
    vid, pid = find_qmk_device("Stars80")
    
    if vid and pid:
        # Define the RGB values for your Backspace key
        # Example: Magenta (R=255, G=0, B=150)
        rgb_color_data = {
            0: 255, # Red
            1: 0,   # Green
            5: 150  # Blue
        }
        
        packet = [0x00] * 33
        for qmk_index, value in rgb_color_data.items():
            packet[qmk_index + 1] = value

        try:
            device = hid.device()
            device.open(vid, pid)
            device.write(packet)
            print("Color data successfully sent to Stars80!")
            device.close()
        except IOError as e:
            print(f"Error communicating with device: {e}")
    else:
        print("Stars80 keyboard not detected. Check USB connection.")
