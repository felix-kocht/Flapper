from datetime import datetime
import os
from apriltag import Detector
import cv2
import math
import likely_useless.Plot as Plot
# import SerialManager as SerialManager #uncomment this and the SerialManager line in main

counter = 0
scale = 1  # represents how many mm is one pixel is changed at at first run of the program to the correct value


'''HOW TO USE
1. Apply the April tags the following way:
    - Tag 1: reference point, not moving (ideally close to the other tags and at the same distance from the camera)
    - Tag 2: on the nose of the flap
    - Tag 3: on the center of the flap
    - Tag 4: on the rear of the flap
2. Calibrate the distance to Points of interests (poi) in the calibration array
    - 5 poi: x&y coordinates of: reference point, nose of flap, start of middle flap, end of middle flap, rear of flap, in mm
3. Run the program
'''
# TODO change everything from pixels to mm
# for each point of interest: x,y coordinates relative to center of its tag in pixels, using a coordinate system attached to the tag
calibration = [[0, 0], [-39.48, -5], [-11, 0], [12, 0], [44.2, 5]]  # in mm
tag_size = 10.15  # size of the tag in mm (side length)


# renames any old state_data.csv file so we can reuse the name
def setup_csv():
    # Check if the file exists
    filename = 'state_data.csv'
    if os.path.exists(filename):
        # Rename the existing file with a timestamp
        new_filename = f"state_{datetime.now().strftime('%Y%m%d_%H%M%S')}.csv"
        os.rename(filename, new_filename)
    # Create a new file and write the header
    # with open(filename, 'w') as file:
        # file.write("Counter,Heave amplitude,Angle, Camber\n")


# rotate camera, or change camera index here if needed
def camera_stream(camera_index=0):
    # Open the camera
    cap = cv2.VideoCapture(camera_index)
    if not cap.isOpened():
        print("Error: Could not open camera.")
        return  # Exit if the camera cannot be opened

    try:
        while True:
            # Capture frame-by-frame, rotate by 90 degrees
            ret, frame = cap.read()
            frame = cv2.rotate(frame, cv2.ROTATE_90_COUNTERCLOCKWISE)
            if not ret:
                print("Failed to capture image from camera.")
                break
            yield frame
    finally:
        # When everything is done, release the capture
        cap.release()


# detects tags and orders them by tag id in a list with the parameters of each: id, center, average_angle
# on first run, it also calculates the scale of the tags
def detect_tags_in_frame(frame):
    global counter
    global scale
    # Create a detector
    detector = Detector()

    # Convert frame to grayscale
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    # Detect AprilTags in the grayscale frame
    detections = detector.detect(gray)

    # Extract tag information
    tags = []
    scales = []

    for detection in detections:
        tag_id = detection.tag_id
        ll, lr, ur, ul = detection.corners
        if counter == 1:  # set the scale on first iteration #TODO maybe exclude the reference tag?
            average_side_length = (math.dist(
                ll, lr) + math.dist(lr, ur) + math.dist(ur, ul) + math.dist(ul, ll)) / 4
            scales.append(average_side_length)

        angle1 = math.atan2(lr[1] - ll[1], lr[0] - ll[0]) * 180 / math.pi
        angle2 = math.atan2(- ul[1] + ur[1], - ul[0] + ur[0]) * 180 / math.pi
        average_angle = (angle1 + angle2) / 2
        center = detection.center
        # starting lower left, lower right, upper right, upper left
        tags.append({"id": tag_id, "center": center,
                    "average_angle": average_angle})

    if counter == 1:
        average_scale = sum(scales) / len(scales)
        scale = tag_size / average_scale

    return tags


# Calculating the perpendicular distance from tag 4 to the line connecting tags 3 and 5 (relevant for camber calculation)
def calculate_perpendicular_distance(tag3, tag4, tag5):
    x3, y3 = tag3[0:2]
    x4, y4 = tag4[0:2]
    x5, y5 = tag5[0:2]

    # Coefficients for the line equation Ax + By + C = 0
    A = y5 - y3
    B = -(x5 - x3)
    C = x5 * y3 - y5 * x3

    # Calculating the perpendicular distance from tag 4 to the line connecting tags 3 and 5
    numerator = A * x4 + B * y4 + C
    denominator = (A**2 + B**2)**0.5

    c1 = numerator / denominator
    return c1


def convert_to_mm(tags):  # multiplies the pixel values with the scale to get the mm values
    for tag in tags:
        tag['center'] = [tag['center'][0]*scale, tag['center'][1]*scale]
    return tags


