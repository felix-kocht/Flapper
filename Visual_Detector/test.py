import cv2
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns


def extract_data_from_plot(image_path, foil_type):
    # Read the image
    img = cv2.imread(image_path)
    img_gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

    # Thresholding to create a binary image
    _, thresh = cv2.threshold(img_gray, 200, 255, cv2.THRESH_BINARY_INV)

    # Find contours of the data points
    contours, _ = cv2.findContours(
        thresh, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    # Extract coordinates of the data points
    data_points = []
    for contour in contours:
        # Get the center of the contour
        M = cv2.moments(contour)
        if M['m00'] != 0:
            cX = int(M['m10'] / M['m00'])
            cY = int(M['m01'] / M['m00'])
            data_points.append((cX, cY))

    # Convert coordinates to actual values based on the plot's axes (assuming known scales)
    def pitch_amplitude_scale(x): return (
        x / img.shape[1]) * 60  # Example scale

    def fx_scale(y): return (img.shape[0] - y) / \
        img.shape[0] * 2  # Example scale
    frequency = 0.5  # Set this based on the plot or include logic to detect different frequencies

    extracted_data = {
        'Pitch Amplitude': [pitch_amplitude_scale(x) for x, y in data_points],
        'Fx': [fx_scale(y) for x, y in data_points],
        'Frequency': [frequency] * len(data_points),
        'Foil Type': [foil_type] * len(data_points)
    }

    return extracted_data


# Paths to the images and their corresponding foil types
image_paths = [
    ('images/Cambered_balloon_Fx.png', 'Cambered_balloon'),
    ('images/Wide_off_Fx.png', 'Wide_off'),
    ('images/Stiff_foils_Fx.png', 'Stiff_foils'),
    ('images/Cambered_balloon_off_Fx.png', 'Cambered_balloon_off'),
    ('images/Cambered_foils_Fx.png', 'Cambered_foils')
]

# Extract data from all plots
all_data = {
    'Pitch Amplitude': [],
    'Fx': [],
    'Frequency': [],
    'Foil Type': []
}

for image_path, foil_type in image_paths:
    extracted_data = extract_data_from_plot(image_path, foil_type)
    for key in all_data:
        all_data[key].extend(extracted_data[key])

# Create a dataframe from the extracted data
df = pd.DataFrame(all_data)

# Plot the combined data
plt.figure(figsize=(12, 8))
sns.scatterplot(data=df, x='Pitch Amplitude', y='Fx',
                hue='Foil Type', palette='bright', style='Frequency', s=100)
plt.title('Comparison of Foils by Achieved Thrust (Fx)')
plt.legend(bbox_to_anchor=(1.05, 1), loc=2, borderaxespad=0.)
plt.show()
