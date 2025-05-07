import datetime
import random
import sys
import time
from binascii import a2b_hex

import Com.Port.serialport as sp
import stepia02cmd


class stepia02(sp.serialport):
    def __init__(self, port, baud):
        """
        :param port: 串口号
        :param baud: 波特率
        """
        super().__init__(port, baud)
        self.initiative_report = 0  # 主动上报模式 0：关闭 1：打开
        self.protocol = 'OEM'
        self.state = 0
        self.address = 0
        self.adp_name = ''
        self.program_version = ''
        self.cmd = stepia02cmd.stepia02_generic_cmd(self.address)

    def Address(self):
        self.OpenPort()
        print('查询旋切阀地址:')
        for ad in range(0, 256):
            if ad < 255:
                sd_datas = str(ad) + '>?'
                if self.protocol == 'OEM':
                    sd_datas = stepia02cmd.EncoderRotaryValve_Dt_To_Oem(sd_datas)
                self.Send(sd_datas)
                rec_state = self.Receive()
                if rec_state == 0:
                    if ad != 255:
                        continue
                    else:
                        print('获取地址失败！')
                else:
                    print('获取地址成功，地址为：', ad)
                    self.address = ad
                    self.cmd = stepia02cmd.stepia02_generic_cmd(self.address)
                    break

    def CmdConfig(self, cmd: str):
        if self.protocol == 'OEM':
            byte_cmd = bytes.fromhex(cmd)
        else:
            byte_cmd = bytes.fromhex(bytes(cmd, 'UTF-8').hex().upper() + '0D0A')
        return byte_cmd

    def Send(self, sd_data: str):
        """
        发送数据
        :param sd_data: 数据字符串：str
        :return:
        """
        self.PortSend(self.CmdConfig(sd_data))
        print(datetime.datetime.now(), end=':')
        print('Send:', sd_data)
        return sd_data

    def Receive(self, timeout=200):
        """
        获取返回数据
        :param timeout: 超时时间：毫秒,0:不检测超时
        :return: 返回两种状态；0：无回复数据；else：数据str
        """
        rec_data = self.PortReceive(timeout)
        print(datetime.datetime.now(), end=':')
        if rec_data:
            hex_data = rec_data.hex().upper()
            if self.protocol != 'OEM':
                ascii_data = rec_data.decode('utf-8')
                print('Receive:', ascii_data)
                return hex_data
            print('Receive:', hex_data)
            return hex_data
        else:
            print('无回复数据！')
        return 0

    def IdexCheckValveState(self):
        """
        IDEX协议检查返回值是否正确
        :return: 0：无回复数据；1：返回正确
        """
        rec_str = self.Receive()
        if rec_str == '2A':
            while True:
                if self.Receive(2000) == '0D':
                    return 1
                else:
                    print('无回复数据')
                    return 0
        else:
            print('无回复数据')
            return 0


def IdexFuncCheck(age_time: int):
    """
    Idex协议老化
    :param age_time:老化时间
    :return:0：错误 1：正常
    """
    ia02 = stepia02('com14', 9600)
    ia02.protocol = 'IDEX'
    ia02.Send('M')
    if ia02.IdexCheckValveState() == 0:
        print('初始化返回错误')
        return 0
    start_time = time.perf_counter()
    while True:
        end_time = time.perf_counter()
        check_time = end_time - start_time
        if (check_time.seconds / 3600) < age_time:
            channel = random.randint(1, 6)
            print('切换{}通道'.format(channel))
            ia02.Send('S')
            now_channel = int(ia02.PortReceive().decode('utf-8').replace('\r', ''), 16)
            cmd_p = 'P' + str(channel)
            if now_channel == channel:
                ia02.Send(cmd_p)
                if ia02.Receive() != '2A0D':
                    print('切阀返回错误')
                    return 0
            else:
                ia02.Send(cmd_p)
                if ia02.IdexCheckValveState() == 0:
                    print('切阀无返回')
                    return 0
            ia02.Send('S')
            check_channel = int(ia02.PortReceive().decode('utf-8').replace('\r', ''), 16)
            if check_channel != channel:
                print('通道返回错误')
                return 0
            time.sleep(1)
        else:
            print('测试完成，功能正常')
            ia02.ClosePort()
            return 1


if __name__ == '__main__':
    IdexFuncCheck(1)
