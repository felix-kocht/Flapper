import pandas as pd
import numpy as np
from scipy.signal import find_peaks
import os

# Function to process each file and save the useful rows with recalculated statistical evaluation


def process_file(input_file_path, output_folder):
    # Load and process the CSV file
    with open(input_file_path, 'r') as file:
        lines = file.readlines()

    # Identify the start of the complete data and the initial rows
    start_index = None
    for i, line in enumerate(lines):
        if "Complete Data" in line:
            start_index = i + 1
            break

    # Remove initial statistical evaluation rows (assumed to be from row 13 to 18)
    # Keeping rows before and after statistical evaluation
    initial_rows = lines[:12] + lines[19:start_index]

    # Extract the header and data
    header = lines[start_index].strip().split(',')
    data_lines = lines[start_index + 1:]

    # Create a dataframe from the data
    data_content = [line.strip().split(',') for line in data_lines]
    df_data = pd.DataFrame(data_content, columns=header)

    # Convert the 'Fx' column to numeric, handling any errors
    df_data['Fx'] = pd.to_numeric(df_data['Fx'], errors='coerce')

    # Remove rows with NaN in 'Fx' column
    df_data = df_data.dropna(subset=['Fx'])

    # Extract the Fx column as a numpy array
    fx_values = df_data['Fx'].to_numpy()

    # Find all local minima in the Fx array
    minima_indices, _ = find_peaks(-fx_values)

    # Filter significant minima (at least 0.1 lower than neighboring maxima)
    significant_minima = []
    for idx in minima_indices:
        if (idx > 0 and idx < len(fx_values) - 1):
            left_max = max(fx_values[:idx])
            right_max = max(fx_values[idx+1:])
            if fx_values[idx] <= left_max - 0.1 and fx_values[idx] <= right_max - 0.1:
                significant_minima.append(idx)

    # Split the dataframe into sections based on significant minima
    sections = np.split(df_data, significant_minima)

    # Determine which of the first two sections has a higher average Fx value
    first_useful_index = 0 if sections[0]['Fx'].mean(
    ) > sections[1]['Fx'].mean() else 1

    # Collect every second section starting from the first useful section
    useful_sections = sections[first_useful_index::2]

    # Combine the useful sections into a single dataframe
    useful_data = pd.concat(useful_sections)

    # Calculate statistical evaluations for each column and round to 3 decimal places
    stats = pd.DataFrame({
        'Statistic': ['average', 'max', 'min', 'std. dev'],
    })

    # Calculate stats for all relevant columns in useful data (excluding 'Time')
    for col in useful_data.columns:
        if col != 'Statistic' and col != 'Time':  # Exclude 'Time' from statistics
            numeric_col = pd.to_numeric(useful_data[col], errors='coerce')
            stats[col] = [
                round(numeric_col.mean(), 3),
                round(numeric_col.max(), 3),
                round(numeric_col.min(), 3),
                round(numeric_col.std(), 3)
            ]

    # Concatenate the initial rows, statistical evaluation, and useful data into one dataframe
    output_content = pd.DataFrame()

    # Create a dataframe for initial rows to facilitate concatenation
    initial_df = pd.DataFrame({'Parameters': initial_rows})

    # Write the initial rows and calculated statistics to the output file
    output_file_path = os.path.join(
        output_folder, f'useful_{os.path.basename(input_file_path)}')
    with open(output_file_path, 'w') as output_file:
        # Write the initial parameter rows
        for row in initial_rows:
            output_file.write(row)

        # Write the 'Statistical Evaluation' header
        output_file.write("\nStatistical Evaluation:\n")

        # Write the statistical evaluation
        stats.to_csv(output_file, index=False)

        # Write a separator line
        output_file.write("\nComplete Data:,,,,,,,,,,,,,\n")

        # Write the useful data
        useful_data.to_csv(output_file, index=False)

    print(f"Processed and saved CSV for: {os.path.basename(input_file_path)}")

# Function to process all files in the input folder


def process_all_files(input_folder, output_folder):
    # Create the output folder if it doesn't exist
    if not os.path.exists(output_folder):
        os.makedirs(output_folder)

    # Iterate over all CSV files in the input folder
    for filename in os.listdir(input_folder):
        if filename.endswith(".csv"):
            input_file_path = os.path.join(input_folder, filename)
            process_file(input_file_path, output_folder)


# Specify your input and output folders
# Replace with the path to your input folder
input_folder = "test_cases/asymmetric_cambered"
# Replace with the path to your output folder
output_folder = "test_cases/asymmetric_cambered_filtered"

# Run the processing
process_all_files(input_folder, output_folder)
