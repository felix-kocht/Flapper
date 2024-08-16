import csv
import re
import os

# Define the input and output file paths
input_file = 'RL_output.txt'
base_output_file = 'RL_output.csv'

# Check if the output file exists and determine a new file name if necessary
output_file = base_output_file

# Initialize lists to store the data
iterations = []
sent_data = []
function_values = []

# Define regex patterns for the required data
iteration_pattern = r"Iteration No: (\d+) started"
data_pattern = r"""Sending data: 0
Sending data:([\s\S]*?)Waiting for"""
function_value_pattern = r"Function value obtained: (-?\d+\.\d+)"

# Read and parse the input file
with open(input_file, 'r') as file:
    content = file.read()
    iteration_matches = re.findall(iteration_pattern, content)
    data_matches = re.findall(data_pattern, content)
    function_value_matches = re.findall(function_value_pattern, content)

    # Append the matches to the lists
    iterations.extend(iteration_matches)
    sent_data.extend(data_matches)
    function_values.extend(function_value_matches)

# Write the data to a CSV file
with open(output_file, 'w', newline='') as csvfile:
    csvwriter = csv.writer(csvfile)
    # Write the header
    csvwriter.writerow(['Iteration', 'Frequency', 'Heave Amplitude', 'Pitch Amplitude', 'Camber Amplitude', 'Pitch Phase', 'Camber Phase', 'Turn Rate', 'Function Value'])

    # Write the rows
    for i in range(len(iterations)):
        # Split the sent data by commas
        data_values = sent_data[i].strip().split(',')
        # Ensure that the row has the correct number of columns
        if len(data_values) == 7:  # Assuming there are 7 data points
            csvwriter.writerow([iterations[i]] +
                               data_values + [function_values[i]])

print(f"Data saved to {output_file}")


