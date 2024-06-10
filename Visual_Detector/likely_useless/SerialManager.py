import csv
import serial

# no need to change anything in this file, only the port and baud rate (and maybe the file names to log to)
ser = serial.Serial('/dev/tty.usbmodem101', 9600)


def outgoing(data):
    """
    Saves outgoing data to a CSV file and sends it via serial.
    Args:
    data (str or float): Data to be logged and sent.
    """
    # shorten the data to 2 decimal places
    data = round(data, 2)

    # Convert data to string if it's not already
    if not isinstance(data, str):
        data = str(data)

    # Send via serial with a newline character as the delimiter
    if ser.is_open:
        ser.write((data + '\n').encode())  # Append '\n' and send data as bytes

    else:
        print("Serial port is not open.")

    # Save to CSV
    with open('unfiltered.csv', 'a', newline='') as file:
        writer = csv.writer(file)
        writer.writerow([data])


def incoming():
    """
    Reads incoming data (in float format) from the serial port.
    """
    if ser.in_waiting > 0:
        data = ser.readline().decode().strip()
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
