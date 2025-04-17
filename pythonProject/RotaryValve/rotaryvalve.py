import datetime
from binascii import a2b_hex

import Com.Port.serialport as sp
import RotaryValve.rotaryvalvecmd as rc
import Com.Port.check as ck


class ROTARYVALVE:

    def __init__(self):
        self.rv_ser = sp.ser
        self.protocol = 'KEYTO'
        self.address = 0
        self.state = 0
        self.program_version = ''
        self.channel = 10
        self.check_cmd = rc.RV_CURRENCY_CHECK_CMD(self.address)
        self.control_cmd = rc.RV_CURRENCY_CONTROL_CMD(self.address)
        self.set_cmd = rc.RV_CURRENCY_SET_CMD(self.address)

    def GetAddress(self):
        """
        获取旋切阀地址，最高优先级执行，若获取失败则停止程序
        :return: 1:获取地址成功   0:获取失败
        """
        print('查询旋切阀地址:')
        for ad in range(0, 255):
            self.check_cmd = rc.RV_CURRENCY_CHECK_CMD(ad)
            check_cmd_byte = bytes.fromhex(self.check_cmd.check_state())
            self.rv_ser.PortSend(check_cmd_byte)
            rec_state = self.rv_ser.PortReceive_Datas(7, 500)
            if rec_state == 0:
                continue
            else:
                print('旋切阀：获取地址成功：', ad)
                self.address = ad
                self.check_cmd = rc.RV_CURRENCY_CHECK_CMD(self.address)
                self.control_cmd = rc.RV_CURRENCY_CONTROL_CMD(self.address)
                self.set_cmd = rc.RV_CURRENCY_SET_CMD(self.address)
                return 1
        print('旋切阀：获取地址失败!')
        return 0

    def ChangeAddress(self, address):
        self.address = address
        self.check_cmd = rc.RV_CURRENCY_CHECK_CMD(self.address)
        self.control_cmd = rc.RV_CURRENCY_CONTROL_CMD(self.address)
        self.set_cmd = rc.RV_CURRENCY_SET_CMD(self.address)

    def Rotaryvalve_Send(self, data: str):
        """
        旋切阀发送数据
        :param data: str
        """
        if self.protocol == 'KEYTO':
            data_byte = bytes.fromhex(data)
        else:
            data_byte = data.encode('utf-8')
        # print(datetime.datetime.now(), end=':')
        if self.rv_ser.PortSend(data_byte) == -1:
            print('Send error!')
        # print('Send:', data)

    def Rotaryvalve_Receive(self, timeout=500):
        """
        旋切阀接收数据
        :param timeout: 接收超时时间，默认1000ms
        :return: 0：返回数据有误 else：返回数据：str
        """
        self.rv_ser.PortClean()
        keyto_len = 7
        while True:
            if self.protocol == 'KEYTO':
                rec_byte = self.rv_ser.PortReceive_Datas(keyto_len, timeout)
                if rec_byte:
                    if self.Check_CmdReceive_Ok(rec_byte) == 1:
                        # print(datetime.datetime.now(), end=':')
                        # print('Rece:', rec_byte)
                        return rec_byte
                    else:
                        return 0
                else:
                    print(datetime.datetime.now(), end=':')
                    print('旋切阀：数据无返回！')
                    return 0
            else:
                print(datetime.datetime.now(), end=':')
                print("旋切阀：协议错误！")
                return 0

    def Check_Rotaryvalve_State(self, timeout=500):
        """
        查询旋切阀状态
        :param timeout: 超时时间：ms，默认500ms
        :return: 0：空闲 1：回复超时 else：错误状态
        """
        check_cmd_byte = self.check_cmd.check_state()
        count = 0
        while True:
            self.Rotaryvalve_Send(check_cmd_byte)
            receive_dat = self.Rotaryvalve_Receive(timeout)
            if receive_dat:
                sta = int(receive_dat[4:-2], 16)
                if sta == 1:
                    continue
                elif sta == 0:
                    return 0
                else:
                    self.Rotaryvalve_Error(sta)
                    return sta
            else:
                count += 1
                if count > 2:
                    print('旋切阀:回复超时！')
                    return 1

    def Rotaryvalve_Error(self, number: int):
        error_dic = {1: '光耦错误',
                     2: '堵转',
                     3: '光耦计数错误',
                     4: '驱动器初始化错误',
                     5: '切换通道光耦间距错误',
                     6: '切换通道计数错误'}
        if number in error_dic.keys():
            print(error_dic[number])
            self.state = number
        else:
            print('旋切阀：无错误状态信息!')

    def Check_CmdReceive_Ok(self, dat: str):
        """
        检测返回数据是否正确
        :param dat: 返回数据：str
        :return: 0：返回有误 1：返回正确
        """
        dat = dat.upper()
        right_header = 'AA'
        right_hex_address = hex(self.address).replace('0x', '').zfill(2).upper()
        right_checksum = ck.Uchar_Checksum_8_Bit(dat[0:-2])
        if right_header == dat[0:2]:
            if right_hex_address == dat[2:4]:
                if right_checksum == dat[-2:]:
                    return 1
                else:
                    print('旋切阀：和校验错误')
                    return 0
            else:
                print('旋切阀：地址错误')
                return 0
        else:
            print('旋切阀：帧头错误')
            return 0


