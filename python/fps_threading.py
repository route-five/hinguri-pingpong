# import the necessary packages
from __future__ import print_function
from imutils.video import FPS
from threading import Thread
import argparse
import imutils
import cv2
import time

CAM_SOURCE = 0


class WebcamVideoStream:
    def __init__(self, src=0):
        # initialize the video camera stream and read the first frame
        # from the stream
        self.stream = cv2.VideoCapture(src)
        (self.grabbed, self.frame) = self.stream.read()
        # initialize the variable used to indicate if the thread should
        # be stopped
        self.stopped = False

    def start(self):
        # start the thread to read frames from the video stream
        Thread(target=self.update, args=()).start()
        return self

    def update(self):
        # keep looping infinitely until the thread is stopped
        while True:
            # if the thread indicator variable is set, stop the thread
            if self.stopped:
                return
            # otherwise, read the next frame from the stream
            (self.grabbed, self.frame) = self.stream.read()

    def read(self):
        # return the frame most recently read
        return self.frame

    def stop(self):
        # indicate that the thread should be stopped
        self.stopped = True


# construct the argument parse and parse the arguments
ap = argparse.ArgumentParser()
ap.add_argument(
    "-n",
    "--num-frames",
    type=int,
    default=100,
    help="# of frames to loop over for FPS test",
)
ap.add_argument(
    "-d",
    "--display",
    type=int,
    default=-1,
    help="Whether or not frames should be displayed",
)
args = vars(ap.parse_args())
# grab a pointer to the video stream and initialize the FPS counter
print("[INFO] sampling frames from webcam...")
stream = cv2.VideoCapture(CAM_SOURCE)
width = int(stream.get(cv2.CAP_PROP_FRAME_WIDTH))
height = int(stream.get(cv2.CAP_PROP_FRAME_HEIGHT))
stream_fps = int(stream.get(cv2.CAP_PROP_FPS))
print(f"[INFO] current resolution: {width}x{height} @ {stream_fps}fps")
prev_time = time.time()
fps = FPS().start()
# moving-average FPS update once per interval
update_interval = 1.0  # seconds
time_acc = 0.0
frame_acc = 0
last_update_time = time.time()
actual_fps = 0.0
# loop over some frames
while fps._numFrames < args["num_frames"]:
    # start of loop: accumulate time and frames
    current_time = time.time()
    elapsed = current_time - prev_time
    prev_time = current_time
    time_acc += elapsed
    frame_acc += 1
    # update actual_fps only every update_interval
    if (current_time - last_update_time) >= update_interval:
        actual_fps = frame_acc / time_acc
        frame_acc = 0
        time_acc = 0.0
        last_update_time = current_time
    # grab the frame from the stream and resize it to have a maximum
    # width of 400 pixels
    (grabbed, frame) = stream.read()
    frame = imutils.resize(frame, width=400)
    # check to see if the frame should be displayed to our screen
    if args["display"] > 0:
        cv2.putText(
            frame,
            f"{width}x{height}",
            (10, 25),
            cv2.FONT_HERSHEY_SIMPLEX,
            0.7,
            (0, 255, 0),
            2,
        )
        cv2.putText(
            frame,
            f"{int(actual_fps)}/{stream_fps} FPS",
            (10, 50),
            cv2.FONT_HERSHEY_SIMPLEX,
            0.7,
            (0, 255, 0),
            2,
        )
        cv2.imshow("Frame", frame)
        key = cv2.waitKey(1) & 0xFF
    # update the FPS counter
    fps.update()
# stop the timer and display FPS information
fps.stop()
print("[INFO] elasped time: {:.2f}".format(fps.elapsed()))
print("[INFO] approx. FPS: {:.2f}".format(fps.fps()))
# do a bit of cleanup
stream.release()
cv2.destroyAllWindows()
# created a *threaded* video stream, allow the camera sensor to warmup,
# and start the FPS counter
print("[INFO] sampling THREADED frames from webcam...")
vs = WebcamVideoStream(src=CAM_SOURCE).start()
# threaded stream moving-average
update_interval = 1.0
time_acc = 0.0
frame_acc = 0
last_update_time = time.time()
actual_fps = 0.0
fps = FPS().start()
# loop over some frames...this time using the threaded stream
while fps._numFrames < args["num_frames"]:
    # start of loop: accumulate time and frames
    current_time = time.time()
    elapsed = current_time - prev_time
    prev_time = current_time
    time_acc += elapsed
    frame_acc += 1
    # update actual_fps only every update_interval
    if (current_time - last_update_time) >= update_interval:
        actual_fps = frame_acc / time_acc
        frame_acc = 0
        time_acc = 0.0
        last_update_time = current_time
    # grab the frame from the threaded video stream and resize it
    # to have a maximum width of 400 pixels
    frame = vs.read()
    frame = imutils.resize(frame, width=400)
    # check to see if the frame should be displayed to our screen
    if args["display"] > 0:
        cv2.putText(
            frame,
            f"{width}x{height}",
            (10, 25),
            cv2.FONT_HERSHEY_SIMPLEX,
            0.7,
            (0, 255, 0),
            2,
        )
        cv2.putText(
            frame,
            f"{int(actual_fps)}/{stream_fps} FPS",
            (10, 50),
            cv2.FONT_HERSHEY_SIMPLEX,
            0.7,
            (0, 255, 0),
            2,
        )
        cv2.imshow("Frame", frame)
        key = cv2.waitKey(1) & 0xFF
    # update the FPS counter
    fps.update()
# stop the timer and display FPS information
fps.stop()
print("[INFO] elasped time: {:.2f}".format(fps.elapsed()))
print("[INFO] approx. FPS: {:.2f}".format(fps.fps()))
# do a bit of cleanup
cv2.destroyAllWindows()
vs.stop()
