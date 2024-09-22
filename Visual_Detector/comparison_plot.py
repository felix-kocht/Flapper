import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

# Read the CSV file into a DataFrame
# Replace this path with the actual file path
csv_file_path = 'foil_comparison_Efficiency.csv'
df = pd.read_csv(csv_file_path, index_col=0)

# Calculate the average for each row and sort by it
df['average'] = df.mean(axis=1)
df_sorted = df.sort_values('average').drop('average', axis=1)

# Prepare data for the box plot
data_to_plot = [df_sorted.loc[row].dropna().values for row in df_sorted.index]

# Create the plot
plt.figure(figsize=(10, 6))
sns.boxplot(data=data_to_plot, palette=["lightgray"] * len(data_to_plot))

# Adding statistics to the plot (mean, std dev, min, max)
for i, row in enumerate(df_sorted.index):
    values = df_sorted.loc[row].dropna().values
    mean_val = values.mean()
    std_val = values.std()
    min_val = values.min()
    max_val = values.max()

    # Plotting additional statistics
    plt.plot(i, mean_val, 'ro', label='Mean' if i ==
             0 else "")  # Mean as red dot
    plt.plot(i, mean_val - std_val, 'bx', alpha=0.5,
             label='-1 Std Dev' if i == 0 else "")  # -1 Std Dev as blue x
    plt.plot(i, mean_val + std_val, 'bx', alpha=0.5,
             label='+1 Std Dev' if i == 0 else "")  # +1 Std Dev as blue x

# Set x-axis labels
plt.xticks(range(len(df_sorted.index)), df_sorted.index, rotation=45)

# Set labels and title
#plt.xlabel('Foil Type')
plt.ylabel('Efficiency [N/W]')
plt.title('Efficiency distribution for different foil types over multiple tests')
plt.legend()
plt.tight_layout()

# Show plot
plt.show()
