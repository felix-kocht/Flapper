from apriltag import Detector
import cv2
import math
import SerialManager as SerialManager

counter = 0
# calibration = [][]  # for each 5 tags, x offset, y offset, angle offset


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


def detect_tags_in_frame(frame):
    # Create a detector
    detector = Detector()

    # Convert frame to grayscale
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    # Detect AprilTags in the grayscale frame
    detections = detector.detect(gray)

    # Extract tag information
    tags = []
    # Compute the average angle
    angles = []
    for detection in detections:
        tag_id = detection.tag_id
        ll, lr, ur, ul = detection.corners
        angle1 = math.atan2(lr[1] - ll[1], lr[0] - ll[0]) * 180 / math.pi
        angle2 = math.atan2(- ul[1] + ur[1], - ul[0] + ur[0]) * 180 / math.pi
        average_angle = (angle1 + angle2) / 2
        center = detection.center
        # starting lower left, lower right, upper right, upper left
        tags.append({"id": tag_id, "center": center,
                    "average_angle": average_angle})

    return tags


def process_and_print_tags(tags):
    if not tags:
        print("No AprilTags detected.")
    else:
        print("Detected AprilTags:")
        for tag in tags:
            print(
                f"ID: {tag['id']}, Center: {tag['center']} , Angle: {tag['average_angle']} degrees")
            # also save the data to a csv file for tag 1
            if tag['id'] == 1:
               # SerialManager.outgoing(f"{tag['center'][0]},{tag['center'][1]},{tag['average_angle']}")
                with open('tag1_data.csv', 'a') as file:
                    file.write(
                        f"{tag['center'][0]},{tag['center'][1]},{tag['average_angle']}\n")


def calculate_perpendicular_distance(tag3, tag4, tag5):
    x3, y3 = tag3['center']
    x4, y4 = tag4['center']
    x5, y5 = tag5['center']

    # Coefficients for the line equation Ax + By + C = 0
    A = y5 - y3
    B = -(x5 - x3)
    C = x5 * y3 - y5 * x3

    # Calculating the perpendicular distance from tag 4 to the line connecting tags 3 and 5
    numerator = A * x4 + B * y4 + C
    denominator = (A**2 + B**2)**0.5

    c1 = numerator / denominator
    return c1


def get_state(tags):
    global counter
    counter += 1
    # error handling for when tags missing
    if len(tags) < 5:
        return None
    state = []  # h,p1,c1,
    # h is height difference between tags 1 and 2

    h = tags[1]['center'][1] - tags[0]['center'][1]
    state.append(h)
    # p1 is the angle of tag 3 relative to tag 1
    p1 = tags[2]['average_angle'] - tags[0]['average_angle']
    state.append(p1)
    # c1 is the perpendicular distance between tag 4 and the line connecting tags 3 and 5
    c1 = calculate_perpendicular_distance(tags[2], tags[3], tags[4])
    state.append(c1)
    # write counter and state to csv
    with open('state_data5.csv', 'a') as file:
        file.write(f"{counter},{state[0]},{state[1]},{state[2]}\n")

    return state


def main():
    for frame in camera_stream():
        tags = detect_tags_in_frame(frame)
        # process_and_print_tags(tags)
        state = get_state(tags)
        if state is not None:
            heave = state[0]  # Access the first element of state
            SerialManager.manage_data(heave)
            print(heave)
        # Display frame for debugging (optional)
        cv2.imshow('Frame', frame)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
    cv2.destroyAllWindows()


if __name__ == "__main__":
    main()
