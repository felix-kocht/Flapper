import serial
import time

# Adjust these variables to match your setup
# Change this to your actual serial port (e.g., 'COM3' on Windows)
serial_port = '/dev/cu.usbmodem142201'
baud_rate = 9600
output_file = '/Users/maxi/Documents/1_Studium/Bachelorarbeit/Experiments_for_validation/Friction_model/serial.txt'
connected = False


def check_serial_connection(port, baud):
    global connected
    try:
        with serial.Serial(port, baud, timeout=1) as ser:
            connected = ser.is_open
            return connected
    except serial.SerialException:
        connected = False
        return connected


def read_from_serial(port, baud, output_path):
    with serial.Serial(port, baud, timeout=1) as ser, open(output_path, 'a') as file:
        while True:
            line = ser.readline().decode('utf-8').strip()
            if line:
                print(line)  # Print to console
                file.write(f"{line}\n")  # Write to file
                # Add the following line to print to console
                print(line)


if __name__ == "__main__":
    # Establish a connection
    while connected is False:
        connected = check_serial_connection(serial_port, baud_rate)
        if connected:
            print("Connection established successfully!")
            break
        else:
            print(
                f"Failed to establish connection with {serial_port} at {baud_rate} baud rate. Retrying...")
            time.sleep(1)

    # Read from the serial port
    while True:
        try:
            read_from_serial(serial_port, baud_rate, output_file)
            time.sleep(1)
        except Exception as e:
            print(f"Error: {e}")
            time.sleep(1)
