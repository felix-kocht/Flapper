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
        average_efficiency = efficiency.mean()

        # Store the averages
        average_velocities.append(average_velocity)
        average_efficiencies.append(average_efficiency)
        file_labels.append(filename)

# Plotting Average Velocity
plt.figure(figsize=(10, 5))
plt.bar(file_labels, average_velocities, color='lightgray')
plt.xlabel('CSV File')
plt.ylabel('Average Velocity')
plt.title('Average Velocity for Different Files')
plt.xticks(rotation=45)
plt.tight_layout()
plt.grid(True)
plt.show()

# Plotting Average Efficiency
plt.figure(figsize=(10, 5))
plt.bar(file_labels, average_efficiencies, color='lightgray')
plt.xlabel('CSV File')
plt.ylabel('Average Efficiency (Velocity / Power)')
plt.title('Average Efficiency for Different Files')
plt.xticks(rotation=45)
plt.tight_layout()
plt.grid(True)
plt.show()
