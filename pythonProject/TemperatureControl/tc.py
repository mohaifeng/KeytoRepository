# import Com.Port.serialport as sp
# import Com.Protocol.protocol as pr


class TC_Cmd:
    def __init__(self):
        self.gap_flag = '2C'
        self.fix_par = 123456
        self.contr_cmd = {
            '设定温度': 'Tc',
        }
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

    def cmd_conf(self, cmd: str, *args: int):
        cmd_hex = cmd.encode('utf-8').hex()
        par = ''
        for i in range(0, len(args)):
            if i < len(args) - 1:
                par += str(args[i]).encode().hex() + self.gap_flag
            else:
                par += str(args[i]).encode().hex()
        return (cmd_hex + par).upper()

    def Tc(self, temp: float):
        ser_cmd = self.cmd_conf(self.contr_cmd['设定温度'], int(temp * 10))
        return ser_cmd


class TC:
    def __init__(self):
        self.tc_ser = sp.ser
        self.report_flag = 0  # 主动上报模式 0：关闭 1：打开
        self.protocol = 'KT_DT'  # KT_DT:keyto DT协议格式；KT_OEM：keyto OEM协议格式
        self.state = 0
        self.address = 30
        self.tc_name = ''
        self.tc_version = ''
        self.dt_prot = pr.keyto_dt_prot(self.address)
        self.oem_prot = pr.keyto_oem_prot(self.address)
        self.send_data = ''
        self.receive_data = ''


if __name__ == '__main__':
    num = [x for x in range(2, 17, 2)]
    print(num)