import cv2
import time
from datetime import datetime
import numpy as np

def main():
    while True:
        now = datetime.now()
        ns = time.time_ns() % 1_000_000_000
        formatted_time = now.strftime('%H:%M:%S.%f')[:-3] + f"{ns % 1000:03d} ns"  # 소수점 이하 9자리

        image = 255 * np.ones((300, 1000, 3), dtype=np.uint8)
        cv2.putText(image, formatted_time, (30, 180), cv2.FONT_HERSHEY_SIMPLEX, 2.0, (0, 0, 0), 4, cv2.LINE_AA)

        cv2.imshow("Current Time (formatted)", image)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    cv2.destroyAllWindows()

if __name__ == "__main__":
    import numpy as np
    main()