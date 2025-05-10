import datetime
import sys
import threading
import time
from binascii import a2b_hex
import Com.Port.check as ck
import Com.Port.serialport as sp
import Pump.pump_cmd as pm


class pusimodbus:
    def __init__(self):
        self.modbus_ser = sp.ser
        self.address = 1
        self.pusi_modbus_cmd = pm.pusi_modbus_cmd(self.address)
        self.send_data = ''
        self.get_data = ''
        self.fun_type = {
            '置零步数': 32, '置零速度': 32, '节点名称': 32, '硬件版本': 32, '软件版本': 32, '软件控制': 16, 'ID': 16,
            '波特率': 32, '错误状态': 16, '控制器状态': 16, '方向': 16, '最大速度': 32, '相对位移': 32, '绝对位移': 32,
            '停止': 16, '启动速度': 16, '停止速度': 16, '加速度系数': 8, '减速度系数': 8, '细分': 16,
            '最大相电流': 16, '电机位置': 32, '实时速度': 32, 'GPIO值': 16, '阀开关': 16, '工作模式': 8
        }
        self.error_state = {
            1: '非法功能码', 2: '非法数据地址', 3: '非法数据值', 4: '从站设备失败', 5: '确定', 6: '从站设备忙',
            8: '存储奇偶性错误'
        }

    def Reset_Address(self, addr: int):
        self.address = addr
        self.pusi_modbus_cmd = pm.pusi_modbus_cmd(self.address)

    def Modbus_Send(self, dat_str: str):
        """
        发送数据
        :param dat_str:数据字符串->hex形式
        :return: 0：发送失败；1：发送成功
        """
        self.send_data = dat_str
        print(datetime.datetime.now(), end=':')
        if self.modbus_ser.PortSend(bytes.fromhex(dat_str)) == -1:
            print('数据发送失败')
            return 0
        print('Send:', dat_str)
        return 1

    def Modbus_Receive(self, timeout=2000):
        """
        pusi_modbus接收数据
        :param timeout: 返回超时时间，单位ms
        :return: ->int:0：无返回 -1:返回数据不符合协议 else：返回接收数据字节个数
        """
        self.modbus_ser.PortClean()
        self.get_data = ''
        while True:
            if self.modbus_ser.PortReceive_byte(timeout):
                self.get_data += self.modbus_ser.receive_buf
                if len(self.get_data) > 4:
                    crc16_modbus = ck.CRC16_MODBUS(self.get_data[:-4])
                    if crc16_modbus == self.get_data[-4:]:
                        print(datetime.datetime.now(), end=':')
                        print('Rece:', self.get_data)
                        return len(self.get_data) // 2
            elif len(self.get_data) == 0:
                print(datetime.datetime.now(), end=':')
                print('pusi_modbus:数据无返回！')
                return 0
            else:
                print(datetime.datetime.now(), end=':')
                print('pusi_modbus:数据返回错误！')
                return -1

    def Get_Rece_Data(self, data):
        """
        获取返回数据值
        :param data: 返回数据字符串
        :return: 数据：str
        """
        data_len = int(data[4:6], 16) * 2
        data_hex = data[6:6 + data_len]
        if self.send_data[4:8] in ['101A', '1022', '1024']:
            data_str = a2b_hex(data_hex).decode()
        else:
            data_str = str(int(data_hex, 16))
        return data_str

    def Modbus_Write(self, func: str, data: int):
        """
        modbus写寄存器功能
        :param func: 功能字符串
        :param data: int型数据
        :return: 0：发送失败；1：发送成功
        """
        if self.fun_type[func] == 32:
            if data > 0xFFFF:
                dat_hex = hex(data).replace('0x', '').zfill(8).upper()
                data_lst = [dat_hex[:4], dat_hex[4:]]
            elif data < 0:
                dat_hex = pm.Negative_To_Hex(data).zfill(8).upper()
                data_lst = [dat_hex[:4], dat_hex[4:]]
            else:
                dat_hex = hex(data).replace('0x', '').zfill(4).upper()
                data_lst = ['0000', dat_hex]
            send_str = self.pusi_modbus_cmd.Write_Mul_Cmd(func, data_lst)
        else:
            send_str = self.pusi_modbus_cmd.Write_One_Cmd(func, data, 0)
        return self.Modbus_Send(send_str)

    def ModBus_Read(self, func):
        """
        modbus读寄存器功能
        :param func: 功能字符串
        :return: 0：发送失败；1：发送成功
        """
        send_str = self.pusi_modbus_cmd.Read_Cmd(func)
        return self.Modbus_Send(send_str)

    def Check_Rece_Ok(self):
        """
        通过返回数据判断发送数据是否执行
        :return: 0：执行；else：错误代码
        """
        if self.Modbus_Receive() > 0:
            rec_dat = self.get_data
            if int(rec_dat[2:4], 16) > 0x80:
                error_num = int(rec_dat[4:6], 16)
                print(self.error_state[error_num])
                return error_num
            else:
                return 0

    def Check_Modbus_State(self, timeout=2000):
        """
        轮询方式检查状态
        :param timeout: 超时时间
        :return: 0：空闲；-1：返回数据出现错误
        """
        error_len = 0
        while True:
            self.Modbus_Send(self.pusi_modbus_cmd.Read_Cmd('控制器状态'))
            if self.Modbus_Receive(timeout) > 0:
                state = int(self.get_data[6:10], 16)
                bit3_state = (state >> 3) & 0x01
                bit8_state = (state >> 8) & 0x01
                if bit3_state == 0 and bit8_state == 0:
                    return 0
                time.sleep(0.01)
            else:
                error_len += 1
            if error_len > 3:
                return -1

    def Move_Set(self, sub: int, speed: int, dire: int):
        """
        移动之前设置相关参数，顺序为细分、最大速度、方向
        :param sub: 细分
        :param speed: 最大速度
        :param dire: 方向
        """
        cmd_lst = ['细分', '最大速度', '方向']
        par_lst = [sub, speed, dire]
        for _i in range(3):
            self.Modbus_Write(cmd_lst[_i], par_lst[_i])
            if self.Check_Rece_Ok() > 0:
                return 1
        return 0


