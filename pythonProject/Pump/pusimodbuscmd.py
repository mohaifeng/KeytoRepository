import datetime
import Com.Port.check as ck


class PUSIMODBUSCMD:
    """
    pusi modbus指令
    """

    def __init__(self, address: int):
        self.address = hex(address).replace('0x', '').zfill(2)
        self.r_func = '03'
        self.w_one_func = '06'
        self.w_mul_fun = '10'
        self.ctr_reg = {
            '置零步数': ['609A', '609B'],
            '置零速度': ['6098', '6099'],
            '节点名称': ['101A', '101B', '101C', '101D', '101E', '101F', '1021'],
            '硬件版本': ['1022', '1023'],
            '软件版本': ['1024', '1025'],
            '软件控制': ['200F'],
            'ID': ['2028'],
            '波特率': ['202A', '202B'],
            '错误状态': ['6000'],
            '控制器状态': ['6001'],
            '方向': ['6002'],
            '最大速度': ['6003', '6004'],
            '相对位移': ['6005', '6006'],
            '工作模式': ['6007'],
            '绝对位移': ['6044', '6045'],
            '停止': ['6053'],
            '启动速度': ['6008'],
            '停止速度': ['6009'],
            '加速度系数': ['600A'],
            '减速度系数': ['600B'],
            '细分': ['600C'],
            '最大相电流': ['600D'],
            '电机位置': ['600E', '600F'],
            '实时速度': ['607F', '6080'],
            'GPIO值': ['6031'],
            '阀开关': ['6038']
        }

    def Read_Cmd(self, func: str):
        num = hex(len(self.ctr_reg[func])).replace('0x', '').zfill(4).upper()
        read_cmd = self.address + self.r_func + self.ctr_reg[func][0] + num.zfill(4).upper()
        crc16_modbus = ck.CRC16_MODBUS(read_cmd)
        return read_cmd + crc16_modbus

    def Write_One_Cmd(self, func: str, data: int, mode=0, ):
        """
        单独写寄存器
        :param func:寄存器功能字符串，详见 ctr_reg字典
        :param data: 写入数据，范围0-0xFFFF
        :param mode: 0：高16位；1：低16位，若只有16位功能寄存器，则不填或写0
        :return: str
        """
        data_hex = hex(data).replace('0x', '').zfill(4).upper()
        func_code = self.ctr_reg[func][mode]
        write_one_cmd = self.address + self.w_one_func + func_code + data_hex
        crc16_modbus_high = ck.CRC16_MODBUS(write_one_cmd)
        return write_one_cmd + crc16_modbus_high

    def Write_Mul_Cmd(self, func: str, data: list):
        """
        写多个寄存器
        :param func: 开始写寄存器功能字符串
        :param data: 写入数据->list
        :return: str
        """
        dat_hex_str = ''
        for _da in data:
            dat_hex_str += _da
        data_len_hex = hex(len(data)).replace('0x', '').zfill(4).upper()
        func_code = self.ctr_reg[func][0]
        byte_num = hex(len(dat_hex_str) // 2).replace('0x', '').zfill(2).upper()
        w_m_cmd = self.address + self.w_mul_fun + func_code + data_len_hex + byte_num + dat_hex_str
        crc16_modbus = ck.CRC16_MODBUS(w_m_cmd)
        return w_m_cmd + crc16_modbus


def Negative_To_Hex(num: int):
    if num < 0:
        # 负数转为补码
        num = (1 << 32) + num
    return hex(num)


if __name__ == '__main__':
    pusi_modbus = PUSIMODBUSCMD(1)
