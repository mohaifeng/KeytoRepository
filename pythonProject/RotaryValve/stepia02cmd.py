from binascii import a2b_hex
import Com.Port.check as ck


class stepia02_generic_cmd:
    def __init__(self, address: int):
        """
        stepia02板卡兼容通用指令
        :param address:旋切阀地址
        """
        self.stepia02_address = str(address)
        self.stepia02cmd = {'置零': 'Rt',
                            '最优路径切换通道': 'Rp',
                            '指定方向切换通道': 'Rd',
                            '写寄存器': 'Wr',
                            '读寄存器': 'Rr',
                            '状态查询': '?',
                            '延时': 'L',
                            '停止': 'T',
                            '复位': 'U',
                            '恢复出厂设置': 'M',
                            '掉电保存设置': 'S',
                            '老化': 'RE'
                            }

    def Zeroing(self):
        rt_cmd = str(self.stepia02_address) + '>' + '置零'
        return rt_cmd

    def OptimalPathSwitchingSalve(self, channel: int):
        rp_cmd = str(self.stepia02_address) + '>' + '最优路径切换通道' + str(channel)
        return rp_cmd

    def DirectionalSwitchingSalve(self, channel: int, direction=0):
        rd_cmd = str(self.stepia02_address) + '>' + '指定方向切换通道' + str(channel) + ',' + str(direction)
        return rd_cmd

    def WriteRegister(self, register: int, value: int):
        rw_cmd = str(self.stepia02_address) + '>' + '写寄存器' + str(register) + ',' + str(value)
        return rw_cmd

    def ReadRegister(self, register: int, value=1):
        rr_cmd = str(self.stepia02_address) + '>' + '读寄存器' + str(register) + ',' + str(value)
        return rr_cmd

    def CheckState(self):
        ck_cmd = str(self.stepia02_address) + '>' + '状态查询'
        return ck_cmd

    def Delay(self, time: int):
        l_cmd = str(self.stepia02_address) + '>' + '延时' + ',' + str(time)
        return l_cmd

    def Stop(self):
        t_cmd = str(self.stepia02_address) + '>' + '停止'
        return t_cmd

    def Reset(self):
        u_cmd = str(self.stepia02_address) + '>' + '复位'
        return u_cmd

    def RestoreFactorySetting(self):
        m_cmd = str(self.stepia02_address) + '>' + '恢复出厂设置' + '123456'
        return m_cmd

    def PowerOffSave(self):
        s_cmd = str(self.stepia02_address) + '>' + '掉电保存设置'
        return s_cmd

    def Age(self, age_str):
        re_cmd = str(self.stepia02_address) + '>' + '老化' + ':' + str(age_str)
        return re_cmd
