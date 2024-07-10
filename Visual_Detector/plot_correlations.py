import os
import glob
import pandas as pd
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

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
folder_path = 'test_cases'  # Update this path to the actual folder path
all_parameters, all_average_values = collect_data_from_folder(folder_path)

# User specifies which parameters and measurements to use for X, Y, Z axes and color
x_param = 'Frequency'  # Example X-axis parameter
y_measurement = 'Pitch Amplitude'  # Example Y-axis parameter
z_measurement = 'Fx'  # Example Z-axis measurement
color_measurement = 'Mz'  # Example color measurement (optional)

x_data = []
y_data = []
z_data = []
color_data = []

for params, values in zip(all_parameters, all_average_values):
    x_value = params.get(x_param) or values.get(x_param)
    y_value = params.get(y_measurement) or values.get(y_measurement)
    z_value = params.get(z_measurement) or values.get(z_measurement)
    color_value = params.get(
        color_measurement) or values.get(color_measurement)

    # Debug: Print values to check if they are being extracted correctly
    #print(f"x: {x_value}, y: {y_value}, z: {z_value}, color: {color_value}")

    if x_value and y_value and z_value and color_value:
        try:
            x_value = float(x_value.strip(','))
            y_value = float(y_value.strip(','))
            z_value = float(z_value.strip(','))
            color_value = float(color_value.strip(','))
            x_data.append(x_value)
            y_data.append(y_value)
            z_data.append(z_value)
            color_data.append(color_value)
        except ValueError:
            # Handle the case where conversion to float fails
            continue

# Plotting the data in 3D with color as the 4th dimension
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
sc = ax.scatter(x_data, y_data, z_data, c=color_data, cmap='viridis')

plt.colorbar(sc, ax=ax, label=color_measurement)
ax.set_xlabel(x_param)
ax.set_ylabel(y_measurement)
ax.set_zlabel(z_measurement)
plt.title(
    f'{y_measurement}, {z_measurement} vs {x_param} with {color_measurement}')

# Automatic scaling of axes
ax.auto_scale_xyz(x_data, y_data, z_data)

plt.show()
