# dashboard.py
import matplotlib.pyplot as plt
import pandas as pd

# To use, call init_plot() to initialize the plot, update_plot() to update the plot with new data, and end_of_data() to keep the plot window open after the data ends.
# make sure to save the data in data.csv or correct the filename here


fig, ax = plt.subplots()


def init_plot():
    """Initializes the plot window and settings."""
    plt.xlabel('Time')
    plt.ylabel('Sensor Value')
    plt.title('Real-Time Sensor Data')
    plt.show(block=False)


def update_plot():
    """Updates the plot with new data from the CSV file, displaying only the last 50 points."""
    try:
        # Read the CSV file and focus on the last 50 data points
        df = pd.read_csv('data.csv')
        df = df.tail(50)  # Select only the last 50 rows
        ax.clear()
        ax.plot(df['Time'], df['Value'], marker='o', linestyle='-')
        plt.draw()
        plt.pause(0.1)  # Force matplotlib to update the plot and wait a bit
    except FileNotFoundError:
        print("File not found. Check if 'data.csv' exists and has data.")
    except Exception as e:
        print(f"An error occurred: {e}")


def end_of_data():
    plt.show()
