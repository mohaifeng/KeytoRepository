from binascii import a2b_hex
import Com.Port.check as cs


class ser_prot_trans:
    def __init__(self):
        self.send_head = 'AA'  # oem发送帧头
        self.receive_head = '55'  # oem接收帧头
        self.send_flag = '>'  # dt发送标志
        self.receive_flag = '<'  # dt接收标志
        self.data_flag = ':'  # 数据标志
        self.oem_idex = 0x80  # oem索引最小值
        self.max_idex = 0xff  # oem索引最大值
        self.addr = 1
        self.data_hex = ''
        self.idex = ''
        self.data_len = 0  # 数据区字节个数

    def Oem_Index(self):
        """
        生成OEM协议序列号
        :return: 返回0x80-0xFF十六进制字符串
        """
        count_hex = hex(self.oem_idex).replace('0x', '').zfill(2).upper()
        self.oem_idex += 1
        if self.oem_idex > self.max_idex:
            self.oem_idex = 0x80
        return count_hex

    # DT协议转OEM协议
    def DtToOem(self, dt_data: str, mode: int):
        """
        将DT协议格式字符串转为OEM协议格式
        注意：返回数据DT->OEM新协议时，必须先转发送数据，再转返回数据
        :param dt_data:
        :param mode: 0：旧协议；1：新协议
        :return:
        """
        if self.send_flag in dt_data:
            self.addr = int(dt_data.split(self.send_flag)[0])
            addr_hex = hex(self.addr).replace('0x', '').zfill(2)  # 获取ADP地址
            self.data_hex = dt_data.split(self.send_flag)[1].encode().hex()  # 获取数据
            self.data_len = len(self.data_hex) // 2
            data_len_hex = hex(self.data_len).replace('0x', '').zfill(2)
            if mode == 0:
                oem_data = self.send_head + addr_hex + data_len_hex + self.data_hex
            else:
                self.idex = self.Oem_Index()
                oem_data = self.send_head + self.idex + addr_hex + data_len_hex + self.data_hex
        elif self.receive_flag in dt_data:
            self.addr = int(dt_data.split(self.receive_flag)[0], 16)
            addr_hex = hex(self.addr).replace('0x', '').zfill(2)
            if self.data_flag in dt_data:
                adp_state = int(dt_data.split(self.receive_flag)[1].split(':')[0])
                adp_state_hex = hex(adp_state).replace('0x', '').zfill(2)
                self.data_hex = dt_data.split(self.data_flag)[1].encode().hex()
                self.data_len = len(self.data_hex) // 2
                data_len_hex = hex(self.data_len).replace('0x', '').zfill(2)
                if mode == 0:
                    oem_data = self.receive_head + addr_hex + adp_state_hex + data_len_hex + self.data_hex
                else:
                    oem_data = self.receive_head + self.Oem_Index() + addr_hex + adp_state_hex + data_len_hex + self.data_hex
            else:
                self.data_len = 0
                data_len_hex = hex(self.data_len).replace('0x', '').zfill(2)
                adp_state = int(dt_data.split(self.receive_flag)[1])  # 获取数据
                adp_state_hex = hex(adp_state).replace('0x', '').zfill(2)
                if mode == 0:
                    oem_data = self.receive_head + addr_hex + adp_state_hex + data_len_hex
                else:
                    oem_data = self.receive_head + self.idex + addr_hex + adp_state_hex + data_len_hex
        else:
            print('数据格式错误！:', dt_data)
            return 0
        and_verification_hex = cs.Uchar_Checksum_8_Bit(oem_data)
        return (oem_data + and_verification_hex).upper()

    # OEM协议转DT协议
    def OemToDt(self, oem_str: str):
        """
        OEM字符串转DT字符串
        :param oem_str: OEM字符串
        :return: 0:格式错误；else：DT字符串
        """
        oem_str = oem_str.replace(' ', '').upper()
        pro_flag = int(oem_str[2:4], 16)
        if pro_flag > 127:
            oem_str = oem_str[:2] + oem_str[4:]
        self.addr = int(oem_str[2:4], 16)
        if oem_str[0:2] == self.send_head:
            self.data_len = int(oem_str[4:6], 16) * 2
            self.data_hex = oem_str[6:6 + self.data_len]
            dt_data_str = a2b_hex(self.data_hex).decode()
            dt_data = str(self.addr) + self.send_flag + dt_data_str
        elif oem_str[0:2] == self.receive_head:
            self.data_len = int(oem_str[6:8], 16)
            if self.data_len == 0:
                dt_state = int(oem_str[4:6], 16)
                dt_data = str(self.addr) + self.receive_flag + str(dt_state)
            else:
                dt_state = int(oem_str[4:6], 16)
                re_data = a2b_hex(bytes(oem_str[8:8 + self.data_len], 'utf-8')).decode()
                dt_data = str(self.addr) + self.receive_flag + str(dt_state) + ':' + re_data
        else:
            print('数据格式错误！')
            return 0
        return dt_data


if __name__ == '__main__':
    ser_p = ser_prot_trans()
    while True:
        print(ser_p.DtToOem(input('请输入oem协议格式指令:'), 1))
