import serial
import csv
import time
from datetime import datetime

# Configure the serial port and CSV file path
serial_port = '/dev/tty.usbmodem101'  # Replace with your serial port
baud_rate = 19200
csv_file_path = 'wireless_command_logs.csv'

# Initialize the serial connection
ser = serial.Serial(serial_port, baud_rate)

# Record the start time
start_time = time.time()


def write_to_csv(timestamp, timestamp2, input_string):
    with open(csv_file_path, mode='a', newline='') as file:
        writer = csv.writer(file)
        writer.writerow([timestamp, timestamp2, input_string])


try:
    while True:
        # Read input from the terminal
        input_string = input("Enter a string to send: ")

        # Send the string to the serial port
        ser.write(input_string.encode())

        # Get the current time and calculate the elapsed time since start
        current_time = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        elapsed_time = time.time() - start_time

        # Write to CSV file
        write_to_csv(current_time, elapsed_time, input_string)

        print(f"Sent '{input_string}' to {serial_port} and logged to CSV.")

except KeyboardInterrupt:
    print("Program terminated.")

finally:
    ser.close()
