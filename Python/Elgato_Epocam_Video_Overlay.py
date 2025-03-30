# base code from nathan
# import cv2

# cap = cv2.VideoCapture(0)  

# while True:
#     ret, frame = cap.read()
#     if not ret:
#         print("Failed to grab frame")
#         break

#     cv2.imshow("EpocCam Stream", frame)

#     if cv2.waitKey(1) & 0xFF == ord('q'):  # Press 'q' to exit
#         break

# cap.release()
# cv2.destroyAllWindows()

#code that takes terminal inputs to update the text
import cv2
import threading
overlay_text = "Starting..."  # Default text
def update_text():
    global overlay_text
    while True:
        new_text = input()
        overlay_text = new_text
# Start the input thread
threading.Thread(target=update_text, daemon=True).start()
cap = cv2.VideoCapture(0)
while True:
    ret, frame = cap.read()
    if not ret:
        print("Failed to grab frame")
        break
    # Draw the current overlay text
    font = cv2.FONT_HERSHEY_SIMPLEX
    position = (10, 30)
    font_scale = 1
    font_color = (0, 255, 0)
    thickness = 2
    line_type = cv2.LINE_AA
    cv2.putText(frame, overlay_text, position, font, font_scale, font_color, thickness, line_type)
    cv2.imshow("EpocCam Stream", frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
cap.release()
cv2.destroyAllWindows()
