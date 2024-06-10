# testdata.py
import pandas as pd
import numpy as np
from datetime import datetime, timedelta
import time
import Visual_Detector.likely_useless.dashboard as dashboard  # Import the dashboard module we defined


def generate_data(num_samples=100, filename='data.csv'):
    start_time = datetime.now()
    sensor_values = np.random.random(
        size=num_samples) * 100  # Simulate sensor values

    # Initialize the plot
    dashboard.init_plot()

    for i in range(num_samples):
        df = pd.DataFrame({
            'Time': [i],
            'Value': [sensor_values[i]]
        })
        mode = 'w' if i == 0 else 'a'
        header = True if i == 0 else False
        df.to_csv(filename, mode=mode, header=header, index=False)
        print(f"Appended data for {i} to CSV.")
        # Sleep for a bit to simulate time delay between data updates
        time.sleep(0.01)

        # Update the plot
        dashboard.update_plot()
    dashboard.end_of_data()


if __name__ == "__main__":
    generate_data()
