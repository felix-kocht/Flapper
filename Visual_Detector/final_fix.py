import os
import pandas as pd


def correct_average_efficiency(folder_path):
    # Iterate through all files in the folder
    for filename in os.listdir(folder_path):
        if filename.endswith('.csv'):
            file_path = os.path.join(folder_path, filename)
            try:
                # Load the CSV file, skipping the metadata rows

                df = pd.read_csv(file_path, skiprows=13)

                # Read the key-value pairs from the first few rows
                with open(file_path, 'r') as file:
                    metadata = [next(file) for _ in range(13)]


                # Calculate the corrected average efficiency
                average_Fx = df.loc[0, 'Fx']
                average_Power_consumption = df.loc[0, 'Power_consumption']
                corrected_average_efficiency = float(
                    average_Fx) * 1000 / float(average_Power_consumption)

                # Update the dataframe with the corrected value
                df.loc[0, 'Efficiency'] = corrected_average_efficiency
        

                with open(file_path, 'w') as file:
                    file.writelines(metadata)
                df.to_csv(file_path, mode='a', index=False)

                print(f"Processed {filename} successfully with average efficiency: {corrected_average_efficiency}")
            except Exception as e:
                print(f"Error processing {filename}: {e}")


# Define the folder path containing the CSV files
folder_path = 'test_cases/asymmetric_cambered_filtered'

# Run the function
correct_average_efficiency(folder_path)
