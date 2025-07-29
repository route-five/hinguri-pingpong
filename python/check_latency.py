import cv2
import time
import collections

def measure_camera_latency(camera_index=0, buffer_size=1, warmup_frames=30, history_len=100):
    # 카메라 열기
    cap = cv2.VideoCapture(camera_index)
    if not cap.isOpened():
        raise RuntimeError(f"Cannot open camera {camera_index}")

    # 버퍼 사이즈 설정 (OpenCV 4.5+)
    try:
        cap.set(cv2.CAP_PROP_BUFFERSIZE, buffer_size)
    except:
        pass  # Windows 등 일부 플랫폼은 지원 안 함

    # 예열용 프레임 읽기
    for _ in range(warmup_frames):
        cap.read()

    latencies = collections.deque(maxlen=history_len)

    while True:
        # Local capture timestamp (nanosecond precision)
        t0_ns = time.time_ns()
        t0 = t0_ns / 1e9
        ret, frame = cap.read()
        if not ret:
            print("Frame grab failed, exiting.")
            break
        t1_ns = time.time_ns()
        t1 = t1_ns / 1e9

        # Latency calculation (ms)
        latency_ms = (t1 - t0) * 1000
        latencies.append(latency_ms)
        avg_latency = sum(latencies) / len(latencies)

        # Overlay with nanosecond precision timestamp
        sec = t1_ns // 1_000_000_000
        nsec = t1_ns % 1_000_000_000
        time_str = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(sec)) + f".{nsec:09d}"
        overlay = [
            f"Latency: {latency_ms:.3f} ms",
            f"Time   : {time_str}"
        ]
        for i, line in enumerate(overlay):
            cv2.putText(frame, line, (10, 30 + i*30),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 255, 0), 2)

        cv2.imshow("Latency Test", frame)
        key = cv2.waitKey(1)
        if key == ord('q'):
            break
        elif key == ord('r'):
            latencies.clear()  # 기록 초기화
        elif key == ord('c'):
            # Save capture with nanosecond timestamp
            ts = t1_ns
            filename = f"capture_{ts}.png"
            cv2.imwrite(filename, frame)
            print(f"Captured: {filename}")

    cap.release()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    # 카메라 인덱스, 버퍼 사이즈, 예열 프레임, 평균 계산에 사용할 기록 길이 지정 가능
    measure_camera_latency(camera_index=1, buffer_size=1, warmup_frames=30, history_len=200)