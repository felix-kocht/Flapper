import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Load the data
data = pd.read_csv('test_cases/propeller/summary.csv')

# Extract the columns
propeller = data['Propeller'].values
percent = data['Percent'].values
efficiency = data['Efficiency'].values
force = data['Force'].values

# Create a vector of names as a combination of propeller and percent
name = [f'{propeller[i]}, {percent[i]}%' for i in range(len(propeller))]

# Create positions for the bars
x = np.arange(len(name))
width = 0.35  # Width of the bars

# Plot the force on the left y-axis
fig, ax1 = plt.subplots(figsize=(9, 5))
ax1.bar(x - width/2, force, width=width,
        color='red', alpha=0.7, label='Force (N)')
ax1.set_xlabel('Used Propeller and Thrust Percentage')
ax1.set_ylabel('Force (N)', color='red')
ax1.tick_params(axis='y', labelcolor='red')
ax1.set_xticks(x)
ax1.set_xticklabels(name, rotation=45, ha='right')

# Create a second y-axis for efficiency
ax2 = ax1.twinx()
ax2.bar(x + width/2, efficiency, width=width,
        color='blue', alpha=0.7, label='Efficiency (N/W)')
ax2.set_ylabel('Efficiency (N/W)', color='blue')
ax2.tick_params(axis='y', labelcolor='blue')

# Add legend for both y-axes
fig.legend(loc='upper left', bbox_to_anchor=(0.1, 0.9))
plt.title('Force and Efficiency of Different Propellers')
plt.tight_layout()  # Adjust layout to prevent clipping
plt.show()
