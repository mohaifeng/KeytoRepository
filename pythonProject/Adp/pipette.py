import Adp.adpcmd as cmd
import Com.Protocol.protocol as pr
from binascii import a2b_hex
import time
import datetime
import Com.Port.serialport as sp

dev_mode = 0


# 错误状态定义：
# 0：返回错误；
# 1：返回正确
class z_axis:
    def __init__(self):
        self.z_ser = sp.ser
        self.report_flag = 0  # 主动上报模式 0：关闭 1：打开
        self.protocol = 'KT_DT'  # KT_DT:keyto DT协议格式；KT_OEM：keyto OEM协议格式
        self.address = 41
        self.dt_prot = pr.keyto_dt_prot()
        self.oem_prot = pr.keyto_oem_prot()
        self.z_cmd = cmd.z_cmd()
        self.sys_cmd = cmd.sys_cmd()
        self.send_data = ''
        self.receive_data = ''
        self.state_dic = {
            0: '空闲', 1: '忙', 2: '执行成功', 10: '参数超限', 11: '参数错误', 12: '语法错误', 13: '无效指令',
            14: '地址错误', 15: '禁止写', 16: '禁止读', 18: 'Z轴未初始化', 19: 'Z轴未初始化', 80: 'Z轴电机堵转',
            81: 'Z轴电机驱动器故障', 82: 'Z轴电机光耦错误', 83: 'Z轴存储错误', 84: 'Z轴未校准错误',
        }

    def Data_Conf(self, _cmd: str):
        if self.protocol == 'KT_DT':
            sd_data = self.dt_prot.Dt_Cmd_Conf(self.address, _cmd)
        else:
            sd_data = self.oem_prot.Oem_Cmd_Conf(self.address, _cmd)
        self.send_data = sd_data

    def ZSend(self, send_cmd: str):
        """
        发送数据
        :param send_cmd:
        :return: False:发送失败；True：发送成功
        """
        self.Data_Conf(send_cmd)
        print(datetime.datetime.now(), end=':')
        print('Send:', self.send_data, '->', 'addr:', self.address, 'Data:', bytes.fromhex(send_cmd).decode())
        return self.z_ser.PortSend(bytes.fromhex(self.send_data))

    def ZReceive(self, timeout=1000):
        """
        ADP获取返回数据
        :param timeout: 超时时间：毫秒,0:不检测超时
        :return: 0：无返回，else：接收到数据字节个数
        """
        self.receive_data = ''
        if self.z_ser.PortReceive_Data(1, timeout):
            self.receive_data += self.z_ser.receive_buf
            while True:
                if self.z_ser.PortReceive_Data(1, 5):
                    self.receive_data += self.z_ser.receive_buf
                    continue
                else:
                    if self.oem_prot.Rec_Data_Conf(self.receive_data) or self.dt_prot.Rec_Data_Conf(self.receive_data):
                        print(datetime.datetime.now(), end=':')
                        print('Rece:', self.receive_data)
                        return len(self.receive_data) // 2
                    else:
                        print('z_axis:数据格式有误！')
                        print('Rece:', self.receive_data)
                        return 0
        else:
            print(datetime.datetime.now(), end=':')
            print('z_axis:数据无返回！')
            return 0

    def ZRecConf(self):
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
                    dat_tup = (addr, state, (int(a2b_hex(data).decode())))
            else:
                dat_tup = (addr, state, ())
        return dat_tup

    def CheckZState(self, delay=10):  # 间隔时间：毫秒
        """
        轮询方式查询adp状态
        :return: 返回状态：0：状态空闲；else：其他错误状态。-1:出现无返回数据状态
        """
        print('查询z_axis状态！')
        if self.report_flag == 0:
            while True:
                self.ZSend(self.sys_cmd.Check_State())
                if self.ZReceive():
                    sta = self.ZRecConf()[1]
                    if sta == 1:
                        time.sleep(delay / 1000)
                        continue
                    elif sta == 0:
                        return 0
                    else:
                        print('Z_axis状态错误', end=':')
                        print(self.state_dic[sta])
                        return sta
                else:
                    print('查询状态指令无返回')
                    time.sleep(delay / 1000)
                    return -1
        else:
            self.ZSend(self.sys_cmd.Check_State())
            if self.ZReceive(0):
                sta = self.ZRecConf()[1]
                if sta == 3:
                    return 0
                else:
                    print('Z状态错误', end=':')
                    print(self.state_dic[sta])
                    return sta


