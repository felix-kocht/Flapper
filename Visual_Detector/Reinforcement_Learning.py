# parameters
# reward function
# state space (parameters and their ranges, somehow normalized)
# initial guess
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
