import serial
import csv
import time
from threading import Thread

# Variables to set by user
csvfile = 'output.csv'
port1 = '/dev/cu.usbserial-110'# usbserial-110'  # Replace with your port
port2 = '/dev/cu.usbmodem1201'  # Replace with your port
baudrate = 9600
parse_pattern = "utf-8"  # Adjust based on your data format

# Class definition for an IO manager


class IOManager:
    def __init__(self, port, baudrate, parse_pattern):
        self.port = port
        self.baudrate = baudrate
        self.parse_pattern = parse_pattern
        self.serial_connection = serial.Serial(port, baudrate)

    def read(self):
        if self.serial_connection.in_waiting > 0:
            data = self.serial_connection.readline().decode(self.parse_pattern).strip()
            return data
        return None

# Function to save to CSV file

def save_to_csv(data):
    with open(csvfile, 'a', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(data)

# Main function


def main():
    io_manager1 = IOManager(port1, baudrate, parse_pattern)
    io_manager2 = IOManager(port2, baudrate, parse_pattern)

    def read_from_device(io_manager):
        while True:
            data = io_manager.read()
            if data:
                save_to_csv([io_manager.port, data])
            time.sleep(1)  # Adjust the sleep time as necessary

    # Create threads to read from each IO manager
    thread1 = Thread(target=read_from_device, args=(io_manager1,))
    thread2 = Thread(target=read_from_device, args=(io_manager2,))

    # Start the threads
    thread1.start()
    thread2.start()

    # Join the threads to ensure they keep running
    thread1.join()
    thread2.join()


if __name__ == "__main__":
    main()


# imports

# variables to set by user
#   csvfile to save to
#   list of instructions to send to io managers (and when)
#       e.g. set of paramters and correspoding duration (or send-out time) for each serial device
#   class definitino stuff for io manager

# class definition for an io manager
#   port, baudrate, parse pattern,
#   read
#   write

# function to save to csv file

# main function, creating 2 / 3 io managers
