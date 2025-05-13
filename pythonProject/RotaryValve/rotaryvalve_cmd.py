import Com.Protocol.protocol as pr


class tc_cmd:
    def __init__(self):
        self.gap_flag = '2C'
        self.contr_cmd = {
            '系统配置': 'U',
            '顺时针置零': 'Z',
            '逆时针置零': 'Y',
            '置零': 'w',
            '顺时针切阀': 'I',
            '逆时针切阀': 'O',
            '最优切阀-E': 'E',
            '最优切阀-B': 'B',
            '执行': 'R',
            '循环': 'G',
            '循环起始标志': 'g',
            '延时': 'M',
            '停止': 'T',
            '查询': '?',
            '查询软件版本': '&',
            '查询状态': 'Q',
            '设置地址': '#',
        }

    def Cmd_Conf(self, cmd: str, *args: int):
        cmd_hex = cmd.encode('utf-8').hex()
        par = ''
        for i in range(0, len(args)):
            if i < len(args) - 1:
                par += str(args[i]).encode().hex() + self.gap_flag
            else:
                par += str(args[i]).encode().hex()
        return (cmd_hex + par).upper()

    def Sys_Config_U(self, mode: int):
        ser_cmd = self.Cmd_Conf(self.contr_cmd['系统配置'], mode)
        return ser_cmd

    def Sequential_Init_Z(self):
        ser_cmd = self.Cmd_Conf(self.contr_cmd['顺时针置零'])
        return ser_cmd

    def Reverse_Init_Y(self):
        ser_cmd = self.Cmd_Conf(self.contr_cmd['逆时针置零'])
        return ser_cmd

    def Init_w(self, channel=1, dir=0):
        ser_cmd = self.Cmd_Conf(self.contr_cmd['置零'], channel, dir)
        return ser_cmd

    def Sequential_Rotation_I(self, channel: int):
        """
        顺序切换通道指令
        :param channel: 目标切换通道
        :return: 指令字符串
        """
        ser_cmd = self.Cmd_Conf(self.contr_cmd['顺时针切阀'], channel)
        return ser_cmd

    def Reverse_Rotation_O(self, channel: int):
        """
        逆序切换通道指令
        :param channel: 目标切换通道
        :return: 指令字符串
        """
        ser_cmd = self.Cmd_Conf(self.contr_cmd['逆时针切阀'], channel)
        return ser_cmd

    def Switch_Channel_E(self, channel: int):
        ser_cmd = self.Cmd_Conf(self.contr_cmd['最优切阀-E'], channel)
        return ser_cmd

    def Switch_Channel_B(self, channel: int):
        ser_cmd = self.Cmd_Conf(self.contr_cmd['最优切阀-B'], channel)
        return ser_cmd

    def Run_R(self):
        ser_cmd = self.Cmd_Conf(self.contr_cmd['执行'])
        return ser_cmd

    def Cycle_G(self, times: int):
        ser_cmd = self.Cmd_Conf(self.contr_cmd['循环起始标志']) + self.Cmd_Conf(self.contr_cmd['循环'], times)
        return ser_cmd


class idex_cmd:
    def __init__(self):
        self.cmd = {
            '最优方式切换': 'P',
            '顺序切换': '-',
            '逆序切换': '+',
            '置零': 'M',
            '读取当前通道位置': 'S',
            '读取当前状态': 'E',
            '设置串口波特率': 'X',
            '读取软件版本号': 'R',
        }

    @staticmethod
    def Cmd_Conf(cmd: str, *args: int):
        data = ''
        for arg in args:
            data += str(arg)
        return (cmd + data).encode().hex().upper()

    def Init(self):
        return self.Cmd_Conf(self.cmd['置零'])

    def Switch_Channel(self, channel: int):
        return self.Cmd_Conf(self.cmd['最优方式切换'], channel)

    def Sequential_Rotation(self, channel: int):
        """
        顺序切换通道指令
        :param channel: 目标切换通道
        :return: 指令字符串
        """
        return self.Cmd_Conf(self.cmd['顺序切换'], channel)

    def Reverse_Rotation(self, channel: int):
        """
        逆序切换通道指令
        :param channel: 目标切换通道
        :return: 指令字符串
        """
        return self.Cmd_Conf(self.cmd['逆序切换'], channel)

    def Read_Current_Channel(self):
        """
        读取当前通道位置
        :return: 指令字符串
        """
        return self.Cmd_Conf(self.cmd['读取当前通道位置'])

    def Check_State(self):
        """
        读取状态
        :return: 指令字符串
        """
        return self.Cmd_Conf(self.cmd['读取当前状态'])

    def Set_Usart_Baudrate(self, num=9600):
        """
        设置串口波特率
        :param num:9600,19200,38400,57600,115200
        :return:指令字符串
        """
        return self.Cmd_Conf(self.cmd['设置串口波特率'], num)

    def Read_Version(self):
        """
        读取软件版本
        :return: 指令字符串
        """
        return self.Cmd_Conf(self.cmd['读取软件版本号'])


