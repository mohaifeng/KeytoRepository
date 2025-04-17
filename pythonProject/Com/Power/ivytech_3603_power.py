import datetime
import Com.Port.serialport as sp


class POWER:
    def __init__(self):
        self.pw_ser = sp.bal

    def Check_Power_State(self):
        """
        查询电源当前状态信息
        :return: True：正常，无错误信息  False：有异常
        """
        print('查询电源状态：')
        cmd = ':\r\nSYST:ERR?\r\n' + r'\n'
        self.pw_ser.PortSend(cmd.encode())
        print(datetime.datetime.now(), end=':')
        print('Send:', cmd)
        sta_rec = self.pw_ser.PortReceive_Data()
        print(datetime.datetime.now(), end=':')
        if sta_rec:
            ascii_data = sta_rec.decode('utf-8').replace('\n', '')
            print('Receive:', ascii_data)
            power_state = int(ascii_data.split(',')[0])
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
        :return:0：设置成功  1：设置失败
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
        sta_rec = self.pw_ser.PortReceive(1000)
        print(datetime.datetime.now(), end=':')
        if sta_rec:
            ascii_data = sta_rec.decode('utf-8').replace('\n', '')
            print('Receive:', ascii_data)
            mode_state = ascii_data.split(':')[1]
            if flag == 1:
                if mode_state == 'REM':
                    return 0
            elif flag == 0:
                if mode_state == 'LOC':
                    return 0
            print('返回有误！')
            return 1
        else:
            print('电源无返回，请检查通讯!')
            return 1

    def Power_Output_Control(self, flag=0):
        """
        打开/关闭电源
        :param flag: 0：关闭电源（默认关闭） 1：打开电源开关
        :return: 0：设置成功  1：设置失败
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
        sta_rec = self.pw_ser.PortReceive(1000)
        print(datetime.datetime.now(), end=':')
        if sta_rec:
            ascii_data = sta_rec.decode('utf-8').replace('\n', '')
            print('Receive:', ascii_data)
            mode_state = int(ascii_data.split(' ')[1])
            if flag == 1:
                if mode_state == 1:
                    print('电源打开成功')
                    return 0
            elif flag == 0:
                if mode_state == 0:
                    print('电源关闭成功')
                    return 0
            print('返回有误！')
            return 1
        else:
            print('电源无返回，请检查通讯!')
            return 1

    def Read_Voltage(self):
        """
        查询电源输出电压值
        :return: 1：无返回  else：点前电压值，单位：V
        """
        check_vol_cmd = ':\r\nVOLT?' + '\r\n' + r'\n'
        print('查询电源电压值:')
        print(datetime.datetime.now(), end=':')
        self.pw_ser.PortSend(check_vol_cmd.encode())
        print('Send:', check_vol_cmd)
        sta_rec = self.pw_ser.PortReceive(1000)
        print(datetime.datetime.now(), end=':')
        if sta_rec:
            ascii_data = sta_rec.decode('utf-8').replace('\n', '')
            print('Receive:', ascii_data)
            check_vol = float(ascii_data)
            return check_vol
        else:
            print('电源无返回，请检查通讯!')
            return 1

    def Read_Current(self):
        """
        查询电源输出电流值
        :return: 1：无返回  else：点前电流值，单位：A
        """
        check_cur_cmd = ':\r\nCURR?' + '\r\n' + r'\n'
        print('查询电源电流值:')
        print(datetime.datetime.now(), end=':')
        self.pw_ser.PortSend(check_cur_cmd.encode())
        print('Send:', check_cur_cmd)
        sta_rec = self.pw_ser.PortReceive(1000)
        print(datetime.datetime.now(), end=':')
        if sta_rec:
            ascii_data = sta_rec.decode('utf-8').replace('\n', '')
            print('Receive:', ascii_data)
            check_vol = float(ascii_data)
            return check_vol
        else:
            print('电源无返回，请检查通讯!')
            return 1

    def Set_Voltage(self, vol: float):
        """
        设置电源电压
        :param vol: 电压设定值，单位：V
        :return: 0：设置成功  1：设置失败
        """
        set_vol_cmd = ':\r\nVOLT ' + str(vol) + '\r\n' + r'\n'
        print('设置电压为:', vol)
        print(datetime.datetime.now(), end=':')
        self.pw_ser.PortSend(set_vol_cmd.encode())
        print('Send:', set_vol_cmd)
        sta_rec = self.pw_ser.PortReceive(1000)
        print(datetime.datetime.now(), end=':')
        if sta_rec:
            ascii_data = sta_rec.decode('utf-8').replace('\n', '')
            print('Receive:', ascii_data)
            vol_rec = float(ascii_data.split(' ')[1])
            if vol_rec == vol:
                print('电压设置成功')
                return 0
            else:
                print('电压设置失败')
                return 1
        else:
            print('电源无返回，请检查通讯!')
            return 1

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
        sta_rec = self.pw_ser.PortReceive(1000)
        print(datetime.datetime.now(), end=':')
        if sta_rec:
            ascii_data = sta_rec.decode('utf-8').replace('\n', '')
            print('Receive:', ascii_data)
            vol_rec = float(ascii_data.split(' ')[1])
            if vol_rec == cur:
                print('电流设置成功')
                return 0
            else:
                print('电流设置失败')
                return 1
        else:
            print('电源无返回，请检查通讯!')
            return 1

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
        sta_rec = self.pw_ser.PortReceive(1000)
        print(datetime.datetime.now(), end=':')
        if sta_rec:
            ascii_data = sta_rec.decode('utf-8').replace('\n', '')
            print('Receive:', ascii_data)
            data_lst = ascii_data.split(',')
            read_vol = float(data_lst[0])
            read_cur = float(data_lst[1])
            out_data = [read_vol, read_cur]
            return out_data
        else:
            print('电源无返回，请检查通讯!')
            return 1


if __name__ == '__main__':
    power = POWER()
    power.Check_Power_State()
    power.Mode_Select(1)
    power.Power_Output_Control(0)
    power.Set_Voltage(24)
    power.Set_Current(1)
    print(power.Read_Voltage())
    print(power.Read_Current())
    power.Read_Vol_And_Cur()
