import sys
import time
from binascii import a2b_hex
import Com.Port.check as ck
import Com.Port.serialport as sp
import Pump.pump_cmd as pm
import datetime
import Com.Protocol.protocol as pr
import Pump.pump_cmd as cmd


class step_drl_meteringpump:
    def __init__(self):
        self.drl_ser = sp.ser
        self.report_flag = 0  # 主动上报模式 0：关闭 1：打开
        self.protocol = 'KT_DT'  # KT_DT:keyto DT协议格式；KT_OEM：keyto OEM协议格式
        self.address = 0
        self.model = ''
        self.version = ''
        self.status = 0
        self.dt_prot = pr.keyto_dt_prot()
        self.oem_prot = pr.keyto_oem_prot()
        self.drl_cmd = cmd.drl_meteringpump_cmd()
        self.sys_cmd = cmd.sys_cmd()
        self.tx_data = ''
        self.rx_data = ''
        self.status_dic = {
            0: '空闲', 1: '忙', 2: '执行成功', 3: '执行完成', 10: '参数超限', 11: '参数错误', 12: '语法错误',
            13: '无效指令', 14: '地址错误', 15: '禁止写', 16: '禁止读',
            17: '未初始化', 50: '电机堵转', 51: '驱动器故障', 52: '光耦1错误', 53: '光耦2错误', 54: '传感器错误',
            55: 'EEPROM错误', 56: '电源欠压', 57: '电源过压', 58: '电机短路', 59: '电机开路'
        }

    def TxData_Conf(self, _cmd: str):
        if self.protocol == 'KT_DT':
            sd_data = self.dt_prot.Dt_Cmd_Conf(self.address, _cmd)
        else:
            sd_data = self.oem_prot.Oem_Cmd_Conf(self.address, _cmd)
        self.tx_data = sd_data

    def Transmit(self, send_cmd: str):
        """
        发送数据
        :param send_cmd:
        :return: False:发送失败；True：发送成功
        """
        self.TxData_Conf(send_cmd)
        print(datetime.datetime.now(), end=':')
        print('Send:', self.tx_data, '->', 'addr:', self.address, 'Data:', bytes.fromhex(send_cmd).decode())
        return self.drl_ser.PortSend(bytes.fromhex(self.tx_data))

    def RxData_Analysis(self):
        if self.protocol == 'KT_DT':
            return self.dt_prot.Rec_Data_Conf(self.rx_data)
        else:
            return self.oem_prot.Rec_Data_Conf(self.rx_data)

    def Wait_Rx_Finish(self, timeout=1000):
        """
        DRL获取返回数据
        :param timeout: 超时时间：毫秒,0:不检测超时
        :return: 0：无返回，else：接收到数据字节个数
        """
        self.rx_data = ''
        if self.drl_ser.PortReceive_Data(1, timeout):
            self.rx_data += self.drl_ser.rx_buf
            while True:
                if self.drl_ser.PortReceive_Data(1, 5):
                    self.rx_data += self.drl_ser.rx_buf
                    continue
                else:
                    if self.RxData_Analysis():
                        print(datetime.datetime.now(), end=':')
                        print('Rece:', self.rx_data)
                        return len(self.rx_data) // 2
                    else:
                        print('数据格式有误！')
                        print('Rece:', self.rx_data)
                        return 0
        else:
            print(datetime.datetime.now(), end=':')
            print('数据无返回！')
            return 0

    def RxData_Formate(self):
        """
        处理返回数据
        :return: 返回元组，包括（地址+状态+数据为元组类型）
        """
        if self.protocol == 'KT_OEM':  # OEM协议判断
            addr = self.oem_prot.rec_addr
            self.status = self.oem_prot.state
            data = self.oem_prot.rx_data
            if self.oem_prot.data_len:
                if self.oem_prot.gap_flag in data:
                    config_data_tuple = tuple(int(a2b_hex(x).decode(), 16) for x in data.split(self.oem_prot.gap_flag))
                    dat_tup = (addr, config_data_tuple)
                else:
                    dat_tup = (addr, (int(a2b_hex(data).decode(), 16),))
            else:
                dat_tup = (addr, ())
        else:
            addr = self.dt_prot.rec_addr
            self.status = self.dt_prot.state
            data = self.dt_prot.rx_data
            if data:
                if self.dt_prot.gap_flag in data:
                    config_data_tuple = tuple(int(a2b_hex(x).decode()) for x in data.split(self.dt_prot.gap_flag))
                    dat_tup = (addr, config_data_tuple)
                else:
                    dat_tup = (addr, (int(a2b_hex(data).decode()),))
            else:
                dat_tup = (addr, ())
        return dat_tup

    def Get_Address(self):
        """
        获取DRL地址
        :return: True：获取成功；False：获取失败
        """
        print('查询地址:')
        self.Transmit(self.sys_cmd.Check_State())
        if self.Wait_Rx_Finish():
            print('获取地址成功，地址为：', self.address)
            return True
        for ad in range(1, 256):
            self.address = ad
            self.Transmit(self.sys_cmd.Check_State())
            if not self.Wait_Rx_Finish():
                continue
            else:
                print('获取地址成功，地址为：', ad)
                self.address = ad
                return True
        print('获取地址失败！')
        return False

    def Get_Version(self):
        """
        获取DRL软件版本号
        :return: True：获取成功；False：获取失败
        """
        print('获取版本:')
        self.Transmit(self.sys_cmd.Rr(90))
        if self.Wait_Rx_Finish():
            self.version = str(self.RxData_Formate()[1][0])
            print('版本获取成功：', self.version)
            return True
        else:
            print('获取版本失败！')
            return False

    def ReportingConfig(self, dat: int):
        """
        :param dat: 0:关闭主动上报；1：打开主动上报
        :return:1:设置成功；0：设置失败
        """
        print('设置主动上报')
        self.Transmit(self.sys_cmd.Wr(82, dat))
        if self.Wait_Rx_Finish():
            self.RxData_Formate()
            if self.status == 2:
                print('主动上报设置成功!')
                self.report_flag = dat
                return True
        print('主动上报设置失败')
        return False

    def Wait_StatusIdle(self, delay=10):  # 间隔时间：毫秒
        """
        轮询方式查询drl状态
        :return: 返回状态：0：状态空闲；else：其他错误状态。
        """
        print('等待空闲！')
        if self.report_flag == 0:
            while True:
                self.Transmit(self.sys_cmd.Check_State())
                if self.Wait_Rx_Finish():
                    self.RxData_Formate()
                    if self.status == 1:
                        time.sleep(delay / 1000)
                        continue
                    elif self.status == 0:
                        return True
                    else:
                        self.Error_Handle(self.status)
                        return False
                else:
                    print('查询状态指令无返回')
                    self.status = -1
                    return False
        else:
            self.Transmit(self.sys_cmd.Check_State())
            if self.Wait_Rx_Finish(0):
                self.RxData_Formate()
                if self.status == 3:
                    return True
                else:
                    self.Error_Handle(self.status)
                    return False

    def Error_Handle(self, number: int):
        if number in self.status_dic.keys():
            print('状态错误', end=':')
            print(self.status_dic[self.status])
        else:
            print('无错误状态信息!')


