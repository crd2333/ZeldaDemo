import matplotlib.pyplot as plt
import matplotlib.image as mpimg
import numpy as np

# 解析输入数据
input_data = """
    -0.341657f, 0.145866f, 0.198011f, 1.000000f,
    -0.455263f, -0.023098f, 0.000000f, 0.331472f,
    -0.459566f, -0.135117f, 0.000000f, 0.031073f,
    -0.404485f, -0.155284f, 0.165863f, 0.000000f,
    -0.297764f, -0.126270f, 0.432402f, 0.000000f,
    -0.217724f, -0.062214f, 0.788183f, 0.000000f,
    -0.177273f, -0.007323f, 1.000000f, 0.269460f,
    -0.084323f, 0.053296f, 1.000000f, 0.543970f,
    -0.029241f, 0.116434f, 1.000000f, 0.662314f,
    -0.017192f, 0.224380f, 1.000000f, 0.863666f,
    -0.030963f, 0.272242f, 1.000000f, 0.981994f,
    -0.080880f, 0.320105f, 0.843150f, 1.000000f,
    -0.196208f, 0.282426f, 0.661263f, 1.000000f,
    -0.267641f, 0.210123f, 0.522718f, 1.000000f,
"""

# 将输入数据转换为点列表
points = []
for line in input_data.strip().split('\n'):
    values = line.strip().split(',')
    points.append([float(v.strip('f')) for v in values if v.strip()])

# 读取图像
img = mpimg.imread('GreatPlateau_river.png')
fig, ax = plt.subplots()
ax.imshow(img)

# 获取图像的宽度和高度
height, width, _ = img.shape

# 偏移量
x_offset = -0.0395
z_offset = 0.0275

# 绘制点和线条
x_vals = []
y_vals = []
for i, point in enumerate(points):
    norm_x = point[0] + 0.5 - x_offset
    norm_y = point[1] + 0.5 - z_offset
    x_vals.append(norm_x * width)
    y_vals.append(norm_y * height)
    ax.plot(norm_x * width, norm_y * height, 'bo')
    ax.text(norm_x * width, norm_y * height, str(i+1), fontsize=9, color='red')

# 绘制线条
ax.plot(x_vals, y_vals, 'r-')

plt.show()