# def Func_Parameter_Set(rv_address):
#     """
#     参数设置功能测试
#     :param rv_address: 测试设备地址
#     :return:1:测试成功 0：测试失败
#     """
#     parameter = [24, 2000, 10, 200000, 200000, 2000]  # 通道数、最大速度、最小速度、加速度、减速度、额定电流
#     func_cmd = [rotaryvalve.set_cmd.set_max_channel(parameter[0]),
#                 rotaryvalve.set_cmd.set_max_speed(parameter[1]),
#                 rotaryvalve.set_cmd.set_min_speed(parameter[2]),
#                 rotaryvalve.set_cmd.set_acceleration(parameter[3]),
#                 rotaryvalve.set_cmd.set_deceleration(parameter[4]),
#                 rotaryvalve.set_cmd.set_rated_current(parameter[5])]
#     check_cmd = [rotaryvalve.set_cmd.set_Poweroff_preservation()]
#     print('开始设置参数测试')
#     right_cmd_count = 0
#     for fc in func_cmd:
#         rotaryvalve.Rotaryvalve_Send(fc)
#         if rotaryvalve.Check_CmdReceive_Ok(rotaryvalve.Rotaryvalve_Receive()) == 0:
#             ip = input('返回数据错误，是否继续测试：1：继续  2：停止')
#             if ip == '1':
#                 continue
#             else:
#                 sys.exit()
#         else:
#             right_cmd_count += 1
#     if right_cmd_count == len(func_cmd):
#         rotaryvalve.Rotaryvalve_Send(check_cmd[0])
#         if rotaryvalve.Check_CmdReceive_Ok(rotaryvalve.Rotaryvalve_Receive()) == 0:
#             print('返回错误，请检查通讯')
#             sys.exit()
#
#     else:
#         print('功能测试失败!')
#         return 0


# def Func_Switch_Channels(rv_address):
#     for i in range(0, 1000):
#         aim_channel = random.randint(1, 24)
#         func_cmd = rotaryvalve.control_cmd.switch_channel(aim_channel)
#         rotaryvalve.Rotaryvalve_Send(func_cmd)
#         ro_receive_data = rotaryvalve.Rotaryvalve_Receive()
#         if ro_receive_data:
#             pass
#         else:
#             print('无回复数据！通讯异常')


if __name__ == '__main__':
    rv = ROTARYVALVE()

    rv.Rotaryvalve_Send(rv.control_cmd.switch_channel(1))
    rv.Rotaryvalve_Receive()
    rv.Check_Rotaryvalve_State()
