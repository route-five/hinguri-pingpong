# hsv_range_finder.py
import csv

log_file = "hsv_log.txt"

h_values, s_values, v_values = [], [], []

with open(log_file, "r") as f:
    reader = csv.reader(f)
    for row in reader:
        if len(row) != 6:
            continue  # 잘못된 형식이면 건너뜀
        h = int(row[3])
        s = int(row[4])
        v = int(row[5])
        h_values.append(h)
        s_values.append(s)
        v_values.append(v)

if not h_values:
    print("No HSV values found.")
else:
    print("HSV Range:")
    print(f"Hue       : min = {min(h_values)}, max = {max(h_values)}")
    print(f"Saturation: min = {min(s_values)}, max = {max(s_values)}")
    print(f"Value     : min = {min(v_values)}, max = {max(v_values)}")

    print("\nOpenCV format:")
    print(f"lower_bound = np.array([{min(h_values)}, {min(s_values)}, {min(v_values)}])")
    print(f"upper_bound = np.array([{max(h_values)}, {max(s_values)}, {max(v_values)}])")
