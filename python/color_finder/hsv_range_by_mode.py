from collections import Counter
import csv

log_file_path = "hsv_log.txt"

h_list, s_list, v_list = [], [], []

with open(log_file_path, "r") as f:
    reader = csv.reader(f)
    for row in reader:
        h, s, v = int(row[3]), int(row[4]), int(row[5])
        h_list.append(h)
        s_list.append(s)
        v_list.append(v)

def get_mode_and_range(lst, tolerance=10, max_val=255):
    c = Counter(lst)
    mode_val, _ = c.most_common(1)[0]
    return max(mode_val - tolerance, 0), min(mode_val + tolerance, max_val), mode_val

h_lower, h_upper, h_mode = get_mode_and_range(h_list, tolerance=10, max_val=180)
s_lower, s_upper, s_mode = get_mode_and_range(s_list, tolerance=40, max_val=255)
v_lower, v_upper, v_mode = get_mode_and_range(v_list, tolerance=40, max_val=255)

print(f"H: mode={h_mode}, range=({h_lower}, {h_upper})")
print(f"S: mode={s_mode}, range=({s_lower}, {s_upper})")
print(f"V: mode={v_mode}, range=({v_lower}, {v_upper})")

with open("hsv_range_result.txt", "w") as out:
    out.write(f"H: mode={h_mode}, range=({h_lower}, {h_upper})\n")
    out.write(f"S: mode={s_mode}, range=({s_lower}, {s_upper})\n")
    out.write(f"V: mode={v_mode}, range=({v_lower}, {v_upper})\n")

print("'hsv_range_result.txt'")