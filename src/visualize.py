import cv2
import matplotlib.pyplot as plt
import numpy as np
from matplotlib import cm

import cv2
import numpy as np

def load_orbit_3d_from_yml(path):
    fs = cv2.FileStorage(path, cv2.FILE_STORAGE_READ)
    if not fs.isOpened():
        raise IOError(f"Failed to open file: {path}")

    node = fs.getNode("orbit_3d")
    if node.empty():
        fs.release()
        raise ValueError(f"'orbit_3d' node not found in {path}")

    # 1) opencv-matrix (map) 형태
    if node.isMap():
        mat = node.mat()
        if mat is None:
            fs.release()
            raise ValueError(f"Failed to read 'orbit_3d' as matrix from {path}")
        data = np.array(mat, dtype=np.float32)
        if data.ndim == 1:
            data = data.reshape(-1, 3)

    # 2) nested sequence ([[x,y,z], …]) 형태
    elif node.isSeq() and node.at(0).isSeq():
        pts = []
        for i in range(node.size()):
            pt = node.at(i)
            x = pt.at(0).real()
            y = pt.at(1).real()
            z = pt.at(2).real()
            pts.append([x, y, z])
        data = np.array(pts, dtype=np.float32)

    # 3) flat sequence ([x,y,z, x,y,z, …]) 형태
    elif node.isSeq():
        vals = [node.at(i).real() for i in range(node.size())]
        if len(vals) % 3 != 0:
            fs.release()
            raise ValueError(f"Expected length divisible by 3, got {len(vals)}")
        data = np.array(vals, dtype=np.float32).reshape(-1, 3)

    else:
        fs.release()
        raise ValueError(f"Unsupported format for 'orbit_3d' in {path}")

    fs.release()
    return data

# 테이블, 네트 크기 (단위: cm)
TABLE_WIDTH = 152.5
TABLE_HEIGHT = 274
TABLE_NET_HEIGHT = 15.25

# 탁구대 좌표
table_x = [0, 0, TABLE_WIDTH, TABLE_WIDTH, 0]
table_y = [0, TABLE_HEIGHT, TABLE_HEIGHT, 0, 0]
table_z = [0, 0, 0, 0, 0]

# 네트 좌표
net_x = [0, TABLE_WIDTH, TABLE_WIDTH, 0, 0]
net_y = [TABLE_HEIGHT / 2] * 5
net_z = [0, 0, TABLE_NET_HEIGHT, TABLE_NET_HEIGHT, 0]

# 예시 target points (추적된 공 위치들)
orbit_path = "data/orbit/5.yml"
orbit_data = load_orbit_3d_from_yml(orbit_path)
target_x = orbit_data[:, 0]
target_y = orbit_data[:, 1] 
target_z = orbit_data[:, 2]

# 시각화 시작
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

# 테이블 선
ax.plot(table_x, table_y, table_z, color=(0.05, 0.15, 0.5), linewidth=2)
# 네트 선
ax.plot(net_x, net_y, net_z, color=(0.05, 0.15, 0.5), linewidth=2)
# 공 궤적 (scatter)
num_points = len(target_x)
colors = cm.rainbow_r(np.linspace(0, 1, num_points))  # rainbow, plasma 등도 가능
for i in range(num_points):
    ax.scatter(target_x[i], target_y[i], target_z[i], color=colors[i], s=50)

# 라벨 및 제목
ax.set_xlabel('X [cm]')
ax.set_ylabel('Y [cm]')
ax.set_zlabel('Z [cm]')
ax.set_title('3D Table Tennis Table')

# 범위 설정 (matplot++와 동일하게)
ax.set_xlim([-50, 300])
ax.set_ylim([-50, 300])
ax.set_zlim([0, 150])

plt.grid(True)
plt.show()
