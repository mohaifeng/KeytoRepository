import Com.Port.check as ck

STATUS_OK = 0
STATUS_BUSY = 1


class modbus_prot:
    def __init__(self):
        self.read_reg = '03'
        self.write_single_reg = '06'
        self.write_mul_reg = '10'
        self.rec_addr = 0
        self.rx_data = ''

    def Read_Reg(self, addr: int, reg: str, num: int):
        """"""
        addr_hex = hex(addr).removeprefix('0x').zfill(2)
        num_hex = hex(num).removeprefix('0x').zfill(4)
        conf_data = addr_hex + self.read_reg + reg + num_hex
        return (conf_data + ck.CRC16_MODBUS(conf_data)).upper()

    def Write_S_Reg(self, addr: int, reg: str, data: int):
        addr_hex = hex(addr).removeprefix('0x').zfill(2)
        data_hex = hex(data).removeprefix('0x').zfill(4)
        conf_data = addr_hex + self.write_single_reg + reg + data_hex
        return (conf_data + ck.CRC16_MODBUS(conf_data)).upper()

    def Write_M_Reg(self, addr: int, reg: str, *args):
        addr_hex = hex(addr).removeprefix('0x').zfill(2)
        reg_num_hex = hex(len(args)).removeprefix('0x').zfill(4)
        data_len_hex = hex(len(args) * 2).removeprefix('0x').zfill(2)
        data_hex = ''
        for arg in args:
            data_hex += hex(arg).removeprefix('0x').zfill(4)
        conf_data = addr_hex + self.write_mul_reg + reg + reg_num_hex + data_len_hex + data_hex
        return (conf_data + ck.CRC16_MODBUS(conf_data)).upper()

    def Rec_Data_Conf(self, data: str):
        data = data.upper()
        if len(data) // 2 == 8 and data[-2:] == ck.CRC16_MODBUS(data[:-2]):
            self.rec_addr = int(data[:2], 16)


class keyto_can_dic:
    def __init__(self):
        self.h_addr = 0  # 主控地址
        self.min_idex = 0x00  # can索引最小值
        self.max_idex = 0xff  # can索引最大值
        self.idex = self.min_idex
        self.frame_ID = ''
        self.frame_data = ''

    def Can_Index(self):
        """
        生成can协议序列号
        :return: 返回0x00-0xFF十六进制字符串
        """
        count_hex = hex(self.idex).removeprefix('0x').zfill(2).upper()
        self.idex += 1
        if self.idex > self.max_idex:
            self.min_idex = self.min_idex
        return count_hex

    def Frame_ID(self, cmd: str, addr: int):
        self.frame_ID = cmd.zfill(4) + bytes(self.h_addr).hex().zfill(2) + hex(addr).removeprefix('0x').zfill(2)
        return self.frame_ID.upper()

    def Frame_Cmd(self, m_idex: str, s_idex: str, data: int):
        self.frame_data = self.Can_Index() + m_idex.zfill(4) + s_idex.zfill(2) + hex(data).removeprefix('0x').zfill(8)
        return self.frame_data.upper()


