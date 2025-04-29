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
