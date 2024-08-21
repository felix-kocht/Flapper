import numpy as np
from skopt.space import Real
from skopt import gp_minimize
import os
import glob
import csv

# Usage instructions:
# in IOMaster.py change the testrun_file to 'test_instructions_rl.csv' and adjust ports there
# in result_eval.py change the output_file to 'test_cases_rl/case.csv'
# afterwards copy the logs in the terminal to the RL_output.txt file and run rl_log_interpreter.py to save the steps in a csv file
#then use plot_rl_steps.py to plot the steps

# parameters for the optimization
max_tests = 100
test_durations = 10
heave_amp = 38  # increase if safe to do so

# TODO: sharpen the ranges to get better results
# TODO define reward function nicely
parameters = {
    'frequency': [0.2, 0.8],  # TODO: increase if safe to do so
    'pitch amplitude': [20, 45],
    'pitch phase': [-1.57, 1.57],
    # 'camber amplitude': [0, 60],
    # 'camber phase': [-1.57, 1.57],
}

initial_guess = {
    'frequency': 0.3,
    'pitch amplitude': 60,
    'pitch phase': 0,
    # 'camber amplitude': 0,
    # 'camber phase': 0,
}

# state space, all ranging from 0 to 1
space = [
    Real(0.0, 1.0, name='param1'),
    Real(0.0, 1.0, name='param2'),
    Real(0.0, 1.0, name='param3')  # ,
    # Real(0.0, 1.0, name='param4'),
    # Real(0.0, 1.0, name='param5')
]

# in space range (0 to 1): x0 is  initial guess, y0 the initial reward
x0 = [0, 0, 0]  # , 0, 0]


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
    # TODO: change here if not average is needed
    avg_values = lines[stat_start + 1].strip().split(',')

    for header, value in zip(headers, avg_values):
        average_values[header.strip()] = value.strip()

    return average_values


def collect_data_from_folder(folder_path):

    for file_path in glob.glob(os.path.join(folder_path, "*.csv")):
        average_values = parse_csv(file_path)

    return average_values


def reward_function(thrust, consumption):
    if consumption == 0:
        return thrust*0.01
    # (for testing just efficiency) + 0.1*thrust
    return (thrust*1000)/consumption 

# initializing x0 on the 0-1 range


def initialize_parameters():
    i = 0
    for key in parameters.keys():
        full_range = parameters[key][1] - parameters[key][0]
        center = parameters[key][0] + 0.5 * full_range
        x0[i] = (
            initial_guess[key] - center)/full_range + 0.5
        i += 1

# we run the physical test. The parameters are given in the 0-1 range.


def run_physical_test(param1, param2, param3):  # , param4, param5):

    # converts the parameters to the real-world values
    frequency = parameters['frequency'][0] + param1 * \
        (parameters['frequency'][1] - parameters['frequency'][0])
    pitch_amp = parameters['pitch amplitude'][0] + param2 * \
        (parameters['pitch amplitude'][1] - parameters['pitch amplitude'][0])
    pitch_phase = parameters['pitch phase'][0] + param3 * \
        (parameters['pitch phase'][1] - parameters['pitch phase'][0])
    camber_amp = 0
    camber_phase = 0
   # camber_amp = parameters['camber amplitude'][0] + param4 * \
    #    (parameters['camber amplitude'][1] - parameters['camber amplitude'][0])
   # camber_phase = parameters['camber phase'][0] + param5 * \
    #   (parameters['camber phase'][1] - parameters['camber phase'][0])

    # writes instructions for this testrun to the test_instructions_rl.csv file
    data = [
        ["Frequency", "heave_amp", "pitch_amp", "camb_amp",
         "pitch_phase", "camber_phase", "turn_rate", "test_duration"],
        [frequency, heave_amp, pitch_amp, camber_amp, pitch_phase, camber_phase, 0, test_durations]]
    with open('test_instructions_rl.csv', 'w', newline='') as file:
        writer = csv.writer(file)
        writer.writerows(data)

    # calls IOMaster.py to run the testrun
    with open('Visual_Detector/IOMaster.py', 'r') as file:
        exec(file.read(), globals())

    # calls result_eval.py to evaluate the results of the testrun
    with open('Visual_Detector/result_eval.py', 'r') as file:
        exec(file.read(), globals())

    averages = collect_data_from_folder('test_cases_rl')
    Fx = float(averages['Fx'])
    Consumption = float(averages['Power_consumption'])
    print(Fx, Consumption)
    return Fx, Consumption
# Example of an objective function interfacing with the real-world setup


def objective(params):
    # Extract parameters
    param1, param2, param3 = params  # , param4, param5 = params
    print(params)
    thrust, consumption = run_physical_test(
        param1, param2, param3)  # , param4, param5)
    reward = reward_function(thrust, consumption)
    return -reward  # Minimize the negative reward to maximize the reward


# Initialize the parameters
initialize_parameters()

# Perform Bayesian Optimization
res = gp_minimize(objective, space, n_calls=max_tests, random_state=1,
                  verbose=True, n_initial_points=3, acq_func="EI")  # , y0=y0) #acq_func="PI" might converge faster, but risks getting stuck in local minima

# Output the best parameters found-
print("Best parameters: ", res.x)
print("Best reward: ", -res.fun)  # Since we minimized the negative reward
# print("details: ", res)


# IO: #TODO: modify or create version of these files that allow for easy calling and input parameters, e.g. name of test case
# TODO: or maybe just implement the naming of test cases via the csv file anyways
# writes instructions for this testrun to the test_instructions.csv file
# calls IOMaster.py to run the testrun
# calls result_eval.py to evaluate the results of the testrun
# reads the test_case csv file