class rv_gen_cmd:
    def __init__(self):
        """
        初始化旋切阀keyto查询指令
        """
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
        self.control_cmd = {'最优方式切换': '01',
                            '顺序切换': '02',
                            '逆序切换': '03',
                            '置零': '05',
                            '停止': '06',
                            '清除错误': '07',
                            }

    @staticmethod
    def Data_Conf(tmp: int):
        if tmp < 0:
            # 负数转为补码
            tmp = (1 << 32) + tmp
        return hex(tmp).removeprefix('0x').zfill(8)

    def Cmd_Conf(self, cmd: str, data: int):
        cmd_hex = cmd.zfill(2)
        data_hex = self.Data_Conf(data)
        return (cmd_hex + data_hex).upper()

    def Read_State(self, num=0):
        return self.Cmd_Conf(self.check_cmd['读取当前状态'], num)

    def Read_Maxspeed(self, num=0):
        return self.Cmd_Conf(self.check_cmd['读取最大速度'], num)

    def Read_Minspeed(self, num=0):
        return self.Cmd_Conf(self.check_cmd['读取最小速度'], num)

    def Read_Acceleration(self, num=0):
        return self.Cmd_Conf(self.check_cmd['读取加速度'], num)

    def Read_Deceleration(self, num=0):
        return self.Cmd_Conf(self.check_cmd['读取减速度'], num)

    def Read_Current(self, num=0):
        return self.Cmd_Conf(self.check_cmd['读取额定电流'], num)

    def Read_maxchannel(self, num=0):
        return self.Cmd_Conf(self.check_cmd['读取最大通道数'], num)

    def Read_Current_Channel(self, num=0):
        return self.Cmd_Conf(self.check_cmd['读取当前通道位置'], num)

    def Read_ID(self, num=0):
        return self.Cmd_Conf(self.check_cmd['读取设备ID'], num)

    def Read_Model(self, num=0):
        return self.Cmd_Conf(self.check_cmd['读取型号'], num)

    def Read_Version(self, num=0):
        return self.Cmd_Conf(self.check_cmd['读取软件版本号'], num)

    def Set_Maxspeed(self, num=1000):
        return self.Cmd_Conf(self.set_cmd['设置最大速度'], num)

    def Set_Minspeed(self, num=10):
        return self.Cmd_Conf(self.set_cmd['设置最小速度'], num)

    def Set_Acceleration(self, num=20000):
        return self.Cmd_Conf(self.set_cmd['设置加速度'], num)

    def Set_Deceleration(self, num=20000):
        return self.Cmd_Conf(self.set_cmd['设置减速度'], num)

    def Set_Current(self, num=1800):
        return self.Cmd_Conf(self.set_cmd['设置额定电流'], num)

    def Set_Maxchannel(self, num=10):
        return self.Cmd_Conf(self.set_cmd['设置最大通道数'], num)

    def Set_Can_Baudrate(self, num=500):
        return self.Cmd_Conf(self.set_cmd['设置CAN波特率'], num)

    def Set_Usart_Baudrate(self, num=9600):
        return self.Cmd_Conf(self.set_cmd['设置串口波特率'], num)

    def Set_Address(self, num=0):
        return self.Cmd_Conf(self.set_cmd['设置地址'], num)

    def Set_Poweroff_Save(self, num=0):
        return self.Cmd_Conf(self.set_cmd['掉电保存'], num)

    def Set_Restore_Set(self, num=123456):
        return self.Cmd_Conf(self.set_cmd['恢复出厂设置'], num)

    def Switch_Channel(self, num: int):
        return self.Cmd_Conf(self.control_cmd['最优方式切换'], num)

    def Sequential_Rotation(self, num: int):
        return self.Cmd_Conf(self.control_cmd['顺序切换'], num)

    def Reverse_Rotation(self, num: int):
        return self.Cmd_Conf(self.control_cmd['逆序切换'], num)

    def Init(self, num=0):
        return self.Cmd_Conf(self.control_cmd['置零'], num)

    def Stop(self, num=0):
        return self.Cmd_Conf(self.control_cmd['停止'], num)

    def Clean_Error(self, num=0):
        return self.Cmd_Conf(self.control_cmd['清除错误'], num)