class keyto_oem_prot:
    def __init__(self):
        self.protocol_type = 0  # 0:旧协议;1:新协议
        self.send_flag = 'AA'  # 发送数据标识
        self.recv_flag = '55'
        self.gap_flag = '2C'  # 数据分隔标识
        self.min_idex = 0x80  # oem索引最小值
        self.max_idex = 0xff  # oem索引最大值
        self.oem_idex = ''  # oem当前指令索引
        self.data_len = 0  # 数据长度
        self.rec_addr = 0
        self.state = 0  # 状态
        self.rx_data = ''  # 接收数据为数据区数据

    def Oem_Index(self):
        """
        生成OEM协议序列号
        :return: 返回0x80-0xFF十六进制字符串
        """
        count_hex = hex(self.min_idex).removeprefix('0x').zfill(2).upper()
        self.min_idex += 1
        if self.min_idex > self.max_idex:
            self.min_idex = 0x80
        return count_hex

    def Oem_Cmd_Conf(self, addr: int, cmd: str):
        """
        生成符合OEM协议指令字符串
        :param addr: 设备地址
        :param cmd: 命令字符串
        :return: 指令字符串
        """
        addr_hex = hex(addr).removeprefix('0x').zfill(2)
        self.oem_idex = self.Oem_Index()
        self.data_len = hex(len(cmd) // 2).removeprefix('0x').zfill(2)
        if self.protocol_type == 0:
            conf_data = self.send_flag + addr_hex + self.data_len + cmd
        else:
            conf_data = self.send_flag + self.oem_idex + addr_hex + self.data_len + cmd
        return (conf_data + ck.Uchar_Checksum_8_Bit(conf_data)).upper()

    def Rec_Data_Conf(self, data: str):
        """
        OEM数据解析，解析成功将类变量赋值
        :param data: 待解析数据
        :return: True/False
        """
        if (data[:2] == self.recv_flag) and (data[-2:] == ck.Uchar_Checksum_8_Bit(data[:-2])):
            if self.protocol_type == 1:
                self.oem_idex = data[2:4]
                self.rec_addr = int(data[4:6], 16)
                self.state = int(data[6:8], 16)
                self.data_len = int(data[8:10], 16)
                if self.data_len:
                    self.rx_data = data[10:10 + self.data_len * 2]
                else:
                    self.rx_data = ''
            else:
                self.rec_addr = int(data[2:4], 16)
                self.state = int(data[4:6], 16)
                self.data_len = int(data[6:8], 16)
                if self.data_len:
                    self.rx_data = data[8:8 + self.data_len * 2]
                else:
                    self.rx_data = ''
            return True
        return False


class keyto_dt_prot:
    def __init__(self):
        self.send_flag = '3E'  # 发送数据标识:">"
        self.recv_flag = '3C'  # 接收数据标识:"<"
        self.gap_flag = '2C'  # 数据分隔标识:","
        self.data_flag = '3A'  # 返回数据标识:":"
        self.end_flag = '0D'  # 指令结束标识"\r"
        self.rec_addr = 0  # 返回数据地址
        self.state = 0  # 返回状态
        self.rx_data = ''  # 数据区数据:发送数据为：指令+参数，接收数据为数据区数据

    def Dt_Cmd_Conf(self, addr: int, cmd: str):
        addr_hex = ''
        for tmp in str(addr):
            str_hex = hex(ord(tmp)).removeprefix('0x').zfill(2)
            addr_hex += str_hex
        conf_data = addr_hex + self.send_flag + cmd + self.end_flag
        return conf_data.upper()

    def Rec_Data_Conf(self, data: str):
        if self.recv_flag in data.upper():
            addr_hex = data.split(self.recv_flag)[0]
            self.rec_addr = int(bytes.fromhex(addr_hex).decode())
            data_hex = data.split(self.recv_flag)[1]
            if self.data_flag in data:
                self.state = int(bytes.fromhex(data_hex.split('3A')[0]))
                self.rx_data = data_hex.split('3A')[1]
            else:
                self.rx_data = ''
            return True
        return False


class keyto_gen_prot:
    """通用协议类,适用对象：阀泵类"""

    def __init__(self):
        self.head = 'AA'
        self.cmd = ''
        self.data = 0
        self.rec_addr = 0

    @staticmethod
    def Data_Conf(tmp: int):
        if tmp < 0:
            # 负数转为补码
            tmp = (1 << 32) + tmp
        return hex(tmp).removeprefix('0x').zfill(8)

    def Gen_Cmd_Conf(self, addr: int, cmd: str, data: int):
        """
        输出格式化指令
        :param addr:
        :param cmd: 指令参数:16进制str
        :param data: 数据：带符号int
        :return: str
        """
        self.data = data
        addr_hex = hex(addr).removeprefix('0x').zfill(2)
        self.cmd = cmd.zfill(2)
        conf_data = self.head + addr_hex + self.cmd + self.Data_Conf(data)
        return (conf_data + ck.Uchar_Checksum_8_Bit(conf_data)).upper()

    def Rec_Data_Conf(self, data: str):
        data = data.upper()
        if data[:2] == self.head:
            if data[-2:] == ck.Uchar_Checksum_8_Bit(data[:-2]):
                self.rec_addr = int(data[2:4], 16)
                self.data = int(data[4:-2], 16)
                return True
        return False


class pusi_prot:
    def __init__(self):
        self.head = 'A5'
        self.recv_flag = '7A'
        self.cmd = ''
        self.rx_data = 0
        self.rec_addr = 0

    @staticmethod
    def data_config(tmp: int):
        fin_tmp = ''
        tmp_hex = hex(tmp).removeprefix('0x').zfill(8)
        for i in range(0, len(tmp_hex) + 1, 2):
            tmp_byte = tmp_hex[i:i + 2]
            fin_tmp = tmp_byte + fin_tmp
        return fin_tmp.upper()

    def Ps_Cmd_Conf(self, addr: int, cmd: str, data: int):
        self.rx_data = data
        self.cmd = cmd.zfill(2)
        addr_hex = hex(addr).removeprefix('0x').zfill(2)
        conf_data = self.head + addr_hex + self.cmd + self.data_config(data)
        return (conf_data + ck.Uchar_Checksum_8_Bit(conf_data)).upper()

    def Rec_Data_Conf(self, data: str):
        data = data.upper()
        if data[:2] == self.head and data[2:4] == self.recv_flag:
            if data[-2:] == ck.Uchar_Checksum_8_Bit(data[:-2]):
                self.rec_addr = int(data[4:6], 16)
                self.rx_data = data[6:-2]
                return True
        return False


class idex_prot:
    def __init__(self):
        self.end_flag = '0D'
        self.rx_data = ''

    def Idex_Cmd_Conf(self, cmd: str, *args: int):
        cmd_hex = cmd.encode('utf-8').hex()
        par = ''
        for i in range(0, len(args)):
            par += str(args[i]).encode().hex()
        return (cmd_hex + par + self.end_flag).upper()

    def Rec_Data_Conf(self, data: str):
        data = data.upper()
        if data[-2:] == self.end_flag:
            self.rx_data = data[0:-2]
            return True
        return False


class tc_dt_prot:
    def __init__(self):
        self.start_flag = '2F'  # 发送数据标识:">"
        self.etx_flag = '03'  # 终止符
        self.cr_flag = '0D'  # 回车符
        self.lf_flag = '0A'  # 换行符
        self.rec_addr = '30'  # 固定主机地址
        self.execute_cmd = '52'  # 执行命令'R'指令末尾必须加才能执行
        self.state = 0  # 返回状态
        self.rx_data = ''  # 接收数据为数据区数据

    def Dt_Cmd_Conf(self, addr: int, cmd: str):
        addr_hex = str(addr).encode().hex()
        if cmd == self.execute_cmd:
            conf_data = self.start_flag + addr_hex + cmd + self.cr_flag
        else:
            conf_data = self.start_flag + addr_hex + cmd + self.execute_cmd + self.cr_flag
        return conf_data.upper()

    def Rec_Data_Conf(self, data: str):
        if (data[:2] == self.start_flag and
                data[-6:-4] == self.etx_flag and data[-4:-2] == self.cr_flag and data[-2:] == self.lf_flag):
            self.state = int(data[4:6], 16)
            self.rx_data = data[6:-6]
            return True
        return False


class tc_oem_prot:
    def __init__(self):
        self.start_flag = '02'  # 发送数据标识
        self.recv_flag = '55'
        self.end_flag = '03'  # 数据分隔标识
        self.idex = '30'  # 保留字节，固定为0x30
        self.rx_addr = '30'  # 主机地址固定为0x30
        self.state = 0  # 状态
        self.rx_data = ''  # 接收数据为数据区数据

    def Oem_Cmd_Conf(self, addr: int, cmd: str):
        """
        生成符合OEM协议指令字符串
        :param addr: 设备地址
        :param cmd: 命令字符串
        :return: 指令字符串
        """
        addr_hex = str(addr).encode().hex()
        conf_data = self.start_flag + addr_hex + self.idex + cmd + self.end_flag
        return (conf_data + ck.Xor_Checksum_8_Bit(conf_data)).upper()

    def Rec_Data_Conf(self, data: str):
        """
        OEM数据解析，解析成功将类变量赋值
        :param data: 待解析数据
        :return: True/False
        """
        if (data[:2] == self.start_flag) and (data[-2:] == ck.Xor_Checksum_8_Bit(data[:-2])):
            self.state = int(data[4:6], 16)
            self.rx_data = data[6:-4]
            return True
        return False


if __name__ == '__main__':
    mbus = modbus_prot()
    print(mbus.Read_Reg(1, '6001', 1))
    print(mbus.Write_S_Reg(255, '0051', 1000))
    print(mbus.Write_M_Reg(1, '0001', 1000, 100))
