import numpy as np
import matplotlib.pyplot as plt
from matplotlib.colors import hsv_to_rgb

# 1) HSV 범위 설정
h_min, h_max = 9, 23
s_min, s_max = 195, 255
v_min, v_max = 215, 255

h_vals = np.arange(h_min, h_max + 1)
s_vals = np.arange(s_min, s_max + 1)
v_vals = np.arange(v_min, v_max + 1)

# 2) 메쉬 그리드 생성 (H, S, V 조합)
H, S, V = np.meshgrid(h_vals, s_vals, v_vals, indexing='ij')

# 3) [0,1] 범위로 정규화
h_norm = H / 180.0   # OpenCV H 범위 0-179 → 0-1
s_norm = S / 255.0
v_norm = V / 255.0

hsv = np.stack([h_norm, s_norm, v_norm], axis=-1)

# 4) RGB로 변환
rgb = hsv_to_rgb(hsv)   # shape = (len(h), len(s), len(v), 3)

# 5) 산점도용 1D 배열로 펼치기
rgb_flat = rgb.reshape(-1, 3)
h_flat = H.ravel()
s_flat = S.ravel()
v_flat = V.ravel()

# 6) 3D 산점도 그리기
fig = plt.figure(figsize=(10, 8))
ax = fig.add_subplot(111, projection='3d')
ax.scatter(h_flat, s_flat, v_flat, c=rgb_flat, s=5, edgecolors='none')

ax.set_xlabel('Hue (0–180)')
ax.set_ylabel('Sat (0–255)')
ax.set_zlabel('Val (0–255)')
ax.set_title('HSV Visualization')

plt.tight_layout()
plt.show()