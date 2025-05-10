# import os
import time
import Adp.pipette as pi
# import Com.Log.log as lg
import Com.Download.software_upgrade as dl
import Com.Port.serialport as sp
import Com.Power.ivytech_3603_power as pw
import RotaryValve.rotaryvalve as rv
import Pump.step_drl as dr

dev_default_par_dic = {
    'ADP16': [38400, 1],
    'ADP18': [38400, 1],
    '旋转阀': [9600, 0],
    '旋转阀-编码器': [9600, 0],
    '旋转阀-I2C': [9600, 0],
    '旋转阀-VICI': [9600, 0],
    'DRL-计量泵': [9600, 0],
    'DRL-柱塞泵': [9600, 0],
    'DRS-柱塞泵': [9600, 255],
    'VLG-柱塞泵': [9600, 255],
    'VLG-PUSI-柱塞泵': [9600, 255],
}

r"D:\Download\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe"


def Dev_Get_Addr(value):
    match value:
        case 1:
            adp = pi.pipette()
            if adp.GetAdpAddress():
                return adp.address
            else:
                return -1
        case 2:
            adp = pi.pipette()
            if adp.GetAdpAddress():
                return adp.address
            else:
                return -1
        case 3:
            rvc = rv.rotaryvalve()
            if rvc.Get_RV_Address():
                return rvc.address
            else:
                return -1
        case 4:
            print("Case 4")
        case 5:
            print("Case 5")
        case 6:
            print("Case 6")
        case 7:
            print("Case 7")
        case 8:
            drl = dr.DRL_PLUNGER_PUMP()
            if drl.GetDrlAddress():
                return drl.address
            else:
                return -1
        case 9:
            print("Case 3")
        case 10:
            print("Case 3")
        case 11:
            print("Case 3")
        case _:
            print("Default case")


