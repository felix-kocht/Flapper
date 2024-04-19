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


def incoming():
    """
    Reads incoming data from the serial port.
    Returns:
    str: The incoming data as a string.
    """
    if ser.in_waiting > 0:
        data = ser.readline().decode().strip()  # Read data as string
        return data
    else:
        return None


def manage_data(data):
    # save incoming data to a csv file
    new_data = incoming()
    with open('filtered.csv', 'a', newline='') as file:
        writer = csv.writer(file)
        writer.writerow([new_data])

    # send the data out
    outgoing(data)
