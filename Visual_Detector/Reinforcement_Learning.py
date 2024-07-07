import numpy as np
from skopt.space import Real
from skopt import gp_minimize

parameters = {
    'frequency': [0.2, 1],
    'pitch amplitude': [10, 80],
    'pitch phase': [-1.57, 1.57],
    'camber amplitude': [0, 60],
    'camber phase': [-1.57, 1.57],
}

initial_guess = {
    'frequency': 0.6,
    'pitch amplitude': 45,
    'pitch phase': 0,
    'camber amplitude': 30,
    'camber phase': 0,
}

max_tests = 10
test_durations = 5

# state space, all ranging from 0 to 1
normalized_parameters = [0,0,0,0,0]

#TODO: find best reward function
def reward_function(thrust,consumption):
    if consumption == 0:
        return 0.1*thrust
    return thrust/consumption + 0.1*thrust

#initializing on the 0-1 range
def initialize_parameters():
    i = 0
    for key in parameters.keys():
        full_range = parameters[key][1] - parameters[key][0]
        center = parameters[key][0] + 0.5 * full_range
        normalized_parameters[i] = (
            initial_guess[key] - center)/full_range + 0.5
        i += 1


""" 
# Define the search space for the 5 parameters
space = [
    Real(0.0, 1.0, name='param1'),
    Real(0.0, 1.0, name='param2'),
    Real(0.0, 1.0, name='param3'),
    Real(0.0, 1.0, name='param4'),
    Real(0.0, 1.0, name='param5')
]

# Example of an objective function interfacing with the real-world setup


def objective(params):
    # Extract parameters
    param1, param2, param3, param4, param5 = params

    # Interface with the real-world system
    # Replace the following lines with your actual test code
    # Function to perform test and get thrust
    thrust = run_physical_test(param1, param2, param3, param4, param5)
    consumption = measure_energy_consumption(
        param1, param2, param3, param4, param5)  # Function to measure energy

    if consumption == 0:
        return -0.1 * thrust  # Use a small negative reward to avoid division by zero

    reward = (thrust / consumption) + (0.1 * thrust)
    return -reward  # Minimize the negative reward to maximize the reward


# Perform Bayesian Optimization
res = gp_minimize(objective, space, n_calls=50, random_state=0)

# Output the best parameters found
print("Best parameters: ", res.x)
print("Best reward: ", -res.fun)  # Since we minimized the negative reward

# Placeholder functions for demonstration


def run_physical_test(param1, param2, param3, param4, param5):
    # Implement the real-world test logic here and return the measured thrust
    pass


def measure_energy_consumption(param1, param2, param3, param4, param5):
    # Implement the real-world test logic here and return the measured energy consumption
    pass
 """

# IO: #TODO: modify or create version of these files that allow for easy calling and input parameters, e.g. name of test case
#TODO: or maybe just implement the naming of test cases via the csv file anyways
# writes instructions for this testrun to the test_instructions.csv file
# calls IOMaster.py to run the testrun
# calls result_eval.py to evaluate the results of the testrun
# reads the test_case csv file

# RL:
# method to decide on next test case (gradient descent with the best available gradient?) --> weigh between exploration and exploitation, use some tested method for this
# then i would have to randomly sample some test cases to get the gradient: either completely random, or assume inital guess is the best and sample around it
#
