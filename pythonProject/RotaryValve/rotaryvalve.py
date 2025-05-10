import datetime
import time
from binascii import a2b_hex
import Com.Protocol.protocol as pr
import Com.Port.serialport as sp
import RotaryValve.rotaryvalvecmd as rc
import Com.Port.check as ck


class rotaryvalve:
    """
    旋转阀类
    """
    def __init__(self):
        self.rv_ser = sp.ser
        self.protocol = 'KEYTO'  # KEYTO/IDEX/TC_DT/TC_OEM/MODBUS
        self.address = 0
        self.state = 0
        self.rv_version = ''
        self.channel = 10
        self.gen_cmd = rc.rv_gen_cmd(self.address)
        self.idex_cmd = rc.idex_cmd()
        self.gen_prot = pr.keyto_gen_prot()
        self.idex_prot = pr.idex_prot()
        self.tx_data = ''
        self.rx_data = ''

    def Get_RV_Address(self):
        """
        获取旋切阀地址，最高优先级执行，若获取失败则停止程序
        :return: True:获取地址成功   False:获取失败
        """
        print('查询旋切阀地址:')
        for ad in range(0, 255):
            self.gen_cmd.address = ad
            self.RV_Send(self.gen_cmd.Read_State())
            if not self.RV_Receive():
                continue
            else:
                print('旋切阀：获取地址成功：', ad)
                self.address = ad
                self.gen_cmd.address = ad
                return True
        print('旋切阀：获取地址失败!')
        return False

    def Get_RV_Version(self):
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
        self.RV_Send(send_cmd)
        if self.RV_Receive():
            self.rv_version = str(self.RV_RecConf()[1])
            print('旋转阀版本获取成功：', self.rv_version)
            return True
        else:
            print('获取版本失败！')
            return False

    def Change_RV_Address(self, address):
        self.address = address
        self.gen_cmd.address = address

    def RV_Send(self, data: str):
        """
        旋切阀发送数据
        :param data: str
        """
        data_byte = bytes.fromhex(data)
        print(datetime.datetime.now(), end=':')
        if self.protocol == 'IDEX':
            print('Send:', data, '->', bytes.fromhex(data).decode())
        else:
            print('Send:', data)
        return self.rv_ser.PortSend(data_byte)

    def Protocol_Check(self):
        if self.protocol == 'KEYTO':
            return self.gen_prot.Rec_Data_Conf(self.rx_data)
        elif self.protocol == 'IDEX':
            return self.idex_prot.Rec_Data_Conf(self.rx_data)
        else:
            return self.gen_prot.Rec_Data_Conf(self.rx_data)

    def RV_Receive(self, timeout=500):
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
                    if self.Protocol_Check():
                        print(datetime.datetime.now(), end=':')
                        print('Rece:', self.rx_data)
                        return len(self.rx_data) // 2
                    else:
                        print('旋转阀:数据格式有误！')
                        print('Rece:', self.rx_data)
                        return 0
        else:
            print(datetime.datetime.now(), end=':')
            print('旋转阀:数据无返回！')
            return 0

    def RV_RecConf(self):
        if self.protocol == 'KEYTO':
            dat_tup = (self.gen_prot.rec_addr, str(self.gen_prot.data))
        elif self.protocol == 'IDEX':
            dat_tup = (self.address, bytes.fromhex(self.idex_prot.rx_data).decode())
        else:
            dat_tup = (self.gen_prot.rec_addr, str(self.gen_prot.data))
        return dat_tup

    def Idex_Wait_Complete(self):
        if self.rv_ser.PortReceive_Data(1, 3000):
            if self.rv_ser.receive_buf == '0D':
                return True
        return False

    def Check_RV_State(self, timeout=500):
        """
        查询旋切阀状态
        :param timeout: 超时时间：ms，默认500ms
        :return: 0：空闲 -1：回复超时 else：错误状态
        """
        check_cmd_byte = self.gen_cmd.Read_State()
        while True:
            self.RV_Send(check_cmd_byte)
            if self.RV_Receive(timeout):
                sta = int(self.RV_RecConf()[1])
                if sta == 1:
                    continue
                elif sta == 0:
                    return 0
                else:
                    self.Rotaryvalve_Error(sta)
                    return sta
            else:
                print('旋切阀:回复超时！')
                return -1

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


if __name__ == '__main__':
    sp.Reset_Ser_Baud(0, 'com32', 9600)
    rv = rotaryvalve()
    rv.protocol = 'IDEX'
    rv.Get_RV_Version()
    rv.RV_Send(rv.idex_cmd.Read_Current_Channel())
    rv.RV_Receive()
    # rv.RV_Send(rv.idex_cmd.Init())
    # rv.Idex_Wait_Complete()
    # rv.RV_Send(rv.idex_cmd.Switch_Channel(5))
    # rv.Idex_Wait_Complete()