class step_drl:
    def __init__(self):
        self.drl_ser = sp.ser
        self.report_flag = 0  # 主动上报模式 0：关闭 1：打开
        self.protocol = 'KT_DT'  # KT_DT:keyto DT协议格式；KT_OEM：keyto OEM协议格式
        self.address = 0
        self.model = ''
        self.version = ''
        self.status = 0
        self.dt_prot = pr.keyto_dt_prot()
        self.oem_prot = pr.keyto_oem_prot()
        self.drl_cmd = cmd.drl_pump_cmd()
        self.sys_cmd = cmd.sys_cmd()
        self.tx_data = ''
        self.rx_data = ''
        self.status_dic = {
            0: '空闲', 1: '忙', 2: '执行成功', 3: '执行完成', 10: '参数超限', 11: '参数错误', 12: '语法错误',
            13: '无效指令', 14: '地址错误', 15: '禁止写', 16: '禁止读',
            17: '未初始化', 50: '电机堵转', 51: '驱动器故障', 52: '光耦1错误', 53: '光耦2错误', 54: '传感器错误',
            55: 'EEPROM错误', 56: '电源欠压', 57: '电源过压', 58: '电机短路', 59: '电机开路'
        }

    def TxData_Conf(self, _cmd: str):
        if self.protocol == 'KT_DT':
            sd_data = self.dt_prot.Dt_Cmd_Conf(self.address, _cmd)
        else:
            sd_data = self.oem_prot.Oem_Cmd_Conf(self.address, _cmd)
        self.tx_data = sd_data

    def Transmit(self, send_cmd: str):
        """
        发送数据
        :param send_cmd:
        :return: False:发送失败；True：发送成功
        """
        self.TxData_Conf(send_cmd)
        print(datetime.datetime.now(), end=':')
        print('Send:', self.tx_data, '->', 'addr:', self.address, 'Data:', bytes.fromhex(send_cmd).decode())
        return self.drl_ser.PortSend(bytes.fromhex(self.tx_data))

    def RxData_Analysis(self):
        if self.protocol == 'KT_DT':
            return self.dt_prot.Rec_Data_Conf(self.rx_data)
        else:
            return self.oem_prot.Rec_Data_Conf(self.rx_data)

    def Wait_Rx_Finish(self, timeout=1000):
        """
        DRL获取返回数据
        :param timeout: 超时时间：毫秒,0:不检测超时
        :return: 0：无返回，else：接收到数据字节个数
        """
        self.rx_data = ''
        if self.drl_ser.PortReceive_Data(1, timeout):
            self.rx_data += self.drl_ser.rx_buf
            while True:
                if self.drl_ser.PortReceive_Data(1, 5):
                    self.rx_data += self.drl_ser.rx_buf
                    continue
                else:
                    if self.RxData_Analysis():
                        print(datetime.datetime.now(), end=':')
                        print('Rece:', self.rx_data)
                        return len(self.rx_data) // 2
                    else:
                        print('数据格式有误！')
                        print('Rece:', self.rx_data)
                        return 0
        else:
            print(datetime.datetime.now(), end=':')
            print('数据无返回！')
            return 0

    def RxData_Formate(self):
        """
        处理返回数据
        :return: 返回元组，包括（地址+状态+数据为元组类型）
        """
        if self.protocol == 'KT_OEM':  # OEM协议判断
            addr = self.oem_prot.rec_addr
            self.status = self.oem_prot.state
            data = self.oem_prot.rx_data
            if self.oem_prot.data_len:
                if self.oem_prot.gap_flag in data:
                    config_data_tuple = tuple(int(a2b_hex(x).decode(), 16) for x in data.split(self.oem_prot.gap_flag))
                    dat_tup = (addr, config_data_tuple)
                else:
                    dat_tup = (addr, (int(a2b_hex(data).decode(), 16),))
            else:
                dat_tup = (addr, ())
        else:
            addr = self.dt_prot.rec_addr
            self.status = self.dt_prot.state
            data = self.dt_prot.rx_data
            if data:
                if self.dt_prot.gap_flag in data:
                    config_data_tuple = tuple(int(a2b_hex(x).decode()) for x in data.split(self.dt_prot.gap_flag))
                    dat_tup = (addr, config_data_tuple)
                else:
                    dat_tup = (addr, (int(a2b_hex(data).decode()),))
            else:
                dat_tup = (addr, ())
        return dat_tup

    def Get_Address(self):
        """
        获取DRL地址
        :return: True：获取成功；False：获取失败
        """
        print('查询地址:')
        self.Transmit(self.sys_cmd.Check_State())
        if self.Wait_Rx_Finish():
            print('获取地址成功，地址为：', self.address)
            return True
        for ad in range(1, 256):
            self.address = ad
            self.Transmit(self.sys_cmd.Check_State())
            if not self.Wait_Rx_Finish():
                continue
            else:
                print('获取地址成功，地址为：', ad)
                self.address = ad
                return True
        print('获取地址失败！')
        return False

    def Get_Version(self):
        """
        获取DRL软件版本号
        :return: True：获取成功；False：获取失败
        """
        print('获取版本:')
        self.Transmit(self.sys_cmd.Rr(90))
        if self.Wait_Rx_Finish():
            self.version = str(self.RxData_Formate()[1][0])
            print('版本获取成功：', self.version)
            return True
        else:
            print('获取版本失败！')
            return False

    def ReportingConfig(self, dat: int):
        """
        :param dat: 0:关闭主动上报；1：打开主动上报
        :return:1:设置成功；0：设置失败
        """
        print('设置主动上报')
        self.Transmit(self.sys_cmd.Wr(82, dat))
        if self.Wait_Rx_Finish():
            self.RxData_Formate()
            if self.status == 2:
                print('主动上报设置成功!')
                self.report_flag = dat
                return True
        print('主动上报设置失败')
        return False

    def Wait_StatusIdle(self, delay=10):  # 间隔时间：毫秒
        """
        轮询方式查询drl状态
        :return: 返回状态：0：状态空闲；else：其他错误状态。
        """
        print('等待空闲！')
        if self.report_flag == 0:
            while True:
                self.Transmit(self.sys_cmd.Check_State())
                if self.Wait_Rx_Finish():
                    self.RxData_Formate()
                    if self.status == 1:
                        time.sleep(delay / 1000)
                        continue
                    elif self.status == 0:
                        return True
                    else:
                        self.Error_Handle(self.status)
                        return False
                else:
                    print('查询状态指令无返回')
                    self.status = -1
                    return False
        else:
            self.Transmit(self.sys_cmd.Check_State())
            if self.Wait_Rx_Finish(0):
                self.RxData_Formate()
                if self.status == 3:
                    return True
                else:
                    self.Error_Handle(self.status)
                    return False

    def Error_Handle(self, number: int):
        if number in self.status_dic.keys():
            print('状态错误', end=':')
            print(self.status_dic[self.status])
        else:
            print('无错误状态信息!')


