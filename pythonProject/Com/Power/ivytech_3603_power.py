import datetime
import time

import Com.Port.serialport as sp


class POWER:
    def __init__(self):
        self.pw_ser = sp.pwr
        self.receive_data = ''

    def PwrReceive(self, timeout=1000):
        """
        获取返回数据
        :param timeout: 超时时间：毫秒,0:不检测超时
        :return: 0：无返回，else：接收到数据字节个数
        """
        self.receive_data = ''
        if self.pw_ser.PortReceive_Data(1, timeout):
            self.receive_data += self.pw_ser.rx_buf
            while True:
                if self.pw_ser.PortReceive_Data(1, 5):
                    self.receive_data += self.pw_ser.rx_buf
                    continue
                else:
                    print(datetime.datetime.now(), end=':')
                    print('Rece:', self.receive_data, '->', bytes.fromhex(self.receive_data).decode())
                    return len(self.receive_data) // 2
        else:
            print(datetime.datetime.now(), end=':')
            print('pwr:数据无返回！')
            return 0

    def Check_Power_State(self):
        """
        查询电源当前状态信息
        :return: True：正常，无错误信息  False：有异常
        """
        print('查询电源状态：')
        cmd = ':\r\nSYST:ERR?\r\n' + r'\n'  # '3A 0D 0A 53 59 53 54 3A 45 52 52 3F 0D 0A 5C 6E'
        self.pw_ser.PortSend(cmd.encode())
        print(datetime.datetime.now(), end=':')
        print('Send:', cmd)
        if self.PwrReceive():
            ascii_data = self.receive_data.replace('0A', '')
            power_state = int(ascii_data.split('2C')[0], 16) - 0x30
            if power_state == 0:
                return True
            else:
                print('电源状态错误！请检查电源')
                return False
        else:
            print('电源无返回，请检查通讯!')
            return False

    def Mode_Select(self, flag=0):
        """
        设置电源控制模式
        :param flag:0：面板控制  1：PC控制
        :return:True：设置成功  False：设置失败
        """
        rem_cmd = ':\r\nSYST:REM\r\n' + r'\n'
        loc_cmd = ':\r\nSYST:LOC\r\n' + r'\n'
        if flag == 1:
            print('设置电源为PC控制模式')
            print(datetime.datetime.now(), end=':')
            self.pw_ser.PortSend(rem_cmd.encode())
            print('Send:', rem_cmd)
        else:
            print('设置电源为面板控制模式：')
            print(datetime.datetime.now(), end=':')
            self.pw_ser.PortSend(loc_cmd.encode())
            print('Send:', loc_cmd)
        if self.PwrReceive():
            ascii_data = self.receive_data.replace('0A', '')
            mode_state = bytes.fromhex(ascii_data.split('3A')[1]).decode()
            if mode_state == 'REM':
                print('设置电源为PC控制模式->成功')
                return True
            elif mode_state == 'LOC':
                print('设置电源为面板控制模式->成功')
                return True
            else:
                print('返回有误！')
                return False
        else:
            print('电源无返回，请检查通讯!')
            return False

    def Power_Output_Control(self, flag=0):
        """
        打开/关闭电源
        :param flag: 0：关闭电源（默认关闭） 1：打开电源开关
        :return: True0：设置成功  False：设置失败
        """
        on_cmd = ':\r\nOUTP 1\r\n' + r'\n'
        off_cmd = ':\r\nOUTP 0\r\n' + r'\n'
        if flag == 1:
            print('打开电源')
            print(datetime.datetime.now(), end=':')
            self.pw_ser.PortSend(on_cmd.encode())
            print('Send:', on_cmd)
        else:
            print('关闭电源：')
            print(datetime.datetime.now(), end=':')
            self.pw_ser.PortSend(off_cmd.encode())
            print('Send:', off_cmd)
        if self.PwrReceive():
            ascii_data = self.receive_data.replace('0A', '')
            mode_state = int(ascii_data.split('20')[1], 16) - 0x30
            if flag == 1:
                if mode_state == 1:
                    print('电源打开成功')
                    return True
            else:
                if mode_state == 0:
                    print('电源关闭成功')
                    return True
            print('返回有误！')
            return False
        else:
            print('电源无返回，请检查通讯!')
            return False

    def Read_Voltage(self):
        """
        查询电源输出电压值
        :return: 0：无返回  else：点前电压值，单位：V
        """
        check_vol_cmd = ':\r\nVOLT?' + '\r\n' + r'\n'
        print('查询电源电压值:')
        print(datetime.datetime.now(), end=':')
        self.pw_ser.PortSend(check_vol_cmd.encode())
        print('Send:', check_vol_cmd)
        if self.PwrReceive():
            ascii_data = self.receive_data.replace('0A', '')
            vol_str = ''
            for i in range(0, len(ascii_data), 2):
                vol_str += chr(int(ascii_data[i:i + 2], 16))
            return float(vol_str)
        else:
            print('电源无返回，请检查通讯!')
            return 0

    def Read_Current(self):
        """
        查询电源输出电流值
        :return: 0：无返回  else：点前电流值，单位：A
        """
        check_cur_cmd = ':\r\nCURR?' + '\r\n' + r'\n'
        print('查询电源电流值:')
        print(datetime.datetime.now(), end=':')
        self.pw_ser.PortSend(check_cur_cmd.encode())
        print('Send:', check_cur_cmd)
        if self.PwrReceive():
            ascii_data = self.receive_data.replace('0A', '')
            vol_str = ''
            for i in range(0, len(ascii_data), 2):
                vol_str += chr(int(ascii_data[i:i + 2], 16))
            return float(vol_str)
        else:
            print('电源无返回，请检查通讯!')
            return 0

    def Set_Voltage(self, vol: float):
        """
        设置电源电压
        :param vol: 电压设定值，单位：V
        :return: 0：设置成功  1：设置失败
        """
        set_vol_cmd = ':\r\nVOLT ' + str(vol) + '\r\n' + r'\n'
        print('设置电压为:', vol)
        print(datetime.datetime.now(), end=':')
        print('Send:', set_vol_cmd)
        self.pw_ser.PortSend(set_vol_cmd.encode())
        if self.PwrReceive():
            ascii_data = self.receive_data.replace('0A', '').split('20')[1]
            vol_str = ''
            for i in range(0, len(ascii_data), 2):
                vol_str += chr(int(ascii_data[i:i + 2], 16))
            vol_f = float(vol_str)
            if vol_f == vol:
                print('电压设置成功')
                return True
            else:
                print('电压设置失败')
                return False
        else:
            print('电源无返回，请检查通讯!')
            return False

    def Set_Current(self, cur: float):
        """
        设置电源电压
        :param cur: 电流设定值，单位：A
        :return: 0：设置成功  1：设置失败
        """
        set_vol_cmd = ':\r\nCURR ' + str(cur) + '\r\n' + r'\n'
        print('设置电流为:', cur)
        print(datetime.datetime.now(), end=':')
        self.pw_ser.PortSend(set_vol_cmd.encode())
        print('Send:', set_vol_cmd)
        if self.PwrReceive():
            ascii_data = self.receive_data.replace('0A', '').split('20')[1]
            cur_str = ''
            for i in range(0, len(ascii_data), 2):
                cur_str += chr(int(ascii_data[i:i + 2], 16))
            vol_f = float(cur_str)
            if vol_f == cur:
                print('电流设置成功')
                return True
            else:
                print('电流设置失败')
                return False
        else:
            print('电源无返回，请检查通讯!')
            return False

    def Read_Vol_And_Cur(self):
        """
        一次读取二个测量值：输出电压值、输出电流值
        :return:数值列表,[电压(V)，电流(A)]
        """
        vol_cur_cmd = ':\r\nMEAS:VCM?' + '\r\n' + r'\n'
        print('查询电压电流值:')
        print(datetime.datetime.now(), end=':')
        self.pw_ser.PortSend(vol_cur_cmd.encode())
        print('Send:', vol_cur_cmd)
        if self.PwrReceive():
            ascii_data = self.receive_data.replace('0A', '')
            data_lst = ascii_data.split('2C')
            vol_str = ''
            cur_str = ''
            for i in range(0, len(data_lst[0]), 2):
                vol_str += chr(int(data_lst[0][i:i + 2], 16))
            for i in range(0, len(data_lst[1]), 2):
                cur_str += chr(int(data_lst[1][i:i + 2], 16))
            out_data = [float(vol_str), float(cur_str)]
            return out_data
        else:
            print('电源无返回，请检查通讯!')
            return 0


if __name__ == '__main__':
    sp.Reset_Ser_Baud(2, 'com36', 9600)
    power = POWER()
    power.Check_Power_State()
    power.Mode_Select(1)
    power.Power_Output_Control(0)
    power.Set_Voltage(24)
    power.Set_Current(3)
    power.Power_Output_Control(1)
    while True:
        power.Check_Power_State()
        data = power.Read_Vol_And_Cur()
        print('电压：', data[0], '电流：', data[1])
        time.sleep(2)
