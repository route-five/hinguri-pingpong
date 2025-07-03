import cv2
import os
import numpy as np

# 설정
image_folder = "images"
log_file_path = "hsv_log.txt"
target_width = 800  # 화면에 표시할 가로 폭 기준

# 이미지 리스트 가져오기
image_files = sorted([f for f in os.listdir(image_folder) if f.lower().endswith(('.jpg', '.png'))])

# HSV 기록용 파일 열기
log_file = open(log_file_path, "w")

# 클릭 이벤트 함수
def on_mouse_click(event, x, y, flags, param):
    if event == cv2.EVENT_LBUTTONDOWN:
        hsv_img, fname = param
        hsv = hsv_img[y, x]
        print(f"[{fname}] Clicked HSV at ({x}, {y}): {hsv}")
        log_file.write(f"{fname},{x},{y},{hsv[0]},{hsv[1]},{hsv[2]}\n")

# 이미지 하나씩 처리
for fname in image_files:
    path = os.path.join(image_folder, fname)
    img = cv2.imread(path)

    if img is None:
        print(f"Image {fname} failed to load.")
        continue

    h, w = img.shape[:2]
    scale = target_width / w
    new_size = (target_width, int(h * scale))
    resized_img = cv2.resize(img, new_size)
    hsv_img = cv2.cvtColor(resized_img, cv2.COLOR_BGR2HSV)  # 리사이즈된 이미지로 HSV 변환!

    print(f"\nNow viewing: {fname} — Click on the ball to record HSV")
    window_name = "Click on the ball (press 'n' for next)"
    cv2.namedWindow(window_name, cv2.WINDOW_AUTOSIZE)
    cv2.imshow(window_name, resized_img)
    cv2.setMouseCallback(window_name, on_mouse_click, (hsv_img, fname))

    while True:
        key = cv2.waitKey(0)
        if key == ord('n'):
            break
        elif key == 27:  # ESC
            print("Exiting.")
            log_file.close()
            cv2.destroyAllWindows()
            exit()

log_file.close()
cv2.destroyAllWindows()
print("\nDone! HSV values saved to hsv_log.txt.")
