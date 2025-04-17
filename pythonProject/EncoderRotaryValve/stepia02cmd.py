from binascii import a2b_hex
import Com.Port.check as ck


class STEPIA02_GENERIC_CMD:
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


def EncoderRotaryValve_Dt_To_Oem(dt_data: str):
    """
    编码器旋转阀通用协议DT指令转OEM指令
    :param dt_data: DT指令：str
    :return: 0：DT指令格式错误；else：OEM指令：str
    """
    if '>' in dt_data:
        address = dt_data.split('>')[0]  # 获取ADP地址：字符串
        data = dt_data.split('>')[1]  # 获取数据
        address_hex = hex(int(address)).replace('0x', '').zfill(2)
        data_hex = data.encode().hex()
        data_len_hex = hex(len(data_hex) // 2).replace('0x', '').zfill(2)
        oem_data = 'AA' + '80' + address_hex + data_len_hex + data_hex
    elif '<' in dt_data:
        address = dt_data.split('<')[0]  # 获取ADP地址：字符串
        address_hex = hex(int(address)).replace('0x', '').zfill(2)
        if ':' in dt_data:
            adp_state = dt_data.split('<')[1].split(':')[0]
            data = dt_data.split(':')[1]
            state_hex = hex(int(adp_state, 10)).replace('0x', '').zfill(2)
            data_hex = data.encode().hex()
            data_hex_len = hex(len(data_hex) // 2).replace('0x', '').zfill(2)
            oem_data = '55' + '80' + address_hex + state_hex + data_hex_len + data_hex
        else:
            data_hex_len = '00'
            adp_state = dt_data.split('<')[1]  # 获取数据
            state_hex = hex(int(adp_state, 10)).replace('0x', '').zfill(2)
            oem_data = '55' + '80' + address_hex + state_hex + data_hex_len
    else:
        print('数据格式错误！:', dt_data)
        return 0
    and_verification_hex = ck.Uchar_Checksum_8_Bit(oem_data)
    oem_cmd_hex = oem_data.upper() + and_verification_hex
    return oem_cmd_hex


def EncoderRotaryValve_Oem_To_Dt(oem_str: str):
    """
    编码器旋转阀通用协议OEM指令转DT指令
    :param oem_str: OEM指令：str
    :return: 0：OEM指令格式错误；else：DT指令：str
    """
    dt_data = ''
    oem_str = oem_str.replace(" ", "")
    if 'AA' in oem_str:
        dt_address = str(int(oem_str[4:6], 16))
        dt_dat_str = a2b_hex(oem_str[11:-2]).decode()
        dt_data = str(dt_address) + '>' + dt_dat_str
    elif '55' in oem_str:
        dt_address = str(int(oem_str[4:6], 16))
        dt_dat_len = int(oem_str[8:10], 16)
        dt_state = str(int(oem_str[6:8], 16))
        if dt_dat_len == 0:
            dt_data = dt_address + '<' + str(dt_state)
        else:
            re_data = a2b_hex(bytes(oem_str[10:-2], 'utf-8')).decode()
            dt_data = dt_address + '<' + dt_state + ':' + re_data
    else:
        print('数据格式错误！:', oem_str)
        return 0
    return dt_data
