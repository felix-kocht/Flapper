import pandas as pd
import math
import os


def calculate_statistics(merged_df):
    stats = {
        'average': merged_df.mean(),
        'max': merged_df.max()
    }
    return stats


files = [f for f in os.listdir('output_data') if os.path.isfile(
    os.path.join('output_data', f))]
file_pairs = len(files) / 2

# for every filepair we merge them, calculate statistics and new columns and write them to a new csv
for i in range(1, int(file_pairs) + 1):
    df1 = pd.read_csv(f'output_data/output1_{i}.csv', skiprows=10)
    df2 = pd.read_csv(f'output_data/output2_{i}.csv', skiprows=10)

    # Read the key-value pairs from the first few rows
    metadata = pd.read_csv(
        f'output_data/output1_{i}.csv', nrows=10, header=None)

    # Create the output CSV content
    output_file = (f'test_cases/test_case{i}.csv')

    # Merge the dataframes based on the Time column
    merged_df = pd.merge_asof(df1, df2, on='Time')

    # Add new columns with computed values
    waterspeed = pd.to_numeric(metadata.iloc[1, 1])
    for i in range(len(merged_df['Time'])):
        # Calculate the inflow velocity
        heave_speed = (pd.to_numeric(merged_df['Heave_pos']).diff(
        ).iloc[i] / pd.to_numeric(merged_df['Time']).diff().iloc[i])/720  # 720 converts degree in m/s, depends on gear ratio
        merged_df.at[i, 'Inflow_velocity'] = math.sqrt(
            waterspeed**2 + heave_speed**2)
        
        # Calculate the angle of attack
        if waterspeed == 0:
            nue = merged_df['Pitch_right'].iloc[i]
        else:
            nue = math.atan2(heave_speed / waterspeed)
        gamma = 0.1  # TODO: get from camber amount and pitch angle
        merged_df.at[i,'Angle_of_attack'] = nue - gamma
    merged_df['Efficiency'] = merged_df['Fx'] / merged_df['Power_consumption']

    # Calculate statistics and prepare them for insertion
    statistics = calculate_statistics(merged_df)
    average_row = pd.DataFrame([['average'] + statistics['average'].tolist()],
                               columns=['Statistic'] + merged_df.columns.tolist())
    max_row = pd.DataFrame([['max'] + statistics['max'].tolist()],
                           columns=['Statistic'] + merged_df.columns.tolist())

    # Write the metadata to the output CSV
    with open(output_file, 'w') as f:
        f.write('Used Parameters:\n')
        metadata.to_csv(f, index=False, header=False)
        f.write('\n')
        f.write('Statistical Evaluation:\n')

    # Write the header to the output CSV
    header = pd.DataFrame(columns=merged_df.columns.tolist())
    header.to_csv(output_file, mode='a', index=False, header=True)

    # Write the statistics rows
    with open(output_file, 'a') as f:
        average_row.iloc[:, [0] + list(range(2, average_row.shape[1]))].to_csv(
            f, index=False, header=False, float_format='%.3f')
        max_row.iloc[:, [0] + list(range(2, max_row.shape[1]))
                     ].to_csv(f, index=False, header=False, float_format='%.3f')
        f.write('\n')
        f.write('Complete Data:\n')

    # Append the merged data to the output CSV, omitting the first row, with specific formatting
    merged_df.iloc[1:].to_csv(output_file, mode='a',
                              index=False, float_format='%.3f')

    print(f"Merged CSV written to {output_file}")
