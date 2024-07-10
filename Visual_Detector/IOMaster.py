import serial
import csv
import time
import threading
from threading import Thread
import os

# ports
port1 = '/dev/cu.usbserial-10'  # Replace with your port
port2 = '/dev/cu.usbmodem1101'  # Replace with your port

# Variables to set by user
target_folder = 'output_data'  # Target folder where data should be saved
testrun_file = 'test_instructions.csv'
used_foils = 'stiff_foils'
waterspeed = '0.0'

# Variables to change only if needed
sample_rate = 20  # Adjust based 0.4 on your data rate in Hz
header_line1 = ["Time", "Heave_pos", "Pitch_right",
                "Pitch_left", "Power_consumption", "Noise"]  # Header line for CSV1
header_line2 = ["Time", "Fx", "Fz",
                "Mx", "My", "Mz"]  # Header line for CSV2
# Metadata keys
metadata_keys = [
    "Used Foils:", "Waterspeed:", "Frequency:", "Heave Amplitude:", "Pitch Amplitude:", "Camber Amplitude", "Pitch Phase", "Camber Phase", "Turn Rate", "Test Duration"]

baudrate = 19200
parse_pattern = "utf-8"  # Adjust based on your data format
starttime = time.time()
tests_completed = threading.Event()
case_number = 1

# Ensure target folder exists
if not os.path.exists(target_folder):
    os.makedirs(target_folder)

# Class definition for an IO manager


class IOManager:
    def __init__(self, port, baudrate, parse_pattern, number):
        self.port = port
        self.baudrate = baudrate
        self.parse_pattern = parse_pattern
        self.serial_connection = serial.Serial(port, baudrate)
        self.number = number

    def read(self):
        if self.serial_connection.in_waiting > 0:
            self.serial_connection.flushInput()  # Clear the input buffer
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


def header_to_csv(csvfile, header_line, metadata):
    with open(csvfile, 'w', newline='') as f:
        writer = csv.writer(f)
        if metadata:
            for key, value in metadata.items():
                writer.writerow([key, value])
            writer.writerow([])  # Blank row to separate metadata from data
        writer.writerow(header_line)  # Write header line


def save_to_csv(csvfile, data):
    with open(csvfile, 'a', newline='') as f:
        writer = csv.writer(f)
        writer.writerow([time.time() - starttime] + data)  # Write data

# Function to handle user input and send to port1


def run_test_cases(io_manager1, test_cases):
    global tests_completed
    global case_number
    time.sleep(1)
    for i, case in enumerate(test_cases[:-1], start=1):
        print(f"Running test case {i}")
        # Metadata for this test case
        case_number = i
        metadata_values = [used_foils, waterspeed] + case
        metadata = {key: value for key, value in zip(
            metadata_keys, metadata_values)}

        # Generate file name for this test case
        csvfile = os.path.join(target_folder, f'output1_{i}.csv')
        csvfile2 = os.path.join(target_folder, f'output2_{i}.csv')

        # Data to send and duration
        data_to_send = data_to_send = ','.join(
            case[:-1])  # Adjust based on your needs
        duration = float(case[-1])  # The last value is the test duration

        # Save metadata and initial data to CSV
        header_to_csv(csvfile, header_line1, metadata)
        header_to_csv(csvfile2, header_line2, metadata)

        # Send data to the device
        print(f"Sending data: {data_to_send}")
        io_manager1.write(data_to_send)
        print(f"Waiting for {duration} seconds")
        time.sleep(duration)

    print("All test cases completed.")
    data_to_send = "0"  # Send a stop signal
    print(f"Sending data: {data_to_send}")
    io_manager1.write(data_to_send)
    tests_completed.set()

# Function to load test cases


def load_test_cases(filename):
    with open(filename, 'r') as f:
        reader = csv.reader(f)
        headers = next(reader)  # Skip the header row
        test_cases = [row for row in reader]
    return test_cases

# Main function


def main():
    io_manager1 = IOManager(port1, baudrate, parse_pattern, 1)
    io_manager2 = IOManager(port2, baudrate, parse_pattern, 2)

    def read_from_device(io_manager):
        file_number = io_manager.number
        while not tests_completed.is_set():
            data = io_manager.read()
            if data:
                cleaned_data = clean_data(data)
                # Here, determine the current test case to save data appropriately
                # As a placeholder, using test case 1 (you may need to track the current case)
                csvfile = os.path.join(
                    target_folder, f'output{file_number}_{case_number}.csv')
                save_to_csv(csvfile, cleaned_data)
            time.sleep(1/sample_rate)  # Adjust the sleep time as necessary

    # Load test cases
    test_cases = load_test_cases(testrun_file)

    # Create threads to read from each IO manager
    thread1 = Thread(target=read_from_device, args=(io_manager1,))
    thread2 = Thread(target=read_from_device, args=(io_manager2,))
    input_thread = Thread(target=run_test_cases,
                          args=(io_manager1, test_cases))

    # Start the threads
    thread1.start()
    thread2.start()
    input_thread.start()

    # Join the threads to ensure they keep running
    thread1.join()
    thread2.join()
    input_thread.join()


if __name__ == "__main__":
    main()