class vlg_pusi:
    def __init__(self):
        self.pusi_ser = sp.ser
        self.protocol = 'PUSI'
        self.mode = '232'
        self.address = 255
        self.state = 0
        self.program_version = ''
        self.pusi_cmd = pm.pusi_cmd(self.address, self.mode)
        self.send_data = ''
        self.rec_data = ''

    def GetAddress(self):
        print('pusi:查询柱塞泵地址:')
        check_cmd = self.pusi_cmd.pusicmd('查询控制器状态1')
        self.Pusi_Send(check_cmd)
        rec_state = self.Pusi_Receive()
        if rec_state:
            print('pusi:获取地址成功：', 255)
            self.address = 255
            return 1
        for ad in range(0, 255):
            self.pusi_cmd = pm.pusi_cmd(ad, self.mode)
            check_cmd = self.pusi_cmd.pusicmd('查询控制器状态1')
            self.pusi_ser.PortSend(check_cmd)
            rec_state = self.pusi_ser.PortReceive_byte(500)
            if rec_state == 0:
                continue
            else:
                print('pusi:获取地址成功：', ad)
                self.address = ad
                return 1
        print('pusi:获取地址失败!')
        return 0

    def Pusi_Send(self, data: str):
        self.send_data = data
        # print(datetime.datetime.now(), end=':')
        # print('Send:', data)
        return self.pusi_ser.PortSend(bytes.fromhex(data))

    def Pusi_Receive(self, timeout=500):
        """
        pusi接收数据
        :param timeout: 返回超时时间，单位ms
        :return: True:返回正确，False：返回错误
        """
        self.pusi_ser.PortClean()
        pusi_len = 8
        while True:
            rec_byte = self.pusi_ser.PortReceive_Data(pusi_len, timeout)
            if rec_byte == pusi_len:
                if self.Check_CmdReceive_Ok(self.pusi_ser.receive_buf):
                    self.rec_data = self.pusi_ser.receive_buf
                    # print(datetime.datetime.now(), end=':')
                    # print('Rece:', self.rec_data)
                    return True
                else:
                    print(datetime.datetime.now(), end=':')
                    print('pusi：数据错误！')
                    return False
            else:
                print(datetime.datetime.now(), end=':')
                print('pusi：数据无返回！')
                return False

    def Check_CmdReceive_Ok(self, dat: str):
        dat = dat.upper()
        right_header = 'A5'
        rec_flag = '7A'
        right_hex_address = hex(self.address).replace('0x', '').zfill(2).upper()
        right_checksum = ck.Uchar_Checksum_8_Bit(dat[0:-2])
        if right_header == dat[0:2]:
            if rec_flag == dat[2:4]:
                if right_hex_address == dat[4:6]:
                    if right_checksum == dat[-2:]:
                        return True
                    else:
                        print('pusi：和校验错误')
                        return False
                else:
                    print('pusi：地址错误')
                    return False
            else:
                print('pusi：返回标志错误')
                return False
        else:
            print('pusi：帧头错误')
            return False

    def Check_PusiDriver_State(self, timeout=500):
        while True:
            self.Pusi_Send(self.pusi_cmd.pusicmd('查询控制器状态1'))
            if self.Pusi_Receive(timeout):
                int_state = (int(self.rec_data[6:8], 16) & 0x01)
                if int_state == 0:
                    return True
                else:
                    # time.sleep(0.01)
                    continue
            else:
                return False


