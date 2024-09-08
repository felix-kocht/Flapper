import os
import pandas as pd
import matplotlib.pyplot as plt

# Directory containing the CSV files
directory = 'GPS_data/valid_stuff'  # Replace with the path to your directory

# Initialize lists to store data for plotting
average_velocities = []
average_efficiencies = []
file_labels = []

# Read each CSV file in the directory
for filename in os.listdir(directory):
    if filename.endswith('.csv'):
        filepath = os.path.join(directory, filename)

        # Read the CSV file into a DataFrame
        df = pd.read_csv(filepath)

        # Extract columns (assuming they are named 'velocity', 'current', 'voltage')
        velocity = df['velocity']
        current = df['current']
        voltage = df['voltage']

        # Compute power (P = V * I) and efficiency (efficiency = velocity / power)
        power = voltage * current
        efficiency = velocity / power

        # Calculate the average values
        average_velocity = velocity.mean()
        average_efficiency = -efficiency.mean()

        # Store the averages
        average_velocities.append(average_velocity)
        average_efficiencies.append(average_efficiency)
        # Extract the label from the filename
        file_labels.append(filename[0:-4])

# Sort data by average velocities before plotting
sorted_velocities_data = sorted(
    zip(average_velocities, file_labels), key=lambda x: x[0])
sorted_velocities, sorted_velocity_labels = zip(*sorted_velocities_data)

# Plotting Average Velocity (sorted)
plt.figure(figsize=(10, 5))
plt.bar(sorted_velocity_labels, sorted_velocities, color='lightgray')
plt.xlabel('Test Case')
plt.ylabel('Average Velocity in m/s')
plt.title('Average Velocity for Different Tests')
plt.xticks(rotation=45)
plt.tight_layout()
plt.grid(True)
plt.show()

# Sort data by average efficiencies before plotting
sorted_efficiencies_data = sorted(
    zip(average_efficiencies, file_labels), key=lambda x: x[0])
sorted_efficiencies, sorted_efficiency_labels = zip(*sorted_efficiencies_data)

# Plotting Average Efficiency (sorted)
plt.figure(figsize=(10, 5))
plt.bar(sorted_efficiency_labels, sorted_efficiencies, color='lightgray')
plt.xlabel('Test Case')
plt.ylabel('Average Efficiency in m/Ws (Velocity / Power)')
plt.title('Average Efficiency for Different Tests')
plt.xticks(rotation=45)
plt.tight_layout()
plt.grid(True)
plt.show()
