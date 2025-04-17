# import os
import time
import pipette as pi
# import Com.Log.log as lg
import Com.Download.software_upgrade as dl
import Com.Port.serialport as sp

dev_baudrate = {
    1: [38400, 9600, 19200, 115200],  # sp16
    2: [38400, 9600, 19200, 115200]  # sp18
}


def Download_Test(dev_type: int):
    """
    adp移液器下载测试
    :param dev_type: 1：sp16；2：sp18
    :return: True or False
    """
    com_str = sp.List_Serial_Ports()
    baudrate_lst = dev_baudrate[dev_type]
    # 自动查询串口号默认波特率及地址
    connet_flag = 0  # 成功连接adp标志位

    default_baud = 38400  # 初始化默认波特率
    default_com = ''  # 初始化默认串口号
    default_addr = 1  # 初始化默认地址

    # 获取当前通信串口号及波特率、地址
    for com_test in com_str:
        for baud_test in baudrate_lst:
            sp.Reset_Ser_Baud(com_test, baud_test)
            adp = pi.ADP()
            if adp.GetAdpAddress():
                default_baud = baud_test  # 获取正确波特率
                default_addr = adp.address  # 获取正确地址
                default_com = com_test  # 获取正确串口号
                connet_flag = 1
                break
        if connet_flag == 1:
            break
    if connet_flag == 0:
        print('adp通讯异常，请检查通讯')
        return False
    del baudrate_lst[baudrate_lst.index(default_baud)]  # 去掉当前波特率
    baudrate_lst.insert(0, default_baud)  # 将当前波特率加入到第一个元素
    target_version_lst = []
    present_version_lst = []
    # 开始程序下载测试流程
    for idex in range(0, len(baudrate_lst)):
        print('当前下载波特率：', baudrate_lst[idex])
        sp.Reset_Ser_Baud(default_com, baudrate_lst[idex])
        adp = pi.ADP()
        adp.address = default_addr  # 重设ADP地址
        if not dl.DownLoadPlus(dev_type, 0):  # 进行升级操作
            print('升级失败')
            return False
        time.sleep(3)
        adp.adp_ser.OpenPort()  # 升完级默认关闭串口
        if not adp.GetAdpVersion():
            return False
        target_version_lst.append(adp.adp_version)  # 获取升级后版本号
        print('回退到上一正式版本：')
        if not dl.DownLoadPlus(dev_type, 1):  # 版本回退到升级之前
            print('回退版本失败')
            return False
        time.sleep(3)
        adp.adp_ser.OpenPort()  # 升完级默认关闭串口
        if not adp.GetAdpVersion():
            return False
        present_version_lst.append(adp.adp_version)  # 获取回退后版本号
        # 设定下一次升级的波特率，若为最后一次，则修改为之前默认波特率，保证程序下载测试结束后保持原波特率
        if idex == (len(baudrate_lst) - 1):
            adp_set_lst = [adp.adp_cmd.Wr(80, baudrate_lst[0]), adp.adp_cmd.Poweroff_Save(),
                           adp.adp_cmd.Sys_Reset()]
        else:
            adp_set_lst = [adp.adp_cmd.Wr(80, baudrate_lst[idex + 1]), adp.adp_cmd.Poweroff_Save(),
                           adp.adp_cmd.Sys_Reset()]
        for cmd in adp_set_lst:
            adp.AdpSend(cmd)
            adp.AdpReceive()
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
        sp.Reset_Ser_Baud(default_com, baudrate_lst[0])
        adp = pi.ADP()
        adp.address = default_addr  # 重设ADP地址
        if not dl.DownLoadPlus(dev_type, 0):  # 进行升级操作
            print('升级失败')
            return False
    return True


if __name__ == '__main__':
    Download_Test(1)
