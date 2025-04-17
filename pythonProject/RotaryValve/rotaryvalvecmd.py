import Com.Port.check as ck


class RV_CURRENCY_CHECK_CMD:
    def __init__(self, address: int):
        """
        初始化旋切阀keyto查询指令
        :param address: 地址
        """
        self.address = hex(address).replace('0x', '').zfill(2)
        self.check_cmd = {'读取当前状态': '90',
                          '读取最大速度': '91',
                          '读取最小速度': '92',
                          '读取加速度': '93',
                          '读取减速度': '94',
                          '读取额定电流': '95',
                          '读取最大通道数': '98',
                          '读取当前通道位置': '99',
                          '读取设备ID': 'F0',
                          '读取型号': 'F1',
                          '读取软件版本号': 'F2'
                          }

    def check_state(self):
        """
        读取状态
        :return: 指令字符串
        """
        cmd = 'AA' + self.address + self.check_cmd['读取当前状态'] + '00000000'
        cmd_checksum = ck.Uchar_Checksum_8_Bit(cmd)
        cmd_fin = cmd + cmd_checksum
        return cmd_fin.upper()

    def check_max_speed(self):
        """
        读取最大速度
        :return: 指令字符串
        """
        cmd = 'AA' + self.address + self.check_cmd['读取最大速度'] + '00000000'
        cmd_checksum = ck.Uchar_Checksum_8_Bit(cmd)
        cmd_fin = cmd + cmd_checksum
        return cmd_fin.upper()

    def check_min_speed(self):
        """
        读取最小速度
        :return: 指令字符串
        """
        cmd = 'AA' + self.address + self.check_cmd['读取最小速度'] + '00000000'
        cmd_checksum = ck.Uchar_Checksum_8_Bit(cmd)
        cmd_fin = cmd + cmd_checksum
        return cmd_fin.upper()

    def check_acceleration(self):
        """
        读取加速度
        :return: 指令字符串
        """
        cmd = 'AA' + self.address + self.check_cmd['读取加速度'] + '00000000'
        cmd_checksum = ck.Uchar_Checksum_8_Bit(cmd)
        cmd_fin = cmd + cmd_checksum
        return cmd_fin.upper()

    def check_deceleration(self):
        """
        读取减速度
        :return: 指令字符串
        """
        cmd = 'AA' + self.address + self.check_cmd['读取减速度'] + '00000000'
        cmd_checksum = ck.Uchar_Checksum_8_Bit(cmd)
        cmd_fin = cmd + cmd_checksum
        return cmd_fin.upper()

    def check_rated_current(self):
        """
        读取额定电流
        :return: 指令字符串
        """
        cmd = 'AA' + self.address + self.check_cmd['读取额定电流'] + '00000000'
        cmd_checksum = ck.Uchar_Checksum_8_Bit(cmd)
        cmd_fin = cmd + cmd_checksum
        return cmd_fin.upper()

    def check_max_channel(self):
        """
        读取最大通道数
        :return: 指令字符串
        """
        cmd = 'AA' + self.address + self.check_cmd['读取最大通道数'] + '00000000'
        cmd_checksum = ck.Uchar_Checksum_8_Bit(cmd)
        cmd_fin = cmd + cmd_checksum
        return cmd_fin.upper()

    def check_current_channel(self):
        """
        读取当前通道位置
        :return: 指令字符串
        """
        cmd = 'AA' + self.address + self.check_cmd['读取当前通道位置'] + '00000000'
        cmd_checksum = ck.Uchar_Checksum_8_Bit(cmd)
        cmd_fin = cmd + cmd_checksum
        return cmd_fin.upper()

    def check_device_ID(self):
        """
        读取设备ID
        :return: 指令字符串
        """
        cmd = 'AA' + self.address + self.check_cmd['读取设备ID'] + '00000000'
        cmd_checksum = ck.Uchar_Checksum_8_Bit(cmd)
        cmd_fin = cmd + cmd_checksum
        return cmd_fin.upper()

    def check_device_model(self):
        """
        读取型号
        :return: 指令字符串
        """
        cmd = 'AA' + self.address + self.check_cmd['读取型号'] + '00000000'
        cmd_checksum = ck.Uchar_Checksum_8_Bit(cmd)
        cmd_fin = cmd + cmd_checksum
        return cmd_fin.upper()

    def check_version(self):
        """
        读取软件版本
        :return: 指令字符串
        """
        cmd = 'AA' + self.address + self.check_cmd['读取软件版本号'] + '00000000'
        cmd_checksum = ck.Uchar_Checksum_8_Bit(cmd)
        cmd_fin = cmd + cmd_checksum
        return cmd_fin.upper()


