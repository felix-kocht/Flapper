import os
import glob
import pandas as pd
import matplotlib.pyplot as plt

# Function to parse a single CSV file and extract parameters and average values


def parse_csv(file_path):
    parameters = {}
    average_values = {}

    with open(file_path, 'r') as file:
        lines = file.readlines()

    # Extract Used Parameters
    for line in lines:
        if line.startswith("Used Parameters:"):
            param_start = lines.index(line) + 1
        elif line.startswith("Statistical Evaluation:"):
            stat_start = lines.index(line) + 1

    for line in lines[param_start:stat_start-1]:
        if line.strip() and ':' in line:
            key, value = line.strip().split(':', 1)
            parameters[key.strip()] = value.strip()

    # Extract average values from Statistical Evaluation
    headers = lines[stat_start].strip().split(',')
    avg_values = lines[stat_start + 1].strip().split(',')

    for header, value in zip(headers, avg_values):
        average_values[header.strip()] = value.strip()

    return parameters, average_values

# Function to collect data from all CSV files in a folder


def collect_data_from_folder(folder_path):
    all_parameters = []
    all_average_values = []

    for file_path in glob.glob(os.path.join(folder_path, "*.csv")):
        parameters, average_values = parse_csv(file_path)
        all_parameters.append(parameters)
        all_average_values.append(average_values)

    return all_parameters, all_average_values


# Example usage
# Update this path to the actual folder path
folder_path = 'test_cases'
all_parameters, all_average_values = collect_data_from_folder(folder_path)

# User specifies which parameter and measurement to use for X and Y axes
x_param = 'Pitch Amplitude'  # Example X-axis parameter
y_measurement = 'Fx'  # Example Y-axis measurement

x_data = []
y_data = []

for params, values in zip(all_parameters, all_average_values):
    if x_param in params and y_measurement in values:
        try:
            x_value = float(params[x_param].strip(','))
            y_value = float(values[y_measurement])
            x_data.append(x_value)
            y_data.append(y_value)
        except ValueError:
            # Handle the case where conversion to float fails
            continue

# Plotting the data
plt.scatter(x_data, y_data)
plt.xlabel(x_param)
plt.ylabel(y_measurement)
plt.title(f'{y_measurement} vs {x_param}')
plt.grid(True)
plt.show()
