# ADP指令
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

    # def SYS_PAUSE(self, h_time):
    #     h_command = str(self.adp_address) + '>' + 'H' + str(h_time)
    #     return h_command

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


class adp_cmd:
    def __init__(self):
        self.gap_flag = '2C'
        self.fix_par = 123456
        self.contr_cmd = {
            '置零': 'It',
            '吸液': 'Ia',
            '排液': 'Da',
            '退TIP': 'Dt',
            '绝对位移': 'Mp',
            '相对上移': 'Up',
            '相对下移': 'Dp',
            '液面探测': 'Ld',
            '吸液跟随': 'Iz',
            '排液跟随': 'Dz',
            '抗悬滴': 'Pc',
            '排空': 'De',
            '滤芯检测': 'Dc',
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

    def It(self, speed=500, power=100, enable=0):
        ser_cmd = self.Cmd_Conf(self.contr_cmd['置零'], speed, power, enable)
        return ser_cmd

    def Ia(self, aspir_vol, start_spd=500, stop_spd=10, compensate=0):
        ser_cmd = self.Cmd_Conf(self.contr_cmd['吸液'], aspir_vol, start_spd, stop_spd, compensate)
        return ser_cmd

    def Da(self, dis_vol, suc_vol=0, start_spd=500, stop_spd=10):
        ser_cmd = self.Cmd_Conf(self.contr_cmd['排液'], dis_vol, suc_vol, start_spd, stop_spd)
        return ser_cmd

    def Dt(self, speed=500, enable=0):
        ser_cmd = self.Cmd_Conf(self.contr_cmd['退TIP'], speed, enable)
        return ser_cmd

    def Ld(self, report_mode=1, timeout=10000, plld_mode=1):
        ser_cmd = self.Cmd_Conf(self.contr_cmd['液面探测'], report_mode, timeout, plld_mode)

        return ser_cmd

    def Mp(self, ab_pos, start_spd=128000, stop_spd=32000):
        ser_cmd = self.Cmd_Conf(self.contr_cmd['绝对位移'], ab_pos, start_spd, stop_spd)
        return ser_cmd

    def Up(self, rela_pos, start_spd=128000, stop_spd=32000):
        ser_cmd = self.Cmd_Conf(self.contr_cmd['相对上移'], rela_pos, start_spd, stop_spd)
        return ser_cmd

    def Dp(self, rela_pos, start_spd=128000, stop_spd=32000, sp_dis=0):
        ser_cmd = self.Cmd_Conf(self.contr_cmd['相对下移'], rela_pos, start_spd, stop_spd, sp_dis)
        return ser_cmd

    def Pc(self, pc_mode=0, speed=200, max_pos=50):
        ser_cmd = self.Cmd_Conf(self.contr_cmd['抗悬滴'], pc_mode, speed, max_pos)
        return ser_cmd

    def Iz(self, aspir_vol, start_spd=100, sur_area=78, z_pos=0):
        ser_cmd = self.Cmd_Conf(self.contr_cmd['吸液跟随'], aspir_vol, start_spd, sur_area, z_pos)
        return ser_cmd

    def Dz(self, dis_vol, start_spd=100, sur_area=78):
        ser_cmd = self.Cmd_Conf(self.contr_cmd['排液跟随'], dis_vol, start_spd, sur_area)
        return ser_cmd

    def De(self, start_spd=500):
        ser_cmd = self.Cmd_Conf(self.contr_cmd['排空'], start_spd)
        return ser_cmd

    def Dc(self):
        ser_cmd = self.Cmd_Conf(self.contr_cmd['滤芯检测'])
        return ser_cmd

    # 系统操作指令


# Z轴控制指令
class z_cmd:
    def __init__(self):
        self.gap_flag = '2C'
        self.fix_par = 123456
        self.contr_cmd = {
            '置零': 'Zz',
            '取TIP': 'Zg',
            '绝对位移': 'Zp',
            '相对上移': 'Zu',
            '相对下移': 'Zd',
            '停止': 'Zt'
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

    def Zz(self, speed=50000):
        z_cmd_str = self.Cmd_Conf(self.contr_cmd['置零'], speed)
        return z_cmd_str

    def Zp(self, position, speed=50000):
        z_cmd_str = self.Cmd_Conf(self.contr_cmd['绝对位移'], position, speed)
        return z_cmd_str

    def Zu(self, position, speed=50000):
        z_cmd_str = self.Cmd_Conf(self.contr_cmd['相对上移'], position, speed)
        return z_cmd_str

    def Zd(self, position, speed=50000):
        z_cmd_str = self.Cmd_Conf(self.contr_cmd['相对下移'], position, speed)
        return z_cmd_str

    def Zg(self, speed=50000, power=80):
        z_cmd_str = self.Cmd_Conf(self.contr_cmd['取TIP'], speed, power)
        return z_cmd_str

    def Zt(self, mode=80):
        z_cmd_str = self.Cmd_Conf(self.contr_cmd['停止'], mode)
        return z_cmd_str


if __name__ == '__main__':
    ap = adp_cmd()
    print(ap.De())
