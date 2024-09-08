import os
import glob


def parse_csv(file_path):
    parameters = {}
    average_values = {}

    with open(file_path, 'r') as file:
        lines = file.readlines()

    # Extract Used Parameters
    for line in lines:
        if line.startswith("Used Parameters:"):
            param_start = lines.index(line) + 1
        elif line.startswith("Statistical Evaluation:"):
            stat_start = lines.index(line) + 1

    for line in lines[param_start:stat_start-1]:
        if line.strip() and ':' in line:
            key, value = line.strip().split(':', 1)
            parameters[key.strip()] = value.strip()

    # Extract average values from Statistical Evaluation
    headers = lines[stat_start].strip().split(',')
    # TODO: change here if not average is needed (1 is average, 4 is std dev)
    avg_values = lines[stat_start + 1].strip().split(',')

    for header, value in zip(headers, avg_values):
        average_values[header.strip()] = value.strip()

    return parameters, average_values


def collect_data_from_folder(folder_path):
    all_parameters = []
    all_average_values = []

    for file_path in glob.glob(os.path.join(folder_path, "*.csv")):
        parameters, average_values = parse_csv(file_path)
        all_parameters.append(parameters)
        all_average_values.append(average_values)

    return all_parameters, all_average_values


def create_vector(all_average_values, header):
    vector = []
    for average_values in all_average_values:
        vector.append(average_values.get(header))
    return vector

# function to call collect_data_from_folder and create_vector for every folder in a given directory and create a csv file with the folder names as headers and the vectors as columns


def create_csv(directory, header):
    for folder in os.listdir(directory):
        # if folder name is .DS_Store, skip it
        if folder == '.DS_Store':
            continue
        all_parameters, all_average_values = collect_data_from_folder(
            os.path.join(directory, folder))
        vector = create_vector(all_average_values, header)
        # create csv file with folder names as headers and the vectors as columns
        with open(f'foil_comparison_{header}.csv', 'a') as file:
            file.write(folder)
            for value in vector:
                file.write(',' + value)
            # skip to next row
            file.write('\n')

        print(f'column created for {folder}')


create_csv('test_cases', 'Efficiency')


""" all_parameters, all_average_values = collect_data_from_folder('test_cases/wide_stiff')
Fx_vector = create_vector(all_average_values, 'Fx')
Efficiency_vector = create_vector(all_average_values, 'Efficiency')
print(Fx_vector)
print(Efficiency_vector) """
