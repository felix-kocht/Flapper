import pandas as pd
import matplotlib.pyplot as plt

# Load the data
data = pd.read_csv('test_cases/propeller/summary.csv')

# Extract the columns
propeller = data['Propeller'].values
percent = data['Percent'].values
efficiency = data['Efficiency'].values
force = data['Force'].values
power = data['Power'].values
noise = data['Noise'].values

#create a vector of names as a combination of propeller and percent
name = [f'{propeller[i]}, {percent[i]}%' for i in range(len(propeller))]

# Plot the data in a bar chart
plt.figure(figsize=(10, 5))
plt.bar(name, efficiency, color='blue', alpha=0.7)
plt.xlabel('Used Propeller and Thrust Percentage')
plt.ylabel('Efficiency (N/W)')
plt.title('Efficiency of Different Propellers')
plt.show()
