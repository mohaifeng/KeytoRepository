import matplotlib.pyplot as plt
import numpy as np

# 设置目标分辨率和颜色
target_width = 1980    # 像素宽度
target_height = 1980   # 像素高度
dpi = 300             # 每英寸点数（DPI）
color_bits = 24       # 24位颜色（RGB 8位/通道）

# 计算 figsize（英寸单位）
figsize_width = target_width / dpi
figsize_height = target_height / dpi

# 创建高分辨率画布
fig = plt.figure(figsize=(figsize_width, figsize_height), dpi=dpi)
ax = fig.add_subplot(111)

t = np.linspace(0, 10, 1000)

# 软停止曲线
soft_pressure = np.piecewise(t, [
    t <= 3,
    (t > 3) & (t <= 5),
    (t > 5) & (t <= 8),
    t > 8
], [
    lambda t: 100,
    lambda t: 100 + 50 * (t - 3),
    lambda t: 150 * np.exp(-(t - 5) / 1),
    lambda t: 0
])

# 硬停止曲线
hard_pressure = np.piecewise(t, [
    t <= 3,
    (t > 3) & (t <= 5),
    (t > 5) & (t <= 5.5),
    t > 5.5
], [
    lambda t: 100,
    lambda t: 100 + 50 * (t - 3),
    lambda t: 150,
    lambda t: 0
])

# 绘制曲线（开启抗锯齿）
ax.plot(t, soft_pressure, label='SOFT STOP', color='blue', linewidth=2, antialiased=True)
ax.plot(t, hard_pressure, label='HARD STOP', color='red', linewidth=2, antialiased=True)

# 标注设置
ax.axvline(x=3, color='gray', linestyle='--', alpha=0.7, label='START')
ax.axvline(x=5, color='red', linestyle='--', alpha=0.7, label='Blockage detection triggered')
ax.axvline(x=5.5, color='green', linestyle='--', alpha=0.7, label='Hard stop completed')
ax.axvline(x=8, color='blue', linestyle='--', alpha=0.7, label='Soft stop completed')

# 坐标轴和标题
ax.set_xlabel('Time (seconds)', fontsize=10)
ax.set_ylabel('Lower pressure (N)', fontsize=10)
ax.set_title('Pressure diagram for soft stop and hard stop in stall detection', fontsize=12)

# 图例和网格
ax.legend(loc='upper right', fontsize=8)
ax.grid(True, alpha=0.3, linestyle='--')

# 保存高清图像（PNG无损格式）
output_path = "high_res_plot.png"
plt.savefig(
    output_path,
    dpi=dpi,
    bbox_inches='tight',  # 去除多余空白
    pad_inches=0.1,       # 保留少量边距
    facecolor='white',    # 背景色
    format='png'          # 避免压缩损失
)

plt.show()