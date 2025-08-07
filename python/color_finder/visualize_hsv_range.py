import cv2
import numpy as np
import os

# HSV 범위
h_mode, h_low, h_high = 21, 17, 24
s_mode, s_low, s_high = 255, 204, 255
v_mode, v_low, v_high = 255, 220, 255

def hsv_to_rgb(h, s, v):
    hsv = np.uint8([[[h, s, v]]])
    rgb = cv2.cvtColor(hsv, cv2.COLOR_HSV2BGR)[0][0]
    return tuple(int(c) for c in rgb)

colors = {
    "LOW": hsv_to_rgb(h_low, s_low, v_low),
    "MODE": hsv_to_rgb(h_mode, s_mode, v_mode),
    "HIGH": hsv_to_rgb(h_high, s_high, v_high)
}

block_height, block_width = 100, 200
img = np.zeros((block_height, block_width * 3, 3), dtype=np.uint8)

for i, (label, color) in enumerate(colors.items()):
    x = i * block_width
    img[:, x:x + block_width] = color
    cv2.putText(img, label, (x + 10, 60), cv2.FONT_HERSHEY_SIMPLEX, 1, (0,0,0), 2)

# 저장 + 경로 출력
save_path = "hsv_visualized_result.png"
success = cv2.imwrite(save_path, img)
print("저장 성공" if success else "저장 실패")
print("저장 경로:", os.path.abspath(save_path))

cv2.imshow("HSV Colors", img)
cv2.waitKey(0)
cv2.destroyAllWindows()