import matplotlib.pyplot as plt
import numpy as np
from tkinter import *
from tkinter import ttk, messagebox, scrolledtext
import serial
import serial.tools.list_ports
from threading import Thread
import time
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import struct
import binascii


class SerialPlotter:
    def __init__(self, root):
        self.root = root
        self.root.title("串口数据交互与曲线显示工具")
        self.root.geometry("1000x800")

        # 串口相关变量
        self.serial_port = None
        self.is_reading = False
        self.data_buffer = []
        self.max_data_points = 0xFFFFFFFF  # 最大显示数据点数

        # 发送相关变量
        self.send_timer = None
        self.is_sending = False
        self.send_interval = 1000  # 默认发送间隔(ms)

        # 创建界面
        self.create_widgets()

        # 初始化图表
        self.init_plot()

    def create_widgets(self):
        # 主框架
        main_frame = Frame(self.root)
        main_frame.pack(fill=BOTH, expand=True, padx=10, pady=10)

        # 左侧设置面板
        left_frame = Frame(main_frame)
        left_frame.pack(side=LEFT, fill=Y, padx=(0, 5))

        # 串口设置框架
        settings_frame = LabelFrame(left_frame, text="串口设置")
        settings_frame.pack(fill=X, pady=5)

        # 端口选择
        port_row = Frame(settings_frame)
        port_row.pack(fill=X, pady=5)

        Label(port_row, text="端口:").pack(side=LEFT, padx=5)
        self.port_combo = ttk.Combobox(port_row, width=15)
        self.port_combo.pack(side=LEFT, padx=5)

        Button(port_row, text="刷新端口", command=self.refresh_ports).pack(side=LEFT, padx=5)

        # 参数设置
        params_row = Frame(settings_frame)
        params_row.pack(fill=X, pady=5)

        Label(params_row, text="波特率:").pack(side=LEFT, padx=5)
        self.baud_combo = ttk.Combobox(params_row, width=10, values=["9600", "19200", "38400", "57600", "115200"])
        self.baud_combo.set("9600")
        self.baud_combo.pack(side=LEFT, padx=5)

        Label(params_row, text="数据位:").pack(side=LEFT, padx=5)
        self.data_bits_combo = ttk.Combobox(params_row, width=5, values=["5", "6", "7", "8"])
        self.data_bits_combo.set("8")
        self.data_bits_combo.pack(side=LEFT, padx=5)

        # 更多参数设置
        params_row2 = Frame(settings_frame)
        params_row2.pack(fill=X, pady=5)

        Label(params_row2, text="停止位:").pack(side=LEFT, padx=5)
        self.stop_bits_combo = ttk.Combobox(params_row2, width=5, values=["1", "1.5", "2"])
        self.stop_bits_combo.set("1")
        self.stop_bits_combo.pack(side=LEFT, padx=5)

        Label(params_row2, text="校验位:").pack(side=LEFT, padx=5)
        self.parity_combo = ttk.Combobox(params_row2, width=8, values=["无", "奇校验", "偶校验"])
        self.parity_combo.set("无")
        self.parity_combo.pack(side=LEFT, padx=5)

        # 控制按钮
        control_row = Frame(settings_frame)
        control_row.pack(fill=X, pady=5)

        self.connect_btn = Button(control_row, text="打开串口", command=self.toggle_serial)
        self.connect_btn.pack(side=LEFT, padx=5)

        Button(control_row, text="清空数据", command=self.clear_data).pack(side=LEFT, padx=5)

        # 数据显示设置
        display_row = Frame(settings_frame)
        display_row.pack(fill=X, pady=5)

        Label(display_row, text="数据点数:").pack(side=LEFT, padx=5)
        self.points_entry = Entry(display_row, width=10)
        self.points_entry.insert(0, "200")
        self.points_entry.pack(side=LEFT, padx=5)

        Button(display_row, text="应用", command=self.apply_settings).pack(side=LEFT, padx=5)

        # 数据发送框架
        send_frame = LabelFrame(left_frame, text="数据发送设置")
        send_frame.pack(fill=X, pady=5)

        # 发送数据格式
        format_row = Frame(send_frame)
        format_row.pack(fill=X, pady=5)

        Label(format_row, text="发送格式:").pack(side=LEFT, padx=5)
        self.send_format = ttk.Combobox(format_row, width=12, values=["十六进制", "ASCII", "十进制"])
        self.send_format.set("十六进制")
        self.send_format.pack(side=LEFT, padx=5)

        # 发送数据内容
        data_row = Frame(send_frame)
        data_row.pack(fill=X, pady=5)

        Label(data_row, text="发送数据:").pack(side=LEFT, padx=5)
        self.send_data_entry = Entry(data_row, width=20)
        self.send_data_entry.insert(0, "02 2D 00 00 00 00 0D 0A")
        self.send_data_entry.pack(side=LEFT, padx=5)

        # 发送间隔设置
        interval_row = Frame(send_frame)
        interval_row.pack(fill=X, pady=5)

        Label(interval_row, text="发送间隔(ms):").pack(side=LEFT, padx=5)
        self.interval_entry = Entry(interval_row, width=10)
        self.interval_entry.insert(0, "1000")
        self.interval_entry.pack(side=LEFT, padx=5)

        # 发送控制按钮
        send_control_row = Frame(send_frame)
        send_control_row.pack(fill=X, pady=5)

        self.send_btn = Button(send_control_row, text="开始发送", command=self.toggle_sending)
        self.send_btn.pack(side=LEFT, padx=5)

        Button(send_control_row, text="手动发送", command=self.send_once).pack(side=LEFT, padx=5)

        # 数据解析设置框架
        parse_frame = LabelFrame(left_frame, text="数据解析设置")
        parse_frame.pack(fill=X, pady=5)

        # 数据格式选择
        parse_format_row = Frame(parse_frame)
        parse_format_row.pack(fill=X, pady=5)

        Label(parse_format_row, text="接收格式:").pack(side=LEFT, padx=5)
        self.recv_format = ttk.Combobox(parse_format_row, width=12,
                                        values=["自动检测", "十六进制", "ASCII", "4字节浮点", "2字节整数"])
        self.recv_format.set("自动检测")
        self.recv_format.pack(side=LEFT, padx=5)

        # 数据起始位置
        parse_pos_row = Frame(parse_frame)
        parse_pos_row.pack(fill=X, pady=5)

        Label(parse_pos_row, text="数据起始位置:").pack(side=LEFT, padx=5)
        self.data_start = Entry(parse_pos_row, width=5)
        self.data_start.insert(0, "0")
        self.data_start.pack(side=LEFT, padx=5)

        Label(parse_pos_row, text="数据长度:").pack(side=LEFT, padx=5)
        self.data_length = Entry(parse_pos_row, width=5)
        self.data_length.insert(0, "4")
        self.data_length.pack(side=LEFT, padx=5)

        # 右侧显示区域
        right_frame = Frame(main_frame)
        right_frame.pack(side=RIGHT, fill=BOTH, expand=True)

        # 数据显示区域
        data_frame = LabelFrame(right_frame, text="接收数据")
        data_frame.pack(fill=BOTH, pady=5)

        # 文本框用于显示原始数据
        self.text_area = scrolledtext.ScrolledText(data_frame, height=10)
        self.text_area.pack(fill=BOTH, expand=True, pady=5)

        # 绘图区域
        plot_frame = LabelFrame(right_frame, text="实时曲线")
        plot_frame.pack(fill=BOTH, expand=True, pady=5)

        # 创建图形
        self.fig, self.ax = plt.subplots(figsize=(8, 5))
        self.canvas = FigureCanvasTkAgg(self.fig, master=plot_frame)
        self.canvas.get_tk_widget().pack(fill=BOTH, expand=True)

        # 状态栏
        self.status = Label(self.root, text="就绪", bd=1, relief=SUNKEN, anchor=W)
        self.status.pack(side=BOTTOM, fill=X)

        # 初始化端口列表
        self.refresh_ports()

    def init_plot(self):
        self.ax.clear()
        self.ax.set_xlabel('时间')
        self.ax.set_ylabel('数值')
        self.ax.set_title('串口数据实时曲线')
        self.ax.grid(True, linestyle='--', alpha=0.7)
        self.line, = self.ax.plot([], [], 'b-', linewidth=1)
        self.canvas.draw()

    def refresh_ports(self):
        ports = serial.tools.list_ports.comports()
        port_list = [f"{port.device} - {port.description}" for port in ports]
        self.port_combo['values'] = port_list
        if port_list:
            self.port_combo.set(port_list[0])

    def toggle_serial(self):
        if self.serial_port and self.serial_port.is_open:
            self.stop_reading()
            self.connect_btn.config(text="打开串口")
            self.status.config(text="串口已关闭")
        else:
            if self.open_serial():
                self.connect_btn.config(text="关闭串口")
                self.status.config(text="串口已打开，等待数据...")

    def open_serial(self):
        try:
            # 从组合框中提取端口设备名
            port_full = self.port_combo.get()
            port_device = port_full.split(' - ')[0] if ' - ' in port_full else port_full

            baudrate = int(self.baud_combo.get())

            # 获取串口参数
            bytesize = int(self.data_bits_combo.get())
            stopbits = float(self.stop_bits_combo.get())
            parity_map = {"无": 'N', "奇校验": 'O', "偶校验": 'E'}
            parity = parity_map[self.parity_combo.get()]

            self.serial_port = serial.Serial(
                port=port_device,
                baudrate=baudrate,
                bytesize=bytesize,
                stopbits=stopbits,
                parity=parity,
                timeout=1
            )

            # 启动数据读取线程
            self.is_reading = True
            self.read_thread = Thread(target=self.read_serial_data)
            self.read_thread.daemon = True
            self.read_thread.start()

            return True

        except Exception as e:
            messagebox.showerror("错误", f"打开串口失败: {str(e)}")
            return False

    def stop_reading(self):
        self.is_reading = False
        self.stop_sending()
        if self.serial_port and self.serial_port.is_open:
            self.serial_port.close()

    def read_serial_data(self):
        while self.is_reading and self.serial_port and self.serial_port.is_open:
            try:
                # 读取可用数据
                if self.serial_port.in_waiting > 0:
                    # 读取所有可用数据
                    data = self.serial_port.readline()

                    # 在UI线程中处理数据
                    self.root.after(0, self.process_data, data)

            except Exception as e:
                self.root.after(0, lambda: self.status.config(text=f"读取错误: {str(e)}"))
                break

    def process_data(self, data):
        try:
            # 显示原始数据
            hex_str = ' '.join([f'{b:02X}' for b in data])
            ascii_str = ''.join([chr(b) if 32 <= b <= 126 else '.' for b in data])

            self.text_area.insert(END, f"HEX: {hex_str}\n")
            self.text_area.insert(END, f"ASCII: {ascii_str}\n")
            self.text_area.insert(END, "-" * 50 + "\n")
            self.text_area.see(END)

            # 解析数据
            value = self.parse_received_data(data)

            if value is not None:
                self.data_buffer.append(value)

                # 限制数据缓冲区大小
                if len(self.data_buffer) > self.max_data_points:
                    self.data_buffer.pop(0)

                # 更新图表
                self.update_plot()

        except Exception as e:
            self.text_area.insert(END, f"处理数据错误: {str(e)}\n")

    def parse_received_data(self, data):
        """解析接收到的数据，返回数值"""

        format_type = self.recv_format.get()
        start_pos = int(self.data_start.get())
        # data_len = int(self.data_length.get())
        data_len = int(data.hex().replace('0x', '')[2:4], 16)
        data = data[2:data_len + 4]
        # 检查数据长度是否足够
        if len(data) < start_pos + data_len:
            return None

        try:
            if format_type == "自动检测":
                # 尝试多种解析方式
                try:
                    # 尝试解析为4字节浮点数
                    if len(data) >= start_pos + 4:
                        value = struct.unpack('f', data[start_pos:start_pos + 4])[0]
                        return round(value, 4)
                except:
                    pass

                try:
                    # 尝试解析为2字节整数
                    if len(data) >= start_pos + 2:
                        value = struct.unpack('h', data[start_pos:start_pos + 2])[0]
                        return value
                except:
                    pass

                # 尝试解析为ASCII数字
                try:
                    ascii_str = data.decode('ascii').strip()
                    if ascii_str and ascii_str.replace('.', '', 1).isdigit():
                        return float(ascii_str)
                except:
                    pass

                # 默认返回第一个字节的整数值
                return data[0]

            elif format_type == "十六进制":
                # 返回十六进制数据的十进制值
                hex_str = ''.join([f'{b:02X}' for b in data[start_pos:start_pos + data_len]])
                return int(hex_str, 16)

            elif format_type == "ASCII":
                # 解析ASCII字符串为数字
                ascii_str = data[start_pos:start_pos + data_len].decode('ascii').strip()
                return float(ascii_str)

            elif format_type == "4字节浮点":
                # 解析为4字节浮点数
                return struct.unpack('f', data[start_pos:start_pos + 4])[0]

            elif format_type == "2字节整数":
                # 解析为2字节整数
                return struct.unpack('h', data[start_pos:start_pos + 2])[0]

        except Exception as e:
            self.text_area.insert(END, f"解析错误: {str(e)}\n")
            return None

        return None

    def prepare_send_data(self):
        """准备要发送的数据"""
        format_type = self.send_format.get()
        data_str = self.send_data_entry.get().strip()

        try:
            if format_type == "十六进制":
                # 处理十六进制数据
                hex_str = data_str.replace(' ', '')
                if len(hex_str) % 2 != 0:
                    hex_str = '0' + hex_str  # 补齐奇数个字符
                return bytes.fromhex(hex_str)

            elif format_type == "ASCII":
                # ASCII文本直接编码
                return data_str.encode('ascii')

            elif format_type == "十进制":
                # 将十进制数字转换为字节
                num = int(data_str)
                # 根据数字大小决定字节数
                if num <= 0xFF:
                    return bytes([num])
                elif num <= 0xFFFF:
                    return struct.pack('H', num)
                else:
                    return struct.pack('I', num)

        except Exception as e:
            messagebox.showerror("错误", f"准备发送数据失败: {str(e)}")
            return None

        return None

    def send_once(self):
        """手动发送一次数据"""
        if not self.serial_port or not self.serial_port.is_open:
            messagebox.showwarning("警告", "请先打开串口")
            return

        data = self.prepare_send_data()
        if data:
            try:
                self.serial_port.write(data)
                self.status.config(text=f"已发送: {len(data)} 字节")
            except Exception as e:
                messagebox.showerror("错误", f"发送数据失败: {str(e)}")

    def toggle_sending(self):
        """切换自动发送状态"""
        if self.is_sending:
            self.stop_sending()
            self.send_btn.config(text="开始发送")
            self.status.config(text="已停止自动发送")
        else:
            self.start_sending()
            self.send_btn.config(text="停止发送")
            self.status.config(text="开始自动发送数据")

    def start_sending(self):
        """开始自动发送数据"""
        try:
            self.send_interval = int(self.interval_entry.get())
            if self.send_interval < 10:
                messagebox.showwarning("警告", "发送间隔不能小于10ms")
                return

            self.is_sending = True
            self.schedule_send()

        except ValueError:
            messagebox.showerror("错误", "请输入有效的发送间隔")

    def stop_sending(self):
        """停止自动发送"""
        self.is_sending = False
        if self.send_timer:
            self.root.after_cancel(self.send_timer)
            self.send_timer = None

    def schedule_send(self):
        """安排下一次发送"""
        if self.is_sending:
            self.send_once()
            self.send_timer = self.root.after(self.send_interval, self.schedule_send)

    def update_plot(self):
        if not self.data_buffer:
            return

        self.ax.clear()
        self.ax.grid(True, linestyle='--', alpha=0.7)
        self.ax.set_xlabel('时间')
        self.ax.set_ylabel('数值')
        self.ax.set_title('串口数据实时曲线')

        # 绘制数据
        x_data = list(range(len(self.data_buffer)))
        self.ax.plot(x_data, self.data_buffer, 'b-', linewidth=1)

        # 自动调整Y轴范围，保留一些边距
        if len(self.data_buffer) > 0:
            y_min = min(self.data_buffer)
            y_max = max(self.data_buffer)
            margin = (y_max - y_min) * 0.1 if y_max != y_min else 1
            self.ax.set_ylim(y_min - margin, y_max + margin)

        self.canvas.draw()

    def clear_data(self):
        self.data_buffer = []
        self.text_area.delete(1.0, END)
        self.update_plot()
        self.status.config(text="数据已清空")

    def apply_settings(self):
        try:
            new_max_points = int(self.points_entry.get())
            if new_max_points > 0:
                self.max_data_points = new_max_points
                # 调整缓冲区大小
                if len(self.data_buffer) > self.max_data_points:
                    self.data_buffer = self.data_buffer[-self.max_data_points:]
                self.status.config(text=f"最大数据点数已设置为: {new_max_points}")
            else:
                messagebox.showerror("错误", "数据点数必须为正整数")
        except ValueError:
            messagebox.showerror("错误", "请输入有效的整数")

    def __del__(self):
        self.stop_reading()


if __name__ == "__main__":
    root = Tk()
    app = SerialPlotter(root)


    # 程序关闭时的清理工作
    def on_closing():
        app.stop_reading()
        root.destroy()


    root.protocol("WM_DELETE_WINDOW", on_closing)
    root.mainloop()
