## HOW TO USE

1. Use IOMaster to run tests
    - fill desired parameters into the test_instructions.csv
    - adjust IOMaster.py variables 
    - let the program run until all test cases are done
2. Run result_eval.py to create one csv testreport for each test case
3. Run plot.py for any values you would like to plot

# IOMaster: 

- can read csv file with instructions
- sends parameters to arduino and notes the response
- can communicate with multiple devices

# result_eval:

- for each file_pair it does a few things:
    - merges the 2 output files from the IOMaster
    - adds a few columns of calculated values
    - statistical evaluations

# plot:

- plots the values, the user decides to plot, with the hardcoded (but changeable) scale factors

# Detection

- not inside the above workflow, reads apriltags and estimates the pose of the flaps therefrom