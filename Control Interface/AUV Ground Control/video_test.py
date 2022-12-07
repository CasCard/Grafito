# dispay video using opencv


import cv2
import numpy as np


def main():
    cap = cv2.VideoCapture(r'C:\Users\abel_\OneDrive\Desktop\RIG\AUV Ground Control\redrod2.mp4')
    while True:
        ret, frame = cap.read()
        if ret:
            cv2.imshow('frame', frame)
            if cv2.waitKey(1) & 0xFF == ord('q'):
                break
        else:
            break

    cap.release()
    cv2.destroyAllWindows()
if __name__ == '__main__':
    main()