class RV_CURRENCY_SET_CMD:
    def __init__(self, address: int):
        """
        初始化旋切阀参数设置keyto指令
        :param address: 地址
        """
        self.address = hex(address).replace('0x', '').zfill(2)
        self.set_cmd = {'设置最大速度': '51',
                        '设置最小速度': '52',
                        '设置加速度': '53',
                        '设置减速度': '54',
                        '设置额定电流': '55',
                        '设置最大通道数': '58',
                        '设置CAN波特率': '6D',
                        '设置串口波特率': '6E',
                        '设置地址': '6F',
                        '掉电保存': 'EF',
                        '恢复出厂设置': 'EE'
                        }

    def set_max_speed(self, speed=1000):
        """
        设置最大速速
        :param speed:50-10000(step/s)
        :return: 指令字符串
        """
        cmd = 'AA' + self.address + self.set_cmd['设置最大速度'] + hex(speed).replace('0x', '').zfill(8)
        cmd_checksum = ck.Uchar_Checksum_8_Bit(cmd)
        cmd_fin = cmd + cmd_checksum
        return cmd_fin.upper()

    def set_min_speed(self, speed=10):
        """
        设置最小速度
        :param speed:0-5000(step/s)
        :return:指令字符串
        """
        cmd = 'AA' + self.address + self.set_cmd['设置最小速度'] + hex(speed).replace('0x', '').zfill(8)
        cmd_checksum = ck.Uchar_Checksum_8_Bit(cmd)
        cmd_fin = cmd + cmd_checksum
        return cmd_fin.upper()

    def set_acceleration(self, speed=20000):
        """
        设置加速度
        :param speed: 50-400000(step/s)
        :return:指令字符串
        """
        cmd = 'AA' + self.address + self.set_cmd['设置加速度'] + hex(speed).replace('0x', '').zfill(8)
        cmd_checksum = ck.Uchar_Checksum_8_Bit(cmd)
        cmd_fin = cmd + cmd_checksum
        return cmd_fin.upper()

    def set_deceleration(self, speed=20000):
        """
        设置减速度
        :param speed: 50-400000(step/s)
        :return:指令字符串
        """
        cmd = 'AA' + self.address + self.set_cmd['设置减速度'] + hex(speed).replace('0x', '').zfill(8)
        cmd_checksum = ck.Uchar_Checksum_8_Bit(cmd)
        cmd_fin = cmd + cmd_checksum
        return cmd_fin.upper()

    def set_rated_current(self, num=1800):
        """
        设置额定电流
        :param num: 100-2000(mA)
        :return:指令字符串
        """
        cmd = 'AA' + self.address + self.set_cmd['设置额定电流'] + hex(num).replace('0x', '').zfill(8)
        cmd_checksum = ck.Uchar_Checksum_8_Bit(cmd)
        cmd_fin = cmd + cmd_checksum
        return cmd_fin.upper()

    def set_max_channel(self, num=10):
        """
        设置最大通道数
        :param num:1-0xFF
        :return:指令字符串
        """
        cmd = 'AA' + self.address + self.set_cmd['设置最大通道数'] + hex(num).replace('0x', '').zfill(8)
        cmd_checksum = ck.Uchar_Checksum_8_Bit(cmd)
        cmd_fin = cmd + cmd_checksum
        return cmd_fin.upper()

    def set_can_baud_rate(self, num=500):
        """
        设置can波特率
        :param num:100,125,250,500,800,1000
        :return:指令字符串
        """
        cmd = 'AA' + self.address + self.set_cmd['设置CAN波特率'] + hex(num).replace('0x', '').zfill(8)
        cmd_checksum = ck.Uchar_Checksum_8_Bit(cmd)
        cmd_fin = cmd + cmd_checksum
        return cmd_fin.upper()

    def set_usart_baud_rate(self, num=9600):
        """
        设置串口波特率
        :param num:9600,19200,38400,57600,115200
        :return:指令字符串
        """
        cmd = 'AA' + self.address + self.set_cmd['设置串口波特率'] + hex(num).replace('0x', '').zfill(8)
        cmd_checksum = ck.Uchar_Checksum_8_Bit(cmd)
        cmd_fin = cmd + cmd_checksum
        return cmd_fin.upper()

    def set_address(self, num=0):
        """
        设置地址
        :param num:0-255
        :return:指令字符串
        """
        cmd = 'AA' + self.address + self.set_cmd['设置地址'] + hex(num).replace('0x', '').zfill(8)
        cmd_checksum = ck.Uchar_Checksum_8_Bit(cmd)
        cmd_fin = cmd + cmd_checksum
        return cmd_fin.upper()

    def set_Poweroff_preservation(self):
        """
        掉电保存
        :return: 指令字符串
        """
        cmd = 'AA' + self.address + self.set_cmd['掉电保存'] + hex(123456).replace('0x', '').zfill(8)
        cmd_checksum = ck.Uchar_Checksum_8_Bit(cmd)
        cmd_fin = cmd + cmd_checksum
        return cmd_fin.upper()

    def set_restore_settings(self):
        """
        恢复出厂设置
        :return: 指令字符串
        """
        cmd = 'AA' + self.address + self.set_cmd['恢复出厂设置'] + hex(123456).replace('0x', '').zfill(8)
        cmd_checksum = ck.Uchar_Checksum_8_Bit(cmd)
        cmd_fin = cmd + cmd_checksum
        return cmd_fin.upper()