def rotate_vector(x, y, theta):
    # Convert angle from degrees to radians
    radians = math.radians(theta)
    # Rotate vector
    x_rot = x * math.cos(radians) - y * math.sin(radians)
    y_rot = x * math.sin(radians) + y * math.cos(radians)
    return (x_rot, y_rot)
# extracts the points of interest from the tags


def get_poi(tags):
    global calibration
    if len(tags) < 4:
        return None
    poi = [None]*5
    # elementwise addition of the calibration values to the tag centers
    poi[0] = tuple(map(sum, zip(tags[0]['center'], rotate_vector(
        calibration[0][0], calibration[0][1], tags[0]['average_angle']))))  # reference point
    poi[1] = tuple(map(sum, zip(tags[1]['center'], rotate_vector(
        calibration[1][0], calibration[1][1], tags[1]['average_angle']))))  # nose of flap
    poi[2] = tuple(map(sum, zip(tags[2]['center'], rotate_vector(
        calibration[2][0], calibration[2][1], tags[2]['average_angle']))))  # start of middle flap
    poi[3] = tuple(map(sum, zip(tags[2]['center'], rotate_vector(
        calibration[3][0], calibration[3][1], tags[2]['average_angle']))))  # end of middle flap
    poi[4] = tuple(map(sum, zip(tags[3]['center'], rotate_vector(
        calibration[4][0], calibration[4][1], tags[3]['average_angle']))))  # rear of flap
    return poi


# poi[5][2] (points of interest)
def get_state_new(poi):
    if poi is None:
        return None
    # heave
    # TODO substitute with correct value (on range of 0-1)
    pivot_distance_from_nose = 0.3
    pivot_point_y = pivot_distance_from_nose * \
        poi[1][1] + (1-pivot_distance_from_nose)*poi[2][1]
    # add calibration offset and scale etc. here
    heave = poi[0][1] - pivot_point_y

    # angle
    # add calibration offset and scale etc. here
    angle = math.atan2(poi[1][1]-poi[4][1], poi[1][0]-poi[4][0])

    # camber
    # TODO test if this gives accurate values
    cord_length = math.sqrt((poi[1][0]-poi[4][0])
                            ** 2 + (poi[1][1]-poi[4][1])**2)
    # add calibration offset and scale etc. here
    camber1 = calculate_perpendicular_distance(poi[1], poi[2], poi[4])
    # add calibration offset and scale etc. here
    camber2 = calculate_perpendicular_distance(poi[1], poi[3], poi[4])
    camber = max(camber1, camber2)/cord_length

    return heave, angle, camber


# adjust here to scale the data to the desired range / dimension (needed by the arduino controller)
def preprocessing(data):
    # scale the incoming data from 0-700 to 0-1
    data = (data) / 600
    # scale data from 0-1 to 1000-2000
    data = data * 1000 + 1000
    return data


def main():
    global counter
    global scale
    setup_csv()
    for frame in camera_stream():
        counter += 1
        tags = detect_tags_in_frame(frame)
        # TODO flatten tags
        tags = convert_to_mm(tags)
        poi = get_poi(tags)
        print(poi)
        state = get_state_new(poi)
        if state is not None:
            heave, angle, camber = state
            with open('state_data.csv', 'a') as file:
                file.write(f"{counter},{heave},{angle},{camber}\n")
            # heave = preprocessing(heave) #will be also unnessecary if calibration is accurate
            # SerialManager.manage_data(heave)
            # print(heave)
            # Draw circles around the points of interest for debugging
            cv2.circle(frame, (int(poi[0][0]/scale),
                               int(poi[0][1]/scale)), 20, (0, 255, 0), -1)
            cv2.circle(frame, (int(poi[1][0]/scale),
                               int(poi[1][1]/scale)), 20, (0, 0, 255), -1)
            cv2.circle(frame, (int(poi[2][0]/scale),
                               int(poi[2][1]/scale)), 20, (0, 0, 255), -1)
            cv2.circle(frame, (int(poi[3][0]/scale),
                               int(poi[3][1]/scale)), 20, (0, 0, 255), -1)
            cv2.circle(frame, (int(poi[4][0]/scale),
                               int(poi[4][1]/scale)), 20, (0, 0, 255), -1)
        # Display frame for debugging (optional)
        cv2.imshow('Frame', frame)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
    cv2.destroyAllWindows()
    # plot automatically after the program ends
    Plot.plot_data('state_data.csv')


if __name__ == "__main__":
    main()
