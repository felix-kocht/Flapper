import csv
import serial

# Setup the serial connection (adjust the port and baud rate according to your setup)
ser = serial.Serial('/dev/ttyUSB0', 9600)  # Example port and baud rate


def outgoing(data):
    """
    Saves outgoing data to a CSV file and sends it via serial.
    Args:
    data (str): A string of data to be logged and sent.
    """
    # Save to CSV
    with open('unfiltered.csv', 'a', newline='') as file:
        writer = csv.writer(file)
        writer.writerow([data])

    # Send via serial
    if ser.is_open:
        ser.write(data.encode())  # Send data as bytes
    else:
        print("Serial port is not open.")
