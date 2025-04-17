import datetime
import random
import sys
import time
import serial
import serial.tools.list_ports


def RunTime(func):
    def log(*args, **kwargs):
        print(datetime.datetime.now(), end=':')
        tmp = func(*args, **kwargs)
        return tmp

    return log


class SERIALPORT:
    """
    初始化串口，参数包括串口号以及波特率
    """

    def __init__(self, port: str, baud: int):
        self.port = port
        self.baud = baud
        self.open_com = serial.Serial()
        self.open_com.port = self.port
        self.open_com.baudrate = self.baud
        self.send_buf = ''
        self.receive_buf = ''

    def OpenPort(self):
        """
        打开串口
        :return:True：打开成功；False:打开失败
        """
        if self.open_com.is_open:
            print(self.port + '连接成功！')
            return True
        else:
            self.open_com.open()
            if self.open_com.is_open:
                print(self.port + '连接成功！')
                return False
            else:
                print(self.port + '连接失败！')
                return False

    def ClosePort(self):
        """
        关闭串口
        :return:True：关闭成功；False：关闭失败
        """
        self.open_com.close()
        if self.open_com.is_open:
            print(self.port + "关闭失败！")
            return False
        else:
            print(self.port + "关闭成功！")
            return True

    def PortSend(self, data: bytes):
        """
        通过串口发送数据
        :param data: 字节类型
        :return: True:发送成功，False：发送失败
        """
        self.send_buf = data
        if self.open_com.is_open:
            length = self.open_com.write(data)
            if length == len(data) // 2:
                return True
            else:
                return False
        else:
            print(self.port + "未打开！")
            return False

    def PortReceive_Data(self, data_len: int, over_time=1000):
        """
        返回data_len个字节数据
        :param data_len: 接收字节个数
        :param over_time: 接收超时时间 0:不检测超时
        :return: 0：无返回值；else：接收到字节个数
        """
        if over_time == 0:
            while True:
                if self.open_com.in_waiting >= data_len:
                    self.receive_buf = self.open_com.read(data_len).hex().upper()
                    return data_len
        else:
            start_time = time.perf_counter()
            while True:
                end_time = time.perf_counter()
                if (end_time - start_time) * 1000 < over_time:
                    if self.open_com.in_waiting >= data_len:
                        self.receive_buf = self.open_com.read(data_len).hex().upper()
                        return data_len
                else:
                    return 0

    def PortClean(self):
        self.send_buf = ''
        self.receive_buf = ''
        self.open_com.reset_input_buffer()
        self.open_com.reset_output_buffer()


def List_Serial_Ports():
    """
    列出所有可用的串口
    :return:所有可用串口列表
    """
    # 获取所有可用的串口
    ports = serial.tools.list_ports.comports()
    port_list = []
    for port in ports:
        port_list.append(port.device)
    return port_list


ser = SERIALPORT(List_Serial_Ports()[0], 9600)  # 控制器总线串口输入
bal = SERIALPORT('com30', 38400)  # 天平串口输入


def Reset_Ser_Baud(com, baud):
    """
    重新设置串口号以及波特率
    :param com:串口号：str
    :param baud:波特率：int
    """
    global ser
    ser.ClosePort()
    ser = SERIALPORT(com, baud)
    ser.OpenPort()


if __name__ == '__main__':
    Reset_Ser_Baud('com56', 9600)
    ser.OpenPort()
    check_stat_cmd = 'A5FF6A000000000E'  # 检查状态
    init_cmd = 'A5FF3800000000DC'  # 初始化
    reverse_direction_cmd = 'A5FF640100000009'  # 反向
    forward_direction_cmd = 'A5FF640000000008'  # 正向
    move_cmd = 'A5FF73B80B0000DA'  # 运动3000 ustep
    count = 0
    while True:
        ser.PortClean()
        ser.PortSend(bytes.fromhex(hex(random.randint(0, 255)).replace('0x', '').zfill(2) * random.randint(1, 10)))
        idex = random.randrange(0, 17, 2)
        ser.PortSend(bytes.fromhex(check_stat_cmd[0:idex]))
        time.sleep(random.random() * 2)
        ser.PortSend(bytes.fromhex(check_stat_cmd[idex:16]))
        if not ser.PortReceive_Data(8, 1000):
            print('无返回')
            print('count:', count)
            sys.exit()
        count += 1
        print('count:', count)