def Rec_Data_Config(rec_str: str):
    fin_data = ''
    rec_str = rec_str[6:-2]
    for k in range(0, len(rec_str) + 1, 2):
        tmp = rec_str[k:k + 2]
        fin_data = tmp + fin_data
    return int(fin_data, 16)


lock = threading.Lock()
if __name__ == '__main__':
    sp.Reset_Ser_Baud(0, 'com41', 9600)
    pusi = vlg_pusi()
    pusi.mode = '485'
    while True:
        pusi.Pusi_Send(pusi.pusi_cmd.pusicmd('置零', 64000))
        pusi.Pusi_Receive()
        pusi.Check_PusiDriver_State()
        pusi.Pusi_Send(pusi.pusi_cmd.pusicmd('查询当前位置'))
        pusi.Pusi_Receive()
        zero_pos = int(pusi.rec_data[6:8], 16)
        print('zero_pos:', zero_pos)
        if zero_pos != 0:
            print('error position:', pusi.rec_data)
            sys.exit()
        pusi.Pusi_Send(pusi.pusi_cmd.pusicmd('设置方向', 1))
        pusi.Pusi_Receive()
        pusi.Pusi_Send(pusi.pusi_cmd.pusicmd('移动', 28800))
        pusi.Pusi_Receive()
        if int(pusi.rec_data[6:8], 16) != 0:
            print('error status:', pusi.rec_data)
            sys.exit()
        pusi.Check_PusiDriver_State()

    # pusi.Pusi_Send(pusi.pusi_cmd.pusicmd('设置加速度系数', 5))
    # pusi.Pusi_Receive()
    # pusi.Pusi_Send(pusi.pusi_cmd.pusicmd('设置减速度系数', 5))  # 944000
    # pusi.Pusi_Receive()
    # pusi.Pusi_Send(pusi.pusi_cmd.pusicmd('设置启动速度', 200))
    # pusi.Pusi_Receive()
    # pusi.Pusi_Send(pusi.pusi_cmd.pusicmd('设置停止速度', 200))
    # pusi.Pusi_Receive()
    # pusi.Pusi_Send(pusi.pusi_cmd.pusicmd('设置方向', 1))
    # pusi.Pusi_Receive()
    # pusi.Pusi_Send(pusi.pusi_cmd.pusicmd('查询控制器状态1'))
    # pusi.Pusi_Receive()
