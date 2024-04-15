from apriltag import Detector
import cv2

def detect_apriltags(camera_feed):
    # Create AprilTags detector
    # If the library requires parameters, check the correct usage in the library documentation

    detector = Detector()

    # Read camera feed
    cap = cv2.VideoCapture(camera_feed)

    april_tags = []

    while True:
        # Capture frame-by-frame
        ret, frame = cap.read()
        if not ret:
            break  # Exit if the camera read fails

        # Convert frame to grayscale
        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

        # Detect AprilTags in the grayscale frame
        detections = detector.detect(gray)

        # Extract AprilTag parameters
        for detection in detections:
            # Get tag ID and 2D parameters (x, y)
            tag_id = detection.tag_id
            x, y = detection.center

            # Append tag information to the list
            april_tags.append({"id": tag_id, "x": x, "y": y})

            # Draw the bounding box around the detected tag
            cv2.polylines(
                frame, [detection.corners.astype(int)], True, (0, 255, 0), 2)

        # Display frame with AprilTag detections
        cv2.imshow('AprilTags Detection', frame)

        # Break the loop if 'q' is pressed
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    # Release the capture
    cap.release()
    cv2.destroyAllWindows()

    return april_tags


# Call the function to detect AprilTags in a camera feed
camera_feed = 0  # Use 0 for default camera or specify camera index
apriltags = detect_apriltags(camera_feed)

# Print detected AprilTags
print("Detected AprilTags:")
for tag in apriltags:
    print(f"ID: {tag['id']}, Position (x, y): {tag['x']}, {tag['y']}")
