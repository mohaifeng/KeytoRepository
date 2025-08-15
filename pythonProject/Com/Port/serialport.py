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


class serialport:
    """
    初始化串口，参数包括串口号以及波特率
    """

    def __init__(self, port: str, baud: int):
        self.port = port
        self.baud = baud
        self.open_com = serial.Serial()
        self.open_com.port = self.port
        self.open_com.baudrate = self.baud
        self.tx_buf = ''
        self.rx_buf = ''

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
        self.tx_buf = data
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
                    self.rx_buf = self.open_com.read(data_len).hex().upper()
                    return data_len
        else:
            start_time = time.perf_counter()
            while True:
                end_time = time.perf_counter()
                if (end_time - start_time) * 1000 < over_time:
                    if self.open_com.in_waiting >= data_len:
                        self.rx_buf = self.open_com.read(data_len).hex().upper()
                        return data_len
                else:
                    return 0

    def Wait_Rx_Finish(self, timeout=1000):
        """
        等待接收完成，5ms没有接收到数据视为接收完成
        :param timeout: 超时时间：毫秒,0:不检测超时
        :return: 0：无返回，else：接收到数据字节个数
        """
        rx_buf = ''
        if self.PortReceive_Data(1, timeout):
            rx_buf += self.rx_buf
            while True:
                if self.PortReceive_Data(1, 5):
                    rx_buf += self.rx_buf
                    continue
                else:
                    print(datetime.datetime.now(), end=':')
                    print('RX:', rx_buf)
                    return len(rx_buf) // 2
        else:
            print(datetime.datetime.now(), end=':')
            print('数据无返回！')
            return 0

    def PortClean(self):
        self.tx_buf = ''
        self.rx_buf = ''
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


ser = serialport(List_Serial_Ports()[0], 9600)  # 控制器总线串口输入
bal = serialport(List_Serial_Ports()[0], 38400)  # 天平控制串口
pwr = serialport(List_Serial_Ports()[0], 9600)  # 电源控制串口


def Reset_Ser_Baud(ser_type, com, baud):
    """
    重新设置串口号以及波特率
    :param ser_type: 需要重设的串口类变量 0:ser;1:bal;2:pwr
    :param com:串口号：str
    :param baud:波特率：int
    """
    global ser, bal, pwr
    if ser_type == 0:
        ser.ClosePort()
        ser = serialport(com, baud)
        ser.OpenPort()
    elif ser_type == 1:
        bal.ClosePort()
        bal = serialport(com, baud)
        bal.OpenPort()
    else:
        pwr.ClosePort()
        pwr = serialport(com, baud)
        pwr.OpenPort()


test_lst = [
    '30 3E 49 74 31 30 30 30 20 31 30 30 0D 0A',
    '30 3E 56 63 31 0D 0A',
    '30 3E 49 61 30 20 31 30 30 30 20 31 30 30 0D 0A',
    '30 3E 49 61 35 30 30 30 20 31 30 30 30 20 31 30 30 0D',
    '30 3E 56 63 30 0D 04',
    '30 3E 4461 30 20 31 30 30 30 20 31 30 30 2C 30 OD',
    '30 3E 44 61 35 30 30 30 20 31 30 30 30 20 31 30 30 2C 30 0D'
]
if __name__ == '__main__':
    Reset_Ser_Baud(0, 'com56', 9600)
    ser.OpenPort()
    while True:
        for i in test_lst:
            ser.PortSend(bytes.fromhex(i))  # 置零
            ser.Wait_Rx_Finish()
            ser.Wait_Rx_Finish()