class stepia02_gen_cmd:
    def __init__(self):
        """
        stepia02板卡兼容通用指令
        """
        self.gap_flag = '2C'
        self.fix_par = 123456
        self.contr_cmd = {'置零': 'Rt',
                          '最优路径切换通道': 'Rp',
                          '指定方向切换通道': 'Rd',
                          '老化': 'RE'
                          }

    def Cmd_Conf(self, cmd: str, *args: int):
        cmd_hex = cmd.encode('utf-8').hex()
        par = ''
        for i in range(0, len(args)):
            if i < len(args) - 1:
                par += str(args[i]).encode().hex() + self.gap_flag
            else:
                par += str(args[i]).encode().hex()
        return (cmd_hex + par).upper()

    def Init(self):
        ser_cmd = self.Cmd_Conf(self.contr_cmd['置零'])
        return ser_cmd

    def Switch_Channel(self, channel: int):
        ser_cmd = self.Cmd_Conf(self.contr_cmd['最优路径切换通道'], channel)
        return ser_cmd

    def Switch_Dir(self, channel: int, direction=0):
        ser_cmd = self.Cmd_Conf(self.contr_cmd['指定方向切换通道'], channel, direction)
        return ser_cmd

    def Age(self, age_str):
        ser_cmd = self.Cmd_Conf(self.contr_cmd['老化'], age_str)
        return ser_cmd


class sys_cmd:
    def __init__(self):
        self.gap_flag = '2C'
        self.fix_par = 123456
        self.sys_cmd = {
            '写寄存器': 'Wr',
            '读寄存器': 'Rr',
            '查状态': '?',
            '延时': 'L',
            '停止': 'T',
            '复位': 'U',
            '恢复出厂': 'M',
            '掉电保存': 'S',
        }

    def Cmd_Conf(self, cmd: str, *args: int):
        cmd_hex = cmd.encode('utf-8').hex()
        par = ''
        for i in range(0, len(args)):
            if i < len(args) - 1:
                par += str(args[i]).encode().hex() + self.gap_flag
            else:
                par += str(args[i]).encode().hex()
        return (cmd_hex + par).upper()

    def Wr(self, register: int, data: int):
        ser_cmd = self.Cmd_Conf(self.sys_cmd['写寄存器'], register, data)
        return ser_cmd

    def Rr(self, register: int, number=1):
        ser_cmd = self.Cmd_Conf(self.sys_cmd['读寄存器'], register, number)
        return ser_cmd

    def Check_State(self):
        ser_cmd = self.Cmd_Conf(self.sys_cmd['查状态'])
        return ser_cmd

    def Sys_Delay(self, l_time):
        ser_cmd = self.Cmd_Conf(self.sys_cmd['延时'], l_time, )
        return ser_cmd

    def Sys_Stop(self):
        ser_cmd = self.Cmd_Conf(self.sys_cmd['停止'])
        return ser_cmd

    def Sys_Reset(self):
        ser_cmd = self.Cmd_Conf(self.sys_cmd['复位'])
        return ser_cmd

    def Restore_Set(self):
        ser_cmd = self.Cmd_Conf(self.sys_cmd['恢复出厂'], self.fix_par)
        return ser_cmd

    def Poweroff_Save(self):
        ser_cmd = self.Cmd_Conf(self.sys_cmd['掉电保存'])
        return ser_cmd
