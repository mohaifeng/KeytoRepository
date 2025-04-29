import Com.Port.check as ck


class PUSICMD:
    """
    普斯指令，需指定地址以及模式（232 and 485）
    """

    def __init__(self, address: int, mode='232'):
        self.start = 'A5'
        self.mode = mode  # 两种模式部分指令不同：指令支持232或者485，默认232
        self.address = hex(address).replace('0x', '').zfill(2)
        self.pusi_com_cmd = {
            '置零': '38',
            '设置触发方式': '4A',
            '清楚外部急停1': '62',
            '查询当前位置': '63',
            '设置方向': '64',
            '设置急停方式': '66',
            '位置归零': '69',
            '查询控制器状态1': '6A',
            '清除外部急停2': '6C',
            '设置细分': '6D',
            '移动': '73',
            '设置最大速度': '76',
            '设置地址': '77',
            '设置启动速度': '4C',
            '设置停止速度': '53',
            '设置加速度系数': '75',
            '设置减速度系数': '6F',
            '设置最大相电流': '65',
            '使能电流自动衰减': '61',
            '设置电流衰减参数': '51',
            'IO端口读取': '78',
            '保留所有参数': '74',
            '读减速度系数': '68',
            '读加速度系数': '72',
            '读取相电流': '6E',
            '读取细分': '6B',
            '读取最大速度': '71',
            'IO端口写': '79',
            '停止': '49',
            '读取外部触发方式': '4B',
            '设定离线自动运行': '46',
            '读取固件版本信息': '52',
            '设置速度模式使能': '4E',
            '读控制器状态2': '4F',
            '读写堵转长度': '54',
            '读取堵转位置': '58',
            '读写堵转配置寄存器': '59',
        }
        self.pusi_232_cmd = {
            '阀控制': '34',
            '读取ID': '50',
            '读取秘钥': '36',
            '写入秘钥': '37',
        }
        self.pusi_485_cmd = {
            '阀控制': '31',
            '写用户数据地址': '33',
            '读用户数据地址': '34',
            '写用户数据': '35',
            '读用户数据': '36',
            '读取ID': 'F1',
            '清除堵转标志': '70',
            '读写堵转触发值': '5A',
            '设置回零速度': '39',
        }
        self.keyto_cmd = {
            '配置模式': 'F0',
            '读keyto版本': 'F2',
            '查询错误状态': 'F3',
            '设置电机方向': 'F4',
            '设置行程': 'F5',
            '设置自动改写触发边沿': 'F6',
            '清除错误': 'F7',
            '设备重启': 'F8',
            '读写堵转触发值': 'F9',
            '设置额定电流': 'FA',
            '恢复出厂设置': 'EE',
        }

    @staticmethod
    def data_config(data: int):
        fin_data = ''
        data = hex(data).replace('0x', '').zfill(8)
        for i in range(0, len(data) + 1, 2):
            tmp = data[i:i + 2]
            fin_data = tmp + fin_data
        return fin_data

    def pusicmd(self, cmd, data=0):
        """

        :param cmd: 普斯指令str
        :param data: 数据区参数
        :return:
        """
        data = self.data_config(data)
        if cmd in self.pusi_232_cmd:
            if self.mode == '232':
                cmd = (self.start + self.address + self.pusi_232_cmd[cmd] + data).upper()
        elif cmd in self.pusi_485_cmd:
            if self.mode == '485':
                cmd = (self.start + self.address + self.pusi_485_cmd[cmd] + data).upper()
        elif cmd in self.pusi_com_cmd:
            cmd = (self.start + self.address + self.pusi_com_cmd[cmd] + data).upper()
        else:
            print('指令输入错误')
        sum_check = ck.Uchar_Checksum_8_Bit(cmd)
        return cmd + sum_check


if __name__ == '__main__':
    ps_cmd = PUSICMD(255, '232')
    print(ps_cmd.pusicmd('置零', 0))

