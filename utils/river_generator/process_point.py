import matplotlib.pyplot as plt
import matplotlib.image as mpimg
import numpy as np

# 初始化一个空列表，用于存储点击的点的归一化坐标和方向向量
points = []
base_point = None

def calculate_intersection(dir_x, dir_y):
    if dir_x == 0: # k -> inf
        return (0.5, 1) if dir_y >= 0 else (0.5, 0)
    k = dir_y / dir_x
    if abs(k) <= 1: # left or right
        return (0, 0.5 - 0.5 * k) if dir_x < 0 else (1, 0.5 + 0.5 * k)
    else:           # bottom or top
        return (0.5 - 0.5 / k, 0) if dir_y < 0 else (0.5 + 0.5 / k, 1)

# 回调函数，用于处理鼠标点击事件
def onclick(event):
    global base_point
    if event.key == 'shift':
        return
    if event.xdata is not None and event.ydata is not None:
        # 计算归一化坐标
        height, width, _ = img.shape
        norm_x = event.xdata / width
        norm_y = event.ydata / height

        if base_point is None:  # 第一次点击，记录基准点
            base_point = (norm_x, norm_y)
            print(f"Base point set at: ({norm_x:.2f}, {norm_y:.2f})")
            ax.plot(norm_x * width, (norm_y) * height, 'bo')
            fig.canvas.draw()
        else:
            dir_x = norm_x - base_point[0]
            dir_y = norm_y - base_point[1]
            intersection = calculate_intersection(dir_x, dir_y)
            points.append((norm_x, norm_y, intersection[0], intersection[1]))
            print(f"click at: ({norm_x:.2f}, {norm_y:.2f}), computed normal vec: ({intersection[0]:.2f}, {intersection[1]:.2f})")
            # 如果有多个点，绘制线条
            if len(points) > 1:
                x_vals, y_vals = zip(*[(p[0], p[1]) for p in points])
                ax.plot([x * width for x in x_vals], [y * height for y in y_vals], 'r-')
            fig.canvas.draw()

# 读取图像
img = mpimg.imread('GreatPlateau_river.png')
fig, ax = plt.subplots()
ax.imshow(img)

# 设置鼠标回调函数
cid = fig.canvas.mpl_connect('button_press_event', onclick)

plt.show()

# 打印所有点的归一化坐标和方向向量
x_offset = -0.0395
z_offset = 0.0275
print("float points[] = {")
for point in points:
    print(f"    {point[0] - 0.5 + x_offset:.6f}f, {point[1] - 0.5 + z_offset:.6f}f, {point[2]:.6f}f, {point[3]:.6f}f,")
print("};")
print("int num_points = ", len(points))

# 可能是图片裁剪的偏移（跟 heightMap 没裁到同一个块，最后结果有一定偏差，手调吧）
while True:
    x_offset = float(input("请输入 x_offset: "))
    z_offset = float(input("请输入 z_offset: "))

    print("float points[] = {")
    for point in points:
        print(f"    {point[0] - 0.5 + x_offset:.6f}f, {point[1] - 0.5 + z_offset:.6f}f, {point[2]:.6f}f, {point[3]:.6f}f,")
    print("};")
    print("int num_points = ", len(points))

    input("ctrl-c 退出，或按任意键继续...")