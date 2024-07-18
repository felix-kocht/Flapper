import serial
import csv
import time
import threading
from threading import Thread
import os

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


def initialize_io_managers(port1, port2, baudrate, parse_pattern):
    io_manager1 = IOManager(port1, baudrate, parse_pattern, 1)
    io_manager2 = IOManager(port2, baudrate, parse_pattern, 2)
    return io_manager1, io_manager2


def load_test_cases(filename):
    with open(filename, 'r') as f:
        reader = csv.reader(f)
        headers = next(reader)  # Skip the header row
        test_cases = [row for row in reader]
    return test_cases


def run_single_test(io_manager, params, duration, target_folder, case_number, metadata):
    csvfile1 = os.path.join(target_folder, f'output1_{case_number}.csv')
    csvfile2 = os.path.join(target_folder, f'output2_{case_number}.csv')

    # Save metadata and initial data to CSV
    header_to_csv(csvfile1, header_line1, metadata)
    header_to_csv(csvfile2, header_line2, metadata)

    # Send data to the device
    data_to_send = ','.join(params)
    io_manager.write(data_to_send)
    time.sleep(duration)

    # Stop signal
    io_manager.write("0")
    return csvfile1, csvfile2


def read_device_data(io_manager, target_folder, case_number, sample_rate, tests_completed):
    file_number = io_manager.number
    csvfile = os.path.join(
        target_folder, f'output{file_number}_{case_number}.csv')

    while not tests_completed.is_set():
        data = io_manager.read()
        if data:
            cleaned_data = clean_data(data)
            save_to_csv(csvfile, cleaned_data)
        time.sleep(1/sample_rate)


def clean_data(raw_data):
    cleaned_data = raw_data.replace('"', '').replace('/', '').replace('*', '')
    return cleaned_data.split(',')


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
