import pandas as pd
import matplotlib.pyplot as plt

# Load the data from CSV files
df1 = pd.read_csv('output1.csv', header=None)
df2 = pd.read_csv('output2.csv', header=None)

# Assign column names
columns = ['Time', 'Data1', 'Data2', 'Data3', 'Data4', 'Data5']
df1.columns = columns
df2.columns = columns

# Merge the two dataframes on the 'Time' column
merged_df = pd.merge_asof(df1, df2, on='Time', suffixes=('_device1', '_device2'))

# Plotting the data
def plot_subplots(df):
    fig, axs = plt.subplots(5, 1, figsize=(14, 20))

    for i in range(1, 6):
        axs[i-1].plot(df['Time'],
                      df[f'Data{i}_device1'], label=f'Device 1 - Data{i}')
        axs[i-1].plot(df['Time'],
                      df[f'Data{i}_device2'], label=f'Device 2 - Data{i}')
        axs[i-1].set_xlabel('Time')
        axs[i-1].set_ylabel(f'Data{i}')
        axs[i-1].legend()

    plt.tight_layout()
    plt.show()


plot_subplots(merged_df)
