import csv
import numpy as np
import matplotlib.pyplot as plt

input_file = 'RL_output_efficiency.csv'

# Read the data from the CSV file
iterations = []
frequencies = []
function_values = []

with open(input_file, 'r') as csvfile:
    csvreader = csv.reader(csvfile)
    next(csvreader)  # Skip the header row
    for row in csvreader:
        iterations.append(int(row[0]))
        # Assuming Pitch Phase is the 2nd column
        #frequencies.append(float(row[1]))
        # Assuming Pitch Phase is the 4nd column
        #frequencies.append(float(row[3]))
        # Assuming Pitch Phase is the 6th column
        frequencies.append(float(row[5]))
        # Function Value is the 9th column (negated as per original code)
        function_values.append(-float(row[8]))

# Convert lists to numpy arrays for mathematical operations
frequencies = np.array(frequencies)
function_values = np.array(function_values)

# Create a 2D scatter plot
plt.figure('Efficiency vs. Pitch Phase', figsize=(5, 4))
sc = plt.scatter(frequencies, function_values,
                 c=iterations, cmap='cool', edgecolor='k')

# Add color bar to represent iteration number
cbar = plt.colorbar(sc)
cbar.set_label('Iteration Number')

# Calculate the trend line using linear regression
slope, intercept = np.polyfit(frequencies, function_values, 1)
trend_line = slope * frequencies + intercept

# Plot the trend line
plt.plot(frequencies, trend_line, color='red', linestyle='--',label=f'Trend Line (y = {slope:.2f}x + {intercept:.2f})')

# Set labels
plt.xlabel('Pitch Phase in radians')
plt.ylabel('Efficiency in N/W')
plt.title('Efficiency vs. Pitch Phase')
plt.legend()

# Show the plot
plt.tight_layout()
plt.show()
