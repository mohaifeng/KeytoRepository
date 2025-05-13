import datetime
import time
from binascii import a2b_hex
import Com.Protocol.protocol as pr
import Com.Port.serialport as sp
import RotaryValve.rotaryvalve_cmd as rc


class rotaryvalve:
    """
    旋转阀类
    """

    def __init__(self):
        self.rv_ser = sp.ser
        self.protocol = 'KEYTO'  # KEYTO/IDEX/TC_DT/TC_OEM/MODBUS
        self.address = 0
        self.version = ''
        self.channel = 10
        self.status = 0
        self.gen_cmd = rc.rv_gen_cmd()
        self.idex_cmd = rc.idex_cmd()
        self.gen_prot = pr.keyto_gen_prot()
        self.idex_prot = pr.idex_prot()
        self.tx_data = ''
        self.rx_data = ''
        self.status_dic = {1: '光耦错误',
                           2: '堵转',
                           3: '光耦计数错误',
                           4: '驱动器初始化错误',
                           5: '切换通道光耦间距错误',
                           6: '切换通道计数错误'}

    def Get_Address(self):
        """
        获取旋切阀地址，最高优先级执行，若获取失败则停止程序
        :return: True:获取地址成功   False:获取失败
        """
        print('查询地址:')
        for ad in range(0, 255):
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
        """
        获取旋切阀地址，最高优先级执行，若获取失败则停止程序
        :return: 1:获取地址成功   0:获取失败
        """
        print('获取版本:')
        if self.protocol == 'KEYTO':
            send_cmd = self.gen_cmd.Read_Version()
        elif self.protocol == 'IDEX':
            send_cmd = self.idex_cmd.Read_Version()
        else:
            send_cmd = ''  # 后续需要补充
        self.Transmit(send_cmd)
        if self.Wait_Rx_Finish():
            self.version = str(self.RxData_Formate()[1])
            print('版本获取成功：', self.version)
            return True
        else:
            print('获取版本失败！')
            return False

    def TxData_Conf(self, _cmd: str):
        if self.protocol == 'KEYTO':
            self.tx_data = self.gen_prot.Gen_Cmd_Conf(self.address, _cmd)
        elif self.protocol == 'IDEX':
            self.tx_data = self.idex_prot.Idex_Cmd_Conf(_cmd)
        else:
            self.tx_data = ''

    def Transmit(self, data: str):
        """
        旋切阀发送数据
        :param data: str
        """
        self.TxData_Conf(data)
        print(datetime.datetime.now(), end=':')
        if self.protocol == 'IDEX':
            print('Send:', data, '->', bytes.fromhex(data).decode())
        else:
            print('Send:', data)
        return self.rv_ser.PortSend(bytes.fromhex(self.tx_data))

    def RxData_Analysis(self):
        if self.protocol == 'KEYTO':
            return self.gen_prot.Rec_Data_Conf(self.rx_data)
        elif self.protocol == 'IDEX':
            return self.idex_prot.Rec_Data_Conf(self.rx_data)
        else:
            return self.gen_prot.Rec_Data_Conf(self.rx_data)

    def Wait_Rx_Finish(self, timeout=500):
        """
        旋切阀接收数据
        :param timeout: 接收超时时间，默认1000ms
        :return: 0：返回数据有误 else：返回数据：str
        """
        self.rx_data = ''
        if self.rv_ser.PortReceive_Data(1, timeout):
            self.rx_data += self.rv_ser.receive_buf
            while True:
                if self.rv_ser.PortReceive_Data(1, 5):
                    self.rx_data += self.rv_ser.receive_buf
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
        if self.protocol == 'KEYTO':
            dat_tup = (self.gen_prot.rec_addr, str(self.gen_prot.rx_data))
        elif self.protocol == 'IDEX':
            dat_tup = (self.address, bytes.fromhex(self.idex_prot.rx_data).decode())
        else:
            dat_tup = (self.gen_prot.rec_addr, str(self.gen_prot.rx_data))
        return dat_tup

    def Idex_Wait_Complete(self):
        if self.rv_ser.PortReceive_Data(1, 3000):
            if self.rv_ser.receive_buf == '0D':
                return True
        return False

    def Wait_StatusIdle(self, timeout=500):
        """
        查询旋切阀状态
        :param timeout: 超时时间：ms，默认500ms
        :return: 0：空闲 -1：回复超时 else：错误状态
        """
        check_cmd_byte = self.gen_cmd.Read_State()
        while True:
            self.Transmit(check_cmd_byte)
            if self.Wait_Rx_Finish(timeout):
                sta = int(self.RxData_Formate()[1])
                if sta == 1:
                    continue
                elif sta == 0:
                    self.status = 0
                    return True
                else:
                    self.Error_Handle(sta)
                    return False
            else:
                print('回复超时！')
                self.status = -1
                return False

    def Error_Handle(self, number: int):
        if (number >> 8) in self.status_dic.keys():
            print(self.status_dic[number])
            self.status = number
        else:
            print('无错误状态信息!')


class stepia02:
    def __init__(self):
        self.ia_ser = sp.ser
        self.report_flag = 0  # 主动上报模式 0：关闭 1：打开
        self.protocol = 'KT_DT'  # KT_DT:keyto DT协议格式；KT_OEM：keyto OEM协议格式
        self.address = 0
        self.model = ''
        self.version = ''
        self.status = 0
        self.dt_prot = pr.keyto_dt_prot()
        self.oem_prot = pr.keyto_oem_prot()
        self.ia_cmd = rc.stepia02_gen_cmd()
        self.sys_cmd = rc.sys_cmd()
        self.tx_data = ''
        self.rx_data = ''
        self.status_dic = {
            0: '空闲', 1: '忙', 2: '执行成功', 3: '执行完成', 10: '参数超限', 11: '参数错误', 12: '语法错误',
            13: '无效指令', 14: '地址错误', 15: '禁止写', 16: '禁止读', 17: '未初始化', 50: '电机堵转',
            51: '驱动器故障', 54: '编码器错误',
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
        return self.ia_ser.PortSend(bytes.fromhex(self.tx_data))

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
        if self.ia_ser.PortReceive_Data(1, timeout):
            self.rx_data += self.ia_ser.receive_buf
            while True:
                if self.ia_ser.PortReceive_Data(1, 5):
                    self.rx_data += self.ia_ser.receive_buf
                    continue
                else:
                    if self.RxData_Analysis():
                        print(datetime.datetime.now(), end=':')
                        print('Rece:', self.rx_data)
                        return len(self.rx_data) // 2
                    else:
                        print('StepIA:数据格式有误！')
                        print('Rece:', self.rx_data)
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
        获取stepia地址，范围0-32
        :return: True：获取成功；False：获取失败
        """
        print('查询地址:')
        for ad in range(self.address, 254):
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
        获取stepia软件版本号
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
        轮询方式查询stepia状态
        :return: 返回状态：0：状态空闲；else：其他错误状态。
        """
        print('等待空闲：')
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
