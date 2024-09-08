import csv
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

input_file = 'RL_output_efficiency.csv'
# Read the data from the CSV file

iterations = []
frequencies = []
pitch_phases = []
function_values = []

with open(input_file, 'r') as csvfile:
    csvreader = csv.reader(csvfile)
    next(csvreader)  # Skip the header row
    for row in csvreader:
        iterations.append(int(row[0]))
        # Assuming Pitch Amplitude is the 4nd column
        # frequencies.append(float(row[3]))
        # Assuming Frequency is the 2nd column
        frequencies.append(float(row[1]))
        # Assuming Pitch Phase is the 6th column
        pitch_phases.append(float(row[5]))
        # Function Value is the 9th column
        function_values.append(-float(row[8]))

# Create a 3D plot
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

# Scatter plot with color coding by iteration number
sc = ax.scatter(frequencies, pitch_phases, function_values,
                c=iterations, cmap='cool')

# Connect the dots with lines according to adjacent iterations
# ax.plot(frequencies, pitch_phases, function_values, color='gray', alpha=0.5)

# Add color bar
cbar = plt.colorbar(sc, ax=ax)
cbar.set_label('Iteration Number')

# Set labels
ax.set_xlabel('Frequency')
ax.set_ylabel('Pitch Phase')
ax.set_zlabel('Function Value')

# Set the title
ax.set_title('3D Plot of Function Value vs. Frequency and Pitch Phase')

# Show the plot
plt.show()
