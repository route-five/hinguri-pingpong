import cv2
import os
import numpy as np

# 설정
image_folder = "images"  # 이미지 폴더명
log_file_path = "hsv_log.txt"  # HSV 로그 파일 경로
target_width = 800  # 화면에 표시할 가로 폭 기준

# 이미지 파일 목록 필터링 (.jpg, .jpeg, .png)
image_files = sorted([
    f for f in os.listdir(image_folder)
    if f.lower().endswith(('.jpg', '.jpeg', '.png'))
])

# HSV 로그 파일 열기
log_file = open(log_file_path, "w")

# 클릭 이벤트 콜백 함수
def on_mouse_click(event, x, y, flags, param):
    if event == cv2.EVENT_LBUTTONDOWN:
        hsv_img, fname = param
        hsv = hsv_img[y, x]
        print(f"[{fname}] Clicked HSV at ({x}, {y}): {hsv}")
        log_file.write(f"{fname},{x},{y},{hsv[0]},{hsv[1]},{hsv[2]}\n")
        log_file.flush()

# 이미지 하나씩 띄우기
for fname in image_files:
    path = os.path.join(image_folder, fname)
    img = cv2.imread(path)

    if img is None:
        print(f"Image {fname} failed to load.")
        continue

    # 크기 조절
    scale = target_width / img.shape[1]
    img = cv2.resize(img, (0, 0), fx=scale, fy=scale)
    hsv_img = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)

    window_name = "Image Viewer"
    cv2.namedWindow(window_name, cv2.WINDOW_AUTOSIZE)
    cv2.imshow(window_name, img)
    cv2.setMouseCallback(window_name, on_mouse_click, param=(hsv_img, fname))

    print(f"\nNow viewing: {fname}")
    print("click")
    print("'n': next image / ESC: exit")

    while True:
        key = cv2.waitKey(0)
        if key == ord('n'):
            break
        elif key == 27:  # ESC
            log_file.close()
            cv2.destroyAllWindows()
            print("\n프로그램 종료")
            exit()

    cv2.destroyWindow(window_name)

# 마무리
log_file.close()
print("\nDone")