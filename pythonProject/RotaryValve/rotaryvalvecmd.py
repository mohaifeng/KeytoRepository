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
        self.prot = pr.idex_prot()
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

    def Init(self):
        return self.prot.Idex_Cmd_Conf(self.cmd['置零'])

    def Switch_Channel(self, channel: int):
        return self.prot.Idex_Cmd_Conf(self.cmd['最优方式切换'], channel)

    def Sequential_Rotation(self, channel: int):
        """
        顺序切换通道指令
        :param channel: 目标切换通道
        :return: 指令字符串
        """
        return self.prot.Idex_Cmd_Conf(self.cmd['顺序切换'], channel)

    def Reverse_Rotation(self, channel: int):
        """
        逆序切换通道指令
        :param channel: 目标切换通道
        :return: 指令字符串
        """
        return self.prot.Idex_Cmd_Conf(self.cmd['逆序切换'], channel)

    def Read_Current_Channel(self):
        """
        读取当前通道位置
        :return: 指令字符串
        """
        return self.prot.Idex_Cmd_Conf(self.cmd['读取当前通道位置'])

    def Check_State(self):
        """
        读取状态
        :return: 指令字符串
        """
        return self.prot.Idex_Cmd_Conf(self.cmd['读取当前状态'])

    def Set_Usart_Baudrate(self, num=9600):
        """
        设置串口波特率
        :param num:9600,19200,38400,57600,115200
        :return:指令字符串
        """
        return self.prot.Idex_Cmd_Conf(self.cmd['设置串口波特率'], num)

    def Read_Version(self):
        """
        读取软件版本
        :return: 指令字符串
        """
        return self.prot.Idex_Cmd_Conf(self.cmd['读取软件版本号'])