def Download_Test(dev_type: int):
    """
    适配使用download下载的驱动器
    :param dev_type:
    1: 'ADP16',\n
    2: 'ADP18',\n
    3: '旋转阀',\n
    4: '旋转阀-编码器',\n
    5: '旋转阀-I2C',\n
    6: '旋转阀-VICI',\n
    7: 'DRL-计量泵',\n
    8: 'DRL-柱塞泵',\n
    9: 'DRS-柱塞泵',\n
    10: 'VLG-柱塞泵',\n
    11: 'VLG-PUSI-柱塞泵',\n
    :return: True or False
    """
    com_str = sp.List_Serial_Ports()
    baudrate_lst = dl.dev_support_baudrate[dl.type_dic[dev_type]]
    # 自动查询串口号默认波特率及地址
    connet_flag = 0  # 成功连接标志位

    default_baud = dev_default_par_dic[dl.type_dic[dev_type]][0]  # 初始化默认波特率
    default_com = ''  # 初始化默认串口号
    pw_default_com = ''
    default_addr = dev_default_par_dic[dl.type_dic[dev_type]][1]  # 初始化默认地址
    for pw_com_test in com_str:
        sp.Reset_Ser_Baud(2, pw_com_test, 9600)
        power = pw.POWER()
        if power.Check_Power_State():
            pw_default_com = pw_com_test
            break
    sp.Reset_Ser_Baud(2, pw_default_com, 9600)
    power = pw.POWER()
    pw_cmd = [power.Check_Power_State(), power.Mode_Select(1), power.Power_Output_Control(0), power.Set_Voltage(24),
              power.Set_Current(3), power.Power_Output_Control(1)]  # 电源设置为24V,3A，打开电源
    for pw_test in pw_cmd:
        if not pw_test:
            print('电源设置失败')
            return False
    # 获取当前通信串口号及波特率、地址
    com_str.remove(pw_default_com)
    for com_test in com_str:
        for baud_test in baudrate_lst:
            print('当前串口号：', com_test, '当前波特率：', baud_test)
            sp.Reset_Ser_Baud(0, com_test, baud_test)
            default_addr = Dev_Get_Addr(dev_type)
            if default_addr > -1:
                default_baud = baud_test  # 获取正确波特率
                default_com = com_test  # 获取正确串口号
                connet_flag = 1
                break
        if connet_flag == 1:
            break
    if connet_flag == 0:
        print('通讯异常，请检查通讯')
        return False
    del baudrate_lst[baudrate_lst.index(default_baud)]  # 去掉当前波特率
    baudrate_lst.insert(0, default_baud)  # 将当前波特率加入到第一个元素
    target_version_lst = []
    present_version_lst = []

    # 开始程序下载测试流程
    for idex in range(0, len(baudrate_lst)):
        print('当前下载波特率：', baudrate_lst[idex])
        sp.Reset_Ser_Baud(0, default_com, baudrate_lst[idex])
        if dev_type == 1 or dev_type == 2:
            obj = pi.pipette()
            obj.address = default_addr  # 重设ADP地址
        elif dev_type == 3:
            obj = rv.rotaryvalve()
            obj.Change_RV_Address(default_addr)  # 重设ADP地址
        elif dev_type == 4:
            obj = pi.pipette()
            obj.address = default_addr  # 重设ADP地址
        elif dev_type == 5:
            obj = pi.pipette()
            obj.address = default_addr  # 重设ADP地址
        elif dev_type == 6:
            obj = pi.pipette()
            obj.address = default_addr  # 重设ADP地址
        elif dev_type == 7:
            obj = pi.pipette()
            obj.address = default_addr  # 重设ADP地址
        elif dev_type == 8:
            obj = dr.DRL_PLUNGER_PUMP()
            obj.address = default_addr  # 重设ADP地址
        elif dev_type == 9:
            obj = dr.DRL_PLUNGER_PUMP()
            obj.address = default_addr  # 重设ADP地址
        elif dev_type == 10:
            obj = dr.DRL_PLUNGER_PUMP()
            obj.address = default_addr  # 重设ADP地址
        elif dev_type == 11:
            obj = dr.DRL_PLUNGER_PUMP()
            obj.address = default_addr  # 重设ADP地址
        else:
            obj = pi.pipette()
            obj.address = default_addr  # 重设ADP地址
        if not dl.DownLoadPlus(dev_type, 0):  # 进行升级操作
            print('升级失败')
            return False
        time.sleep(3)
        version = ''
        if dev_type == 1 or dev_type == 2:
            obj.adp_ser.OpenPort()  # 升完级默认关闭串口
            if obj.GetAdpVersion():
                version = obj.adp_version
        elif dev_type == 3:
            obj.rv_ser.OpenPort()  # 升完级默认关闭串口
            if obj.Get_RV_Version():
                version = obj.rv_version
        elif dev_type == 4:
            obj.adp_ser.OpenPort()  # 升完级默认关闭串口
            if obj.GetAdpVersion():
                version = obj.adp_version
        elif dev_type == 5:
            obj.adp_ser.OpenPort()  # 升完级默认关闭串口
            if obj.GetAdpVersion():
                version = obj.adp_version
        elif dev_type == 6:
            obj.adp_ser.OpenPort()  # 升完级默认关闭串口
            if obj.GetAdpVersion():
                version = obj.adp_version
        elif dev_type == 7:
            obj.adp_ser.OpenPort()  # 升完级默认关闭串口
            if obj.GetAdpVersion():
                version = obj.adp_version
        elif dev_type == 8:
            obj.drl_ser.OpenPort()  # 升完级默认关闭串口
            if obj.GetDrlVersion():
                version = obj.drl_version
        elif dev_type == 9:
            obj.drl_ser.OpenPort()  # 升完级默认关闭串口
            if obj.GetDrlVersion():
                version = obj.drl_version
        elif dev_type == 10:
            obj.drl_ser.OpenPort()  # 升完级默认关闭串口
            if obj.GetDrlVersion():
                version = obj.drl_version
        elif dev_type == 11:
            obj.drl_ser.OpenPort()  # 升完级默认关闭串口
            if obj.GetDrlVersion():
                version = obj.drl_version
        else:
            obj.adp_ser.OpenPort()  # 升完级默认关闭串口
            if obj.GetAdpVersion():
                version = obj.adp_version
        target_version_lst.append(version)  # 获取升级后版本号
        print('回退到上一正式版本：')
        if not dl.DownLoadPlus(dev_type, 1):  # 版本回退到升级之前
            print('回退版本失败')
            return False
        time.sleep(3)
        version = ''
        if dev_type == 1 or dev_type == 2:
            obj.adp_ser.OpenPort()  # 升完级默认关闭串口
            if obj.GetAdpVersion():
                version = obj.adp_version
        elif dev_type == 3:
            obj.rv_ser.OpenPort()  # 升完级默认关闭串口
            if obj.Get_RV_Version():
                version = obj.rv_version
        elif dev_type == 4:
            pass
        elif dev_type == 5:
            pass
        elif dev_type == 6:
            obj.adp_ser.OpenPort()  # 升完级默认关闭串口
            if obj.GetAdpVersion():
                version = obj.adp_version
        elif dev_type == 7:
            obj.adp_ser.OpenPort()  # 升完级默认关闭串口
            if obj.GetAdpVersion():
                version = obj.adp_version
        elif dev_type == 8:
            obj.drl_ser.OpenPort()  # 升完级默认关闭串口
            if obj.GetDrlVersion():
                version = obj.drl_version
        else:
            obj.adp_ser.OpenPort()  # 升完级默认关闭串口
            if obj.GetAdpVersion():
                version = obj.adp_version
        present_version_lst.append(version)  # 获取回退后版本号
        # 设定下一次升级的波特率，若为最后一次，则修改为之前默认波特率，保证程序下载测试结束后保持原波特率
        if dev_type == 1 or dev_type == 2:
            if idex == (len(baudrate_lst) - 1):
                adp_set_lst = [obj.sys_cmd.Wr(80, baudrate_lst[0]), obj.sys_cmd.Poweroff_Save()]
            else:
                adp_set_lst = [obj.sys_cmd.Wr(80, baudrate_lst[idex + 1]), obj.sys_cmd.Poweroff_Save()]
            for cmd in adp_set_lst:
                obj.AdpSend(cmd)
                obj.AdpReceive()
                time.sleep(1)
        elif dev_type == 3:
            if idex == (len(baudrate_lst) - 1):
                adp_set_lst = [obj.gen_cmd.Set_Usart_Baudrate(baudrate_lst[0]), obj.gen_cmd.Set_Poweroff_Save()]
            else:
                adp_set_lst = [obj.gen_cmd.Set_Usart_Baudrate(baudrate_lst[idex + 1]), obj.gen_cmd.Set_Poweroff_Save()]
            for cmd in adp_set_lst:
                obj.RV_Send(cmd)
                obj.RV_Receive()
                time.sleep(1)
        elif dev_type == 8:
            if idex == (len(baudrate_lst) - 1):
                adp_set_lst = [obj.sys_cmd.Wr(80, baudrate_lst[0]), obj.sys_cmd.Poweroff_Save()]
            else:
                adp_set_lst = [obj.sys_cmd.Wr(80, baudrate_lst[idex + 1]), obj.sys_cmd.Poweroff_Save()]
            for cmd in adp_set_lst:
                obj.DrlSend(cmd)
                obj.DrlReceive()
                time.sleep(1)
        power.Power_Output_Control(0)
        time.sleep(2)
        power.Power_Output_Control(1)
        time.sleep(2)
    # 检查下载是否成功
    for tar_idex in range(0, len(target_version_lst) - 1):
        if target_version_lst[tar_idex] != target_version_lst[tar_idex + 1]:
            print('升级版本不一致，升级功能验证不通过')
            return False
    for pre_idex in range(0, len(target_version_lst) - 1):
        if target_version_lst[pre_idex] != target_version_lst[pre_idex + 1]:
            print('回退版本不一致，回退功能验证不通过')
            return False
    # 重新升级到最新版本，方便后续测试
    sp.Reset_Ser_Baud(0, default_com, baudrate_lst[0])
    if not dl.DownLoadPlus(dev_type, 0):  # 进行升级操作
        print('升级失败')
        return False
    power.Power_Output_Control(0)
    time.sleep(2)
    power.Power_Output_Control(1)
    time.sleep(2)
    return True


if __name__ == '__main__':
    Download_Test(3)
