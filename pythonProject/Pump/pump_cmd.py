import Com.Port.check as ck
import Com.Protocol.protocol as pr


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

    # def sys_restart(self):
    #     r_command = str(self.adp_address) + '>' + 'R'
    #     return r_command

    def Sys_Reset(self):
        ser_cmd = self.Cmd_Conf(self.sys_cmd['复位'])
        return ser_cmd

    def Restore_Set(self):
        ser_cmd = self.Cmd_Conf(self.sys_cmd['恢复出厂'], self.fix_par)
        return ser_cmd

    def Poweroff_Save(self):
        ser_cmd = self.Cmd_Conf(self.sys_cmd['掉电保存'])
        return ser_cmd


class drl_pump_cmd:
    def __init__(self):
        self.gap_flag = '2C'
        self.fix_par = 123456
        self.contr_cmd = {
            '置零': 'It',
            '吸液': 'Ia',
            '排液': 'Da',
            '绝对位移': 'Mp',
            '相对上移': 'Up',
            '相对下移': 'Dp',
            '阀控制': 'Vc',
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

    def It(self, speed=1000, power=100, enable=0):
        ser_cmd = self.Cmd_Conf(self.contr_cmd['置零'], speed, power, enable)
        return ser_cmd

    def Ia(self, aspir_vol, start_spd=1000, stop_spd=10):
        ser_cmd = self.Cmd_Conf(self.contr_cmd['吸液'], aspir_vol, start_spd, stop_spd)
        return ser_cmd

    def Da(self, dis_vol, start_spd=1000, stop_spd=10, suc_vol=0):
        ser_cmd = self.Cmd_Conf(self.contr_cmd['排液'], dis_vol, start_spd, stop_spd, suc_vol)
        return ser_cmd

    def Mp(self, ab_pos, start_spd=4000, stop_spd=1000):
        ser_cmd = self.Cmd_Conf(self.contr_cmd['绝对位移'], ab_pos, start_spd, stop_spd)
        return ser_cmd

    def Up(self, rela_pos, start_spd=4000, stop_spd=1000):
        ser_cmd = self.Cmd_Conf(self.contr_cmd['相对上移'], rela_pos, start_spd, stop_spd)
        return ser_cmd

    def Dp(self, rela_pos, start_spd=4000, stop_spd=1000, sp_dis=0):
        ser_cmd = self.Cmd_Conf(self.contr_cmd['相对下移'], rela_pos, start_spd, stop_spd, sp_dis)
        return ser_cmd

    def Vc(self, pc_mode=0):
        ser_cmd = self.Cmd_Conf(self.contr_cmd['阀控制'], pc_mode)
        return ser_cmd


class pusi_cmd:
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


class pusi_modbus_cmd:
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


class drs_cmd:
    def __init__(self, address: int):
        """
        初始化旋切阀keyto查询指令
        :param address: 地址
        """
        self.prot = pr.keyto_gen_prot()
        self.address = address
        self.check_cmd = {
            '读取当前状态': '90',
            '读取最大速度': '91',
            '读取最小速度': '92',
            '读取加速度': '93',
            '读取减速度': '94',
            '读取额定电流': '95',
            '读取细分': '96',
            '读取电机方向': '97',
            '读取光耦触发模式': '98',
            '阀节能模式': '99',
            '读取主动上传使能': '9A',
            '读取背隙': '9B',
            '读取最大体积': '9C',
            '读取行程': '9D',
            '读取串口波特率': '9E',
            '读取当前位置': '9F',
            '读取位置计数模式': 'B0',
            '读取编码器使能': 'B1',
            '读取编码器分辨率': 'B2',
            '读取编码器方向': 'B3',
            '读取设备ID': 'F0',
            '读取型号': 'F1',
            '读取软件版本号': 'F2'
        }
        self.set_cmd = {
            '设置最大速度': '51',
            '设置最小速度': '52',
            '设置加速度': '53',
            '设置减速度': '54',
            '设置额定电流': '55',
            '设置细分': '56',
            '设置电机方向': '57',
            '设置光耦触发模式': '58',
            '设置阀节能使能': '59',
            '设置主动上传使能': '6A',
            '设置背隙': '6B',
            '设置最大体积': '6C',
            '设置行程': '6D',
            '设置串口波特率': '6E',
            '设置地址': '6F',
            '设置位置计数模式': '70',
            '设置编码器使能': '71',
            '设置编码器分辨率': '72',
            '设置编码器方向': '73',
            '掉电保存': 'EF',
            '恢复出厂设置': 'EE'
        }
        self.control_cmd = {
            '速度模式运动': '01',
            '指定位置位移': '02',
            '指定方向位移': '03',
            '位移到光耦处': '04',
            '置零': '05',
            '停止': '06',
            '清除错误': '07',
            '阀控制': '08',
            '吸液': '10',
            '排液': '11',
            '指定速度位移': '31',
        }

    def Read_State(self):
        return self.prot.Gen_Cmd_Conf(self.address, self.check_cmd['读取当前状态'], 0)

    def Read_MaxSpeed(self):
        return self.prot.Gen_Cmd_Conf(self.address, self.check_cmd['读取最大速度'], 0)

    def Read_MinSpeed(self):
        return self.prot.Gen_Cmd_Conf(self.address, self.check_cmd['读取最小速度'], 0)

    def Read_Acceleration(self):
        return self.prot.Gen_Cmd_Conf(self.address, self.check_cmd['读取加速度'], 0)

    def Read_Deceleration(self):
        return self.prot.Gen_Cmd_Conf(self.address, self.check_cmd['读取减速度'], 0)

    def Read_Current(self):
        return self.prot.Gen_Cmd_Conf(self.address, self.check_cmd['读取额定电流'], 0)

    def Read_Mres(self):
        return self.prot.Gen_Cmd_Conf(self.address, self.check_cmd['读取细分'], 0)

    def Read_Dir(self):
        return self.prot.Gen_Cmd_Conf(self.address, self.check_cmd['读取电机方向'], 0)

    def Read_TriggerMode(self):
        return self.prot.Gen_Cmd_Conf(self.address, self.check_cmd['读取光耦触发模式'], 0)

    def Read_EnergySaveMode(self):
        return self.prot.Gen_Cmd_Conf(self.address, self.check_cmd['阀节能模式'], 0)

    def Read_ReportMode(self):
        return self.prot.Gen_Cmd_Conf(self.address, self.check_cmd['读取主动上传使能'], 0)

    def Read_Backlash(self):
        return self.prot.Gen_Cmd_Conf(self.address, self.check_cmd['读取背隙'], 0)

    def Read_MaxVolume(self):
        return self.prot.Gen_Cmd_Conf(self.address, self.check_cmd['读取最大体积'], 0)

    def Read_Travel(self):
        return self.prot.Gen_Cmd_Conf(self.address, self.check_cmd['读取行程'], 0)

    def Read_SerialBaudRate(self):
        return self.prot.Gen_Cmd_Conf(self.address, self.check_cmd['读取串口波特率'], 0)

    def Read_Position(self):
        return self.prot.Gen_Cmd_Conf(self.address, self.check_cmd['读取当前位置'], 0)

    def Read_PositionMode(self):
        return self.prot.Gen_Cmd_Conf(self.address, self.check_cmd['读取位置计数模式'], 0)

    def Read_EncoderMode(self):
        return self.prot.Gen_Cmd_Conf(self.address, self.check_cmd['读取编码器使能'], 0)

    def Read_EncoderResolution(self):
        return self.prot.Gen_Cmd_Conf(self.address, self.check_cmd['读取编码器分辨率'], 0)

    def Read_EncoderDir(self):
        return self.prot.Gen_Cmd_Conf(self.address, self.check_cmd['读取编码器方向'], 0)

    def Read_ID(self):
        return self.prot.Gen_Cmd_Conf(self.address, self.check_cmd['读取设备ID'], 0)

    def Read_Model(self):
        return self.prot.Gen_Cmd_Conf(self.address, self.check_cmd['读取型号'], 0)

    def Read_Version(self):
        return self.prot.Gen_Cmd_Conf(self.address, self.check_cmd['读取软件版本号'], 0)

    def Set_MaxSpeed(self, num: int):
        return self.prot.Gen_Cmd_Conf(self.address, self.set_cmd['设置最大速度'], num)

    def Set_MinSpeed(self, num: int):
        return self.prot.Gen_Cmd_Conf(self.address, self.set_cmd['设置最小速度'], num)

    def Set_Acceleration(self, num: int):
        return self.prot.Gen_Cmd_Conf(self.address, self.set_cmd['设置加速度'], num)

    def Set_Deceleration(self, num: int):
        return self.prot.Gen_Cmd_Conf(self.address, self.set_cmd['设置减速度'], num)

    def Set_Current(self, num: int):
        return self.prot.Gen_Cmd_Conf(self.address, self.set_cmd['设置额定电流'], num)

    def Set_Mres(self, num: int):
        return self.prot.Gen_Cmd_Conf(self.address, self.set_cmd['设置细分'], num)

    def Set_Dir(self, num: int):
        return self.prot.Gen_Cmd_Conf(self.address, self.set_cmd['设置电机方向'], num)

    def Set_TriggerMode(self, num: int):
        return self.prot.Gen_Cmd_Conf(self.address, self.set_cmd['设置光耦触发模式'], num)

    def Set_EnergySaveMode(self, num: int):
        return self.prot.Gen_Cmd_Conf(self.address, self.set_cmd['设置阀节能使能'], num)

    def Set_ReportMode(self, num: int):
        return self.prot.Gen_Cmd_Conf(self.address, self.set_cmd['设置主动上传使能'], num)

    def Set_Backlash(self, num: int):
        return self.prot.Gen_Cmd_Conf(self.address, self.set_cmd['设置背隙'], num)

    def Set_MaxVolume(self, num: int):
        return self.prot.Gen_Cmd_Conf(self.address, self.set_cmd['设置最大体积'], num)

    def Set_Travel(self, num: int):
        return self.prot.Gen_Cmd_Conf(self.address, self.set_cmd['设置行程'], num)

    def Set_SerialBaudRate(self, num: int):
        return self.prot.Gen_Cmd_Conf(self.address, self.set_cmd['设置串口波特率'], num)

    def Set_Address(self, num: int):
        return self.prot.Gen_Cmd_Conf(self.address, self.set_cmd['设置地址'], num)

    def Set_PositionMode(self, num: int):
        return self.prot.Gen_Cmd_Conf(self.address, self.set_cmd['设置位置计数模式'], num)

    def Set_EncodeMode(self, num: int):
        return self.prot.Gen_Cmd_Conf(self.address, self.set_cmd['设置编码器使能'], num)

    def Set_EncodeResolution(self, num: int):
        return self.prot.Gen_Cmd_Conf(self.address, self.set_cmd['设置编码器分辨率'], num)

    def Set_EncodeDir(self, num=1800):
        return self.prot.Gen_Cmd_Conf(self.address, self.set_cmd['设置编码器方向'], num)

    def Set_Poweroff_Save(self):
        return self.prot.Gen_Cmd_Conf(self.address, self.set_cmd['掉电保存'], 123456)

    def Set_Restore_Set(self):
        return self.prot.Gen_Cmd_Conf(self.address, self.set_cmd['恢复出厂设置'], 123456)

    def Move_SpeedMode(self, num: int):
        return self.prot.Gen_Cmd_Conf(self.address, self.set_cmd['速度模式运动'], num)

    def Move_DesignatedPosition(self, num: int):
        return self.prot.Gen_Cmd_Conf(self.address, self.set_cmd['指定位置位移'], num)

    def Move_DesignatedDir(self, num: int):
        return self.prot.Gen_Cmd_Conf(self.address, self.set_cmd['指定方向位移'], num)

    def Move_ToOpt(self, num: int):
        return self.prot.Gen_Cmd_Conf(self.address, self.set_cmd['位移到光耦处'], num)

    def Init(self, num: int):
        return self.prot.Gen_Cmd_Conf(self.address, self.control_cmd['置零'], num)

    def Stop(self, num: int):
        return self.prot.Gen_Cmd_Conf(self.address, self.control_cmd['停止'], num)

    def Clean_Error(self):
        return self.prot.Gen_Cmd_Conf(self.address, self.control_cmd['清除错误'], 0)

    def Valve_Control(self, num: int):
        return self.prot.Gen_Cmd_Conf(self.address, self.control_cmd['阀控制'], num)

    def Aspiration(self, num: int):
        return self.prot.Gen_Cmd_Conf(self.address, self.control_cmd['吸液'], num)

    def Dispense(self, num: int):
        return self.prot.Gen_Cmd_Conf(self.address, self.control_cmd['排液'], num)

    def Move_DesignatedSpeed(self, num: int):
        return self.prot.Gen_Cmd_Conf(self.address, self.control_cmd['指定速度位移'], num)


def Negative_To_Hex(num: int):
    if num < 0:
        # 负数转为补码
        num = (1 << 32) + num
    return hex(num).removeprefix('0x')
