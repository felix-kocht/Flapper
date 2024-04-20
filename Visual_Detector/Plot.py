# plotting functions from cv file
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd


def plot_data():
    # Load the data from the CSV file
    data = pd.read_csv('state_data5.csv', header=None)
    data.columns = ['counter', 'h', 'p1', 'c1']
    # Plot the data
    fig, axs = plt.subplots(3, 1, figsize=(10, 10))
    axs[0].plot(data['counter'], data['h'], label='Height difference (h)')
    axs[0].set_xlabel('Counter')
    axs[0].set_ylabel('Height difference (h)')
    axs[0].legend()
    axs[1].plot(data['counter'], data['p1'], label='Angle difference (p1)')
    axs[1].set_xlabel('Counter')
    axs[1].set_ylabel('Angle difference (p1)')
    axs[1].legend()
    axs[2].plot(data['counter'], data['c1'], label='Height difference (c1)')
    axs[2].set_xlabel('Counter')
    axs[2].set_ylabel('Height difference (c1)')
    axs[2].legend()
    plt.tight_layout()
    plt.show()


def plot_data_points():
    # Load the data from the CSV file

    data = pd.read_csv('state_data4.csv', header=None)
    data.columns = ['counter', 'h', 'p1', 'c1']

    # Plot the data
    fig, axs = plt.subplots(3, 1, figsize=(10, 10))

    # Plot with circle markers
    axs[0].plot(data['counter'], data['h'], 'o', label='Height difference (h)')
    axs[0].set_xlabel('Counter')
    axs[0].set_ylabel('Height difference (h)')
    axs[0].legend()

    axs[1].plot(data['counter'], data['p1'], 'o',
                label='Angle difference (p1)')
    axs[1].set_xlabel('Counter')
    axs[1].set_ylabel('Angle difference (p1)')
    axs[1].legend()

    axs[2].plot(data['counter'], data['c1'], 'o',
                label='Height difference (c1)')
    axs[2].set_xlabel('Counter')
    axs[2].set_ylabel('Height difference (c1)')
    axs[2].legend()

    plt.tight_layout()
    plt.show()


def plot_csv(filename):
    """
    Plot data from a CSV file.

    Args:
    filename (str): Path to the CSV file.
    """
    # Read CSV file
    data = pd.read_csv(filename)

    # Determine the number of columns
    num_columns = len(data.columns)

    # Check if the file has a 'Time' column
    if 'Time' in data.columns:
        # Plot multiple variables over time
        time = data['Time']
        for col in data.columns:
            if col != 'Time':
                plt.plot(time, data[col], label=col)
        plt.xlabel('Time')
    else:
        # Plot a single column of variables
        plt.plot(data.index, data.iloc[:, 0])

    # Add labels and legend
    plt.ylabel('Value')
    plt.title('Data Plot')
    plt.legend()


# Example usage
#plot_csv('unfiltered.csv')
#plt.figure()  # Create a new figure
#plot_csv('filtered.csv')
#plt.show()  # Show all plots


plot_data()
