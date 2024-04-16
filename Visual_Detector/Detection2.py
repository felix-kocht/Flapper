from apriltag import Detector
import cv2
import math
import Visual_Detector.SerialManager as SerialManager


def camera_stream(camera_index=0):
    # Open the camera
    cap = cv2.VideoCapture(camera_index)
    if not cap.isOpened():
        print("Error: Could not open camera.")
        return  # Exit if the camera cannot be opened

    try:
        while True:
            # Capture frame-by-frame
            ret, frame = cap.read()
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
                SerialManager.outgoing(
                    f"{tag['center'][0]},{tag['center'][1]},{tag['average_angle']}")
                with open('tag1_data.csv', 'a') as file:
                    file.write(
                        f"{tag['center'][0]},{tag['center'][1]},{tag['average_angle']}\n")


def main():
    for frame in camera_stream():
        tags = detect_tags_in_frame(frame)
        process_and_print_tags(tags)
        # Display frame for debugging (optional)
        cv2.imshow('Frame', frame)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
    cv2.destroyAllWindows()


if __name__ == "__main__":
    main()
