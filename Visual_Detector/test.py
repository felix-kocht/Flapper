import pandas as pd


def calculate_efficiency(merged_df):
    # Placeholder for the actual efficiency calculation
    efficiency = 1  # just a dummy value
    return efficiency


def calculate_statistics(merged_df):
    stats = {
        'average': merged_df.mean(),
        'max': merged_df.max()
    }
    return stats


# Read the CSV files, skipping the first few rows with key-value pairs
df1 = pd.read_csv('output_data/output1_1.csv', skiprows=10)
df2 = pd.read_csv('output_data/output2_1.csv', skiprows=10)

# Read the key-value pairs from the first few rows
metadata1 = pd.read_csv('output_data/output1_1.csv', nrows=10, header=None)
metadata2 = pd.read_csv('output2_1.csv', nrows=10, header=None)

# Merge the dataframes based on the Time column
merged_df = pd.merge_asof(df1, df2, on='Time')

# Calculate efficiency and add it to the metadata
efficiency = calculate_efficiency(merged_df)
efficiency_row = pd.DataFrame([['efficiency', efficiency]])

# Calculate statistics and add them to the metadata
statistics = calculate_statistics(merged_df)
statistics_rows = pd.DataFrame([
    [f'{stat}_of_{col}', value] for stat, values in statistics.items() for col, value in values.items()
])

# Append the efficiency and statistics rows to the metadata
metadata1 = pd.concat(
    [metadata1, efficiency_row, statistics_rows], ignore_index=True)

# Create the output CSV content
output_file = 'merged_output.csv'

# Write the metadata to the output CSV
with open(output_file, 'w') as f:
    metadata1.to_csv(f, index=False, header=False)
    f.write('\n')

# Append the merged data to the output CSV
merged_df.to_csv(output_file, mode='a', index=False)

print(f"Merged CSV written to {output_file}")
