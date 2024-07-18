import os

# this works now!!


def execute_io():

    if os.path.exists('Visual_Detector/IOMaster.py'):
        with open('Visual_Detector/IOMaster.py', 'r') as file:
            exec(file.read(), globals())


# Execute the script and get the local scope dictionary
io_scope = execute_io()
