import numpy as np
import time #just for testing
from skopt.space import Real
from skopt import gp_minimize

parameters = {
    'frequency': [0.2, 0.6], #TODO: increase if safe to do so
    'pitch amplitude': [10, 80],
    'pitch phase': [-1.57, 1.57],
    'camber amplitude': [0, 60],
    'camber phase': [-1.57, 1.57],
}

initial_guess = {
    'frequency': 0.5,
    'pitch amplitude': 60,
    'pitch phase': 0,
    'camber amplitude': 50,
    'camber phase': 0,
}

max_tests = 6
test_durations = 5

# state space, all ranging from 0 to 1
space = [
    Real(0.0, 1.0, name='param1'),
    Real(0.0, 1.0, name='param2'),
    Real(0.0, 1.0, name='param3'),
    Real(0.0, 1.0, name='param4'),
    Real(0.0, 1.0, name='param5')
]

#in space range (0 to 1): x0 is  initial guess, y0 the initial reward
x0 = [0,0,0,0,0]
#y0 = -16 #TODO: fill in if available

#TODO: find best reward function
def reward_function(thrust,consumption):
    if consumption == 0:
        return 0.1*thrust
    return thrust/consumption + 0.1*thrust

#initializing x0 on the 0-1 range
def initialize_parameters():
    i = 0
    for key in parameters.keys():
        full_range = parameters[key][1] - parameters[key][0]
        center = parameters[key][0] + 0.5 * full_range
        x0[i] = (
            initial_guess[key] - center)/full_range + 0.5
        i += 1

# Placeholder functions for demonstration
def run_physical_test(param1, param2, param3, param4, param5):
    #TODO
    # calls IOMaster.py to run the testrun
    # calls result_eval.py to evaluate the results of the testrun
    #delay for test duration
    time.sleep(test_durations/2)
    return 50, 5

# Example of an objective function interfacing with the real-world setup
def objective(params):
    # Extract parameters
    param1, param2, param3, param4, param5 = params
    print(params)
    thrust, consumption = run_physical_test(param1, param2, param3, param4, param5)
    reward = reward_function(thrust, consumption)
    return -reward  # Minimize the negative reward to maximize the reward

# Initialize the parameters
initialize_parameters()
print(x0)

# Perform Bayesian Optimization
res = gp_minimize(objective, space, n_calls=max_tests, random_state=0, verbose = True, n_initial_points = 3, x0 = x0)#, y0=y0)

# Output the best parameters found
print("Best parameters: ", res.x)
print("Best reward: ", -res.fun)  # Since we minimized the negative reward
#print("details: ", res)


# IO: #TODO: modify or create version of these files that allow for easy calling and input parameters, e.g. name of test case
#TODO: or maybe just implement the naming of test cases via the csv file anyways
# writes instructions for this testrun to the test_instructions.csv file
# calls IOMaster.py to run the testrun
# calls result_eval.py to evaluate the results of the testrun
# reads the test_case csv file

