import matplotlib.pyplot as plt

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