class RV_CURRENCY_CONTROL_CMD:
    def __init__(self, address: int):
        """
        初始化旋切阀keyto控制指令
        :param address: 地址
        """

        self.address = hex(address).replace('0x', '').zfill(2)
        self.control_cmd = {'最优方式切换': '01',
                            '顺序切换': '02',
                            '逆序切换': '03',
                            '置零': '05',
                            '停止': '06',
                            '清除错误': '07',
                            }

    def switch_channel(self, channel: int):
        """
        以最优方式切换通道指令
        :param channel:目标切换通道
        :return: 指令字符串
        """
        cmd = 'AA' + self.address + self.control_cmd['最优方式切换'] + hex(channel).replace('0x', '').zfill(8)
        cmd_checksum = ck.Uchar_Checksum_8_Bit(cmd)
        cmd_fin = cmd + cmd_checksum
        return cmd_fin.upper()

    def sequential_rotation(self, channel: int):
        """
        顺序切换通道指令
        :param channel: 目标切换通道
        :return: 指令字符串
        """
        cmd = 'AA' + self.address + self.control_cmd['顺序切换'] + hex(channel).replace('0x', '').zfill(8)
        cmd_checksum = ck.Uchar_Checksum_8_Bit(cmd)
        cmd_fin = cmd + cmd_checksum
        return cmd_fin.upper()

    def reverse_rotation(self, channel: int):
        """
        逆序切换通道指令
        :param channel: 目标切换通道
        :return: 指令字符串
        """
        cmd = 'AA' + self.address + self.control_cmd['逆序切换'] + hex(channel).replace('0x', '').zfill(8)
        cmd_checksum = ck.Uchar_Checksum_8_Bit(cmd)
        cmd_fin = cmd + cmd_checksum
        return cmd_fin.upper()

    def zeroing(self):
        """
        置零
        :return: 指令字符串
        """
        cmd = 'AA' + self.address + self.control_cmd['置零'] + '00000000'
        cmd_checksum = ck.Uchar_Checksum_8_Bit(cmd)
        cmd_fin = cmd + cmd_checksum
        return cmd_fin.upper()

    def stopping(self):
        """
        停止
        :return: 指令字符串
        """
        cmd = 'AA' + self.address + self.control_cmd['停止'] + '00000000'
        cmd_checksum = ck.Uchar_Checksum_8_Bit(cmd)
        cmd_fin = cmd + cmd_checksum
        return cmd_fin.upper()

    def clean_error(self):
        """
        清除错误
        :return: 指令字符串
        """
        cmd = 'AA' + self.address + self.control_cmd['清除错误'] + '00000000'
        cmd_checksum = ck.Uchar_Checksum_8_Bit(cmd)
        cmd_fin = cmd + cmd_checksum
        return cmd_fin.upper()
