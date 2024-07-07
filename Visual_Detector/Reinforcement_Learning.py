# parameters
parameters = {
    'frequency': [0.2, 1],
    'pitch amplitude': [10, 80],
    'pitch phase': [-1.57, 1.57],
    'camber amplitude': [0, 60],
    'camber phase': [-1.57, 1.57],
}

# initial guess
initial_guess = {
    'frequency': 0.6,
    'pitch amplitude': 45,
    'pitch phase': 0,
    'camber amplitude': 30,
    'camber phase': 0,
}

# state space (all initialized in the middle of the range(0-1))
normalized_parameters = [0,0,0,0,0]

#TODO: find best reward function
def reward_function(thrust,consumption):
    if consumption == 0:
        return 0.1*thrust
    return thrust/consumption + 0.1*thrust

def initialize_parameters():
    i = 0
    for key in parameters.keys():
        full_range = parameters[key][1] - parameters[key][0]
        center = parameters[key][0] + 0.5 * full_range
        normalized_parameters[i] = (initial_guess[key] - center)/full_range + 0.5
        i += 1


# number of tests
# step sizes (?) #TODO: think about how to implement this in all dimensions (variable step size?)

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
