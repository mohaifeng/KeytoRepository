import datetime

import Pump.pump_cmd as cmd
import Com.Port.serialport as sp
import Com.Protocol.protocol as pr


class step_drs:
    def __init__(self):
        self.drs_ser = sp.ser
        self.protocol = 'KEYTO'  # KEYTO:keyto通用协议
        self.address = 0
        self.drs_version = ''
        self.gen_prot = pr.keyto_gen_prot()
        self.gen_cmd = cmd.drs_cmd(self.address)
        self.tx_data = ''
        self.rx_data = ''

    def Get_RV_Address(self):
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
        print('获取版本:')
        if self.protocol == 'KEYTO':
            send_cmd = self.gen_cmd.Read_Version()
        else:
            send_cmd = ''  # 后续需要补充
        self.RV_Send(send_cmd)
        if self.RV_Receive():
            self.drs_version = str(self.RV_RecConf()[1])
            print('旋转阀版本获取成功：', self.drs_version)
            return True
        else:
            print('获取版本失败！')
            return False

    def RV_Send(self, data: str):
        """
        旋切阀发送数据
        :param data: str
        """
        data_byte = bytes.fromhex(data)
        print(datetime.datetime.now(), end=':')
        if self.protocol != 'KEYTO':
            print('Send:', data, '->', bytes.fromhex(data).decode())
        else:
            print('Send:', data)
        return self.drs_ser.PortSend(data_byte)

    def Protocol_Check(self):
        if self.protocol == 'KEYTO':
            return self.gen_prot.Rec_Data_Conf(self.rx_data)
        else:
            return self.gen_prot.Rec_Data_Conf(self.rx_data)

    def RV_Receive(self, timeout=500):
        """
        旋切阀接收数据
        :param timeout: 接收超时时间，默认1000ms
        :return: 0：返回数据有误 else：返回数据：str
        """
        self.rx_data = ''
        if self.drs_ser.PortReceive_Data(1, timeout):
            self.rx_data += self.drs_ser.receive_buf
            while True:
                if self.drs_ser.PortReceive_Data(1, 5):
                    self.rx_data += self.drs_ser.receive_buf
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
        else:
            dat_tup = (self.gen_prot.rec_addr, str(self.gen_prot.data))
        return dat_tup


if __name__ == '__main__':
    drs = step_drs()
