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
metadata = pd.read_csv('output_data/output1_1.csv', nrows=10, header=None)

# Merge the dataframes based on the Time column
merged_df = pd.merge_asof(df1, df2, on='Time')

# Add new columns with dummy values
merged_df['Angle of attack'] = merged_df['Heave_pos'] * \
    0.1  # Dummy calculation for illustration
merged_df['Inflow velocity'] = merged_df['Power_consumption'] * \
    0.05  # Dummy calculation for illustration

# Calculate efficiency and append it to the metadata
efficiency = calculate_efficiency(merged_df)
efficiency_row = pd.DataFrame([['Efficiency:', efficiency]])
metadata = pd.concat([metadata, efficiency_row], ignore_index=True)

# Calculate statistics and prepare them for insertion
statistics = calculate_statistics(merged_df)
average_row = pd.DataFrame([['average'] + statistics['average'].tolist()],
                           columns=['Statistic'] + merged_df.columns.tolist())
max_row = pd.DataFrame([['max'] + statistics['max'].tolist()],
                       columns=['Statistic'] + merged_df.columns.tolist())

# Create the output CSV content
output_file = 'merged_output.csv'

# Write the metadata to the output CSV
with open(output_file, 'w') as f:
    f.write('Used Parameters:\n')
    metadata.to_csv(f, index=False, header=False)
    f.write('\n')
    f.write('Statistical Evaluation:\n')

# Write the header to the output CSV
header = pd.DataFrame(columns= merged_df.columns.tolist())
header.to_csv(output_file, mode='a', index=False, header=True)

# Write the statistics rows
with open(output_file, 'a') as f:
    average_row.iloc[:, [0] + list(range(2, average_row.shape[1]))].to_csv(
        f, index=False, header=False, float_format='%.2f')
    max_row.iloc[:, [0] + list(range(2, max_row.shape[1]))
                 ].to_csv(f, index=False, header=False, float_format='%.2f')
    f.write('\n')
    f.write('Complete Data:\n')

# Append the merged data to the output CSV, omitting the first row, with specific formatting
merged_df.iloc[1:].to_csv(output_file, mode='a',
                          index=False, float_format='%.2f')

print(f"Merged CSV written to {output_file}")
