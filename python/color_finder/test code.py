import cv2
import numpy as np

# Kalman 필터 설정
kalman = cv2.KalmanFilter(4, 2)
kalman.measurementMatrix = np.array([[1, 0, 0, 0],
                                     [0, 1, 0, 0]], np.float32)
kalman.transitionMatrix = np.array([[1, 0, 1, 0],
                                    [0, 1, 0, 1],
                                    [0, 0, 1, 0],
                                    [0, 0, 0, 1]], np.float32)
kalman.processNoiseCov = np.eye(4, dtype=np.float32) * 0.5
kalman.measurementNoiseCov = np.eye(2, dtype=np.float32) * 0.1
kalman.errorCovPost = np.eye(4, dtype=np.float32)

cap = cv2.VideoCapture(0)
prev_gray = None
last_centroid = None

# 너가 측정한 최적 HSV 범위
lower = np.array([19, 215, 225])
upper = np.array([23, 255, 255])

while True:
    ret, frame = cap.read()
    if not ret:
        break

    frame = cv2.resize(frame, (640, 480))
    blurred = cv2.GaussianBlur(frame, (5, 5), 0)
    hsv = cv2.cvtColor(blurred, cv2.COLOR_BGR2HSV)

    # 색 마스크
    hsv_mask = cv2.inRange(hsv, lower, upper)

    # 모션 마스크
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    motion_mask = np.zeros_like(gray)
    if prev_gray is not None:
        diff = cv2.absdiff(gray, prev_gray)
        _, motion_mask = cv2.threshold(diff, 15, 255, cv2.THRESH_BINARY)
    prev_gray = gray.copy()

    # 색 + 모션 동시 마스크
    combined_mask = cv2.bitwise_and(motion_mask, hsv_mask)
    kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (5, 5))
    combined_mask = cv2.morphologyEx(combined_mask, cv2.MORPH_OPEN, kernel)

    contours, _ = cv2.findContours(combined_mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    best_centroid = None

    for cnt in contours:
        area = cv2.contourArea(cnt)
        if area < 50:
            continue
        x, y, w, h = cv2.boundingRect(cnt)
        aspect = w / h
        if 0.6 < aspect < 1.4:
            cx, cy = x + w // 2, y + h // 2
            best_centroid = np.array([[np.float32(cx)], [np.float32(cy)]])
            cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)
            cv2.circle(frame, (cx, cy), 5, (0, 0, 255), -1)
            break

    if best_centroid is not None:
        kalman.correct(best_centroid)
        last_centroid = best_centroid.copy()
        cv2.circle(frame, tuple(best_centroid.ravel().astype(int)), 6, (0, 255, 255), 2)  # 실측
    elif last_centroid is not None:
        kalman.correct(last_centroid)

    pred = kalman.predict()
    pred_pos = tuple(pred[:2].ravel().astype(int))
    cv2.circle(frame, pred_pos, 6, (255, 0, 0), 2)  # 예측값

    cv2.imshow("Detected", frame)
    cv2.imshow("HSV Mask Only", hsv_mask)
    cv2.imshow("Motion Only", motion_mask)

    if cv2.waitKey(1) == 27:  # ESC
        break

cap.release()
cv2.destroyAllWindows()