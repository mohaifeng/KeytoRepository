import Adp.adpcmd as cmd
import Com.Protocol.protocol as pr
from binascii import a2b_hex
import time
import datetime
import Com.Port.serialport as sp


class z_axis:
    def __init__(self):
        self.z_ser = sp.ser
        self.report_flag = 0  # 主动上报模式 0：关闭 1：打开
        self.protocol = 'KT_DT'  # KT_DT:keyto DT协议格式；KT_OEM：keyto OEM协议格式
        self.address = 41
        self.status = 0
        self.version = ''
        self.model = ''
        self.dt_prot = pr.keyto_dt_prot()
        self.oem_prot = pr.keyto_oem_prot()
        self.z_cmd = cmd.z_cmd()
        self.sys_cmd = cmd.sys_cmd()
        self.tx_data = ''
        self.rx_data = ''
        self.status_dic = {
            0: '空闲', 1: '忙', 2: '执行成功', 10: '参数超限', 11: '参数错误', 12: '语法错误', 13: '无效指令',
            14: '地址错误', 15: '禁止写', 16: '禁止读', 18: 'Z轴未初始化', 19: 'Z轴未初始化', 80: 'Z轴电机堵转',
            81: 'Z轴电机驱动器故障', 82: 'Z轴电机光耦错误', 83: 'Z轴存储错误', 84: 'Z轴未校准错误',
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
        print('TX:', self.tx_data, '->', 'addr:', self.address, 'Data:', bytes.fromhex(send_cmd).decode())
        return self.z_ser.PortSend(bytes.fromhex(self.tx_data))

    def RxData_Analysis(self):
        if self.protocol == 'KT_DT':
            return self.dt_prot.Rec_Data_Conf(self.rx_data)
        else:
            return self.oem_prot.Rec_Data_Conf(self.rx_data)

    def Wait_Rx_Finish(self, timeout=1000):
        """
        获取返回数据
        :param timeout: 超时时间：毫秒,0:不检测超时
        :return: 0：无返回，else：接收到数据字节个数
        """
        self.rx_data = ''
        if self.z_ser.PortReceive_Data(1, timeout):
            self.rx_data += self.z_ser.receive_buf
            while True:
                if self.z_ser.PortReceive_Data(1, 5):
                    self.rx_data += self.z_ser.receive_buf
                    continue
                else:
                    if self.RxData_Analysis():
                        print(datetime.datetime.now(), end=':')
                        print('RX:', self.rx_data)
                        return len(self.rx_data) // 2
                    else:
                        print('数据格式有误！')
                        print('RX:', self.rx_data)
                        return 0
        else:
            print(datetime.datetime.now(), end=':')
            print('数据无返回！')
            return 0

    def RxData_Formate(self):
        """
        处理返回数据
        :return: 返回元组，包括（地址+数据为元组类型）
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
        获取地址，范围1-32
        :return: True：获取成功；False：获取失败
        """
        print('查询地址:')
        for ad in range(self.address, 33):
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
        获取软件版本号
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
            if self.RxData_Formate()[1] == 2:
                print('主动上报设置成功!')
                self.report_flag = dat
                return True
        print('主动上报设置失败')
        return False

    def Wait_StatusIdle(self, delay=10):  # 间隔时间：毫秒
        """
        轮询方式查询状态
        :return: 返回状态：0：状态空闲；else：其他错误状态,-1:无返回。
        """
        # print('查询adp状态！')
        if self.report_flag == 0:
            while True:
                self.Transmit(self.sys_cmd.Check_State())
                if self.Wait_Rx_Finish():
                    self.RxData_Formate()
                    if self.status == 1:
                        time.sleep(delay / 1000)
                        continue
                    elif self.status == 0:
                        return 0
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
            print(self.status_dic[number])
            self.status = number
        else:
            print('无错误状态信息!')


class pipette:
    def __init__(self):
        self.adp_ser = sp.ser
        self.report_flag = 0  # 主动上报模式 0：关闭 1：打开
        self.protocol = 'KT_DT'  # KT_DT:keyto DT协议格式；KT_OEM：keyto OEM协议格式
        self.address = 1
        self.model = ''
        self.version = ''
        self.status = 0
        self.dt_prot = pr.keyto_dt_prot()
        self.oem_prot = pr.keyto_oem_prot()
        self.adp_cmd = cmd.adp_cmd()
        self.sys_cmd = cmd.sys_cmd()
        self.tx_data = ''
        self.rx_data = ''
        self.status_dic = {
            0: '空闲', 1: 'ADP忙', 2: '执行成功', 3: '探测到液面', 4: 'Z轴忙', 5: 'Z轴忙，ADP忙', 10: '参数超限',
            11: '参数错误', 12: '语法错误', 13: '无效指令', 14: '地址错误', 15: '禁止写', 16: '禁止读',
            17: 'ADP未初始化', 18: 'Z轴未初始化', 19: 'Z轴未连接', 20: '未检测到TIP', 21: '顶出TIP失败',
            22: '超时报警', 23: '吸堵检测报警', 24: '气泡检测报警', 25: '吸空检测报警', 26: '排气凝块报警',
            27: '排气气泡报警', 28: '抗悬滴范围超限', 50: '电机堵转', 51: '驱动器故障', 52: '光耦1错误',
            53: '光耦2错误', 54: '传感器错误', 55: 'EEPROM错误', 56: '电源欠压', 57: '电源过压', 58: '电机短路',
            59: '电机开路', 80: 'Z轴电机堵转', 81: 'Z轴电机驱动器故障', 82: 'Z轴电机光耦错误'
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
        print('TX:', self.tx_data, '->', 'addr:', self.address, 'Data:', bytes.fromhex(send_cmd).decode())
        return self.adp_ser.PortSend(bytes.fromhex(self.tx_data))

    def RxData_Analysis(self):
        if self.protocol == 'KT_DT':
            return self.dt_prot.Rec_Data_Conf(self.rx_data)
        else:
            return self.oem_prot.Rec_Data_Conf(self.rx_data)

    def Wait_Rx_Finish(self, timeout=1000):
        """
        获取返回数据
        :param timeout: 超时时间：毫秒,0:不检测超时
        :return: 0：无返回，else：接收到数据字节个数
        """
        self.rx_data = ''
        if self.adp_ser.PortReceive_Data(1, timeout):
            self.rx_data += self.adp_ser.receive_buf
            while True:
                if self.adp_ser.PortReceive_Data(1, 5):
                    self.rx_data += self.adp_ser.receive_buf
                    continue
                else:
                    if self.RxData_Analysis():
                        print(datetime.datetime.now(), end=':')
                        print('RX:', self.rx_data)
                        return len(self.rx_data) // 2
                    else:
                        print('数据格式有误！')
                        print('RX:', self.rx_data)
                        return 0
        else:
            print(datetime.datetime.now(), end=':')
            print('数据无返回！')
            return 0

    def RxData_Formate(self):
        """
        处理返回数据
        :return: 返回元组，包括（地址+数据为元组类型）
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
        获取地址，范围1-32
        :return: True：获取成功；False：获取失败
        """
        print('查询地址:')
        for ad in range(self.address, 33):
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
        获取软件版本号
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
            if self.RxData_Formate()[1] == 2:
                print('主动上报设置成功!')
                self.report_flag = dat
                return True
        print('主动上报设置失败')
        return False

    def Wait_StatusIdle(self, delay=10):  # 间隔时间：毫秒
        """
        轮询方式查询状态
        :return: 返回状态：0：状态空闲；else：其他错误状态,-1:无返回。
        """
        # print('查询adp状态！')
        if self.report_flag == 0:
            while True:
                self.Transmit(self.sys_cmd.Check_State())
                if self.Wait_Rx_Finish():
                    self.RxData_Formate()
                    if self.status == 1:
                        time.sleep(delay / 1000)
                        continue
                    elif self.status == 0:
                        return 0
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

    def Wait_PlldIdle(self, timeout=10000):
        """
        检测液面探测状态
        :param timeout: 超时时间：ms
        :return: 0：超时；1：探测到液面
        """
        check_cmd = self.sys_cmd.Rr(2)
        print('探测液面中！')
        if timeout:
            starttime = time.perf_counter()
            while (time.perf_counter() - starttime) * 1000 <= timeout:
                self.Transmit(check_cmd)
                if self.Wait_Rx_Finish():
                    if self.RxData_Formate()[1][0] == 1:
                        print('探测到液面！')
                        return True
                    else:
                        time.sleep(0.01)
                        continue
                else:
                    print('查询探测指令无返回', end=':')
                    time.sleep(0.01)
                    continue
            print('探测失败！')
            return False
        else:
            while True:
                self.Transmit(check_cmd)
                if self.Wait_Rx_Finish():
                    if self.RxData_Formate()[1][0] == 0:
                        time.sleep(0.01)
                        continue
                    else:
                        print('探测到液面！')
                        return True

    def Error_Handle(self, number: int):
        if number in self.status_dic.keys():
            print(self.status_dic[number])
            self.status = number
        else:
            print('无错误状态信息!')


if __name__ == '__main__':
    # lg.Start_Log()  # 开始日志
    PROTOCOL_TYPE = 0