class step_drs:
    def __init__(self):
        self.drs_ser = sp.ser
        self.protocol = 'KEYTO'  # KEYTO:keyto通用协议
        self.address = 0
        self.version = ''
        self.status = 0
        self.gen_prot = pr.keyto_gen_prot()
        self.gen_cmd = cmd.drs_cmd()
        self.tx_data = ''
        self.rx_data = ''
        self.status_dic = {
            0: '光耦错误',
            2: '光耦1触发状态',
            3: '运动完成',
            7: '光耦2触发状态',
            8: '驱动器故障',
            9: '堵转检测报警',
            11: '编码器故障',
            16: '其他故障',
        }

    def Get_Address(self):
        print('查询地址:')
        self.Transmit(self.gen_cmd.Read_State())
        if self.Wait_Rx_Finish():
            return True
        for ad in range(1, 256):
            self.gen_cmd.address = ad
            self.Transmit(self.gen_cmd.Read_State())
            if not self.Wait_Rx_Finish():
                continue
            else:
                print('获取地址成功：', ad)
                self.address = ad
                self.gen_cmd.address = ad
                return True
        print('获取地址失败!')
        return False

    def Get_Version(self):
        print('获取版本:')
        self.Transmit(self.gen_cmd.Read_Version())
        if self.Wait_Rx_Finish():
            self.version = str(self.RxData_Formate()[1])
            print('版本获取成功：', self.version)
            return True
        else:
            print('获取版本失败！')
            return False

    def TxData_Conf(self, _cmd: str):
        if self.protocol == 'KEYTO':
            sd_data = self.gen_prot.Gen_Cmd_Conf(self.address, _cmd)
        else:
            sd_data = self.gen_prot.Gen_Cmd_Conf(self.address, _cmd)
        self.tx_data = sd_data

    def Transmit(self, data: str):
        """
        发送数据
        :param data: str
        """
        self.TxData_Conf(data)
        print(datetime.datetime.now(), end=':')
        print('Send:', self.tx_data)
        return self.drs_ser.PortSend(bytes.fromhex(self.tx_data))

    def RxData_Analysis(self):
        if self.protocol == 'KEYTO':
            return self.gen_prot.Rec_Data_Conf(self.rx_data)
        else:
            return self.gen_prot.Rec_Data_Conf(self.rx_data)

    def Wait_Rx_Finish(self, timeout=500):
        """
        旋切阀接收数据
        :param timeout: 接收超时时间，默认1000ms
        :return: 0：返回数据有误 else：返回数据：str
        """
        self.rx_data = ''
        if self.drs_ser.PortReceive_Data(1, timeout):
            self.rx_data += self.drs_ser.rx_buf
            while True:
                if self.drs_ser.PortReceive_Data(1, 5):
                    self.rx_data += self.drs_ser.rx_buf
                    continue
                else:
                    if self.RxData_Analysis():
                        print(datetime.datetime.now(), end=':')
                        print('Rece:', self.rx_data)
                        return len(self.rx_data) // 2
                    else:
                        print('数据格式有误！')
                        print('Rece:', self.rx_data)
                        return 0
        else:
            print(datetime.datetime.now(), end=':')
            print('旋转阀:数据无返回！')
            return 0

    def RxData_Formate(self):
        if self.protocol == 'KEYTO':
            dat_tup = (self.gen_prot.rec_addr, self.gen_prot.rx_data)
        else:
            dat_tup = (self.gen_prot.rec_addr, self.gen_prot.rx_data)
        return dat_tup

    def Wait_StatusIdle(self, timeout=500):
        while True:
            self.Transmit(self.gen_cmd.Read_State())
            if self.Wait_Rx_Finish(timeout):
                if (self.RxData_Formate()[1] & 0x01) == 0:
                    return True
                else:
                    time.sleep(0.01)
                    continue
            else:
                return False

    def Error_Handle(self, number: int):
        if (number >> 8) in self.status_dic.keys():
            print(self.status_dic[number])
            self.status = number
        else:
            print('无错误状态信息!')


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
                self.get_data += self.modbus_ser.rx_buf
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
        self.pusi_name = ''
        self.pusi_version = ''
        self.pusi_prot = pr.pusi_prot()
        self.pusi_cmd = pm.pusi_cmd(self.mode)
        self.tx_data = ''
        self.rx_data = ''

    def Get_Pusi_Address(self):
        print('查询地址:')
        self.Pusi_Send(self.pusi_cmd.Read_State1())
        if self.Pusi_Receive():
            print('获取地址成功：', self.address)
            return True
        for ad in range(0, 255):
            self.address = ad
            self.Pusi_Send(self.pusi_cmd.Read_State1())
            if not self.Pusi_Receive():
                continue
            else:
                print('获取地址成功：', ad)
                self.address = ad
                return True
        print('获取地址失败!')
        return False

    def Get_Pusi_Version(self):
        print('获取版本:')
        self.Pusi_Send(self.pusi_cmd.Keyto_ReadVersion())
        if self.Pusi_Receive():
            self.pusi_version = str(self.Pusi_RecConf()[1])
            print('版本获取成功：', self.pusi_version)
            return True
        else:
            print('获取版本失败！')
            return False

    def Pusi_Send(self, data: str):
        self.tx_data = self.pusi_prot.Ps_Cmd_Conf(self.address, data)
        print(datetime.datetime.now(), end=':')
        print('Send:', self.tx_data)
        return self.pusi_ser.PortSend(bytes.fromhex(self.tx_data))

    def Protocol_Check(self):
        return self.pusi_prot.Rec_Data_Conf(self.rx_data)

    def Pusi_Receive(self, timeout=500):
        """
        pusi接收数据
        :param timeout: 接收超时时间，默认1000ms
        :return: 0：返回数据有误 else：返回数据：str
        """
        self.rx_data = ''
        if self.pusi_ser.PortReceive_Data(1, timeout):
            self.rx_data += self.pusi_ser.rx_buf
            while True:
                if self.pusi_ser.PortReceive_Data(1, 5):
                    self.rx_data += self.pusi_ser.rx_buf
                    continue
                else:
                    if self.Protocol_Check():
                        print(datetime.datetime.now(), end=':')
                        print('Rece:', self.rx_data)
                        return len(self.rx_data) // 2
                    else:
                        print('数据格式有误！')
                        print('Rece:', self.rx_data)
                        return 0
        else:
            print(datetime.datetime.now(), end=':')
            print('数据无返回！')
            return 0

    def Pusi_RecConf(self):
        dat_tup = (self.pusi_prot.rec_addr, self.pusi_prot.rx_data)
        return dat_tup

    def Check_Pusi_State(self, timeout=500):
        while True:
            self.Pusi_Send(self.pusi_cmd.Read_State1())
            if self.Pusi_Receive(timeout):
                if (self.Pusi_RecConf()[1] & 0x01) == 0:
                    return True
                else:
                    time.sleep(0.01)
                    continue
            else:
                return False


if __name__ == '__main__':
    sp.Reset_Ser_Baud(0, 'com36', 9600)
    pusi = vlg_pusi()
    pusi.Get_Pusi_Address()
    pusi.Pusi_Send(pusi.pusi_cmd.Init(0))
    if not pusi.Pusi_Receive():
        sys.exit()
    pusi.Check_Pusi_State()
    # drs = step_drs()
    # drs.Transmit(drs.gen_cmd.Valve_Control(2))
    # drs.Wait_Rx_Finish()
    # time.sleep(2)
    # drs.Transmit(drs.gen_cmd.Valve_Control(0))
    # drs.Wait_Rx_Finish()
