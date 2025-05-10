import datetime
import time
from binascii import a2b_hex

import Com.Port.serialport as sp
import Com.Protocol.protocol as pr
import Pump.pump_cmd as cmd


class DRL_PLUNGER_PUMP:
    def __init__(self):
        self.drl_ser = sp.ser
        self.report_flag = 0  # 主动上报模式 0：关闭 1：打开
        self.protocol = 'KT_DT'  # KT_DT:keyto DT协议格式；KT_OEM：keyto OEM协议格式
        self.address = 0
        self.drl_name = ''
        self.drl_version = ''
        self.dt_prot = pr.keyto_dt_prot()
        self.oem_prot = pr.keyto_oem_prot()
        self.drl_cmd = cmd.drl_pump_cmd()
        self.sys_cmd = cmd.sys_cmd()
        self.send_data = ''
        self.receive_data = ''
        self.state_dic = {
            0: '空闲', 1: '忙', 2: '执行成功', 3: '执行完成', 10: '参数超限', 11: '参数错误', 12: '语法错误',
            13: '无效指令', 14: '地址错误', 15: '禁止写', 16: '禁止读',
            17: '未初始化', 50: '电机堵转', 51: '驱动器故障', 52: '光耦1错误', 53: '光耦2错误', 54: '传感器错误',
            55: 'EEPROM错误', 56: '电源欠压', 57: '电源过压', 58: '电机短路', 59: '电机开路'
        }

    def Data_Conf(self, _cmd: str):
        if self.protocol == 'KT_DT':
            sd_data = self.dt_prot.Dt_Cmd_Conf(self.address, _cmd)
        else:
            sd_data = self.oem_prot.Oem_Cmd_Conf(self.address, _cmd)
        self.send_data = sd_data

    def DrlSend(self, send_cmd: str):
        """
        发送数据
        :param send_cmd:
        :return: False:发送失败；True：发送成功
        """
        self.Data_Conf(send_cmd)
        print(datetime.datetime.now(), end=':')
        print('Send:', self.send_data, '->', 'addr:', self.address, 'Data:', bytes.fromhex(send_cmd).decode())
        return self.drl_ser.PortSend(bytes.fromhex(self.send_data))

    def Protocol_Check(self):
        if self.protocol == 'KT_DT':
            return self.dt_prot.Rec_Data_Conf(self.receive_data)
        else:
            return self.oem_prot.Rec_Data_Conf(self.receive_data)

    def DrlReceive(self, timeout=1000):
        """
        DRL获取返回数据
        :param timeout: 超时时间：毫秒,0:不检测超时
        :return: 0：无返回，else：接收到数据字节个数
        """
        self.receive_data = ''
        if self.drl_ser.PortReceive_Data(1, timeout):
            self.receive_data += self.drl_ser.receive_buf
            while True:
                if self.drl_ser.PortReceive_Data(1, 5):
                    self.receive_data += self.drl_ser.receive_buf
                    continue
                else:
                    if self.Protocol_Check():
                        print(datetime.datetime.now(), end=':')
                        print('Rece:', self.receive_data)
                        return len(self.receive_data) // 2
                    else:
                        print('drl:数据格式有误！')
                        print('Rece:', self.receive_data)
                        return 0
        else:
            print(datetime.datetime.now(), end=':')
            print('drl:数据无返回！')
            return 0

    def DrlRecConf(self):
        """
        处理返回数据
        :return: 返回元组，包括（地址+状态+数据为元组类型）
        """
        if self.protocol == 'KT_OEM':  # OEM协议判断
            addr = self.oem_prot.rec_addr
            state = self.oem_prot.state
            data = self.oem_prot.rx_data
            if self.oem_prot.data_len:
                if self.oem_prot.gap_flag in data:
                    config_data_tuple = tuple(int(a2b_hex(x).decode(), 16) for x in data.split(self.oem_prot.gap_flag))
                    dat_tup = (addr, state, config_data_tuple)
                else:
                    dat_tup = (addr, state, (int(a2b_hex(data).decode(), 16),))
            else:
                dat_tup = (addr, state, ())
        else:
            addr = self.dt_prot.rec_addr
            state = self.dt_prot.state
            data = self.dt_prot.rx_data
            if data:
                if self.dt_prot.gap_flag in data:
                    config_data_tuple = (int(a2b_hex(x).decode()) for x in data.split(self.dt_prot.gap_flag))
                    dat_tup = (addr, state, config_data_tuple)
                else:
                    dat_tup = (addr, state, (int(a2b_hex(data).decode()),))
            else:
                dat_tup = (addr, state, ())
        return dat_tup

    def GetDrlAddress(self):
        """
        获取DRL地址，范围0-32
        :return: True：获取成功；False：获取失败
        """
        print('查询ADP地址:')
        for ad in range(self.address, 254):
            self.address = ad
            self.DrlSend(self.sys_cmd.Check_State())
            if not self.DrlReceive():
                continue
            else:
                print('获取地址成功，地址为：', ad)
                self.address = ad
                return True
        print('获取地址失败！')
        return False

    def GetDrlVersion(self):
        """
        获取DRL软件版本号
        :return: True：获取成功；False：获取失败
        """
        print('获取版本:')
        self.DrlSend(self.sys_cmd.Rr(90))
        if self.DrlReceive():
            self.drl_version = self.DrlRecConf()[2][0]
            print('DRL版本获取成功：', self.drl_version)
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
        self.DrlSend(self.sys_cmd.Wr(82, dat))
        if self.DrlReceive():
            if self.DrlRecConf()[1] == 2:
                print('主动上报设置成功!')
                self.report_flag = dat
                return True
        print('主动上报设置失败')
        return False

    def CheckDrlState(self, delay=10):  # 间隔时间：毫秒
        """
        轮询方式查询drl状态
        :return: 返回状态：0：状态空闲；else：其他错误状态。
        """
        # print('查询adp状态！')
        if self.report_flag == 0:
            while True:
                self.DrlSend(self.sys_cmd.Check_State())
                if self.DrlReceive():
                    sta = self.DrlRecConf()[1]
                    if sta == 1:
                        time.sleep(delay / 1000)
                        continue
                    elif sta == 0:
                        return 0
                    else:
                        print('DRL状态错误', end=':')
                        print(self.state_dic[sta])
                        return sta
                else:
                    print('查询状态指令无返回')
                    time.sleep(delay / 1000)
                    return -1
        else:
            self.DrlSend(self.sys_cmd.Check_State())
            if self.DrlReceive(0):
                sta = self.DrlRecConf()[1]
                if sta == 3:
                    return 0
                else:
                    print('DRL状态错误', end=':')
                    print(self.state_dic[sta])
                    return sta


if __name__ == '__main__':
    sp.Reset_Ser_Baud(0, 'com32', 9600)
    drl = DRL_PLUNGER_PUMP()
    drl.GetDrlVersion()
    drl.DrlSend(drl.drl_cmd.It(1000))
