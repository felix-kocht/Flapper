import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

# Load the data
#data = pd.read_csv('GPS_data/valid_stuff/0.6_45.csv')
path = 'GPS_data/0_8_25.csv'
data = pd.read_csv(path)

# Extract all columns
latitude = data['latitude'].values
longitude = data['longitude'].values
velocity = data['velocity'].values
time = data['time'].values
voltage = data['voltage'].values
current = data['current'].values
power = -voltage*current

# Compute additional data
# Differences in coordinates (assuming small distances, curvature neglected)
dlat = np.diff(latitude)
dlon = np.diff(longitude)

# Approximate distance calculation (1 degree of latitude ≈ 111.32 km, 1 degree of longitude ≈ 111.32 km * cos(latitude))
lat_factor = 111320  # meters per degree latitude
# meters per degree longitude at the given latitude
lon_factor = 75207 # = 111320 * np.cos(np.radians(latitude[:-1]))

distance = np.sqrt((dlat * lat_factor)**2 + (dlon * lon_factor)**2)

# Compute velocity
time_diff = np.diff(time)
computed_velocity = distance / time_diff

# Plot the trajectory from the GPS data including a pink point at the starting point
plt.figure(figsize=(10, 5))
plt.plot(longitude, latitude)
plt.plot(longitude[0], latitude[0], 'o', color='purple')
#adding a midway point in the color orange
plt.plot(longitude[int(len(longitude)/2)], latitude[int(len(latitude)/2)], 'o', color='red')
plt.plot(longitude[-1], latitude[-1], 'o', color='orange')
plt.xlabel('Longitude')
plt.ylabel('Latitude')
plt.title('Trajectory from purple via red to orange')
plt.gca().ticklabel_format(useOffset=False)  # Disable scientific notation
plt.show()

# Plot the velocity and computed velocity into a second plot
plt.figure(figsize=(10, 5))
plt.plot(time[:-1], velocity[:-1], label='Velocity')
plt.plot(time[1:], computed_velocity,
         label='Computed velocity')
#plt.plot(time,power,label='Power_consumption')
plt.xlabel('Time')
plt.ylabel('Velocity (m/s)')
plt.title(path)
plt.legend()
plt.show()