class rv_gen_cmd:
    def __init__(self, address: int):
        """
        初始化旋切阀keyto查询指令
        :param address: 地址
        """
        self.prot = pr.keyto_gen_prot()
        self.address = address
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

    def Read_State(self):
        """
        读取状态
        :return: 指令字符串
        """
        return self.prot.Gen_Cmd_Conf(self.address, self.check_cmd['读取当前状态'], 0)

    def Read_Maxspeed(self):
        """
        读取最大速度
        :return: 指令字符串
        """
        return self.prot.Gen_Cmd_Conf(self.address, self.check_cmd['读取最大速度'], 0)

    def Read_Minspeed(self):
        """
        读取最小速度
        :return: 指令字符串
        """
        return self.prot.Gen_Cmd_Conf(self.address, self.check_cmd['读取最小速度'], 0)

    def Read_Acceleration(self):
        """
        读取加速度
        :return: 指令字符串
        """
        return self.prot.Gen_Cmd_Conf(self.address, self.check_cmd['读取加速度'], 0)

    def Read_Deceleration(self):
        """
        读取减速度
        :return: 指令字符串
        """
        return self.prot.Gen_Cmd_Conf(self.address, self.check_cmd['读取减速度'], 0)

    def Read_Current(self):
        """
        读取额定电流
        :return: 指令字符串
        """
        return self.prot.Gen_Cmd_Conf(self.address, self.check_cmd['读取额定电流'], 0)

    def Read_maxchannel(self):
        """
        读取最大通道数
        :return: 指令字符串
        """
        return self.prot.Gen_Cmd_Conf(self.address, self.check_cmd['读取最大通道数'], 0)

    def Read_Current_Channel(self):
        """
        读取当前通道位置
        :return: 指令字符串
        """
        return self.prot.Gen_Cmd_Conf(self.address, self.check_cmd['读取当前通道位置'], 0)

    def Read_ID(self):
        """
        读取设备ID
        :return: 指令字符串
        """
        return self.prot.Gen_Cmd_Conf(self.address, self.check_cmd['读取设备ID'], 0)

    def Read_Model(self):
        """
        读取型号
        :return: 指令字符串
        """
        return self.prot.Gen_Cmd_Conf(self.address, self.check_cmd['读取型号'], 0)

    def Read_Version(self):
        """
        读取软件版本
        :return: 指令字符串
        """
        return self.prot.Gen_Cmd_Conf(self.address, self.check_cmd['读取软件版本号'], 0)

    def Set_Maxspeed(self, speed=1000):
        """
        设置最大速速
        :param speed:50-10000(step/s)
        :return: 指令字符串
        """
        return self.prot.Gen_Cmd_Conf(self.address, self.set_cmd['设置最大速度'], speed)

    def Set_Minspeed(self, speed=10):
        """
        设置最小速度
        :param speed:0-5000(step/s)
        :return:指令字符串
        """
        return self.prot.Gen_Cmd_Conf(self.address, self.set_cmd['设置最小速度'], speed)

    def Set_Acceleration(self, speed=20000):
        """
        设置加速度
        :param speed: 50-400000(step/s)
        :return:指令字符串
        """
        return self.prot.Gen_Cmd_Conf(self.address, self.set_cmd['设置加速度'], speed)

    def Set_Deceleration(self, speed=20000):
        """
        设置减速度
        :param speed: 50-400000(step/s)
        :return:指令字符串
        """
        return self.prot.Gen_Cmd_Conf(self.address, self.set_cmd['设置减速度'], speed)

    def Set_Current(self, num=1800):
        """
        设置额定电流
        :param num: 100-2000(mA)
        :return:指令字符串
        """
        return self.prot.Gen_Cmd_Conf(self.address, self.set_cmd['设置额定电流'], num)

    def Set_Maxchannel(self, num=10):
        """
        设置最大通道数
        :param num:1-0xFF
        :return:指令字符串
        """
        return self.prot.Gen_Cmd_Conf(self.address, self.set_cmd['设置最大通道数'], num)

    def Set_Can_Baudrate(self, num=500):
        """
        设置can波特率
        :param num:100,125,250,500,800,1000
        :return:指令字符串
        """
        return self.prot.Gen_Cmd_Conf(self.address, self.set_cmd['设置CAN波特率'], num)

    def Set_Usart_Baudrate(self, num=9600):
        """
        设置串口波特率
        :param num:9600,19200,38400,57600,115200
        :return:指令字符串
        """
        return self.prot.Gen_Cmd_Conf(self.address, self.set_cmd['设置串口波特率'], num)

    def Set_Address(self, num=0):
        """
        设置地址
        :param num:0-255
        :return:指令字符串
        """
        return self.prot.Gen_Cmd_Conf(self.address, self.set_cmd['设置地址'], num)

    def Set_Poweroff_Save(self):
        """
        掉电保存
        :return: 指令字符串
        """
        return self.prot.Gen_Cmd_Conf(self.address, self.set_cmd['掉电保存'], 123456)

    def Set_Restore_Set(self):
        """
        恢复出厂设置
        :return: 指令字符串
        """
        return self.prot.Gen_Cmd_Conf(self.address, self.set_cmd['恢复出厂设置'], 123456)

    def Switch_Channel(self, channel: int):
        """
        以最优方式切换通道指令
        :param channel:目标切换通道
        :return: 指令字符串
        """
        return self.prot.Gen_Cmd_Conf(self.address, self.control_cmd['最优方式切换'], channel)

    def Sequential_Rotation(self, channel: int):
        """
        顺序切换通道指令
        :param channel: 目标切换通道
        :return: 指令字符串
        """
        return self.prot.Gen_Cmd_Conf(self.address, self.control_cmd['顺序切换'], channel)

    def Reverse_Rotation(self, channel: int):
        """
        逆序切换通道指令
        :param channel: 目标切换通道
        :return: 指令字符串
        """
        return self.prot.Gen_Cmd_Conf(self.address, self.control_cmd['逆序切换'], channel)

    def Init(self):
        """
        置零
        :return: 指令字符串
        """
        return self.prot.Gen_Cmd_Conf(self.address, self.control_cmd['置零'], 0)

    def Stop(self):
        """
        停止
        :return: 指令字符串
        """
        return self.prot.Gen_Cmd_Conf(self.address, self.control_cmd['停止'], 0)

    def Clean_Error(self):
        """
        清除错误
        :return: 指令字符串
        """
        return self.prot.Gen_Cmd_Conf(self.address, self.control_cmd['清除错误'], 0)


if __name__ == '__main__':
    idex = idex_cmd()
    print(idex.Switch_Channel(21))
