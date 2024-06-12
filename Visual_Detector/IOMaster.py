import serial
import csv
import time
import sys
import threading
from threading import Thread

# Variables to set by user
csvfile1 = 'output1.csv'
csvfile2 = 'output2.csv'
port1 = '/dev/cu.usbserial-1110'  # Replace with your port
port2 = '/dev/cu.usbmodem11201'  # Replace with your port
baudrate = 9600
parse_pattern = "utf-8"  # Adjust based on your data format
sample_rate = 10  # Adjust based on your data rate in Hz
starttime = time.time()
testrun_file = 'testrun.csv'
tests_completed = threading.Event()

# Class definition for an IO manager


class IOManager:
    def __init__(self, port, baudrate, parse_pattern, csvfile):
        self.port = port
        self.baudrate = baudrate
        self.parse_pattern = parse_pattern
        self.csvfile = csvfile
        self.serial_connection = serial.Serial(port, baudrate)

    def read(self):
        if self.serial_connection.in_waiting > 0:
            data = self.serial_connection.readline().decode(self.parse_pattern).strip()
            return data
        return None

    def write(self, data):
        self.serial_connection.write(data.encode(self.parse_pattern))

# Function to clean data


def clean_data(raw_data):
    cleaned_data = raw_data.replace('"', '').replace('/', '').replace('*', '')
    return cleaned_data.split(',')

# Function to save to CSV file with a timestamp
def save_to_csv(csvfile, data):
    with open(csvfile, 'a', newline='') as f:
        writer = csv.writer(f)
        writer.writerow([time.time() - starttime] + data)


# Function to load test cases
def load_test_cases(filename):
    with open(filename, 'r') as f:
        reader = csv.reader(f)
        headers = next(reader)  # Skip the header row
        test_cases = [row for row in reader]
    return test_cases

# Function to run test cases
def run_test_cases(io_manager1, test_cases):
    global tests_completed
    for case in test_cases:
        # Join the first seven values into a single string
        data_to_send = ','.join(case[:-1])
        duration = float(case[-1])  # The last value is the test duration
        print(f"Sending data: {data_to_send}")
        io_manager1.write(data_to_send)
        print(f"Waiting for {duration} seconds")
        time.sleep(duration)
    print("All test cases completed.")
    tests_completed = tests_completed.set()


# Main function
def main():
    io_manager1 = IOManager(port1, baudrate, parse_pattern, csvfile1)
    io_manager2 = IOManager(port2, baudrate, parse_pattern, csvfile2)

    def read_from_device(io_manager):
        while not tests_completed.is_set():
            data = io_manager.read()
            if data:
                cleaned_data = clean_data(data)
                save_to_csv(io_manager.csvfile, cleaned_data)
            time.sleep(1/sample_rate)  # Adjust the sleep time as necessary

    # Load test cases
    test_cases = load_test_cases(testrun_file)

    # Create threads to read from each IO manager
    thread1 = Thread(target=read_from_device, args=(io_manager1,))
    thread2 = Thread(target=read_from_device, args=(io_manager2,))
    test_thread = Thread(target=run_test_cases, args=(io_manager1, test_cases))

    # Start the threads
    thread1.start()
    thread2.start()
    test_thread.start()

    # Join the threads to ensure they keep running
    thread1.join()
    thread2.join()
    test_thread.join()


if __name__ == "__main__":
    main()
