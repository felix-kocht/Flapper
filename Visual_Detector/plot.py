import pandas as pd
import matplotlib.pyplot as plt


def plot_columns(file_path, columns_to_plot, title='Specified Columns over Time'):
    # TODO: user can change these
    scale_factors = {
        'Heave_pos': 0.02,
        'Pitch_right': 1,
        'Pitch_left': 1,
        'Power_consumption': 0.1,
        'Noise': 10,
        'Fx': 1,
        'Fz': 100,
        'Mx': 1000,
        'My': 1000,
        'Mz': 1000,
        'Angle_of_attack': 1,
        'Inflow_velocity': 100,
        'Efficiency': 100
    }

    # Read the CSV file and skip the metadata lines
    data = pd.read_csv(file_path, skiprows=13)

    # Remove rows with metadata and repeated headers
    data_cleaned = data.drop([0, 1, 2, 3, 4, 5, 6])

    # Convert appropriate columns to numeric, ignoring errors for non-numeric rows
    data_cleaned = data_cleaned.apply(pd.to_numeric, errors='ignore')

    # Plotting the data
    plt.figure(figsize=(10, 6))
    for column in columns_to_plot:
        if column in scale_factors and scale_factors[column] != 1:
            scale = scale_factors[column]
            plt.plot(data_cleaned['Time'], data_cleaned[column]
                     * scale, label=f'{column} (scaled by {scale})')
        else:
            plt.plot(data_cleaned['Time'], data_cleaned[column], label=column)

    plt.xlabel('Time')
    plt.ylabel('Force in Newton')
    plt.title(title)
    plt.legend()
    plt.grid(True)
    plt.show()


# Example usage:
#plot_columns('test_cases/wide_stiff_sharklets/wide_stiff_sharklet0.8_35_90_0_0_0.csv',
             #['Heave_pos', 'Fx'], title='Various Parameters over Time')
#plot_columns('test_cases/Cambered_foils/Jul_18_cambered0.8_45_90_0_0_0.csv',
                  #['Heave_pos', 'Fx'], title='Various Parameters over Time')
plot_columns('test_cases/asymmetric_cambered/asymmetric_cambered0.5_20_90_0_0_0.csv',
             ['Heave_pos', 'Fx'], title = 'With -1.12 Phase shift')

# TODO: add multiple different unit values to the y-axis