class pipette:
    def __init__(self):
        self.adp_ser = sp.ser
        self.report_flag = 0  # 主动上报模式 0：关闭 1：打开
        self.protocol = 'KT_DT'  # KT_DT:keyto DT协议格式；KT_OEM：keyto OEM协议格式
        self.address = 1
        self.adp_name = ''
        self.adp_version = ''
        self.dt_prot = pr.keyto_dt_prot()
        self.oem_prot = pr.keyto_oem_prot()
        self.adp_cmd = cmd.adp_cmd()
        self.sys_cmd = cmd.sys_cmd()
        self.send_data = ''
        self.receive_data = ''
        self.state_dic = {
            0: '空闲', 1: 'ADP忙', 2: '执行成功', 3: '探测到液面', 4: 'Z轴忙', 5: 'Z轴忙，ADP忙', 10: '参数超限',
            11: '参数错误', 12: '语法错误', 13: '无效指令', 14: '地址错误', 15: '禁止写', 16: '禁止读',
            17: 'ADP未初始化', 18: 'Z轴未初始化', 19: 'Z轴未连接', 20: '未检测到TIP', 21: '顶出TIP失败',
            22: '超时报警', 23: '吸堵检测报警', 24: '气泡检测报警', 25: '吸空检测报警', 26: '排气凝块报警',
            27: '排气气泡报警', 28: '抗悬滴范围超限', 50: '电机堵转', 51: '驱动器故障', 52: '光耦1错误',
            53: '光耦2错误', 54: '传感器错误', 55: 'EEPROM错误', 56: '电源欠压', 57: '电源过压', 58: '电机短路',
            59: '电机开路', 80: 'Z轴电机堵转', 81: 'Z轴电机驱动器故障', 82: 'Z轴电机光耦错误'
        }

    def Data_Conf(self, _cmd: str):
        if self.protocol == 'KT_DT':
            sd_data = self.dt_prot.Dt_Cmd_Conf(self.address, _cmd)
        else:
            sd_data = self.oem_prot.Oem_Cmd_Conf(self.address, _cmd)
        self.send_data = sd_data

    def AdpSend(self, send_cmd: str):
        """
        发送数据
        :param send_cmd:
        :return: False:发送失败；True：发送成功
        """
        self.Data_Conf(send_cmd)
        print(datetime.datetime.now(), end=':')
        print('Send:', self.send_data, '->', 'addr:', self.address, 'Data:', bytes.fromhex(send_cmd).decode())
        return self.adp_ser.PortSend(bytes.fromhex(self.send_data))

    def Protocol_Check(self):
        if self.protocol == 'KT_DT':
            return self.dt_prot.Rec_Data_Conf(self.receive_data)
        else:
            return self.oem_prot.Rec_Data_Conf(self.receive_data)

    def AdpReceive(self, timeout=1000):
        """
        ADP获取返回数据
        :param timeout: 超时时间：毫秒,0:不检测超时
        :return: 0：无返回，else：接收到数据字节个数
        """
        self.receive_data = ''
        if self.adp_ser.PortReceive_Data(1, timeout):
            self.receive_data += self.adp_ser.receive_buf
            while True:
                if self.adp_ser.PortReceive_Data(1, 5):
                    self.receive_data += self.adp_ser.receive_buf
                    continue
                else:
                    if self.Protocol_Check():
                        print(datetime.datetime.now(), end=':')
                        print('Rece:', self.receive_data)
                        return len(self.receive_data) // 2
                    else:
                        print('adp:数据格式有误！')
                        print('Rece:', self.receive_data)
                        return 0
        else:
            print(datetime.datetime.now(), end=':')
            print('adp:数据无返回！')
            return 0

    def AdpRecConf(self):
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

    def GetAdpAddress(self):
        """
        获取ADP地址，范围1-32
        :return: True：获取成功；False：获取失败
        """
        print('查询ADP地址:')
        for ad in range(self.address, 33):
            self.address = ad
            self.AdpSend(self.sys_cmd.Check_State())
            if not self.AdpReceive():
                continue
            else:
                print('获取地址成功，地址为：', ad)
                self.address = ad
                return True
        print('获取地址失败！')
        return False

    def GetAdpVersion(self):
        """
        获取ADP软件版本号
        :return: True：获取成功；False：获取失败
        """
        print('获取版本:')
        self.AdpSend(self.sys_cmd.Rr(90))
        if self.AdpReceive():
            self.adp_version = self.AdpRecConf()[2][0]
            print('ADP版本获取成功：', self.adp_version)
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
        self.AdpSend(self.sys_cmd.Wr(82, dat))
        if self.AdpReceive():
            if self.AdpRecConf()[1] == 2:
                print('主动上报设置成功!')
                self.report_flag = dat
                return True
        print('主动上报设置失败')
        return False

    def CheckAdpState(self, delay=10):  # 间隔时间：毫秒
        """
        轮询方式查询adp状态
        :return: 返回状态：0：状态空闲；else：其他错误状态,-1:无返回。
        """
        # print('查询adp状态！')
        if self.report_flag == 0:
            while True:
                self.AdpSend(self.sys_cmd.Check_State())
                if self.AdpReceive():
                    sta = self.AdpRecConf()[1]
                    if sta == 1:
                        time.sleep(delay / 1000)
                        continue
                    elif sta == 0:
                        return 0
                    else:
                        print('ADP状态错误', end=':')
                        print(self.state_dic[sta])
                        return sta
                else:
                    print('查询状态指令无返回')
                    return -1
        else:
            self.AdpSend(self.sys_cmd.Check_State())
            if self.AdpReceive(0):
                sta = self.AdpRecConf()[1]
                if sta == 3:
                    return 0
                else:
                    print('ADP状态错误', end=':')
                    print(self.state_dic[sta])
                    return sta

    def CheckPlldState(self, timeout=10000):
        """
        检测液面探测状态
        :param timeout: 超时时间：ms
        :return: 0：超时；1：探测到液面
        """
        check_cmd = self.sys_cmd.Rr(2)
        if self.protocol == 'KT_OEM':
            check_cmd = self.oem_prot.Oem_Cmd_Conf(self.address, check_cmd)
        print('探测液面中！')
        if timeout:
            starttime = time.perf_counter()
            while (time.perf_counter() - starttime) * 1000 <= timeout:
                self.AdpSend(check_cmd)
                if self.AdpReceive():
                    if self.AdpRecConf()[2] == 1:
                        print('探测到液面！')
                        return True
                    else:
                        time.sleep(0.01)
                        continue
                else:
                    print('查询探测指令无返回', end=':')
                    time.sleep(timeout / 1000)
                    continue
            print('探测失败！')
            return False
        else:
            while True:
                self.AdpSend(check_cmd)
                if self.AdpReceive():
                    if self.AdpRecConf()[2] == 0:
                        time.sleep(0.01)
                        continue
                    else:
                        return True


if __name__ == '__main__':
    # lg.Start_Log()  # 开始日志
    PROTOCOL_TYPE = 0
